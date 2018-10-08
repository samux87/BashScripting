/* file: es114.c
 * job: sincronizzazione con segnali
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
void sigfun(int sigid);
int figlio(void);
volatile int consenso;
char msg[256];
int main(int argc, char **argv) {
	pid_t pidfiglio;
	
	/* colleghiamo SIGUSR1 alla nostra sigfun */
	signal(SIGUSR1,sigfun);
	consenso=0; /* consenso assente */
	switch(pidfiglio=fork()) {
		case 0: /* figlio */
			return(figlio());
		case -1: /* errore */
			snprintf(msg,sizeof(msg),
				"Errore fork()\n");
			write(2,msg,strlen(msg));
			return(1);
	}
	/* padre */
	consenso=1; /* il padre inizia */
	for (;;) {
		snprintf(msg,sizeof(msg),"Padre\n");
		while(!consenso) pause();
		write(1,msg,strlen(msg));
		sleep(1); /* rallentatore! */
		consenso=0; /* mi blocco */
		kill(pidfiglio,SIGUSR1); /* sblocco */
	}
	return(0);
}	
int figlio() {
	for (;;) {
		snprintf(msg,sizeof(msg),"Figlio\n");
		while(!consenso) pause();
		write(1,msg,strlen(msg));
		sleep(1); /* rallentatore! */
		consenso=0; /* mi blocco */
		kill(getppid(),SIGUSR1); /* sblocco */
	}
	return(0);
}
void sigfun(int sigid) {
	if (sigid==SIGUSR1) {
		consenso=1;
	}
}

