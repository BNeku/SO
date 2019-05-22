//Nerea Jiménez González y Fernando Bellot Rodríguez
#include <stdio.h>
#include <time.h>
#include <fcntl.h> //para el open
#include <unistd.h> //para el close
#include <string.h> //para strlen
#include <stdlib.h>

int menu();
void contador(int file);
void minijuego(int file);

int main(){
	int leds = open("/dev/leds", O_WRONLY | O_CREAT | O_TRUNC); //abrimos el fichero
	int opcion = -1;
	printf("Práctica 4, leds: \n");
	while(opcion != 0){
		opcion = menu();
		if(opcion == 1){
			contador(leds);
		}
		else if(opcion == 2){
			minijuego(leds);
		}
	}

	close(leds);

	return 0;
}

int menu(){
	int op;
	printf("1 - Contador ascendente\n");
	printf("2 - Minijuego:prueba tu suerte\n");
	printf("0 - Salir\n");
	printf("Elige una opción: ");
	scanf("%i", &op);
	while(op < 0 || op > 2){
		printf("Opción incorrecta. Selecciona de nuevo: ");
		scanf("%i", &op);
	}
	return op;
}

/*Contador ascendente en forma de bits*/
void contador(int file){ //contador ascendente
	char* secuencia[8]={"","3","2","23","1","13","12","123"};
	int i= 0;
	for(i = 0; i < 8; i++){
		write(file,secuencia[i],strlen(secuencia[i]));
		sleep(1);
	}

}

/* Juego similar a una mini-tragaperras, si las tres luces acaban encendidas, ganas.
 * El juego va iluminando en un orden determinado las luces led.
 * En cada iteración del bucle se va ralentizando el tiempo hasta llegar a una tiempo límite.
 * Cuando se llega a ese tiempo límite, se elige la iluminación actual.*/
void minijuego(int file){
	char* secuencia[9]={"1","12","123","23","3","23","123", "12","1"};
	srand(time(NULL));
	int r = rand() % 1000; //Aquí actúa el azar, a la hora de sumar tiempo al contador
	int ganador, j = 0, i;
	int time = 90000;
	printf("\nMinijuego: apuesta qué leds se van a encender. Si aciertas, ganas!\n\n");
	while(time < 500000){ //se va ralentizando el tiempo poco a poco, acabará cuando el tiempo sea lo suficientemente lento
		if(j == 9){
			j = 0;
		}
		write(file,secuencia[j],strlen(secuencia[j]));
		usleep(time);
		time += 1500 + r;
		if(time >= 180000) //a partir de cierto punto las ralentizaciones son más drásticas
			time += 27000;

		if(time>= 500000)
			ganador = j;
		j++;
	}

	usleep(500000); //dejamos en pausa el resultado

	for(i = 0; i < 3; i++){ //Mostramos el resultado con parpadeos
		write(file,"",0);
		usleep(300000);
		write(file,secuencia[ganador],strlen(secuencia[ganador]));
		usleep(300000);
	}

	write(file,"",0);

}

