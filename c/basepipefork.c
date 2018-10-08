/* Esempio di base comune */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdarg.h>
/* Numero di pipe da creare */
int npipe;
/* Numero di processi figli */
int nfigli;
/* tipo di dato per contenere i file descriptors
 * di una pipe */
typedef int pipe_t[2];
/* contenitore per i file descriptors di tutte
 * le pipe utilizzate */
pipe_t *pipe_fds;
/* prototipo funzione processo figlio, da personalizzare */
int figlio(int arg);
/* prototipo zprintf */
void zprintf(int fd, const char *fmt, ...);

int main(int argc, char **argv) {
	int pid_f;
	int status;
	int i;
	npipe=1;
	nfigli=1;
	/* allocazione memoria dinamica per pipe_fds */
	pipe_fds=malloc(npipe*sizeof(pipe_t));
	if (pipe_fds==0) {
		printf("Errore\n");
		return(1);
	}
	/* creazione delle pipe */
	for (i=0; i<npipe; i++) {
		if(pipe(pipe_fds[i])!=0) {
			printf("Errore\n");
			return(1);
		}
	}
	/* creazione dei processi figli */
	for (i=0; i<nfigli; i++) {
		switch(fork()) {
			case 0:
				/* invocazione della funzione figlio
				 * e terminazione del processo con
				 * exit value uguale al return value
				 * della funzione figlio() */
				return(figlio(i));
			case -1:
				printf("Errore\n");
				return(1);
			default:
				/* solo dal padre */
				;
		}
	}
	/* ciclo eseguito dal solo processo padre
	 * per recuperare le info di terminazione
	 * dei processi figli */
	for (i=0; i<nfigli; i++) {
		pid_f=wait(&status);
	}
	/* exit value del processo padre */
	return(0);
}
/* funzione processo figlio, definizione */
int figlio(int arg) {
	return(0);
}
/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...) {
	static char msg[256];
	va_list ap;
	int n;
	va_start(ap, fmt);
	n=vsnprintf (msg, 256, fmt, ap);
	write(fd,msg,n);
	va_end(ap);
}
