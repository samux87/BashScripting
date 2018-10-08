#Created by Samuele Buosi
#!/bin/sh
#Controllo numero parametri
if [ $# -lt 3 ]
then
	echo "Uso: $0 G F1 F2 ... FN"
	exit 1;
fi

#Controllo primo parametro
case $1 in
	 /*) if test ! -d $1 -o ! -x $1
	 	 then
	 	 echo $1 non direttorio
	 	 exit 2
	 	 fi;;
	 *)  echo $1 non nome assoluto; exit 3;;
esac
 
# Assegno nomi ai parametri
G=$1

shift
N=$#
echo "Numero di File passati $N"
M=`expr $N / 2`
for i in $*
do
	case $i in
		*/*) echo "Errore: nome file non relativo semplice\n"; exit 4;;
		*) ;;
	esac
done


# Predisposizione ricorsione
PATH=$(pwd):$PATH
export PATH

# richiamo script risorsivo
shellR.sh $G $M $*

