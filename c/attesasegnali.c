/* file: es113.c
 * job: attesa temporizzata
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <setjmp.h>
void sigfun(int sigid);
char msg[256];
jmp_buf stato; /* salvataggio stato */
int main(int argc, char **argv) {
	/* colleghiamo SIGALRM alla nostra sigfun */
	signal(SIGALRM,sigfun);
	if (setjmp(stato)) {
		/* se non zero, allora torno */
		snprintf(msg,sizeof(msg),
			"Attesa scaduta\n");
		write(1,msg,strlen(msg));
	} else {
		/* se zero, primo passaggio */
		snprintf(msg,sizeof(msg),
			"Attesa iniziata\n");
		write(1,msg,strlen(msg));
		alarm(10);
		snprintf(msg,sizeof(msg),
			"Sveglia impostata\n");
		write(1,msg,strlen(msg));
		for (;;) {
			pause();
		}
	}
	return(0);
}	
void sigfun(int sigid) {
	if (sigid==SIGALRM) {
		longjmp(stato,1);
	}
}

