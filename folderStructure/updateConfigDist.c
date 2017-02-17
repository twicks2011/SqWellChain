/*
Readin in a list of files which consists of three columns: x, y, z coordinates of a configuration of particles, then computes the Steinhardt-Nelson order paramter of the configuration and updates a distribution for P(N_c | E).
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define L 6
#define DC 0.5

int SNorderParam(double[][500],int,double,int*,int[][500]);
double plgndr(int, int, double);
double E_swSum(double[][500],int,double);
double E_swSingle(double rijsq,  double lambdaSq);

int main(int argc, char *argv[])
{

  if(argc<1){
    fprintf(stderr,"Expected at least 1 file!\n");
    fprintf(stderr,"Usage: ./updateConfigDist [list of files] \n");
    exit(EXIT_FAILURE);
  }
  int i;
  int j=0;
  double x[3][500];

  fprintf(stderr,"Given %d files\n",argc-1);
 
  FILE *inFilePtr;
  FILE *distInputPtr;
  int index, distTemp;
  int distribution[300];

  //____Check for distribution data and read in_____
  if((distInputPtr=fopen("distribution.dat","r"))!=NULL){
    while(!feof(distInputPtr)){
      fscanf(distInputPtr,"%d %d", &index,&distTemp);
      if(!feof(distInputPtr))
	distribution[index]=distTemp;
    }
    fclose(distInputPtr);
  }
  else
    printf("No Distribution data yet!\n");


  //_____Read in all the data_____
  for(i=1;i<=argc-1;i++){
    if(!strcmp(argv[i],"distribution.dat")) continue;
    if(!strncmp(argv[i],"DONE",4)) continue;
    if((inFilePtr = fopen(argv[i],"r"))==NULL){
      fprintf(stderr,"Can't open file %s\n",argv[i]);
      exit(EXIT_FAILURE);
    }
    fprintf(stderr,"File %d open %s\n",i,argv[i]);    

    //____Parse through the file____
    j=0;
    while(!feof(inFilePtr)){
      fscanf(inFilePtr,"%f %f %f", &x[0][j], &x[1][j], &x[2][j]);
      j++;
    }
    fclose(inFilePtr);
    
    //____Compute Steinhardt-Nelson Order Parameter____
    int particleState[500]={0};
    int corPoints[2][500]={0};
    int N=j-1;
    int SNstate=SNorderParam(x,N,1.05,particleState,corPoints);
    
    //___Update distribution____
    distribution[SNstate]++;
    
    //___Update filename
    char newFilename[100];
    sprintf(newFilename,"DONE%s",argv[i]);
    rename(argv[i],newFilename);

    //___Output new distribution____
    FILE *distOutputPtr;
    distOutputPtr=fopen("distribution.dat","w");
    for(j=0;j<=N;j++){
      if(distribution[j]>0)
	fprintf(distOutputPtr,"%d %d\n",j,distribution[j]);
    }
    fclose(distOutputPtr);
  }

  return 0;
}

  
int SNorderParam(double x[][500], int N, double lambda, int *particleState, int corPoints[][500]){

  /*
	if(E_sw < 0.0)
		return -(int)E_sw;
	else
		return 0;
  */
	int crystParts = 0;
	int i,j,k,m,cmpt;
	double xij[3], rijsq;
	int Nn[200]={0}, Np[200][200]={0};
	double Sct[200][200]={0.0}, Sp[200][200]={0.0};
	double qr[200][200]={0.0}, qi[200][200]={0.0};
	double d[200][200]={0.0};
	int Nc[200]={0};
	double mqi2, mqj2,qiqjc;
	int cryst[200]={0}, coil[200]={0};

	double lambdaSQ = lambda*lambda;	
	double c;
	double factRatio = 1.0, sign;

	
	for(i=0;i<N;i++){
		particleState[i] = 2;
		k=0;
		for(j=0;j<N;j++){
			if(j != i){
				rijsq = 0.0;
				for(cmpt=0;cmpt<3;cmpt++){
					xij[cmpt] = x[cmpt][j]-x[cmpt][i];
					rijsq += xij[cmpt]*xij[cmpt];
				}
				if(rijsq < lambdaSQ){					
					Nn[i]++;
					Np[i][k]=j;
					Sct[i][k]=xij[2]/(sqrt(rijsq));
					Sp[i][k]=atan2(xij[1],xij[0]);
					k++;
				}
			}
		}
		corPoints[0][i] = Nn[i];
	}

	double Lfactor = sqrt((2.0*L+1.0)/(4*M_PI));
	for(i=0;i<N;i++){
		sign = 1.0;
		for(m=0;m<=L;m++){
			if(m>0){
				factRatio *= (L+m)*(L-m+1);
				sign *= -1.0;
			}			
			for(j=0;j<Nn[i];j++){
				c = Lfactor/sqrt(factRatio);
				c *= plgndr(L,m,Sct[i][j]);
				qr[i][m+L] += c*sin(m*Sp[i][j])/(1.0*Nn[i]);
				qi[i][m+L] += c*sin(m*Sp[i][j])/(1.0*Nn[i]);
				if(m>0){
					qr[i][L-m] += sign*c*cos(m*Sp[i][j])/(1.0*Nn[i]);
					qi[i][L-m] -= sign*c*cos(m*Sp[i][j])/(1.0*Nn[i]);
				}
			}
		}
		factRatio = 1.0;
	}

	for(i=0;i<N;i++){
		for(j=0;j<Nn[i];j++){
			mqi2 = 0.0;
			mqj2 = 0.0;
			qiqjc = 0.0;
			for(m=-L;m<=L;m++){
			        mqi2 += qr[i][m+L]*qr[i][m+L]+qi[i][m+L]*qi[i][m+L];
				mqj2 += qr[Np[i][j]][m+L]*qr[Np[i][j]][m+L]+qi[Np[i][j]][m+L]*qi[Np[i][j]][m+L];
 				qiqjc += (qr[i][m+L]*qr[Np[i][j]][m+L]) + (qi[i][m+L]*qi[Np[i][j]][m+L]);
			}
			d[i][j]=qiqjc/sqrt(mqi2*mqj2);
		}
	}

	for(i=0;i<N;i++){
		for(j=0;j<Nn[i];j++){
			if(d[i][j]-DC>0)
				Nc[i]++;
		}
		if(Nn[i] >= 5 && Nc[i] >= Nn[i]-1){			
			cryst[i] = 1;
			particleState[i] = 1; //crystalline particle
			crystParts++;
		}
		else{
			if(Nn[i] <= 4){
				coil[i] = 1; 
				particleState[i] = 0; //coil-like
			}
			else
				particleState[i] = 4; //intermediate						
		}
		corPoints[1][i] = Nc[i];
	}

	return crystParts;
}

