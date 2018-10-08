#!/bin/sh
#entro nel direttorio
cd $1
#il secondo parametro è il numero minimo di nomi di file da trovare
M=$2

#elimino i primi due parametri
shift 2
#azzero la variabile che serve per il conteggio dei file  
F=0
#variabile che serve per memorizzare i nomi dei file trovati per poi passarli alla parte C
files=

for i in $* #per tutti i nomi dei file passati
do
#per ogni nome di file lo cerchiamo
        if test -f $i 
        then
		#aggiorniamo il contatore 
		F=`expr $F + 1`
		files="$files $i"
        fi
done
                 
if test $F -ge $M 
then
#abbiamo trovato un direttorio giusto
#stampiamo il nome su standard output
       echo Trovato direttorio giusto `pwd` 
#inseriamo il nome nel file temporaneo
       echo "Chiamiamo 17Lug13 $files"
       17Lug13 $files
fi

#chiamata ricorsiva
for i in *
do
        if test -d $i -a -x $i
        then 
		#chiamata ricorsiva
		echo chiamo $0 `pwd`/$i $M $* 
		$0 `pwd`/$i $M $*
        fi
done

