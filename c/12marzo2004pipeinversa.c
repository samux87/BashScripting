/* file: es103.c
 * job: inversione d'ordine su es102.c
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
#define SCRIT 1
#define LETT 0

char *F; /* Nome del file, variabile globale */
int N; /* Numero di caratteri */
char *C; /* Elenco dei caratteri, allocazione dinamica */
int Npipe; /* Numero di pipe da creare */
pipe_t *Pfds; /* Array di pipe, allocazione dinamica */
int Nfigli; /* Numero di figli da forkare */
int *Pids; /* Array di pid, allocazione dinamica */

/* abbinamento snprintf+write */
void zprintf(int fd, const char *fmt, ...);
/* funzione figlio */
int figlio(int);

char ok='Q'; /* carattere (casuale) che inviamo sulla pipe
	    per la sincronizzazione */

int main(int argc, char **argv, char **envp) {
	int i,j; /* qualche contatore */

	/* Determinazione dei dati dalla command line */
	switch (argc) {
		case 0:
		case 1:
		case 2:
			zprintf(2,"Inserire il nome di file ed almeno un carattere\n");
			return(1);
	}
	F = argv[1];
	N = argc-2; /* argc vale 1 se non ci sono argomenti */
	C = malloc(N*sizeof(char));
	if (C==NULL) {
		zprintf(2,"Impossibile allocare C\n");
		return(1);
	}
	for (i=0; i<N; i++) {
		C[i]=argv[i+2][0]; /* prendo il primo carattere della stringa
				      argv[i+2] */
	}
#ifdef DEBUG
	zprintf(1,"Nome del file: %s\n",F);
	zprintf(1,"Numero di caratteri %d\n",N);
	for (i=0; i<N; i++) {
		zprintf(1,"Carattere di indice %d = %c\n",i,C[i]);
	}
#endif
	/* Predisposizione del sistema di sincronizzazione
	 * fra i processi figli: si utilizza una pipe per
	 * collegare ogni figlio al successivo; la sincronizzazione
	 * del figlio di indice 'k' si ottiene attendendo un
	 * carattere dalla pipe che lo collega al figlio di
	 * indice 'k-1'. */
	Npipe = N; /* una pipe in uscita da ogni figlio */
	Pfds = malloc(Npipe*sizeof(pipe_t));
	if (Pfds==NULL) {
		zprintf(2,"Impossibile allocare Pfds\n");
		return(1);
	}
	for (i=0; i<Npipe; i++) {
		if (pipe(Pfds[i])!=0) {
			zprintf(2,"Impossibile creare la pipe %d\n",i);
			return(1);
		}
	}
	/* Creazione processi figli:
	 * dal testo, bisogna creare un processo figlio per ogni carattere
	 * quindi Nfigli=N
	 * Visto che il padre deve stampare i valori di uscita dei processi
	 * figli, si devono memorizzare i pid in ordine di indice per poi
	 * abbinare i risultati delle wait. L'array di pid deve essere
	 * allocato dinamicamente */
	Nfigli = N;
	Pids=malloc(Nfigli*sizeof(int));
	if (Pids==NULL) {
		zprintf(2,"Impossibile allocare Pids\n");
		return(1);
	}
	for (i=0; i<Nfigli; i++) {
		Pids[i]=fork();
		switch (Pids[i]) {
			case 0: /* figlio di indice i */
				return(figlio(i)); /* chiamo funzione figlio
				e uso valore di ritorno come exit del processo*/
			case -1:
				zprintf(2,"Impossibile forkare il processo %d\n",i);
				return(1);
				break;
		}
#ifdef DEBUG
		/* solo il processo padre esegue queste linee */
		zprintf(1,"Forkato processo %d per indice %d\n",Pids[i],i);
#endif
	}
	/* Il padre non utilizza nessuna pipe in quando la sincronizzazione
	 * viene fatta totalmente fra i figli. Eventualmente (ma ci sono
	 * anche altri modi) il padre puo' dare il via al primo figlio.
	 * Utilizziamo questa possibilita' ed utilizziamo un singolo
	 * carattere memorizzato in ok per l'invio */
	if(write(Pfds[0][SCRIT],&ok,1)!=1) {
		/* controllo di scrittura avvenuta */
		zprintf(2,"Impossibile inviare su %d\n",Pfds[0][SCRIT]);
		return(1);
	}
	/* ora chiudiamo tutte le pipe inutilizzate dal padre, compresa
	 * quella in cui abbiamo appena scritto (il carattere rimane!) */
	for (i=0; i<Npipe; i++) {
		close(Pfds[i][SCRIT]);
		close(Pfds[i][LETT]);
	}
	/* Recupero delle informazioni dai processi figli tramite la wait */
	for (i=0; i<Nfigli; i++) {
		int pid_x;
		int status;

		pid_x = wait(&status);
		/* abbinamento delle informazioni all'indice del figlio */
		for (j=0; j<Nfigli; j++) {
			if (pid_x==Pids[j]) {
				if (WEXITSTATUS(status)==255) {
					/* convenzione: segnalazione d'errore */
					zprintf(1,
			"Il figlio che cerca il carattere %c ha incontrato un errore\n",
					C[j]);
				} else {
					zprintf(1,
			"Il figlio che cerca il carattere %c ha trovato %d linee\n",
					C[j],WEXITSTATUS(status));
				}
			}
		}
	}
	return(0);
}
			
