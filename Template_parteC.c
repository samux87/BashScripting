#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef int pipe_t[2];

typedef struct
{
  int c1;
  int c2;
} costrutto;


char msg[256];
char ch;
pipe_t *pipes; /*array di puntatori a pipe*/
int nfigli, npipe, pid, i, j, status;
int fd;
int exit_s;
int h,k;
costrutto var;

/*funziona agganciata al segnale*/
void handler(int segnale)
{
  /*qualcosa*/
  /*il collegamento al segnale si disabilita dopo la prima chiamata, se deve essere usato ancora bisogna riassegnarlo al termine dell'handler*/
  signal(SIGUSR1,handler);
}

/*funzione figlio*/
int figlio(char *file)
{
  char cs[256];
  int numlinea,indice, H;
  pipe_t *pipesD,*pipesU;

  /*lancio e presa di un allarme settato*/
  signal(SIGALRM,handler);
  alarm(10);

  /*invio di un segnale generico con kill, sintassi kill(pid del processo, nome o numero segnale)*/
  signal(SIGUSR1,handler);
  kill(pid,SIGUSR1);

  /*intercetta un eventuale segnale di pipe rotta, scatenata dalla write sotto*/
  signal(SIGPIPE,handler);
  write(0,msg,strlen(msg));
  
  /*attende un segnale qualsiasi*/
  pause();

  /*lettura e scrittura della pipe si fanno con read e write*/

  /*apertura file*/
  fd=open(file, O_RDONLY);

  /* lettura file a linee */
  while (read(fd,&(cs[i]),1))
  {
    /* conteggio linee */
    if(cs[i] != '\n')
    {
	  i++;
	}
    else
    {                                       
	  numlinea++;
	  if(numlinea == indice)
      {       
	               	/* siamo sulla linea giusta e quindi deve avvenire la comunicazione nella pipe: il protocollo do comunicazione
			   prevede che il figlio generico mandi a Psotto o Psopra tutto il buffer che contiene quindi la linea oltre a
			   dei caratteri 'spuri'  */
	    		/* incrementiamo i dato che si deve tenere conto nel conteggio del teminatore di linea */
 	    i++;                              
        if(i>=H)
        {
		  write(pipesD[indice][1],&cs,sizeof(cs));
        }
		else
        {
	      write(pipesU[indice][1],&cs,sizeof(cs));
        }
		return (i);
                        }
		else i = 0; /* si deve azzerare l'indice della linea */
    } 
  } 

  
}


void main(int argc, char **argv)
{
  /*redirezione in entrata ed uscita coi file*/
  close(0);
  open("dati", O_RDONLY);
  close(1);
  creat("risultati",0755);

  /*redirezione in entrata ed uscita con pipe e dup*/
  close(0);
  dup(fd[0]);
  close(1);
  dup(fd[1]);

  /*controllo numero argomenti*/
  if(argc</*numero minimo argomenti richiesti*/)
  {
    snprintf(msg,sizeof(msg),"Troppi pochi argomenti");
    return(1);
  }
  /*da usare se il numero di figli è passato tramite parametro con numero minimo*/
  nfigli=atoi(argv[/*posizione del parametro*/]);
  if (nfigli</*numero minimo*/)
  {
    snprintf(msg,sizeof(msg),"N(%d) non valido\n",nfigli);
    write(2,msg,strlen(msg));
    return(1);
  }

  /*controllo che il penultimo argomento sia un carattere*/
  if(strlen(argc[argv-2])!=1)
  {
    snprintf(msg,sizeof(msg),"C non è un singolo carattere\n");
    write(2,msg,strlen(msg));
    return(1);
  }
  
  /*controllo che l'ultimo argomento sia un numero strettamente positivo*/
  h=atoi(argv[argc-1]);
  if(k<=0)
  {
    snprintf(msg,256,"Inserire h intero strettamente positivo \n");
    write(1,msg,strlen(msg));
    return(2);
  }

  /*allocazione di un'array di pipe per la comunicazine tra figli (quindi sempre una in meno dei figli)*/
  npipe=nfigli-1;
  pipes = malloc(sizeof(pipe_t)*npipe);
  if (pipes==NULL)
  { /* controllo malloc */
    snprintf(msg,sizeof(msg),"Allocazione fallita\n");
    write(2,msg,strlen(msg));
    return(1);
  }

  /*creazione pipe*/
  for (i=0; i<npipe; i++)
  {/*creazione della pipe*/
    if (pipe(pipes[i])!=0)
    {/*in caso di errore*/
      snprintf(msg,sizeof(msg),"pipe fallita in %d\n",i);
      write(2,msg,strlen(msg));
      return(1);
    }
  }


  /*creazione figli*/
  for (i=0; i<nfigli; i++)
  {
    pid=fork();
    switch(pid)
    {
      case 0: /*ogni figlio fa quello che deve fare e termina*/
	  /*la prima cosa che i figli devono fare è chiudere il lato della pipe che non usano*/
          for (j=0; j<npipe; j++)
          {
	    if (j!=(i-1)) /*collegamento diretto --> j!=i*/
	    { /* figlio[i] scrive su pipe[i-1] */
	      close(pipes[j][1]);
	    }
	    if (j!=i) /*collegamento diretto --> j!=(i-1)*/
	    { /* e legge da pipe[i] */
	      close(pipes[j][0]);
	    }
	  }
	  /*richiama la funzione figlio per fare quel che deve*/
	  figlio(argv[i]);
	  return;
      case -1:
	  snprintf(msg,sizeof(msg),"fork() fallita in %d\n",i);
          write(2,msg,strlen(msg));
 	  return(1);
    }
  }

  /* solo il padre arriva qui in quanto la variabile pid per lui viene sempre riempita con il pid del figlio appena generata*/
  for (i=0; i<npipe; i++)
  { /* per tutte le pipe */
    close(pipes[i][0]); /* il padre non legge, chiudo il fd di lettura*/
    close(pipes[i][1]); /* e non scrive, chiudo il fd di scrittura */
  }
  /*attende i figli per recuperare gli exit value*/
  for (i=0; i<nfigli; i++)
  {
    pid=wait(&status);
    /*attesa figli con recupero variabile di ritorno e controllo errore*/
    if (pid<0)
    { 
      printf("Errore in wait\n");
      exit(1);
    }
    /*se c'è un errore, i primi 8 bit sono a 0, gli altri indicano il codice di errore*/
    if ((status & 0xFF) != 0)
    {  
      printf("Errore in status\n");
    }
    else
    {
      /*se non c'è un'errore, i primi 8 bit rappresentano il valore di ritorno della funzione, gli altri a 0, quindi per recuperare il valore shiftiamo tutto a destra di 8 bit e azzeriamo i primi 8 bit (in realtà non prettamnte utile, lo shift li ha già azzerati)*/
      exit_s = status >> 8;
      exit_s &= 0xFF;
      printf("Per il figlio %d lo stato di EXIT e %d\n",pid,exit_s);
    }
    exit(0);
  }
}
