#Created by Samuele Buosi
#!/bin/sh
#Controllo numero parametri
if [ $# -ne 3 ]
then
 	echo "Uso: $0 G1 G2 K"
	exit 1
fi

# verifico primo percorso
case $1 in
	 /*) if test ! -d $1 -o ! -x $1
	 	 then
	 	 echo $1 non direttorio
	 	 exit 2
	 	 fi;;
	 *)  echo $1 non nome assoluto; exit 3;;
esac
# verifico secondo percorso
case $2 in
	/*) if test ! -d $2 -o ! -x $2
	 	then
	 	 	echo $2 non direttorio
	 	 	exit 4
	 	fi;;
	*)  echo $2 non nome assoluto; exit 5;;
esac
# Assegno nomi ai parametri
G1=$1
G2=$2
K=$3
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
#creo i file temporanei
> /tmp/tempfile1
> /tmp/tempfile2
chmod 711 /tmp/tempfile1
chmod 711 /tmp/tempfile2
# richiamo script risorsivo per G1 e tengo traccia dei num file trovati
./shellR.sh $G1 $K 1
contaG1=$?

# richiamo script risorsivo per G2
./shellR.sh $G2 $K 2
contaG2=$?
echo contaG1=$contaG1 contaG2=$contaG2 K=$K
# verifico se il num file trovati in G1 == n file trovati in G2 == K
if [ $contaG1 -gt 0 -a $contaG2 -gt 0 -a $contaG1 -eq $contaG2 -a $contaG1 -eq $K -a $contaG2 -eq $K ]
then
	# inserisco tutti i path trovati nei file in una lista
	for i in `cat /tmp/tempfile1`
	do
		lista="$lista $i"
	done

	for i in `cat /tmp/tempfile2`
	do
		lista="$lista $i"
	done

	# richiamo la parteC se le condizioni richieste sono sodisfatte e passando i nomi dei file
	parteC $lista
fi
