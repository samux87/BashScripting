#Created by Samuele Buosi
#!/bin/sh
#entro nel direttorio
cd $1

K=$2
Y=$3

conta=0
lista=""

for i in *
do #Cerco un file 
	 if [ -f $i -a -r $i ]
	 then # Verifico se accettabile 
	 	 if [ `wc -c < $i` -eq $Y ]
		 then
     			 case `cat $i` in
	 			*[!0-9]*) ;; # Contiene un carattere 	 
				*)           # Non contiene carattari
					#Aggiorno Contatore
	 	 	 		conta=`expr $conta + 1`;
	 	 	 		lista="$lista $i";; #Tengo traccia dei file
			esac
	 	 fi
	 fi
done

#controllo numero di file trovati e chiamata alla parte C
if test $conta -eq $K
then
	 echo "Trovata directory utile:" `pwd`
	 parteC $lista
fi

#Chiamata ricorsiava
for i in *
do
	 if test -d $i -a -x $i
	 then
	 	 #Chiamata Ricorsiva
	 	 $0 `pwd`/$i $K $Y
	 fi
done

exit 
