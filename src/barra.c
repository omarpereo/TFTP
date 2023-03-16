#include <stdio.h>
#include <unistd.h> // Necesario para la función sleep()

void progress_bar(int percentage, int steps){
    int i = 0, j = 0;
    int full = 0;
    int empty = steps;

    full = (int) ((percentage * steps+0.5)/100);
    empty = steps - full;
    


    printf("\rProgress: %3.d%% ", percentage);

    for(j = 1; j <= full; j++){
        printf("%c", 219);
    }
    for(j = 1; j <=empty; j++) {
        printf("%c", 177);
    }

    printf(" %d %d ", full, empty); 
    

    if(percentage == 100) {
        printf("\n");
    }
    fflush(stdout); // Flushing del buffer de salida
    sleep(1); // Espera de 1 segundo
}

int main() {
    int i;
    
    for (i = 0; i <= 100; i++) {

        progress_bar(i,50);    

    }  

    printf("\n Fin"); // Salto de línea para finalizar la barra de progreso
    return 0;
}
