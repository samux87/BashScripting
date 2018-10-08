
#Created by Samuele Buosi
#!/bin/sh

echo "Nome file C da creare , senza .c" ; read nomefile

nomefile=$nomefile.c

>$nomefile

chmod 711 $nomefile

echo "/* Created by Samuele Buosi */" >>$nomefile
echo "#include <stdio.h>" >>$nomefile
echo "#include <stdlib.h>" >>$nomefile
echo "#include <unistd.h>" >>$nomefile
echo "#include <sys/wait.h>" >>$nomefile
echo "#include <fcntl.h>" >>$nomefile
echo "#include <stdarg.h>" >>$nomefile
echo "#include <string.h>" >>$nomefile

echo 'typedef int pipe_t[2]; 	/* tipo per la pipe */' >>$nomefile
echo '/* Definisco le variabili globali */' >>$nomefile
echo 'pipe_t *pipes; 		/* pipes */' >>$nomefile
echo 'int     pid;		/* appoggio per valore pid (pid unico)*/' >>$nomefile
echo 'int     status; 	/* appoggio per wait */' >>$nomefile
echo 'int 	fd; 		/* per la gestione dei file creati  */' >>$nomefile
echo 'int nfigli,npipe;	/* numero di figli e numero di pipe da creare */' >>$nomefile

echo '/* abbinamento snprintf+write */' >>$nomefile
echo 'void zprintf(int fd, const char *fmt, ...);' >>$nomefile

echo "int main(int argc, char **argv) {" >>$nomefile

echo $'\t' 'int i;'>> $nomefile
#richiesta del numero minimo di argomenti
echo "Numero minimo di argomenti, 0 per nessun controllo"; read minargs



test $minargs -eq 0 2> /dev/null
if test $? -eq 2
then 
     echo Valore non Valido
     exit
fi 

if test $minargs -gt 0
then
	echo $'\t' '/* controllo numero di argomenti */'>> $nomefile
	echo $'\t' "if (argc<"$minargs+1") {" >> $nomefile
	echo $'\t' $'\t' 'zprintf(2,"Errore nel numero di argomenti\n");' >> $nomefile
	echo $'\t' $'\t' "return(1);" >> $nomefile
	echo $'\t'"}" >> $nomefile
	echo $'\t''/*CONTROLLO PARAMETRI*/' >>$nomefile
fi
	
echo "K?, [0,1]"; read risp
if [ $risp -gt 0 ]
then

    echo $'\t' 'K=atoi(argv[2]);' >> $nomefile
	echo $'\t' '/* Controllo K */' >> $nomefile
 	echo $'\t' 'if(K <= 0)' >> $nomefile
	echo $'\t' '{' >> $nomefile
    echo $'\t' $'\t' 'zprintf(2,"Errore, K non strettamente positivo\n");' >> $nomefile
    echo $'\t' $'\t' 'return 2;' >> $nomefile
	echo $'\t' '}' >> $nomefile
	
fi
		
echo "C?, [0,1]"; read risp
if [ $risp -gt 0 ]
then
		
	echo $'\t' '/* controllo che C sia un carattere */' >> $nomefile
	echo $'\t' 'C=argv[argc-1]; /* Ultimo Argomento */' >> $nomefile	
	echo $'\t' 'if(strlen(C)!=1)' >> $nomefile
	echo $'\t' '{' >> $nomefile
    echo $'\t' $'\t' 'zprintf(2,"Errore, C non carattere singolo\n");' >> $nomefile
    echo $'\t' $'\t' 'return 3;' >> $nomefile
	echo $'\t' '}' >> $nomefile

fi		

echo "fork()?, [0,1]"; read risp
if [ $risp -gt 0 ]
then
		echo $'\t' '/* Controllo su nfigli */' >> $nomefile
		echo $'\t' 'nfigli=atoi(argv[argc-1]);' >> $nomefile
        echo $'\t' 'if(nfigli</*Numero minimo*/)' >> $nomefile
		echo $'\t' '{' >> $nomefile
	    echo $'\t' $'\t' 'zprintf(2,"Errore, numero nfigli non corretto\n");' >> $nomefile
	    echo $'\t' $'\t' 'return 4;' >> $nomefile
		echo $'\t' '}' >> $nomefile
		
fi

