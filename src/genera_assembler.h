// genera_assembler.h
#include <stdio.h>

//
// MACROS
//
#define FILENAME_OUT "final.asm"

#define BUFF_MAX    256

#define TYPE_STRING "200"
#define TYPE_INT    "201"
#define TYPE_FLOAT  "202"

#define CONST_STRING_NAME "_str_%d"
#define CONST_INT_NAME    "_int_%d"
#define CONST_FLOAT_NAME  "_float_%d"

//
// VARIABLES GLOBALES
//
FILE* fileout = NULL;

//
// DECLARACION DE FUNCIONES
//
void create_file();
void close_file();
void write_to_file(const char* line);
void genera_file();

//
// DEFINICION DE FUNCIONES
//

/*
 * Crea el archivo de output
 */
void create_file() {
    fileout = fopen(FILENAME_OUT, "w");
}   

/*
 * Cierra el archivo de output
 */
void close_file() {
    if(!fileout) {
        fclose(fileout);
        fileout = NULL;
    }
}

/*
 * Escribe una linea en el archivo de output
 */
void write_to_file(const char* line) {
    if (fileout != NULL){
        fprintf(fileout, "%s\n", line);
        fflush(fileout);
    } 
}

/*
 * Genera el archivo assembler
 */ 
void genera_file(const char* filename_ts) {

    char buffer[BUFF_MAX];
    char* ptr_aux = NULL;
    unsigned int cont_const_string  = 1;
    unsigned int cont_const_int     = 1;
    unsigned int cont_const_float   = 1;

    FILE* file_ts = fopen(filename_ts, "r");
    if (file_ts == NULL ) {

        printf("ERROR al abrir archivo: %s\n", filename_ts);
        exit(1);
    }

    create_file();

    // Inclusion de archivos
    write_to_file("include macros2.asm");
    write_to_file("include number.asm");
    write_to_file("");

    // Definicion de arquitectura
    write_to_file(".MODEL SMALL");
    write_to_file(".386");
    write_to_file(".STACK 200h");
    write_to_file("");

    // Area de datos
    write_to_file(".DATA");
    write_to_file("");

    while(fgets(buffer, BUFF_MAX, file_ts) != NULL) {

        char var_name[BUFF_MAX];
        char var_value[BUFF_MAX];
        char var_type[BUFF_MAX];
        char buffaux[BUFF_MAX];
        char* new_line = NULL;

        ptr_aux = strtok(buffer, "|");
        strcpy(var_name, ptr_aux);

        ptr_aux = strtok(NULL, "|");
        strcpy(var_value, ptr_aux);

        ptr_aux = strtok(NULL, "|");
        strcpy(var_type, ptr_aux);
        new_line = strchr(var_type, '\n');
        if(new_line != NULL){
            *new_line = '\0';
        }

        // Valida que sea una constante
        if(strcmp(var_type, TYPE_STRING) == 0) {

            if (var_name[0] == '\"' && var_name[strlen(var_name) - 1] == '\"') {

                strcpy(var_value, var_name);
                sprintf(var_name, CONST_STRING_NAME, cont_const_string);
                cont_const_string++;
            }
        }
        if(strcmp(var_type, TYPE_INT) == 0) {

            
        }        

        sprintf(buffaux, "\t%s\tdd\t%s", var_name, var_value);
        write_to_file(buffaux);
    }

    write_to_file("");
    fclose(file_ts);

    // Area de codigo
    write_to_file(".CODE");
    write_to_file("");

    close_file();
}

/*
 * Parsea el archivo con la polaca 
 */
void parse_polaca(FILE* file_pol) {


}