/* file: es9A.c
 * job: N pipe, N+1 processi figli, da figlio a figlio in ordine inverso
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
typedef int pipe_t[2];
char msg[256];
char ch;
pipe_t *pipes;
int figlio(int arg, int ultimo);
int main(int argc, char **argv) {
	int npipe;
	int nfigli;
	pid_t pid;
	int i,j;
	if (argc<2) { /* controllo num args */
		snprintf(msg,sizeof(msg),
			"Uso: %s N\n",argv[0]);
		write(2,msg,strlen(msg));
		return(1);
	}
	nfigli=atoi(argv[1]);
	if (nfigli<=1) { /* controllo N */
		snprintf(msg,sizeof(msg),
			"N(%d) non valido\n",nfigli);
		write(2,msg,strlen(msg));
		return(1);
	}
	npipe=nfigli-1;
	pipes = malloc(sizeof(pipe_t)*npipe);
	if (pipes==NULL) { /* controllo malloc */
		snprintf(msg,sizeof(msg),
			"Allocazione fallita\n");
		write(2,msg,strlen(msg));
		return(1);
	}
	for (i=0; i<npipe; i++) { /* per ogni pipe */
		/* creo la coppia di fds */
		if (pipe(pipes[i])!=0) { /* errore */
			snprintf(msg,sizeof(msg),
				"pipe fallita in %d\n",i);
			write(2,msg,strlen(msg));
			return(1);
		}
	}
	for (i=0; i<nfigli; i++) { /* per ogni figlio */
		pid=fork(); /* lo creo */
		switch(pid) {
			case 0: /* ogni figlio esegue la funzione
				   e termina. Prima della chiamata chiudo i fd
				   che il figlio non deve usare */
				for (j=0; j<npipe; j++) {
					if (j!=(i-1)) { /* figlio[i] scrive su pipe[i-1] */
						close(pipes[j][1]);
					}
					if (j!=i) { /* e legge da pipe[i] */
						close(pipes[j][0]);
					}
				}
				return(figlio(i,nfigli-1));
			case -1:
				snprintf(msg,sizeof(msg),
					"fork() fallita in %d\n",i);
				write(2,msg,strlen(msg));
				return(1);
		}
	}
	/* solo il padre arriva qui */
	for (i=0; i<npipe; i++) { /* per tutte le pipe */
		close(pipes[i][0]); /* il padre non legge, chiudo il fd di lett*/
		close(pipes[i][1]); /* e non scrive, chiudo il fd di scrittura */
	}
	/* volendo, posso attendere i figli per recuperare gli exit value */
	for (i=0; i<nfigli; i++) {
		wait(NULL);
	}
	return(0);
}
/* funzione figlio */
int figlio(int arg, int ultimo) {
	int nr,nw;
	if (arg!=ultimo) { /* F[N-1] non deve attendere */
		nr=read(pipes[arg][0],&ch,1);
		/* read bloccante */
		if (nr!=1) {
			snprintf(msg,sizeof(msg),
					"Figlio con indice %d ottiene %d da read\n",arg,nr);
			write(2,msg,strlen(msg));
			return(1);
		}
	}
	/* se qui, ho ricevuto l'ok e stampo il mio pid */
	snprintf(msg,sizeof(msg),"Figlio con pid %d\n",getpid());
	write(1,msg,strlen(msg));
	/* ora do il via al figlio successivo a meno che questo non sia
	 * il primo figlio */
	if (arg!=0) {
		nw=write(pipes[arg-1][1],&ch,1);
		if (nw!=1) {
			snprintf(msg,sizeof(msg),
					"Figlio con indice %d ottiene %d da write\n",arg,nw);
			write(2,msg,strlen(msg));
			return(1);
		}
	}
	return(0);
}

