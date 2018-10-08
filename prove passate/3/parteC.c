#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdarg.h>

// Struttura da passare con le pipe
typedef struct{
	int pid;
	int num;
} valore;

typedef int pipe_t[2];
char msg[256];
char ch;
pipe_t *pipes;
valore v,v2;
int nfigli;

/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...);

int figlio(char *Fi, int arg, int ultimo);

int main(int argc, char **argv) {
	int npipe;
	pid_t pid;
	int i,j;
	char status;
	
	nfigli=argc-1;
	
	pipes = malloc(sizeof(pipe_t)*nfigli);
	if (pipes==NULL) { /* controllo malloc */
		snprintf(msg,sizeof(msg),
			"Allocazione fallita\n");
		write(2,msg,strlen(msg));
		return(1);
	}

	for (i=0; i<nfigli; i++) { /* per ogni pipe */
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
				for (j=0; j<nfigli; j++) {
					if (j!=i-1) { /* figlio[i] scrive su pipe[i-1] */
						close(pipes[j][1]);
					}
					if (j!=(i)) { /* e legge da pipe[i] */
						close(pipes[j][0]);
					}
				}
				return(figlio(argv[i+1],i,nfigli-1));
			case -1:
				snprintf(msg,sizeof(msg),
					"fork() fallita in %d\n",i);
				write(2,msg,strlen(msg));
				return(1);
		}
	}

	/* solo il padre arriva qui */
	for (i=0; i<nfigli; i++) { /* per tutte le pipe */
		if(i!=0) close(pipes[i][0]); /* il padre non legge, chiudo il fd di lett tranne figlio 0 */
		close(pipes[i][1]); /* e non scrive, chiudo il fd di scrittura */
	}
	
	/* leggo i valori passati dal primo figlio fino a che ce ne sono */
	while (read(pipes[0][0],&v,sizeof(valore))) {
		zprintf(1,"\nNumero calcolato: %d dal figlio pid=%d\n",v.num, v.pid);    
	} 

	/* Attesa della terminazione dei figli */
	for(i=0;i<nfigli;i++) {
		pid = wait(&status);
		if(WIFEXITED(status)) /* se il figlio e` uscito normalmente... */
			zprintf(1,"Figlio pid: %d terminato con exit: %c\n", pid, WEXITSTATUS(status));
		else
			zprintf(1,"Figlio con pid: %d terminato in modo anomalo\n", pid);
	}
	return(0);
}

/* funzione figlio */
int figlio(char *Fi, int arg, int ultimo) {
	int fd;

	// Apro il file i-esimo e leggo un carattere alla volta e lo converto
	fd=open(Fi, O_RDONLY);
	if(fd < 0){
		printf("Errore nell'apertura del file %s\n", Fi);
		return(1);
	}
	
	while (read(fd,&ch,1)) { 
				/* inizializzo la mia struttura dati: ATTENZIONE farlo dentro al while datro che puÚ essere modificata */
				v.pid = getpid();
				v.num=(int) (ch - '0');
				printf("ho letto numero=%d e pid=%d\n", v.num, v.pid);
				/* ora posso comunicare al figlio seguente la mia informazione. ATTENZIONE: il figlio N-1esimo non deve leggere da nessuna pipe, ma gli altri figli devono prima confrontare la loro informazione con quella ricevuta sulla pipe */
				if (arg != nfigli-1) {
					/* se non sono l'ultimo processo, resto bloccato sulla read fino a quando non mi viene passato un qualche dato */
					read(pipes[arg+1][0],&v2,sizeof(valore));
				   	/* confronto i dati ricevuti */
				    zprintf(1,"ho ricevuto numero=%d e pid=%d\n", v2.num, v2.pid);
				   	if (v2.num >= v.num){
				   	v.num = v2.num;
				   	v.pid = v2.pid;
				   						}
				   	}
				   	/* invio i dati */
				    zprintf(1,"sto per inviare numero=%d e pid=%d\n", v.num, v.pid);
				   	write(pipes[arg][1],&v,sizeof(valore));
	}
	return(ch);
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
