/* file: es101.c
 * job: soluzione 8 aprile 2005
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
int	M;	/* numero di processi figli */
int	K;	/* ultimo argomento */
pid_t	pid;	/* appoggio per valore pid */
int	status;	/* appoggio per wait */
int	pfn[2];	/* pipe figlio-nipote, attenzione ogni processo figlio
ha la sua istanza di questa variabile quindi le pipe create sono "M" */
int	fdr,fdw;	/* file descriptors per read e write */
int 	indice;	/* indice su linea */
char	linea[256];	/* buffer di linea */
char	FiK[80];	/* nome file di uscita */
int	LP,LD;	/* lunghezze linea */
int 	lineacorr;	/* linea corrente */
int	nonscritte;	/* numero di linee non scritte */
/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...);
/* funzione figlio */
int figlio(char *filename, int i);
/* funzione nipote */
int nipote(char *filename, int i);
int main(int argc, char **argv) {
	int i;		/* int di appoggio */
	/* controllo numero di argomenti */
	if (argc<3) {
		zprintf(2,"Errore nel numero di argomenti\n");
		return(1);
	}
	M=argc-2;	/* numero di processi figli */
	K=atoi(argv[argc-1]);	/* numero K, ultimo argomento */
	if ((K<=0)||(K>=7)||((K%2)!=0)) {
		zprintf(2,"Parametro K non valido: %d\n",K);
		return(2);
	}
	/* creazione figli e attribuzione argomenti */
	for (i=0; i<M; i++) {
		switch (fork()) {
			case 0: /* figlio */
				return(figlio(argv[i+1],i));
			case -1:
				/* errore */
				zprintf(2,"Errore nella %d fork\n",i);
				return(3);
		}
	}
	/* recupero exit status dei figli (non dei nipoti) */
	for (i=0; i<M; i++) {
		pid=wait(&status);
		if (WIFEXITED(status)) {
			zprintf(1,
			"Il figlio con pid %d ritorna %d\n",
			pid, WEXITSTATUS(status));
		} else {
			zprintf(1,
			"Il figlio con pid %d e' terminato per il segnale %d\n",
			pid, WTERMSIG(status));
		}
	}
	return(0);
}
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
int figlio(char *filename, int i) {
	char ch;
	/* creo la pipe figlio-nipote */
	if (pipe(pfn)!=0) {
		zprintf(2,"Errore creazione pipe figlio-nipote %d\n",i);
		abort(); /* termino per SIGABRT, il padre se ne accorge */
		return(4);
	}
	/* creo il nipote */
	switch (fork()) {
		case 0:
			return(nipote(filename,i));
		case -1:
			/* errore */
			zprintf(2,"Errore nella %d fork (nipote)\n",i);
			abort(); 
	}
	close(pfn[1]); /* non scrivo mai */
	/* legge le linee 1,3,... evitando quelle pari */
	fdr=open(filename,O_RDONLY);
	if (fdr<0) {
		zprintf(2,"Impossibile aprire %s\n",filename);
		abort(); 
	}
	snprintf(FiK,sizeof(FiK),"%s%d",filename,K);
#ifdef DEBUG
	zprintf(1,"Creato %s\n",FiK);
#endif
	fdw=open(FiK,O_WRONLY|O_CREAT|O_EXCL,0666);
	if (fdw<0) {
		zprintf(2,"Impossibile aprire %s\n",FiK);
		abort(); 
	}
	lineacorr=1;
	indice=0;
	for(;;) {
		if (read(fdr,&ch,1)!=1) {
			return(nonscritte);
		}
#ifdef DEBUG1
		zprintf(1,"Char %d\n",ch);
#endif
		if (ch=='\n') {
			/* fine linea */
			if ((lineacorr%2)==1) {
				LD=indice;
				/* attendo info da nipote */
#ifdef DEBUG
				zprintf(1,"Attendo info per linea %d lung %d\n",
						lineacorr,LD);
#endif
				if (read(pfn[0],&LP,sizeof(LP))!=sizeof(LP)) {
					zprintf(2,"Errore lettura pipe\n");
					abort(); 
				}
				if (LD>LP) {
					write(fdw,linea,LD);
					write(fdw,"\n",1); /* fine linea */
				} else {
					nonscritte++;
				}
			}
			indice=0;
			lineacorr++;
		} else if ((lineacorr%2)==1) {
			/* linea dispari, memorizzo */
			if (indice<sizeof(linea)) {
				linea[indice++]=ch;
			}
		}
	}
	/* fine figlio, mai qui */
	abort(); 
	return(0);
}
int nipote(char *filename, int i) {
	char ch;
	/* nipote, linee pari */
	fdr=open(filename,O_RDONLY);
	if (fdr<0) {
		zprintf(2,"Impossibile aprire %s\n",filename);
		return(1); /* recuperabile con wait dal figlio */
	}
	lineacorr=1;
	indice=0;
	for(;;) {
		if (read(fdr,&ch,1)!=1) {
			/* fine file */
			return(0);
		}
		if (ch=='\n') {
			/* fine linea */
			if ((lineacorr%2)==0) {
				LP=indice;
				write(pfn[1],&LP,sizeof(LP));
#ifdef DEBUG
				zprintf(1,"Inviata lung linea %d, pari a %d\n",
						lineacorr,LP);
#endif
			}
			indice=0;
			lineacorr++;
		} else {
			indice++;
		}
	}
	return(0);
}
