/* Created by Samuele Buosi */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
/* Definisco le variabili globali */
int     pid;		/* appoggio per valore pid (pid unico)*/
int     status; 	/* appoggio per wait */
int 	fd; 		/* per la gestione dei file creati  */
int 	nfigli;		/* numero di figli */
int 	fdp[2];
char 	chj;	
char 	cchj;

int figlio(char *file1, char* file2, int indice);
int nipote(char *file, int indice);

/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...);
int main(int argc, char **argv) {
	 int i;
	 /* controllo numero di argomenti */
	 if (argc<3) {
	 	 zprintf(2,"Errore nel numero di argomenti\n");
	 	 return(1);
	}
	/*CONTROLLO PARAMETRI*/
	 /* Controllo su nfigli */
	 nfigli=argc-1;
	 if(nfigli<2 || (nfigli % 2) != 0 )
	 {
	 	 zprintf(2,"Errore, numero nfigli non corretto\n");
	 	 return 4;
	 }
	 /*creazione figli*/
	nfigli = nfigli / 2;
	 for (i=0; i<nfigli; i++)
	 {
	 	 pid=fork();
	 	 switch(pid)
	 	 {
	 	 	 case 0: /* Figlio */
	 	 	 /*richiama la funzione figlio per fare quel che deve*/
	 	 	 return (figlio(argv[i+1],argv[i+1+nfigli],i));
	 	 	 case -1:
	 	 	 	 zprintf(2,"Errore nella %d fork\n",i);
	 	 	 	 return(7);
	 	 }
	 }
	 /* PADRE */
	 /*attende i figli per recuperare gli exit value*/
	 for (i=0; i<nfigli; i++){
	 	 pid=wait(&status);
		zprintf(1,"EXITSTATUS: %d STATUS: %d\n", WEXITSTATUS(status),WIFEXITED(status));
	 	 /*attesa figli con recupero variabile di ritorno e controllo errore*/
	 	 if (WIFEXITED(status) == 0) {
	 	 	 zprintf(1,"Figlio con pid: %d terminato in modo anomalo\n", pid);
	 	 } else if (WEXITSTATUS(status) > 1) {
	 	 	 zprintf(1,"Per il figlio con pid %d lo stato di EXIT e' %c\n",pid,WEXITSTATUS(status));
		 } else {
			zprintf(1,"Figlio con pid: %d terminato con -%d\n", pid, WEXITSTATUS(status));
		 }
	 }
	 return 0;
}


int figlio(char *file1, char *file2, int indice){

	char temp;
	int chj_int;

	 /* creazione pipe */
	 if (pipe(fdp)!=0)
	 {/*in caso di errore*/
	 	zprintf(2,"Errore, pipe fallita in %d\n",indice);
	 	return(6);
	 }

	 pid=fork();
	 switch(pid)
	 {
	 	 case 0: /* Nipote */
	 	 	/*richiama la funzione nipote */
	 	 	return (nipote(file2,indice));
	 	 case -1:
	 	 	 zprintf(2,"Errore nella fork figlio %d\n",indice);
	 	 	 return(7);
	 }

	/* Figlio */
	/* Dopo fork nipote chiudo lato non utilizzato */
 	  close(fdp[1]); /* non scrive, chiudo il fd di scrittura */
	
	fd = open(file1,O_RDONLY);	
	
	zprintf(1,"FIGLIO pid: %d\n", getpid());
	sleep(1);

	/* leggo l'i-esimo ch */
	read(fd,&chj,1);

	zprintf(1,"FIGLIO: letto da file -> %c\n", chj);
	read(fdp[0],&temp,1);
	zprintf(1,"FIGLIO %d: Letto da pipe: %c\n",indice,temp);
	if(chj == temp){
		chj_int = chj;
		zprintf(1,"chj==temp!\n");
		return(chj_int);
	}
	
	return(1);
}
int nipote(char *file, int indice){

	int i=indice+1;
	int flag;

	close(fdp[0]); /* chiudo il fd di lettura */
	fd = open(file,O_RDONLY);
	
	zprintf(1,"NIPOTE pid: %d\n", getpid());
	lseek(fd,-i,2);
	read(fd,&cchj,1);
	
	zprintf(1,"NIPOTE: letto da file -> %c\n", cchj);
	write(fdp[1],&cchj,1);
	

	return(22);
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
