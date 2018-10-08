/* file: es94.c
 * job: array di pipe
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char msg[256];
int *pfds;
int main(int argc, char **argv) {
	int npipe;
	int i;
	if (argc<2) { /* controllo num args */
		snprintf(msg,sizeof(msg),
			"Uso: %s N\n",argv[0]);
		write(2,msg,strlen(msg));
		return(1);
	}
	npipe=atoi(argv[1]);
	if (npipe<=0) { /* controllo N */
		snprintf(msg,sizeof(msg),
			"N(%d) non valido\n",npipe);
		write(2,msg,strlen(msg));
		return(1);
	}
	pfds = malloc(2*sizeof(int)*npipe);
	if (pfds==NULL) { /* controllo malloc */
		snprintf(msg,sizeof(msg),
			"Allocazione fallita\n");
		write(2,msg,strlen(msg));
		return(1);
	}
	for (i=0; i<npipe; i++) { /* per ogni pipe */
		/* creo la coppia di fds */
		if (pipe(&(pfds[2*i]))!=0) { /* errore */
			snprintf(msg,sizeof(msg),
				"pipe fallita in %d\n",i);
			write(2,msg,strlen(msg));
			return(1);
		}
	}
	for (i=0; i<npipe; i++) { /* per ogni pipe */
		/* stampo i file descriptor */
		snprintf(msg,sizeof(msg),"pfds[2*%d+0]=%d, pfds[2*%d+1]=%d\n",
			i,pfds[2*i+0],
			i,pfds[2*i+1]);
		write(1,msg,strlen(msg)); /* su stdout */
	}
	return(0);
}

