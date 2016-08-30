#include <math.h>
#include <stdio.h>
#define EPSILON 1e-4

int bondLengthCheck(double x[][500], int N)
{

  int i, cmpt;
  double rsq;
  //double target_length= 1.1225;
  //double target_length= 2.0;
  double target_length = 1.0;
  int fault = 0;

  target_length=   target_length *   target_length;

  for(i=0;i<N-1;i++){
    rsq = 0.0;
    for(cmpt=0;cmpt<3;cmpt++)
      rsq += (x[cmpt][i]-x[cmpt][i+1])*(x[cmpt][i]-x[cmpt][i+1]);
    if((rsq-target_length)*(rsq-target_length) > EPSILON){
      fault = 1;
      printf("Bad bond length[%d][%d]: %f\n",i,i+1,sqrt(rsq));
    }
  }

  return fault;
}