double plgndr(int l, int m, double x)
{

  double fact,pll,pmm,pmmp1,somx2;
  int i,ll;



  pmm=1.0;
  if (m > 0) {
    somx2=sqrt((1.0-x)*(1.0+x));
    fact=1.0;
    for (i=1;i<=m;i++) {
      pmm *= -fact*somx2;
      fact += 2.0;
    }
  }
  if (l == m)
    return pmm;
  else {
    pmmp1=x*(2*m+1)*pmm;
    if (l == (m+1))
      return pmmp1;
    else {
      for (ll=m+2;ll<=l;ll++) {
        pll=(x*(2*ll-1)*pmmp1-(ll+m-1)*pmm)/(ll-m);
        pmm=pmmp1;
        pmmp1=pll;
      }
      return pll;
    }
  }
}

double E_swSingle(double rijsq,  double lambdaSq)
{

  
  if (rijsq < 1) return 1000.0;
  if (rijsq > lambdaSq) return 0;
  return -1.0;

}

double E_swSum(double x[][500], int N, double lambda)
{
	double Ei, Esum=0.0;
	int i, j, cmpt;
	double xi[3], xij[3], rijsq;

	double lambdaSQ = lambda*lambda;

	for(i=0;i<N-1;i++){
		Ei = 0.0;
		for(cmpt=0;cmpt<3;cmpt++)
			xi[cmpt] = x[cmpt][i];
		for(j=i+1;j<N;j++){
			rijsq = 0.0;
			for(cmpt=0;cmpt<3;cmpt++){
				xij[cmpt] = x[cmpt][j]-xi[cmpt];
				rijsq += xij[cmpt]*xij[cmpt];
			}

			if((i-j)*(i-j)>1){

			   Ei    +=   E_swSingle( rijsq  ,  lambdaSQ) ;
			
			}
		}

		Esum += Ei;
	}

	return Esum;
}


