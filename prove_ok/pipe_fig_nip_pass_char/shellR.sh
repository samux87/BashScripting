#Created by Samuele Buosi
#!/bin/sh
#entro nel direttorio
cd $1
K=$2
flag=$3
conta=0
for i in *
do
	 if [ -f $i ]
	 then
	 	 if [ `wc -c < $i` -gt $K ]
	 	 then
	 	 	 #Aggiorno Contatore
	 	 	 conta=`expr $conta + 1`
			
	 	 	 #Tengo traccia dei file in tempfile
			if [ $flag -eq 1 ]
			then
	 	 	 	echo `pwd`/$i >> /tmp/tempfile1
			else
	 	 	 	echo `pwd`/$i >> /tmp/tempfile2
			fi

			# scrivo in output il percorso assoluto del file trovato
			echo Trovato: `pwd`/$i 
	 	 fi
	 fi
done
#Chiamata ricorsiava
for i in *
do
	 if test -d $i -a -x $i
	 then
	 	 #Chiamata Ricorsiva
	 	 $0 `pwd`/$i $K $flag
	 	 #Aggiorno contatore
	 	 conta=`expr $conta + $?`
	 fi
done

exit $conta