echo "pipe?, [0,1]"; read risp
if [ $risp -gt 0 ]
then
		
		echo $'\t' '/* Allocazione di un array di pipe per la comunicazione fra figli */' >> $nomefile
		echo $'\t' 'npipe=nfigli-1;' >> $nomefile
		echo $'\t' 'pipes= (pipe_t*)malloc(sizeof(pipe_t)*npipe);' >> $nomefile
		echo $'\t' 'if(pipes==NULL)' >> $nomefile
        echo $'\t' '{' >> $nomefile
        echo $'\t' $'\t' 'zprintf(2,"errore nella malloc del vettore di pipe\n");' >> $nomefile
        echo $'\t' $'\t' 'return(5);' >> $nomefile
        echo $'\t' '}' >> $nomefile
	
	
	    echo $'\t' '/* creazione pipe */' >> $nomefile
	    echo $'\t' 'for (i=0; i<npipe; i++)' >> $nomefile
	    echo $'\t' '{/* creazione della pipe */' >> $nomefile
	    echo $'\t' $'\t' 'if (pipe(pipes[i])!=0)' >> $nomefile
	    echo $'\t' $'\t' '{/*in caso di errore*/' >> $nomefile
	    echo $'\t' $'\t' $'\t' 'zprintf(2,"Errore, pipe fallita in %d\n",i);' >> $nomefile
	    echo $'\t' $'\t' $'\t' 'return(6);' >> $nomefile
	    echo $'\t' '}' >> $nomefile
	    echo $'\t' '}' >> $nomefile

	    echo $'\t' '/*creazione figli*/' >> $nomefile
	    echo $'\t' 'for (i=0; i<nfigli; i++)' >> $nomefile
	    echo $'\t' '{' >> $nomefile
	    echo $'\t' $'\t' 'pid=fork();' >> $nomefile
	    echo $'\t' $'\t' 'switch(pid)' >> $nomefile
	    echo $'\t' $'\t' '{' >> $nomefile
	    echo $'\t' $'\t' $'\t' 'case 0: /* Figlio */' >> $nomefile
	  	echo $'\t' $'\t' $'\t' '/*la prima cosa che i figli devono fare è chiudere il lato della pipe che non usano*/' >> $nomefile
	    echo $'\t' $'\t' $'\t' 'for (j=0; j<npipe; j++)' >> $nomefile
	    echo $'\t' $'\t' $'\t' '{' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' 'if (j!=(i-1)) /*collegamento diretto --> j!=i*/' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' '{ /* figlio[i] scrive su pipe[i-1] */' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' $'\t' 'close(pipes[j][1]);' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' '}' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' 'if (j!=i) /*collegamento diretto --> j!=(i-1)*/' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' '{ /* e legge da pipe[i] */' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' $'\t' 'close(pipes[j][0]);' >> $nomefile
	    echo $'\t' $'\t' $'\t' $'\t' '}' >> $nomefile
	  	echo $'\t' $'\t' $'\t' '}' >> $nomefile
	  	echo $'\t' $'\t' $'\t' '/*richiama la funzione figlio per fare quel che deve*/' >> $nomefile
	  	echo $'\t' $'\t' $'\t' 'return (figlio(argv[i+1]));' >> $nomefile
	    echo $'\t' $'\t' $'\t' 'case -1:' >> $nomefile
	  	echo $'\t' $'\t' $'\t' $'\t' 'zprintf(2,"Errore nella %d fork\n",i);' >> $nomefile
	   	echo $'\t' $'\t' $'\t' $'\t' 'return(7);' >> $nomefile
	    echo $'\t' $'\t' '}' >> $nomefile
	    echo $'\t' '}' >> $nomefile
	
	
	    echo $'\t' '/* PADRE */' >> $nomefile
	    echo $'\t' 'for (i=0; i<npipe; i++)' >> $nomefile
	    echo $'\t' '{ /* per tutte le pipe */' >> $nomefile
	    echo $'\t' $'\t' ' close(pipes[i][0]); /* il padre non legge, chiudo il fd di lettura*/' >> $nomefile
	    echo $'\t' $'\t' ' close(pipes[i][1]); /* e non scrive, chiudo il fd di scrittura */' >> $nomefile
	    echo $'\t' '}' >> $nomefile
	    echo $'\t' '/*attende i figli per recuperare gli exit value*/' >> $nomefile
	    echo $'\t' 'for (i=0; i<nfigli; i++){' >> $nomefile
		echo $'\t' $'\t' 'pid=wait(&status);' >> $nomefile
    	echo $'\t' $'\t' '/*attesa figli con recupero variabile di ritorno e controllo errore*/' >> $nomefile
		echo $'\t' $'\t' 'if (WIFEXITED(status) == 0) {' >> $nomefile
        echo $'\t' $'\t' $'\t' 'zprintf(1,"Figlio con pid: %d terminato in modo anomalo\n", pid);' >> $nomefile
        echo $'\t' $'\t' '} else {' >> $nomefile
        echo $'\t' $'\t' $'\t' 'zprintf(1,"Per il figlio con pid %d lo stato di EXIT e %d\n",pid,WEXITSTATUS(status));' >> $nomefile
        echo $'\t' $'\t' '}' >> $nomefile
		echo $'\t' '}' >> $nomefile

fi		
		
echo $'\t' "return 0;" >>$nomefile
echo "}" >>$nomefile

# Funzione figlio
echo "Funzione Figlio?, [0,1]"; read risp
if [ $risp -gt 0 ]
then
    echo '/* Figlio */' >>$nomefile
    echo 'int figlio(char *file, int indice) {' >>$nomefile

    echo '}' >>$nomefile

fi

echo 'void zprintf(int fd, const char *fmt, ...) {' >>$nomefile
echo $'\t' '/* printf wrapper using write instead */' >>$nomefile
echo $'\t' 'static char msg[256];' >>$nomefile
echo $'\t' 'va_list ap;' >>$nomefile
echo $'\t' 'int n;' >>$nomefile
echo $'\t' 'va_start(ap, fmt);' >>$nomefile
echo $'\t' 'n=vsnprintf (msg, 256, fmt, ap);' >>$nomefile
echo $'\t' 'write(fd,msg,n);' >>$nomefile
echo $'\t' 'va_end(ap);' >>$nomefile
echo '}' >>$nomefile




