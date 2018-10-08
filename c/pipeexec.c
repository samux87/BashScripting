/* file: pipeline.c
 * Esempio di exec()
 * N processi, ogni processo esegue expr per moltiplicare un parametro numerico
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

typedef int pipe_t[2]; /* tipo per pipe */
pipe_t *Pfds; /* Array di pipe, allocazione dinamica */
int Nfigli; /* Numero di figli da forkare */

void zprintf(int, const char *, ...);
int figlio(int, int);

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

int figlio(int indice, int number) {
	int nr_rd;
	char mybuff[256];

	memset(mybuff, 0, 256);
	sprintf(mybuff, "%d", number);

	execlp("expr","expr",mybuff,"*","2",NULL);
}


int main(int argc, char **argv, char **envp) {
	int i; /* un indice */
	int j; /* altro indice, se serve */
	char buffer[1024];

	/* Processamento command line */
	if (argc < 2) {
		zprintf(2,"Uso %s N\n",argv[0]);
		return(1);
	}

	Nfigli = atoi(argv[1]);
	if (Nfigli<1) {
		zprintf(2,"N>0\n");
		return(1);
	}

	/* Creazione delle pipe necessarie */
	Pfds = malloc(Nfigli * sizeof(pipe_t));
	if (Pfds==NULL) {
		zprintf(2,"Impossibile allocare Pfds\n");
		return(1);
	}
	for (i=0; i<Nfigli; i++) {
		if (pipe(Pfds[i])!=0) {
			zprintf(2,"Impossibile creare la pipe %d\n",i);
			return(1);
		}
	}

	/* Creazione processi figli:
	 * Non serve Pids, semplifichiamo! */
	for (i=0; i<Nfigli; i++) {
		switch (fork()) {
			case 0: /* figlio di indice i */
				/* Chiusura delle pipe non usate */
				for (j=0; j<Nfigli; j++) {
					close(Pfds[j][0]);
					if (j!=i) {
						close(Pfds[j][1]);
					}
				}
				close(1);
				dup2(Pfds[i][1], 1);
				return(figlio(i, (i+1)));
			case -1:
				zprintf(2,"Impossibile forkare il processo %d\n",i);
				return(1);
				break;
		}
	}

	/* Chiusura delle pipe non usate */
	for (j=0; j<Nfigli; j++) {
			close(Pfds[j][1]);
	}
	
	/* Recupero delle informazioni dai processi figli tramite la wait,
	 * se non serve l'abbinamento si deve semplificare */
	for (i=0; i<Nfigli; i++) {
		int status;
		int pid_x;
		int n;
		
		memset(buffer, 0, 1024);
		read(Pfds[i][0], buffer, 1024);

		pid_x = wait(&status);
		if (WIFEXITED(status)) {
			zprintf(1, "pid[%d] exit_code=%d value=%s\n", pid_x,WEXITSTATUS(status),buffer);
		} else {
			zprintf(1, "pid[%d] quit with signal=%d\n", pid_x,WTERMSIG(status));
		}
	}
	return(0);
}
			

