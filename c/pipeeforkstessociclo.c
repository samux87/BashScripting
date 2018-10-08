#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef int piped[2];

int main(int argc, char** argv)
{
  int i,pid,n;
  piped *pfd;
  char msg[]="culo!";
  n=atoi(argv[1]);
  printf("Parametro: %d\n",n);
  //controlliamo che il primo parametro sia un numero positivo
  if(n<=0){exit(1);}
  printf("Il parametro è positivo!\n");
  //alloca lo spazio di memoria per i file pointer
  pfd=(piped*)malloc(sizeof(piped)*(n-1));
  printf("Memoria allocata\n");
  //crea n-1 pipe e n figli
  for(i=0;i<n;i++)
  {
    //crea le n-1 pipe
    if(i!=(n-1))
    {
      pipe(pfd[i]);
      printf("Creata %d-esima pipe\n",i);
    }
    printf("Creato %d-esimo figlio\n",i);
    //crea gli n figli
    pid=fork();
    if(pid==0)
    {
      close(pfd[i][1]);
      if(i!=0){close(pfd[i-1][0]);}
      if(i!=(n-1)){read(pfd[i][0],msg,sizeof(msg));}
      pid=getpid();
      printf("Pid %d-esimo figlio: %d\n",i,pid);
      if(i!=0){write(pfd[i-1][1],msg,sizeof(msg));}
      exit(0);
    }
  }
  exit(0);
}
