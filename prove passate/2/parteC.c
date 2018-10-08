/* Created by Samuele Buosi */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

typedef int pipe_t[2]; 	/* tipo per la pipe */
int figlio(char *Fi,int indice);

// Definisco le variabili globali
pipe_t *pipes_f; 		/* pipes da padre a figlio */
pipe_t *pipes_s; 		/* pipes da figlio a padre */
int nfigli,npipe;	/* numero di figli e numero di pipe da creare */
long int count;     /* Var per conteggio occorrenze ch */
char ch;			/* Var per passaggio caratteri da input utente */
int stato;

/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...);

int main(int argc, char **argv) {
	int i;
	int     pid;		/* appoggio per valore pid (pid unico)*/
	int     status; 	/* appoggio per wait */
	 /* controllo numero di argomenti */
	 if (argc<1+1) {
	 	 zprintf(2,"Errore nel numero di argomenti\n");
	 	 return(1);
	}

	/*CONTROLLO PARAMETRI*/
	 //Controllo su nfigli
	 nfigli=argc;
	 //Allocazione di un array di pipe per la comunicazione fra filgi
	 npipe=nfigli-1;
	 pipes_f= malloc(sizeof(pipe_t)*npipe);
	 if(pipes_f==NULL)
	 {
	 	 zprintf(2,"errore nella malloc del vettore di pipe\n");
	 	 return(5);
	 }
	 /*creazione pipe*/
	 for (i=0; i<npipe; i++)
	 {/*creazione della pipe*/
	 	 if (pipe(pipes_f[i])!=0)
	 	 {/*in caso di errore*/
	 	 	 zprintf(2,"Errore, pipe fallita in %d\n",i);
	 	 	 return(6);
		 }
	 }
	 
	 pipes_s= malloc(sizeof(pipe_t)*npipe);
	 if(pipes_s==NULL)
	 {
	 	 zprintf(2,"errore nella malloc del vettore di pipe\n");
	 	 return(5);
	 }
	 /*creazione pipe*/
	 for (i=0; i<npipe; i++)
	 {/*creazione della pipe*/
	 	 if (pipe(pipes_s[i])!=0)
	 	 {/*in caso di errore*/
	 	 	 zprintf(2,"Errore, pipe fallita in %d\n",i);
	 	 	 return(6);
	 }
	 
	 }
	 /*creazione figli*/
	 for (i=0; i<nfigli; i++)
	 {
	 	 pid=fork();
	 	 switch(pid)
	 	 {
	 	 	 case 0: /* Figlio */
	 	 	 /*richiama la funzione figlio */
	 	 	 figlio(argv[i+1],i);
	 	 	 
	 	 	 case -1:
	 	 	 	 zprintf(2,"Errore nella %d fork\n",i);
	 	 	 	 return(7);
	 	 }
	 }
	 /* PADRE */
	 for (i=0; i<npipe; i++)
	 { /* per tutte le pipe */
	 	  close(pipes_f[i][0]); /* il padre non legge da pipe_f */
		  close(pipes_s[i][1]); /* e non scrive da pipe_s */
	 }

	 sleep(10); // Per attesa figli e la read sia bloccante
	 // Chiede caratteri in ingresso
	 zprintf(1,"Inserisci i caratteri che i figli devono cercare nei file\n");
	 while(read(0,&ch,1)) {
	 	// Passa i ch ai figli
 	 	for(i=0;i<npipe;i++)	write(pipes_f[i][1],&ch,sizeof(ch));
		// Leggo i valori della pipe_s
		for(i=0;i<npipe;i++){
			read(pipes_s[i][0],&count,sizeof(count));
			zprintf(1,"Il figlio con pid %d ha contato %ld caratteri",pid, count);
		}	   
	 }
	 
	 for(i=0;i<npipe;i++){
		 close(pipes_f[i][1]); // Chiudo i lati di scrittura pipe_f
		 close(pipes_s[i][0]); // Chiudo i lati di lettura pipe_s
	 }
	 
	 /*attende i figli per recuperare gli exit value*/
	 for (i=0; i<nfigli; i++){
	 	 pid=wait(&status);
	 	 /*attesa figli con recupero variabile di ritorno e controllo errore*/
	 	 if (WEXITSTATUS(status)==255) {
	 	 	 zprintf(1,"Errore in status\n");
	 	 } else {
	 	 	 zprintf(1,"Per il figlio con pid %d lo stato di EXIT e %d\n",pid,WEXITSTATUS(status));
	 	 }
	 }
	 return 0;
}

int figlio(char *Fi, int indice){
	int i,fd;
	char chr;

	// Chiudo parte pipe non utilizzata
	for(i=0;i<npipe;i++){
		close(pipes_f[i][0]);
		close(pipes_s[i][1]);
		// Tengo aperto in scrittura solo quella del processo corrente
		if(i != indice) close(pipes_s[i][0]); 
	}
		
	// Legge ch passato da padre
	while(read(pipes_f[indice][0],&chr,1)){
		count=0; //Inizializzo a 0 il contatore
		// cerca ch nel proprio file
		while(read(fd,&ch,1)>0){ // fino alla fine del file Fi
			// Trovato?
			if(ch == chr){
				count++;
				stato=1;
			}	
		}
		// Comunico al padre il numero di occorrenze
		write(pipes_s[indice][1],&count,sizeof(count));
		// ritorno all'inizio del file per una nuova ricerca
		lseek(fd,0L,0);
	}
	
	
	return stato;
}


void zprintf(int fd, const char *fmt, ...) {
	 /* printf wrapper using write instead */
	 static char msg[256];
	 va_list ap;
	 int n;
	 va_start(ap, fmt);
	 n=vsnprintf (msg, 256, fmt, ap);
	 write(fd,msg,n);
	 va_end(ap);
}
