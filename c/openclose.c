/* file: es82.c
 * job: open, close
 */
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

char msg[256];
int main(int argc, char **argv) {
	int fd;
	int nw;/* numero di caratteri scritti */
	if (argc<2) {
		/* manca il nome, usciamo */
		write(2,"Inserire il nome del file!\n",28);
		exit(1);
	} else {
		fd=open(argv[1],O_WRONLY|O_CREAT|O_EXCL,00666);
		snprintf(msg,sizeof(msg),"fd=%d\n",fd);
		write(1,msg,strlen(msg));
	}
	nw=write(fd,"Ciao, Mondo!\n",13);
	if(close(fd)!=0) {
		write(2,"Errore in chiusura\n",20);
	}
	return(nw);
}

