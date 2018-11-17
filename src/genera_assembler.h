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
#define VAR_AUX_NAME      "@aux%d"

#define MAXSIZEPILA 5000

//
// VARIABLES GLOBALES
//
FILE* fileout = NULL;

int aux_cant = 1;

// Polaca en memoria
char POLACA[MAXSIZEPILA][BUFF_MAX];

// Pila
char PILA[MAXSIZEPILA][BUFF_MAX];
int tope = 0;

//
// DECLARACION DE FUNCIONES
//
void create_file();
void close_file();
void write_to_file(const char* line);
void genera_file();

// Funciones que generan codigo assembler
void genera_asignacion();
void genera_operacion();
void read_func();
void write_func();

// Funciones de la pila
void push(const char* data);
void pop(char* data);
void print_stack();

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
void genera_file(const char* filename_ts, const char* filename_polaca){

    char buffer[BUFF_MAX];
    char* ptr_aux = NULL;
    unsigned int cont_const_string  = 1;
    unsigned int cont_const_int     = 1;
    unsigned int cont_const_float   = 1;
    char* new_line = NULL;    

    FILE* file_ts = fopen(filename_ts, "r");
    if (file_ts == NULL ) {

        printf("ERROR al abrir archivo: %s\n", filename_ts);
        exit(1);
    }

    FILE* file_polaca = fopen(filename_polaca, "r");
    if (filename_polaca == NULL){

        printf("ERROR al abrir archivo: %s\n", filename_polaca);
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

        // @TODO variables constantes

        sprintf(buffaux, "\t_%s\tdd\t%s", var_name, "?");
        write_to_file(buffaux);
    }
    fclose(file_ts);

    write_to_file("");

    // Area de codigo
    write_to_file(".CODE");
    write_to_file("");
    write_to_file("START:");

    // Carga en memoria la polaca
    int indice_polaca = 0;
    while(fgets(buffer, BUFF_MAX, file_polaca) != NULL) {

        strcpy(POLACA[indice_polaca], buffer);
        indice_polaca++;
    }
    fclose(file_polaca);

    // Lee la polaca para cargarla en la pila y generar codigo assembler
    for(int i=0; i < indice_polaca; i++){

        strtok(POLACA[i], "|");
        char* aux_ptr = strtok(NULL, "|");
        new_line = strchr(aux_ptr, '\n');
        if(new_line != NULL){
            *new_line = '\0';
        }        

        push(aux_ptr);

        //
        // Operadores Binarios
        //
        // Igual
        if ( strcmp(aux_ptr, ":=") == 0) {
            genera_asignacion();
        }
        // Operacion
        if ( strcmp(aux_ptr, "+") == 0
            || strcmp(aux_ptr, "-") == 0
            || strcmp(aux_ptr, "/") == 0
            || strcmp(aux_ptr, "*") == 0) {
            genera_operacion();
        }

        //
        // Operadores unarios
        //
        // Read
        if ( strcmp(aux_ptr, "READ") == 0) {
            read_func();
        }
        if ( strcmp(aux_ptr, "WRITE") == 0) {
            write_func();
        }

        // END OF PROGRAM
        if ( strcmp(aux_ptr, "END") == 0) {
            write_to_file("END START");
        }

    }
    print_stack();

    close_file();
}

//
// Funciones para la generacion del codigo assembler por partes
//

/*
 * Asignacion
 */
void genera_asignacion() {

    char asig[BUFF_MAX];
    char op_der[BUFF_MAX];
    char op_izq[BUFF_MAX];
    char buffer[BUFF_MAX];

    pop(asig);
    pop(op_der);
    pop(op_izq);

    sprintf(buffer, "\tmov _%s, %s", op_izq, op_der);
    write_to_file(buffer);
}

/*
 * Operacion de resta, suma, multiplicacion o division
 */
void genera_operacion() {

    char operando[BUFF_MAX];
    char op_der[BUFF_MAX];
    char op_izq[BUFF_MAX];
    char buffer[BUFF_MAX];
    char assembler_operator[BUFF_MAX];
    char new_var[BUFF_MAX];

    pop(operando);
    pop(op_der);
    pop(op_izq);

    if(strcmp(operando, "+") == 0) {
        strcpy(assembler_operator, "sum");
    }
    if(strcmp(operando, "-") == 0) {
        strcpy(assembler_operator, "sub");
    }
    if(strcmp(operando, "/") == 0) {
        strcpy(assembler_operator, "div");
    }
    if(strcmp(operando, "*") == 0) {
        strcpy(assembler_operator, "mul");
    }    

    sprintf(new_var, VAR_AUX_NAME, aux_cant);
    aux_cant++;

    sprintf(buffer, "\tmov R1, _%s", op_izq);
    write_to_file(buffer);
    sprintf(buffer, "\t%s R1, _%s", assembler_operator, op_der);
    write_to_file(buffer);
    sprintf(buffer, "\tmov %s, R1", new_var);
    write_to_file(buffer);

    push(new_var);
}

/*
 * Escribe en pantalla
 */ 
void write_func() {

    char func[BUFF_MAX];
    char msg[BUFF_MAX];
    char buffer[BUFF_MAX];

    pop(func);
    pop(msg);

    sprintf(buffer, "\tdisplayString %s", msg);
    write_to_file(buffer);
}

/*
 * Lee del teclado
 */
void read_func() {

    char func[BUFF_MAX];
    char var[BUFF_MAX];
    char buffer[BUFF_MAX];

    pop(func);
    pop(var);

    sprintf(buffer, "\tgetString %s", var);
    write_to_file(buffer);
}

//
// Funciones de la pila
//


/*
 * Push en la pila
 */
void push(const char* data){

    strcpy(PILA[tope], data);
    tope++;
}

/*
 * Pop de la pila
 */ 
void pop(char* data) {

    if(tope > 0) {
        tope--;
        strcpy(data, PILA[tope]);
    }
}

/*
 * Print de la pila
 */
void print_stack(){

    for(int i=tope-1; i >= 0; i--) {
        printf("PILA[%d] = %s\n", i, PILA[i]);
        fflush(stdin);
    }
}