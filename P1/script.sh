#! /bin/bash
#cd home/mytar
if [ -e mytar ] && [ -x mytar ]; then #existe y es ejecutable
	if [ -d tmp/ ]; then #si existe tmp borra la carpeta y todo su contenido
	rm -r tmp/
	fi
	mkdir tmp/ #creamos tmp
	cd tmp/
	echo "Hello world!" > file1.txt
	head -n 10 /etc/passwd > file2.txt #copiamos las primeras 10 lineas del fichero
	head -c 1024 /dev/urandom > file3.dat #copiamos los primeros 1024 bytes del fichero
	../mytar -cf filetar.mtar file1.txt file2.txt file3.dat #creamos filetar.mtar
	mkdir out/ #creamos la carpeta out
	cp filetar.mtar out/ #copia filetar.mtar a out
	cd out/
	../../mytar -xf filetar.mtar #extraemos
	if ! diff file1.txt ../file1.txt ; then #comparamos si los ficheros extraídos son iguales que los comprimidos
	echo "file1.txt no es igual"
	exit 1
	elif ! diff file2.txt ../file2.txt ; then
	echo "file2.txt no es igual"
	exit 1
	elif ! diff file3.dat ../file3.dat ; then
	echo "file3.dat no es igual"
	exit 1
	fi
	cd ../..
	echo "Correct"
	exit 0
else echo "No se ha encontrado el fichero ejecutable" #no existe o no es ejecutable
exit 1
fi
