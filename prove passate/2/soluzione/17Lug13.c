#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
/*Esame 17 Luglio 2013*/

/*Tipo di Dato pipe*/
typedef int pipe_t[2];
/*Dichiarazione Esplicita Funzione Figlio*/
int figlio(char *Fi,int indice);

/*Variabili Globali*/
int M; /*Numero di file passati come parametri*/
int retval;
pipe_t *pfd_p;
pipe_t *pfd_f;
/*NOTA:Si utilizzano 2M pipe !!*/
/*M pipe pfd_p per la comunicazione figli-padre*/
/*M pipe pfd_f per la comunicazione padre-figli*/
char str[256]; /*Stringa di appoggio per scrittura su stderror e stdout*/
long int cont_ch; /*Variabile long int per conteggio occorrenze */
char ch; /* variabile per contenere i caratteri forniti dall'utente */

int main(int argc,char **argv )
{
/*Variabili Locali*/
int i; /* variabile per i cicli */
int pid; /*PID per creazione figli*/
int status; /* variabile per wait */

/*Controllo numero parametri: almeno un file */
if (argc < 2)
{
        snprintf(str,256,"USO F1 F2 FM \n");
        write(2,str,strlen(str));
        return(1);
}

/*Assegno Numero di figli*/
M=argc-1;
/*Allocazione dinamica pipe pfd_p*/
pfd_p=(pipe_t*)malloc(sizeof(pipe_t)*M);
if (pfd_p==NULL)
{
        snprintf(str,256,"Errore allocazione pipe pfd_p \n ");
        write(2,str,strlen(str));
        return(2);
}
/*Creazione Pipe pfd_p*/
for (i=0;i<M;i++)
{
        retval=pipe(pfd_p[i]);
        if (retval!=0)
	{
                snprintf(str,256,"Errore nella creazione della pipe pfd_p %d \n ",i);
                write(2,str,strlen(str));
                return(3);
         }
}
/*Allocazione dinamica pipe pfd_f*/
pfd_f=(pipe_t*)malloc(sizeof(pipe_t)*M);
if (pfd_f==NULL)
{
        snprintf(str,256,"Errore allocazione pipe pfd_f \n ");
        write(2,str,strlen(str));
        return(4);
}
/*Creazione Pipe pfd_f*/
for (i=0;i<M;i++)
{
        retval=pipe(pfd_f[i]);
        if (retval!=0)
	{
                snprintf(str,256,"Errore nella creazione della pipe pfd_f %d \n ",i);
                write(2,str,strlen(str));
                return(5);
        }
}
/*Creazione figli*/
for (i=0;i<M;i++)
{
        pid=fork();
        switch(pid)
	{
                   case 0: /*Figlio*/
                           return (figlio(argv[i+1],i));

                   case -1:/*Errore*/
                           snprintf(str,256,"Errore creazione figlio numero %d PID %d \n ",i,pid);
                           write(2,str,strlen(str));
                           return(6);
        }
}	
/*Se arrivo qui sono il padre*/
/*Chiudo i lati delle pipe che il padre non usa*/
/*Il padre scrive solo sulle pipe pfd_f*/
/*Il padre legge solo dalle pipe pfd_p*/

for (i=0;i<M;i++)
{
        close(pfd_p[i][1]);
        close(pfd_f[i][0]);
}	

/* facciamo una sleep per fare in modo che quando il padre arriva alla read dalle pipe i figli siano già partiti e la read sia bloccante 
sleep(10);
*/
snprintf(str,256,"Inserisci i caratteri che i figli devono cercare nei file\n ");
write(1,str,strlen(str));

while (read(0, &ch, 1))
{
/*
        snprintf(str,256,"Letto carattere %c \n ",ch);
        write(1,str,strlen(str));
*/
	/*Per ogni figlio, il padre manda il carattere letto */
	for (i=0;i<M;i++)
       		write(pfd_f[i][1],&ch, sizeof(ch));
	for (i=0;i<M;i++)
	{
 	/*Leggo il valore dalla pipe pfd_p*/
       		read(pfd_p[i][0],&cont_ch,sizeof(cont_ch));
        	snprintf(str,256,"Il figlio %d ha contato %ld occorrenze del carattere %c nel file %s\n ",i,cont_ch, ch,argv[i+1]);
        	write(1,str,strlen(str));
	}
}
/*Chiudo i lati di scrittura delle pipe in modo che i figli si sblocchino dalle
letture ie già che ci siamo chiudiamo anche i
lati di lettura, tanto non servono più */
for (i=0;i<M;i++)
{
        close(pfd_p[i][0]);
        close(pfd_f[i][1]);
}	

/*Stampo i valori di uscita dei figli*/
for (i=0;i<M;i++)
{
        /*Attendo i figli e stampo il valori di uscita*/
        pid=wait(&status);
	if (WIFEXITED(status) == 0)
		printf("Figlio con pid: %d terminato in modo anomalo\n", pid);
    	else
    	{
        	snprintf(str,256,"Figlio con PID %d RITORNA %d \n",pid,WEXITSTATUS(status));
        	write(1,str,strlen(str));
	}	
}	
return(0);
}

