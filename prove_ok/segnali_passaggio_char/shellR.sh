#Created by Samuele Buosi
#!/bin/sh
#entro nel direttorio
cd $1
K=$2
conta=0
for i in *
do
	 if [ -f $i ]
	 then
	 	 if [ `wc -c < $i` -eq $K ]
	 	 then
	 	 	 #Aggiorno Contatore
	 	 	 conta=`expr $conta + 1`
	 	 	echo trovato: `pwd`/$i 
			#Tengo traccia dei file in tempfile
	 	 	 echo `pwd`/$i >> /tmp/tempfile
	 	 fi
	 fi
done

#Chiamata ricorsiava
for i in *
do
	 if test -d $i -a -x $i
	 then
	 	 #Chiamata Ricorsiva
	 	 $0 `pwd`/$i $G $K
	 	 #Aggiorno contatore
	 	 conta=`expr $conta + $?`
	 fi
done
exit $conta
