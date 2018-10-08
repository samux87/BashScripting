#Created by Samuele Buosi
#!/bin/sh
#entro nel direttorio
cd $1
conta=0
lista="";
for i in *
do
	 if [ -f $i -a -r $i ]
	 then
	 	 if [ `wc -l < $i` -gt $2 -a `grep -c $c $i` -gt 0 ]
	 	 then
	 	 	 #Aggiorno Contatore
	 	 	 conta=`expr $conta + 1`
	 	 	 lista=$lista $i
	 	 	 #Tengo traccia dei file in tempfile
	 	 	 echo `pwd`/$i >> /tmp/tempfile
	 	 fi
	 fi
done
#controllo numero di file trovati e chiamata alla parte C
if test $conta -eq $n
then
	 echo "Trovata directory utile:" `pwd`
	 parteC $lista $c $h
fi
#Chiamata ricorsiava
for i in *
do
	 if test -d $i -a -x $i
	 then
	 	 #Chiamata Ricorsiva
	 	 $0 `pwd`/$i $2 $3
	 	 #Aggiorno contatore
	 	 conta=`expr $conta + $?`
	 fi
done
exit $conta
