#Created by Samuele Buosi
#!/bin/sh

#Controllo numero parametri
case $# in
#Controllo primo parametro
3) case $1 in
	 /*) if test ! -d $1 -o ! -x $1
	 	 then
	 	 echo $1 non direttorio
	 	 exit 1
	 	 fi;;
	 *)  echo $1 non nome assoluto; exit 2;;
	 esac;;
*) 	echo "Uso: $0 G K Y"
	 exit 3;;
esac

# Assegno nomi ai parametri
G=$1
K=$2
Y=$3

# Controllo se K numerico
expr "$K" + 0 > /dev/null 2>&1
if [ $? -eq 2 -o $? -eq 3 ]
then echo K non numerico: "$K"; exit 7
fi

# Controllo se K positivo >= 2
if [ $K -ge 2 ]
then echo K non >= 2: "$K"; exit 8
fi

# Predisposizione ricorsione
PATH=$(pwd):$PATH
export PATH

# richiamo script risorsivo
shellR.sh $G $K $Y
