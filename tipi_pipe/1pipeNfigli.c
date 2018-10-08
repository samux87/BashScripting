/* file: es96.c
 * job: 1 pipe, N processi figli
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef int pipe_t[2];
char msg[256];
pipe_t *pipes;
struct {
	int indice;
} struttura;
int figlio(int arg);
int main(int argc, char **argv) {
	int npipe;
	int nfigli;
	pid_t pid;
	int i;
	int nr;
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
	npipe=1;
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
				   e termina */
				return(figlio(i));
			case -1:
				snprintf(msg,sizeof(msg),
					"fork() fallita in %d\n",i);
				write(2,msg,strlen(msg));
				return(1);
		}
	}
	/* solo il padre arriva qui */
	close(pipes[0][1]); /* il padre non scrive, chiudo il fd di scritt*/
	for (i=0; i<nfigli; i++) { /* per ogni figlio */
		nr=read(pipes[0][0],&struttura,sizeof(struttura));
		/* Nella pipe vengono inserite dai figli dei "blocchi"
		 * di byte la cui dimensione e' pari a quella del tipo
		 * "struttura". Visto che l'operazione di write e'
		 * atomica, il padre o legge una intera struttura o
		 * si sospende fino al momento della disponibilita'
		 * o, ancora, ottiene 0 per indicare che la pipe
		 * non ha dati e non ne potra' mai piu' avere, ovvero
		 * quando tutti i processi che avevano il file descriptor
		 * del lato di scrittura lo hanno chiuso. */
		switch (nr) {
			case 0: /* pipe chiusa, tutti i figli sono terminati
			ma qualcuno non ha inviato la struttura */
				snprintf(msg,sizeof(msg),
					"Ricevo solo %d strutture\n",i);
				write(2,msg,strlen(msg));
				return(1);
			case sizeof(struttura): /* lettura di una struct */
				snprintf(msg,sizeof(msg),
					"Ricevo la struttura:%d\n",struttura.indice);
				write(1,msg,strlen(msg));
				break;
			default: /* errore */
				snprintf(msg,sizeof(msg),
					"Ricevo solo %d byte\n",nr);
				write(2,msg,strlen(msg));
				return(1);
		}
	}
	return(0);
}
/* funzione figlio */
int figlio(int arg) {
	int nw;
	close(pipes[0][0]); /* figlio non legge */
	struttura.indice=arg;
	nw=write(pipes[0][1],&struttura,sizeof(struttura));
	if (nw!=sizeof(struttura)) {
		snprintf(msg,sizeof(msg),
			"Figlio con indice %d scrive solo %d bytes\n",arg,nw);
		write(2,msg,strlen(msg));
		return(1);
	}
	return(0);
}

