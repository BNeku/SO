#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"


/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
static int copynFile(FILE * origin, FILE * destination, unsigned int nBytes)
{
	int cont=0;
	int c;

	//si ambos ficheros existen
	if(origin != NULL && destination !=NULL){
		while(c!= EOF && cont < nBytes){
			c=getc(origin);  //coge un caracter del origen
			if(c!=EOF){
				//lo copiamos en el destino
				//hay que castearlo a char porque si no lo guarda como numero ASCII
				putc((char)c, destination);
				cont++;
			}
		}
		//devolvemos el número de bytes copiados
		return cont;
	}else{
		//si da error
		return -1;
	}


}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
static char* loadstr(FILE * file)
{
	int c=(int)'a'; //inicializamos
	int cont=0;

	//calculamos el tamaño del array para reservar su espacio
	//mientras c diferente a \0 ya que esto indica final del string

	while(c != (int)'\0' && c!=EOF){
		c=fgetc(file);
		cont++;
	}

	//en caso de que haya salido de while por EOF y no por \0, el fichero tar no
	//tiene el formato correcto
	if(c==EOF){
		return NULL;
	}else{
		//reservamos el espacio
		char *p = malloc(cont);

		//colocamos el puntero al inicio
		//mirar porqué SEEK_CUR
		fseek(file, -(cont), SEEK_CUR);

		//copiamos de nuevo
		for(int i=0; i<cont;i++){
			p[i]=fgetc(file);
		}

		return p;
	}

}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
static stHeaderEntry* readHeader(FILE * tarFile, unsigned int *nFiles)
{

	//leemos el numero de archivos
	fread(nFiles, sizeof(int), 1, tarFile);

	//reservamos el espacio correspondiente
	stHeaderEntry *s = malloc(sizeof(stHeaderEntry)*(*nFiles));

	//cogemos la cabecera de cada archivo
	for(int i=0; i<*nFiles;i++ ){
		s[i].name = loadstr(tarFile);

		//si el nombre no acaba en \0 está mal
		if(s[i].name == NULL){
			return NULL;
		}else{
			fread(&s[i].size, sizeof(int), 1, tarFile);
		}
	}

	return s;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE *mtar = fopen(tarName, "w");
	FILE *e; //entrada
	int tamCab;

	//reservamos espacio
	stHeaderEntry *cabecera = malloc(sizeof(stHeaderEntry)*nFiles);

	//tamaño cabecera total
	tamCab = sizeof(int);
	for(int i=0; i<nFiles;i++){
		//tamaño de cada cabecera
		tamCab = tamCab + strlen(fileNames[i]) + 1 + sizeof(int);
	}

	//ponemos el puntero donde queremos escribir los datos
	fseek(mtar, tamCab, SEEK_SET);

	//copiamos la informacion en cabecera y los datos en el archivo
	for(int j=0; j<nFiles;j++){
		//abro el archivo solo para leer
		e= fopen(fileNames[j],"r");

		//reservo espacio para el nombre de la cabecera
		cabecera[j].name = malloc(sizeof(fileNames[j])+1);

		strcpy(cabecera[j].name, fileNames[j]);

		//copiamos los datos del archivo
		cabecera[j].size = copynFile (e, mtar, INT_MAX);

		//en el caso de que no se encuentre el fichero
		if(cabecera[j].size == -1){
			printf("ERROR: Fichero %s no encontrado.\n", cabecera[j].name);
			return EXIT_FAILURE;
		}

		//cerramos el archivo!!!!
		fclose(e);
	}

	//Realizamos el archivo tar
	//primero escribrimos el número de archivos y los datos de la cabecera
	fseek(mtar, 0, SEEK_SET);
	fwrite(&nFiles, sizeof(int),1,mtar);

	for(int k=0; k<nFiles; k++){
		fwrite(cabecera[k].name, strlen(fileNames[k]),1,mtar);
		fwrite("\0", sizeof(char),1,mtar);
		fwrite(&cabecera[k].size,sizeof(int),1,mtar);
	}

	//liberamos la memoria
	for(int t=0; t<nFiles;t++){
		free(cabecera[t].name);
	}

	free(cabecera);
	fclose(mtar);
	printf("Fichero %s creado correctamente.\n", tarName);

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[])
{
	//abrimos el tar
	FILE *mtar = fopen(tarName, "r");

	//si no encuentra el tar
	if(mtar==NULL){
		printf("ERROR: No existe el fichero %s.\n",tarName);
		return EXIT_FAILURE;
	}

	FILE *a;
	int nFiles;
	//cogemos la cabecera
	stHeaderEntry *cab = readHeader(mtar, &nFiles);

	//si está mal el formato
	if(cab==NULL){
		printf("ERROR: No se ha podido leer el fichero %s.\n", tarName);

		return EXIT_FAILURE;
	}

	int b;

	for(int i=0; i<nFiles;i++){
		//abrimos el archivo i de cabecera
		a=fopen(cab[i].name, "w");
		//cogemos sus datos
		b=copynFile(mtar, a, cab[i].size);
		//cerramos el archivo abierto
		fclose(a);

		//si b es -1 es que ha dado error
		if(b==-1){
			printf("ERROR: No se ha podido extraer el tar %s.\n", tarName);
			fclose(mtar);
			return EXIT_FAILURE;
		}
	}

	printf("Tar %s extraído con éxito.\n", tarName);
	fclose(mtar);
	return EXIT_SUCCESS;

}
