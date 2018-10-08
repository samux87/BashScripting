#!/bin/sh

#controllo numero parametri: si devono passare N+1 con N >= 2
if test $# -lt 3
then 
        echo "Errore numero parametri: devono essere almeno 3 (gerarchia e almeno 2 nomi di file)"
	exit 1
fi

#controllo primo argomento
case $1 in
        /*) if test ! -d $1 -o ! -x $1
            then echo Non direttorio o non eseguibile; exit 1
            fi;;
        *) echo Nome direttorio non assoluto; exit 2;;
esac
G=$1

#elimino il primo parametro per avere solo i nomi di file e quindi calcolarne la metà
shift
N=$#
echo Il numero di nomi di file passati uguale a $N
M=`expr $N / 2`
echo Si devono trovare almeno $M nomi di file 
#controllo su nomie relativi degli altri N parametri 
for i in $*
do
	case $i in
        */*) echo Nome file $i non relativo semplice; exit 3;; 
        *);; 
	esac
done

PATH=`pwd`:$PATH
export PATH

#chiamiamo la parte ricorsiva passando anche il numero minimo di nomi di file
17Lug13R.sh $G $M $*

