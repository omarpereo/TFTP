#include <stdio.h>
#include <string.h>


int main(int argc, char * argv[]){

    char short_filename[30] = "";

    int i = 0, index = 0, size_filename = 0;

    printf("\nFull path: %s", argv[1]);

    size_filename = strlen(argv[1]);

    for(i = 0; i < size_filename; i++) {
        if(argv[1][i] == '\\') {
            index = i +1;
        }        
    }
        
    strncpy(short_filename,argv[1] + index, size_filename-index);


/*++++++++++++++++++++++++++++++++++++++++++++++++++ */
    printf("\n Short path: %s", short_filename);
    
return 0;  

}