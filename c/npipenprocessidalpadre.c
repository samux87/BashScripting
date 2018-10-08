/* file: es98.c
 * job: N pipe, N processi figli, da padre a figlio
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef int pipe_t[2];
char msg[256];
char ch;
pipe_t *pipes;
int figlio(int arg);
int main(int argc, char **argv) {
	int npipe;
	int nfigli;
	pid_t pid;
	int i,j;
	int nw;
	if (argc<2) { /* controllo num args */
		snprintf(msg,sizeof(msg),
			"Uso: %s N\n",argv[0]);
		write(2,msg,strlen(msg));
		return(1);
	}
	nfigli=atoi(argv[1]);
	if (nfigli<=0) { /* controllo N */
		snprintf(msg,sizeof(msg),
			"N(%d) non valido\n",nfigli);
		write(2,msg,strlen(msg));
		return(1);
	}
	npipe=nfigli;
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
					close(pipes[j][1]); /* figlio non scrive */
					if (j!=i) {
						/* figlio non legge dalle altre */
						close(pipes[j][0]);
					}
				}
				return(figlio(i));
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
	}
	for (i=npipe-1; i>=0; i--) { /* per ogni pipe, dall'ultima alla prima */
		nw=write(pipes[i][1],&ch,1);
		switch (nw) {
			case 0: /* impossibile, se non c'e' spazio attende */
				snprintf(msg,sizeof(msg),
					"Non scrivo il carattere\n");
				write(2,msg,strlen(msg));
				return(1);
			case 1: /* OK */
				snprintf(msg,sizeof(msg),
					"Do il via al figlio %d\n",i);
				write(1,msg,strlen(msg));
				break;
			default: /* errore */
				snprintf(msg,sizeof(msg),
					"Errore in write\n");
				write(2,msg,strlen(msg));
				return(1);
		}
	}
	return(0);
}
/* funzione figlio */
int figlio(int arg) {
	int nr;
	nr=read(pipes[arg][0],&ch,1);
	if (nr!=1) {
		snprintf(msg,sizeof(msg),
			"Figlio con indice %d ottiene %d da read\n",arg,nr);
		write(2,msg,strlen(msg));
		return(1);
	}
	/* se qui, ho ricevuto l'ok e stampo il mio pid */
	snprintf(msg,sizeof(msg),"Figlio con pid %d\n",getpid());
	write(1,msg,strlen(msg));
	return(0);
}
/* Attenzione: questo sistema non assicura che le write dei figli su stdout 
avvengano in sequenza! Una volta ottenuto l'ok dal padre i figli diventano 
processi "eseguibili" e l'ordine con il quale il sistema assegna le slice 
ai figli e' casuale. Una soluzione esatta del problema si ottiene con il 
prossimo sorgente */