int figlio(char *Fi,int indice)
{
/*Ogni figlio e' associato a un file Fi*/
/*Ogni figlio deve calcolare il numero di occorrenze del carattere ricevuto dal padre e inviarlo al padre */
/*Riceve il carattere comunicato dal padre su pfd_f[indice]*/
/*Comunica il numero di occorrezne al processo padre su pfd_p[indice]*/

int i;/*Indice generico*/
int fd;/*File descriptor file Fi*/
char chr; /* Variabile per leggere dal file */

/*Stampa di Controllo*/
snprintf(str,256,"Figlio INDICE %d FILE %s \n",indice,Fi);
write(1,str,strlen(str));
/*Chiusura lati pipe non utilizzati pfd_f*/
/*Ogni figlio usa solo la pipe pfd_f[indice]*/
for (i=0;i<M;i++)
	{
        if (i!=indice)
		{		
                close(pfd_f[i][0]);
                close(pfd_f[i][1]);
        	}
	}

/* Leggo SOLO dalla mia pipe pfd_f*/
close(pfd_f[indice][1]);

/*Chiusura lati pipe non utilizzati pfd_p*/
/*Ogni figlio usa solo la pipe pfd_f[indice], in lettura */
for (i=0;i<M;i++)
        {
        close(pfd_f[i][1]);
        if (i!=indice)
                close(pfd_f[i][0]);
        }

/*Chiusura lati pipe non utilizzati pfd_p*/
/*Ogni figlio usa solo la pipe pfd_p[indice], in scrittura */
for (i=0;i<M;i++)
        {
        close(pfd_p[i][0]);
        if (i!=indice)
                close(pfd_p[i][1]);
        }

/* se non viene mai trovata un'occorrenza dei caratteri passati dal padre, retval rimane a zero */
retval=0;

while(read(pfd_f[indice][0], &ch, 1))
{
/*
        snprintf(str,256,"Figlio INDICE %d ricevuto da padre %c \n",indice,ch);
        write(1,str,strlen(str));
*/
	/*Inizializzo variabile contatore caratteri*/
	cont_ch=0;
	/*Scansione file fd per conteggio caratteri*/
	while(read(fd,&chr,1)>0)
	{
        /*
	snprintf(str,256,"Figlio INDICE %d letto %c \n",indice,chr);
        write(1,str,strlen(str));
	*/
        /*Ho letto un carattere*/
	/* controllo se è quello cercato */
	if (chr == ch)
		{
        	/*Incremento contatore*/
        	cont_ch++;
        	/*setto la variabile retval */
		retval=1;
		}
	}
 /*
        snprintf(str,256,"Figlio INDICE %d calcolato %ld\n",indice,cont_ch);
        write(1,str,strlen(str));
*/
	/*Comunico al padre il numero di occorrenze */
	write(pfd_p[indice][1],&cont_ch,sizeof(long int));
	/* adesso dobbiamo fare un lseek all'inizio per la prossima ricerca */
	lseek(fd, 0L, 0); 
}
/*Ritorno retval al padre*/
return(retval);
}
