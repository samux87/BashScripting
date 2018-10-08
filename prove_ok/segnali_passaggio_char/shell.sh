#Created by Samuele Buosi
#!/bin/sh
lista="";
#Controllo numero parametri
case $# in
#Controllo primo parametro
2) case $1 in
	 /*) if test ! -d $1 -o ! -x $1
	 	 then
	 	 echo $1 non direttorio
	 	 exit 1
	 	 fi;;
	 *)  echo $1 non nome assoluto; exit 2;;
	 esac;;
*) 	echo "Uso: $0 G K"
	 exit 3;;
esac
# Assegno nomi ai parametri
G=$1
K=$2
# Controllo se K numerico
expr "$K" + 0 > /dev/null 2>&1
if [ $? -eq 2 -o $? -eq 3 ]
then echo K non numerico: "$K"; exit 7
fi
# Controllo se K positivo
if [ $K -le 0 ]
then echo K non Strettamente Positivo: "$K"; exit 8
fi
# Predisposizione ricorsione
PATH=$(pwd):$PATH
export PATH
#creo file temporaneo
> /tmp/tempfile
chmod 777 /tmp/tempfile
# richiamo script risorsivo
shellR.sh $G $K
conta=$?
echo Conta:$conta
n=`expr $conta % 2`
echo n:$n
#controllo numero di file trovati e chiamata alla parte C
if [ $conta -ne 0 -a $n -eq 0 ]
then
	# Inserisco tutti i path dei file trovati in una lista
	for i in `cat /tmp/tempfile`
	do
		lista="$lista $i"
	done
	
	# Richiamo parteC con la lista dei file
	./parteC $lista
fi
