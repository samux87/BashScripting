
#Created by Samuele Buosi
#!/bin/sh

echo "Nome script da creare , senza .sh" ; read nomefile

nomefile=$nomefile.sh

>$nomefile

chmod 711 $nomefile

echo "#Created by Samuele Buosi" >>$nomefile
echo "#!/bin/sh" >>$nomefile

#richiesta del numero minimo di argomenti
echo "Numero minimo di argomenti, 0 per nessun controllo"; read minargs

test $minargs -eq 0 2> /dev/null
if test $? -eq 2
then 
     echo Valore non Valido
     exit
fi 

if test $minargs -gt 0
then
	echo '#Controllo numero parametri' >> $nomefile
	echo 'case $# in' >> $nomefile
	echo '#Controllo primo parametro' >> $nomefile
	echo $minargs') case $1 in' >> $nomefile
	echo $'\t' '/*) if test ! -d $1 -o ! -x $1' >> $nomefile
	echo $'\t' $'\t' 'then' >> $nomefile
	echo $'\t' $'\t' 'echo $1 non direttorio' >> $nomefile
	echo $'\t' $'\t' 'exit 1' >> $nomefile
	echo $'\t' $'\t' 'fi;;' >> $nomefile
	echo $'\t' '*)  echo $1 non nome assoluto; exit 2;;' >> $nomefile
	echo $'\t' 'esac;;' >> $nomefile
	echo '*) 	echo "Uso: $0 G XXXXXXXX"' >> $nomefile
	echo $'\t' 'exit 3;;' >> $nomefile
	echo 'esac' >> $nomefile

	echo '# Assegno nomi ai parametri' >> $nomefile
	echo 'G=$1' >> $nomefile
	echo 'XXXXXXXXXXXXXXXXXXXXXXXXX' >> $nomefile

	echo "D?, [0,1]"; read risp
	if [ $risp -gt 0 ]
	then
		echo '# Verifico D path relativo semplice' >> $nomefile
		echo 'case $D in' >> $nomefile
		echo $'\t' '*/*) echo D non semplice; exit 4;;' >> $nomefile
		echo $'\t' '/*) echo D non relativo; exit 5;;' >> $nomefile
		echo $'\t' '*) ;;' >> $nomefile
		echo 'esac' >> $nomefile
	fi

	echo "Cn?, [0,1]"; read risp
	if [ $risp -gt 0 ]
	then
		echo '# verifico Cn singoli caratteri' >> $nomefile
		echo 'for Cx in $*' >> $nomefile
		echo 'do' >> $nomefile
		echo $'\t' 'case $Cx in' >> $nomefile
		echo $'\t' $'\t' '?) ;;' >> $nomefile
		echo $'\t' $'\t' '*) echo $Cx non singolo char; exit 6;;' >> $nomefile
		echo $'\t' 'esac' >> $nomefile
		echo 'done' >> $nomefile
	fi

	echo "K?, [0,1]"; read risp
	if [ $risp -gt 0 ]
	then
		echo '# Controllo se K numerico' >> $nomefile
		echo 'expr "$K" + 0 > /dev/null 2>&1' >> $nomefile
		echo 'if [ $? -eq 2 -o $? -eq 3 ]' >> $nomefile
		echo 'then echo K non numerico: "$K"; exit 7' >> $nomefile
		echo 'fi' >> $nomefile
		echo '# Controllo se K positivo' >> $nomefile
		echo 'if [ $K -le 0 ]' >> $nomefile
		echo 'then echo K non Strettamente Positivo: "$K"; exit 8' >> $nomefile
		echo 'fi' >> $nomefile
	fi
fi

echo '# Predisposizione ricorsione' >> $nomefile
echo 'PATH=$(pwd):$PATH' >> $nomefile
echo 'export PATH' >> $nomefile

echo '#creo file temporaneo' >> $nomefile
echo '> /tmp/tempfile' >> $nomefile
echo 'chmod 711 /tmp/tempfile' >> $nomefile

echo '# richiamo script risorsivo' >> $nomefile


#Creo il file ricorsivo
echo "File ricorsivo? [0,1]"; read risp
if test $risp -gt 0
then
	echo "Nome?, senza .sh" ; read nomefile
	
	nomefile=$nomefile.sh
	>$nomefile
	chmod 711 $nomefile
	
	echo "#Created by Samuele Buosi" >>$nomefile
	echo "#!/bin/sh" >>$nomefile

	echo "#entro nel direttorio" >>$nomefile
	echo 'cd $1' >>$nomefile
	
	echo 'conta=0' >>$nomefile
	echo 'lista="";' >>$nomefile

	echo 'for i in *' >>$nomefile
	echo 'do' >>$nomefile
	echo $'\t'  'if [ -f $i -a -r $i ]' >>$nomefile
	echo $'\t'  'then' >>$nomefile
	echo $'\t' $'\t' 'if [ `wc -l < $i` -gt $2 -a `grep -c $c $i` -gt 0 ]' >>$nomefile 
	echo $'\t' $'\t' 'then' >>$nomefile
	echo $'\t' $'\t' $'\t' '#Aggiorno Contatore' >>$nomefile
	echo $'\t' $'\t' $'\t' 'conta=`expr $conta + 1`' >>$nomefile
	echo $'\t' $'\t' $'\t' 'lista=$lista $i' >>$nomefile
	echo $'\t' $'\t' $'\t' '#Tengo traccia dei file in filetemp' >>$nomefile
	echo $'\t' $'\t' $'\t' 'echo `pwd`/$i >> /tmp/filetemp' >>$nomefile
	echo $'\t' $'\t' 'fi' >>$nomefile
	echo $'\t'  'fi' >>$nomefile
	echo 'done' >>$nomefile

	echo "#controllo numero di file trovati e chiamata alla parte C" >>$nomefile
	echo 'if test $conta -eq $n' >>$nomefile
	echo 'then' >>$nomefile
	echo $'\t' 'echo "Trovata directory utile:" `pwd`' >>$nomefile
	echo $'\t' 'parteC $lista $c $h' >>$nomefile
	echo 'fi' >>$nomefile

	
	echo '#Chiamata ricorsiava' >> $nomefile
	echo 'for i in *' >> $nomefile
	echo 'do' >> $nomefile
	echo $'\t' 'if test -d $i -a -x $i' >> $nomefile
	echo $'\t' 'then' >> $nomefile
	echo $'\t' $'\t' '#Chiamata Ricorsiva' >> $nomefile
	echo $'\t' $'\t' '$0 `pwd`/$i $2 $3' >> $nomefile
    echo $'\t' $'\t' '#Aggiorno contatore' >> $nomefile
	echo $'\t' $'\t' 'conta=`expr $conta + $?`' >> $nomefile
	echo $'\t' 'fi' >> $nomefile
	echo 'done' >> $nomefile
	
	echo 'exit $conta' >>$nomefile
	
fi




