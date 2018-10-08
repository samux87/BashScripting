/* file: es84.c
 * job: lettura di N caratteri da F
 * e copia in posizione K
 * uso: es84 N F K
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char array[101]; /* 100 + '\0'*/
char msg[256]; /* messaggio */
int fdr;/* file descriptor, lettura */
int fdw;/* file descriptor, scrittura */
int N; /* primo arg */
char *F; /* secondo arg */
int K; /* terzo arg */
int nr; /* numero char letti */
int nw; /* numero char scritti */

int main(int argc, char **argv) {
	switch (argc) { /* num args */
		case 4:
			N=atoi(argv[1]);
			F=argv[2];
			K=atoi(argv[3]);
			break;
		default:
			snprintf(msg,sizeof(msg),
			"uso: %s N F K\n",argv[0]);
			write(2,msg,strlen(msg));
			exit(1);
	}
	fdr=open(F,O_RDONLY);
	if (fdr<0) { /* chk open */
		snprintf(msg,sizeof(msg),
		"Errore in apertura (rd)\n");
		write(2,msg,strlen(msg));
		exit(1);
	}
	if (N>100) { /* chk N */
		snprintf(msg,sizeof(msg),
		"N troppo grande\n");
		write(2,msg,strlen(msg));
		exit(1);
	}
	nr=read(fdr,array,N);
	if (nr<0) { /* chk read */
		snprintf(msg,sizeof(msg),
		"Errore in lettura\n");
		write(2,msg,strlen(msg));
		exit(1);
	}
	fdw=open(F,O_WRONLY);
	if (fdw<0) { /* chk open */
		snprintf(msg,sizeof(msg),
		"Errore in apertura (wr)\n");
		write(2,msg,strlen(msg));
		exit(1);
	}
	if (lseek(fdw,K,SEEK_SET)!=K) {
		/* retval=posizione */
		snprintf(msg,sizeof(msg),
		"Errore nel posizionamento\n");
		write(2,msg,strlen(msg));
		exit(1);
	}
	nw=write(fdw,array,nr);
	snprintf(msg,sizeof(msg),
		"Letti %d caratteri\n"
		"Copiati %d caratteri alla posizione %d\n",
		nr,nw,K);
	write(1,msg,strlen(msg));
	return(0);
}

