/* file: es95.c
 * job: pipe fra processi
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
typedef int pipe_t[2]; /* tipo per pipe */
char msg[256];
char ch; /* carattere da inviare */
int intero; /* intero da inviare */
struct {
	int unint;
	char unchar;
	char string[16];
} struttura; /* struttura da inviare */
pipe_t pfd; /* contenitore per i 2 fd */
int figlio(void); /* funzione figlio */
int main(int argc, char **argv) {
	if (pipe(pfd)!=0) {
		write(2,"Errore in pipe()\n",17);
		return(1);
	}
	switch (fork()) {
		case 0: /* figlio */
			return(figlio());
		case -1:
			break;
	}
	/* solo il padre arriva qui */
	ch='A';
	if (write(pfd[1],&ch,sizeof(ch))!=sizeof(ch)) {
		snprintf(msg,sizeof(msg),"Errore invio ch\n");
		write(2,msg,strlen(msg));
	}
	intero=1250;
	if (write(pfd[1],&intero,sizeof(intero))!=sizeof(intero)) {
		snprintf(msg,sizeof(msg),"Errore invio intero\n");
		write(2,msg,strlen(msg));
	}
	struttura.unchar='B';
	struttura.unint=334;
	strncpy(struttura.string,"Ciao Mondo!",sizeof(struttura.string));
	if (write(pfd[1],&struttura,sizeof(struttura))!=sizeof(struttura)) {
		snprintf(msg,sizeof(msg),"Errore invio struttura\n");
		write(2,msg,strlen(msg));
	}
	return(0);
}
int figlio() {
	int nr;
	/* chiudo il lato di scrittura */
	close(pfd[1]); /* cosa succede se non lo faccio?*/
	nr=read(pfd[0],&ch,sizeof(ch));
	if (nr==sizeof(ch)) {
		snprintf(msg,sizeof(msg),
			"Ricevo il char %c\n",ch);
		write(1,msg,strlen(msg));
	} else {
		snprintf(msg,sizeof(msg),
			"Attendendo ch, ottengo %d\n",nr);
		write(2,msg,strlen(msg));
	}
	nr=read(pfd[0],&intero,sizeof(intero));
	if (nr==sizeof(intero)) {
		snprintf(msg,sizeof(msg),
			"Ricevo l'intero %d\n",intero);
		write(1,msg,strlen(msg));
	} else {
		snprintf(msg,sizeof(msg),
			"Attendendo intero, ottengo %d\n",nr);
		write(2,msg,strlen(msg));
	}
	nr=read(pfd[0],&struttura,sizeof(struttura));
	if (nr==sizeof(struttura)) {
		snprintf(msg,sizeof(msg),
			"Ricevo la struttura:\n%c\n%d\n%s\n",
			struttura.unchar,
			struttura.unint,
			struttura.string);
		write(1,msg,strlen(msg));
	} else {
		snprintf(msg,sizeof(msg),
			"Attendendo struttura, ottengo %d\n",nr);
		write(2,msg,strlen(msg));
	}
	nr=read(pfd[0],&ch,1);
	if (nr==0) {
		snprintf(msg,sizeof(msg),
			"Ora la pipe e' chiusa\n");
		write(1,msg,strlen(msg));
	}
	return(0);
}

