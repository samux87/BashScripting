/* file: es112.c
 * job: intercettamento del segnale broken pipe
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
char msg[256];
char msg2[256];
/* prototipo dell'handler */
void intercetta(int segnale);
int main(int argc, char **argv) {
	int pfd[2]; /* contenitore per i 2 fd */
	int nw,nr; /* ritorni di read e write */
	int st;
	if (pipe(pfd)!=0) {
		write(2,"Errore in pipe()\n",17);
		return(1);
	}
	snprintf(msg,sizeof(msg),"pfd[0]=%d, pfd[1]=%d\n",
		pfd[0],
		pfd[1]);
	write(1,msg,strlen(msg)); /* su stdout */
	close(pfd[0]); /* chiudo lato lettura */
	/* utilizziamo la fork per lasciare al processo figlio
	 * l'incombenza di scrivere sulla pipe e recuperiamo
	 * le informazioni di terminazione con la wait */
	switch (fork()) {
		case 0: /* figlio */
			/* aggancio intercetta() a SIGPIPE */
			signal(SIGPIPE,intercetta);
			nw=write(pfd[1],msg,strlen(msg)); /* su msg->pipe */
			/* questa syscall scatena un segnale di
			 * "broken pipe" che provoca la terminazione del
			 * processo */
			nr=read(pfd[0],msg2,sizeof(msg2)); /* da pipe->msg2 */
			write(1,msg2,strlen(msg2)); /* msg2 su stdout */
			snprintf(msg,sizeof(msg),"nw= %d, nr= %d\n",
					nw,nr);
			write(1,msg,strlen(msg)); /* su stdout */
			return(0);
		case -1: /* errore fork */
			write(2,"Errore fork\n",12); /* su stderr */
			return(1);
			break;
	}
	/* padre: attendo il figlio (unico, non devo recuperare il pid) */
	wait(&st);
	if (WIFEXITED(st)) { /* terminazione "naturale", exit value valido */
		snprintf(msg,sizeof(msg),"Figlio torna %d\n",
			WEXITSTATUS(st));
		write(1,msg,strlen(msg));
	} else { /* terminazione forzata, no exit value ma id segnale */
		snprintf(msg,sizeof(msg),"Figlio terminato da %d\n",
			WTERMSIG(st));
		write(1,msg,strlen(msg));
	}
	return(0);
}
void intercetta(int segnale) {
	snprintf(msg,sizeof(msg),"Figlio: intercetto %d\n",
		segnale);
	write(1,msg,strlen(msg));
	return;
}