/* Funzione figlio */
int figlio(int indice) {
	/* Ogni figlio deve cercare le occorrenze di C[i] nel file F,
	 * quindi il file NON deve essere condiviso ma aperto indipendentemente
	 * da ogni figlio */
	int fd;
	int linenum; /* contatore di linea corrente */
	char ch; /* carattere di appoggio */
	int retval; /* valore di ritorno */
	int nr; /* numero di caratteri letti dalla read */
	int flag; /* se 0 carattere non trovato, se 1 trovato */
	int nrp; /* numero di car letti da read su pipe */
	char dummy; /* carattere di appoggio per lett/scritt pipe */
	int i; /* un contatore */
	int pipe_di_sinc; /* indice della pipe di sincronizzazione da
	cui fare la read */
	
	/* Chiusura dei lati delle pipe per evitare il deadlock quando
	 * un figlio termina prima degli altri e non puo' proseguire
	 * nell'attivita' di sincronizzazione. Se almeno i lati
	 * di scrittura sono posseduti ognuno da un solo processo
	 * la terminazione di questo provoca un comportamento non
	 * bloccante per la read */
	for (i=0; i<Npipe; i++) {
		/* chiudo i lati di scrittura se non e' la pipe
		 * che usa questo figlio */
		if (i!=(indice)) {
			close(Pfds[i][SCRIT]);
		}
	}
		
	fd=open(F,O_RDONLY);
	if (fd<0) {
#ifdef DEBUG
		zprintf(2,"Impossibile aprire il file %s\n",F);
#endif
		return(-1);
	}
	/* il file deve essere letto per linee */
	linenum=1; /* prima linea */
	retval=0; /* linee che soddisfano=0 */
	flag=0;
	/* sincronizzazione 'estesa' in grado di assicurarci l'alternanza delle
	 * informazioni anche in seguito alla terminazione di uno o piu' figli
	 * si usa una variabile per memorizzare l'indice della pipe da cui 
	 * attendere l'ok. Se la read da tale indice restituisce 0 -> allora il 
	 * figlio relativo e' terminato e bisogna 'agganciarsi' al precedente.
	 * Nel caso estremo in cui un solo processo figlio rimanga in essere,
	 * la pipe di lettura e quella di scrittura vanno a coincidere 
	 * sbloccando continuamente l'output di tale processo */
	pipe_di_sinc = (indice+1)%Npipe; /* iniziamo con quella 'canonica' */
	for (;;) {
		nr=read(fd,&ch,1);
		if (nr==0) {
			return(retval);
		}
		if (nr<0) {
			/* errore */
#ifdef DEBUG
			zprintf(2,"Impossibile leggere dal file %s\n",F);
#endif
			return(-1);
		}
		/* carattere letto, controllo se fine linea */
		if (ch=='\n') {
			linenum++;
			flag=0;
		}
		/* dal testo: se trovo il carattere assegnato C[indice], devo riportare
		 * il numero di linea su stdout. Se il carattere si trova piu'
		 * volte nella stessa linea devo riportare il numero solo
		 * una volta, quindi uso un flag che controllo a fine linea */
		if ((ch==C[indice])&&flag==0) {
			flag=1;
			retval++; /* incremento contatore */
			/* aggiungo sincronizzazione, attendo carattere da pipe
			 * con indice pipe_di_sinc e gestisco l'eventuale
			 * terminazione del figlio associato */
			for (;;) {
				nrp=read(Pfds[pipe_di_sinc][LETT],&dummy,1);
				if (nrp==0) {
					/* pipe chiusa, agganciamoci al figlio successivo */
					if (pipe_di_sinc<(Npipe-1)) {
						pipe_di_sinc++;
					} else {
						pipe_di_sinc=0;
					}
					continue;
				}
				if (nrp==-1) {
					/* errore, segnalare */
#ifdef DEBUG
					zprintf(2,"Errore pipe di sincronizzazione\n");
#endif
					return(-1);
				}
				break;
			}
			/* se sono qui posso scrivere */
			zprintf(1,"Linea %d contiene %c\n",linenum,C[indice]);
			/* ora do il via al figlio che deve eseguire l'operazione
			 * dopo quello corrente, ovvero il figlio avente indice minore
			 * di 1 (o l'ultimo nel caso del figlio con indice==0): 
			 * l'indice della pipe in cui scrivere e' uguale all'indice 
			 * del figlio */
			write(Pfds[indice][SCRIT],&dummy,1);
		}
	}
	/* mai qui! */
	return (-1);
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
