#!/bin/bash
#Script práctica 2 de SO
#Nerea Jiménez y Fernando Bellot

if [ -d temp/ ]; then #si existe temp se borra la carpeta y todo su contenido
	rm -r temp/
	fi
#crear carpeta auxiliar y copiar nuestros ficheros en la carpeta y la unidad
mkdir temp/ #creamos temp
cp src/fuseLib.c mount-point
cp src/fuseLib.c temp
cp src/myFS.h temp
cp src/myFS.h mount-point

#auditar el disco y hacer diff entre ficheros originales y los copiados en el SF
./my-fsck virtual-disk
if diff mount-point/fuseLib.c temp/fuseLib.c ; then
	echo "Son iguales fuseLib"
else
	echo "Son diferentes fuseLib"
	exit 0
fi

if diff mount-point/myFS.h temp/myFS.h ; then
	echo "iguales myFS"
else
	echo "Son diferentes myFS"
	exit 0
fi
#truncar el primer fichero, que ocupe un bloque menos, restamos 4096 bytes (el tamaño de un bloque)
truncate -s -4096 mount-point/fuseLib.c
truncate -s -4096 temp/fuseLib.c

#auditar el disco y diff entre los fuseLib.c truncados
./my-fsck virtual-disk
if diff mount-point/fuseLib.c temp/fuseLib.c ; then
	echo "Son iguales fuseLib truncados"
else
	echo "Son diferentes fuseLib truncados"
	exit 0
fi

#copia un tercer archivo al SF
echo "Tercer fichero de prueba" > file.txt
cp file.txt mount-point

#auditar y hacer diff
./my-fsck virtual-disk
if diff file.txt mount-point/file.txt ; then
	echo "iguales file"
else
	echo "file.txt son diferentes"
fi

#truncar 2o fichero en temp y en SF para que ocupa un bloque mas (4096 bytes más)
truncate -s 4096 mount-point/myFS.h
truncate -s 4096 temp/myFS.h

#auditar y diff entre los myFS.h truncados
./my-fsck virtual-disk
if diff mount-point/myFS.h temp/myFS.h ; then
	echo "Son iguales myFS truncados"
else
	echo "Son diferentes myFS truncados"
	exit 0
fi
