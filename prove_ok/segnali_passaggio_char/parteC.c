/* Created by Samuele Buosi */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

typedef int pipe_t[2]; 	/* tipo per la pipe */
/* Definisco le variabili globali */
pipe_t *pipes; 		/* pipes */
pipe_t *pipes_nip;
int     pid;		/* appoggio per valore pid (pid unico)*/
int 	pid_nip;
int     status; 	/* appoggio per wait */
int 	fd; 		/* per la gestione dei file creati  */
int 	fd_fout; 	/* appoggio per nuovo file fout */
int 	nfigli;		/* numero di figli */
char    ch;
int 	flag;

/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...);

void handler(int segnale);

char figlio(char *file1,char *file2, int);
int nipote(char *file,int);

int main(int argc, char **argv) {
	 int i, j;
	 nfigli = argc -1;
	 /* controllo numero di argomenti */
	 if (argc<nfigli || (nfigli % 2) != 0) {
	 	 zprintf(2,"Errore parametri parteC\n");
	 	 return(1);
	}
	nfigli = nfigli/2;
	
	 /*creazione figli*/
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
	 	 /*attesa figli con recupero variabile di ritorno e controllo errore*/
	 	 if (WIFEXITED(status) == 0) {
	 	 	 zprintf(1,"Figlio con pid: %d terminato in modo anomalo\n", pid);
	 	 } else {
	 	 	 zprintf(1,"Per il figlio con pid %d lo stato di EXIT e': %c\n",pid, WEXITSTATUS(status));
	 	 }
	 }
	 return 0;
}

char figlio (char *file, char *fileNipote, int indice){
	char nomefile[100];
	char *str1 = "merge";
	//char ch;
	int i;
    int primo=0;

	signal(SIGUSR1, handler);
	

	
	zprintf(1,"File FIGLIO: %s\n", file);
	/* creo fout */
	sprintf(nomefile, "%s%d", str1, indice);
	fd_fout= creat(nomefile, 0644);
	
	/* creo processo nipote */
	pid_nip = fork();
	switch(pid_nip){
		case 0: /* nipote */
			
			return (nipote(fileNipote, indice));
		case -1:
	 		 zprintf(2,"Errore nella fork nipote pid_nip: %d\n",pid_nip);
	 		 return(11);
	}
	
	/* Codice del figlio */
	/* Apertura file */
	fd = open(file, O_RDONLY);

    //sleep(2);
	/* scorrimento carattere per carattere */
	while(read(fd, &ch,1)) {
		
        if (primo != 0) {
            pause();
        }
        primo =1;
		write(fd_fout, &ch, 1);
        zprintf(1,"FIGLIO: Scritto %c\n",ch);
		zprintf(1,"FIGLIO: %d\n", getpid());
        sleep(2);
		kill(pid_nip,SIGUSR1);

	}
    pause(); /* aspetto che finisca nipote */

    kill(pid_nip,SIGUSR1);
    pid=wait(&status);
    /*attesa figli con recupero variabile di ritorno e controllo errore*/
    if (WIFEXITED(status) == 0) {
        zprintf(1,"Figlio con pid: %d terminato in modo anomalo\n", pid);
        return (WEXITSTATUS(status));
    } else {
        return (WEXITSTATUS(status));
    }
	
}

int nipote (char *file, int indice){
	zprintf(1,"File NIPOTE: %s\n", file);
	
    int ch_intero;
	/* Apertura file */
	fd = open(file, O_RDONLY);
	
	/* scorrimento carattere per carattere */
	while(read(fd, &ch,1)) {
           sleep(2);
        zprintf(1,"NIPOTE LEGGE DA FILE: %c\n",ch);
		zprintf(1,"NIPOTE: %d\n", getpid());
        write(fd_fout, &ch, 1);
		kill(getppid(), SIGUSR1);
        pause();
	}
    
    ch_intero= ch;
    zprintf(1,"ch: %c ch_intero: %d\n",ch, ch_intero);
	return(ch_intero);
	
}


void handler(int segnale){
	
    zprintf(1,"SEGNALE da %d\n", getpid());
//	write(fd_fout, &ch, 1);
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
