/* file: es86.c
 * job: file condiviso
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
char msg[256];
int fd;
int main(int argc, char **argv) {
	pid_t fr; /* fork return value */
	int st; /* spazio per l'exit value */
	int count; /* contatore di read */
	char ch; /* spazio per la read */
	fd=open(argv[1],O_RDONLY);
	if (fd<0) return(-1);
	fr=fork();
	switch (fr) {
		case 0: /* figlio */
			for (count=0; read(fd,&ch,1)==1; count++);
			snprintf(msg,sizeof(msg),"Figlio conta %d\n",count);
			write(1,msg,strlen(msg));
			return(4); /* exit value del figlio */
			break;
		case -1: /* errore, nessun nuovo processo */
			snprintf(msg,sizeof(msg),"Errore in fork\n");
			write(1,msg,strlen(msg));
			break;
		default: /* padre */
			for (count=0; read(fd,&ch,1)==1; count++);
			snprintf(msg,sizeof(msg),
				"Padre del figlio %d,conta: %d\n",fr,
				count);
			write(1,msg,strlen(msg));
			/* uso la wait per attendere il processo
			 * figlio e recuperare il suo exit value */
			wait(&st);
			snprintf(msg,sizeof(msg),
				"Exit value: %d\n",WEXITSTATUS(st));
			write(1,msg,strlen(msg));
	}
	return(0);
}

