#!/bin/bash


echo "Introduce el nombre del fichero (tiene que estar dentro de la carpeta examples): "
read nfile #nombre archivo
while [ ! -e examples/$nfile ] || [ ! -f examples/$nfile ] ; do
	echo "No existe el fichero o no es regular. Vuelva a introducir el nombre del fichero: "
	read nfile
done
echo "Introduce el número de CPU's a utilizar en la simulación"
read ncpus #numero maximo de cpus
while [ $ncpus -gt 8 ] || [ $ncpus -lt 1 ] ; do
	echo "Valor incorrecto. Introduce el número de CPU's a utilizar en la simulación"
	read ncpus
done

if [ -d resultados/ ]; then #si existe resultados se borra la carpeta y todo su contenido
	rm -r resultados/
	fi
mkdir resultados/ #creamos resultados

listadeSchedulersDisponibles=("RRdynQ SJF RR FCFS PRIO")
for nameSched in $listadeSchedulersDisponibles #para cada algoritmo
do
	echo "Generando las gráficas con $nameSched"
	for (( cpuIterator=1 ; cpuIterator <= $ncpus ; cpuIterator++ ))
	do
		if [ "$nameSched" == "PRIO" ]; then
			./schedsim -i examples/$nfile -n $cpuIterator -s $nameSched -p #tiene un parametro adicional
		else
			./schedsim -i examples/$nfile -n $cpuIterator -s $nameSched
		fi
		for (( i=0 ; i < $cpuIterator ; i++ ))
		do
			mv CPU_$i.log resultados/$nameSched-CPU-$cpuIterator-$i.log
			cd ../gantt-gplot
			./generate_gantt_chart ../schedsim/resultados/$nameSched-CPU-$cpuIterator-$i.log
			cd ../schedsim
		done
	done
done

exit 0


