#Created by Samuele Buosi
#!/bin/sh
#entro nel direttorio
cd $1

M=$2
shift 2
conta=0
lista=
for j in $*
do
 	if [ -f $j ]
 	then
 	 	#Aggiorno Contatore
 	 	conta=`expr $conta + 1`
		echo "Conta:$conta"
		lista="$lista $j" #Mi annoto i file trovati
	 fi
done
#controllo numero di file trovati e chiamata alla parte C
if test $conta -ge $M
then
	 echo "Trovata directory utile:" `pwd`
	 parteC $M $lista
fi

#Chiamata ricorsiava
for i in *
do
	 if test -d $i -a -x $i
	 then
	 	 #Chiamata Ricorsiva
	 	 $0 `pwd`/$i $M $*
	 fi
done
exit 
