// genera_assembler.h
#include <stdio.h>

//
// MACROS
//
#define FILENAME_OUT "final.asm"

#define BUFF_MAX    256

#define TYPE_STRING "STRING"
#define TYPE_INT    "ENTERO"
#define TYPE_FLOAT  "FLOAT"
#define TYPE_CONST_STRING "const_string_"

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

// Lista con el destino de los branchs
int branch_list[MAXSIZEPILA] = {0};
int branch_count = 0;

int jump_list[MAXSIZEPILA] = {0};
int jump_count = 0;

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
void genera_branch();
void genera_jump();
void set_tag(int idx);

// Funciones de la pila
void push(const char* data);
void pop(char* data);
void print_stack();

// Funciones auxiliares
int isConst(const char* name);
int isAux(const char* name);
int is_in_list(int i);

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

    unsigned int branch_flag = 0;
    unsigned int jump_flag = 0;

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

    // Lee el archivo de tabla de simbolos
    while(fgets(buffer, BUFF_MAX, file_ts) != NULL) {

        char var_name[BUFF_MAX];
        char var_value[BUFF_MAX];
        char var_type[BUFF_MAX];
        char buffaux[BUFF_MAX];

        ptr_aux = strtok(buffer, "|");
        strcpy(var_name, ptr_aux);

        ptr_aux = strtok(NULL, "|");
        strcpy(var_value, ptr_aux);
        if (strcmp(var_value, TYPE_STRING) == 0 ) {
            strcpy(var_value, "");
        }

        ptr_aux = strtok(NULL, "|");
        strcpy(var_type, ptr_aux);
        new_line = strchr(var_type, '\n');
        if(new_line != NULL){
            *new_line = '\0';
        }

        // Valida que sea una constante
        if (var_name[0] >= '0' && var_name[0] <= '9' || var_name[0] == '-') {

            ;

        } else {

            if ( strstr(var_name, TYPE_CONST_STRING) > 0 ) {

                sprintf(buffaux, "\t_%s\tdd\t\"%s\"", var_name, var_value);

            } else {

                strcpy(var_value, "?");
                sprintf(buffaux, "\t_%s\tdd\t%s", var_name, var_value);
            }

            write_to_file(buffaux);
            
        }
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

        char buffer_aux[BUFF_MAX];
        strcpy(buffer_aux, buffer);
        char* idx = strtok(buffer_aux, "|");
        char* data = strtok(NULL, "|");
        new_line = strchr(data, '\n');
        if(new_line != NULL){
            *new_line = '\0';
        }             
        
        if(branch_flag > 0 ){
            branch_flag++;
            if(branch_flag==3){
                
                branch_list[branch_count] = atoi(data);
                branch_count++;
                branch_flag = 0;
            }
        }
        if(jump_flag > 0) {
            jump_flag++;
            if(jump_flag == 2){

                jump_list[jump_count] = atoi(data);
                jump_count++;
                jump_flag = 0;                
            }
        }        
        if( strcmp(data, "CMP") == 0) {
            branch_flag++;
        }
        if( strcmp(data, "JMP") == 0) {
            jump_flag++;
        }

        strcpy(POLACA[indice_polaca], buffer);
        indice_polaca++;
    }
    fclose(file_polaca);

    // Lee la polaca para cargarla en la pila y generar codigo assembler
    branch_flag = 0;
    jump_flag = 0;
    for(int i=0; i < indice_polaca; i++){

        strtok(POLACA[i], "|");
        char* aux_ptr = strtok(NULL, "|");
        new_line = strchr(aux_ptr, '\n');
        if(new_line != NULL){
            *new_line = '\0';
        }        

        if (is_in_list(i) == 1) {
            set_tag(i);
        }

        push(aux_ptr);

        // Genera codigo branch
        if (branch_flag > 0) {
            branch_flag++;
            if ( branch_flag == 3) {
                genera_branch();
                branch_flag = 0;
            }
            continue;
        }

        // Genera codigo jump
        if (jump_flag > 0 ) {
            jump_flag++;
            if ( jump_flag == 2 ) {
                genera_jump();
                jump_flag = 0;
            }       
            continue;             
        }

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

        //
        // Branch o Jump
        //
        if ( strcmp(aux_ptr, "CMP") == 0) {
            branch_flag++;
        }
        if ( strcmp(aux_ptr, "JMP") == 0) {
            jump_flag++;
        }

        //
        // END OF PROGRAM
        //
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

    char buff_izq[BUFF_MAX];
    char buff_der[BUFF_MAX];

    pop(asig);
    pop(op_der);
    pop(op_izq);

    if (isConst(op_izq) == 1) {
        sprintf(buff_izq, "%s", op_izq);
    } else {

        if(isAux(op_izq) == 1) {
            sprintf(buff_izq, "%s", op_izq);
        } else {
            sprintf(buff_izq, "_%s", op_izq);
        }
    }

    if (isConst(op_der) == 1) {
        sprintf(buff_der, "%s", op_der);
    } else {

        if(isAux(op_der) == 1) {
            sprintf(buff_der, "%s", op_der);
        } else {
            sprintf(buff_der, "_%s", op_der);
        }
    }    

    sprintf(buffer, "\tmov %s, %s", buff_izq, buff_der);
    write_to_file(buffer);
    write_to_file("");

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

    if (isConst(op_izq) == 1) {
        sprintf(buffer, "\tmov R1, %s", op_izq);
    } else {

        if(isAux(op_izq) == 1) {
            sprintf(buffer, "\tmov R1, %s", op_izq);
        } else {
            sprintf(buffer, "\tmov R1, _%s", op_izq);
        }
    }
    write_to_file(buffer);

    if (isConst(op_der) == 1) {
        sprintf(buffer, "\t%s R1, %s", assembler_operator, op_der);
    } else {
        if(isAux(op_der) == 1) {
            sprintf(buffer, "\t%s R1, %s", assembler_operator, op_der);
        } else {
            sprintf(buffer, "\t%s R1, _%s", assembler_operator, op_der);
        }
        
    }
    write_to_file(buffer);

    sprintf(buffer, "\tmov %s, R1", new_var);
    write_to_file(buffer);
    write_to_file("");

    push(new_var);
}

/*
 * Escribe en pantalla
 */ 
void write_func() {

    char func[BUFF_MAX];
    char var[BUFF_MAX];
    char buffer[BUFF_MAX];

    pop(func);
    pop(var);

    if (isConst(var) == 1) {
        sprintf(buffer, "\tdisplayString %s", var);
    } else {
        if (isAux(var) == 1) {
            sprintf(buffer, "\tdisplayString %s", var);    
        } else {
            sprintf(buffer, "\tdisplayString _%s", var);
        }
    }

    write_to_file(buffer);
    write_to_file("");

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

    if (isConst(var) == 1) {
        sprintf(buffer, "\tgetString %s", var);
    } else {
        sprintf(buffer, "\tgetString _%s", var);
    }

    write_to_file(buffer);
    write_to_file("");

}

void genera_branch() {

    char go_to[BUFF_MAX];
    char comparator[BUFF_MAX];
    char cmp_assembler[BUFF_MAX];
    char func[BUFF_MAX];
    char buffer[BUFF_MAX];
    char op_izq[BUFF_MAX];
    char op_der[BUFF_MAX];
    char buffer_der[BUFF_MAX];
    char buffer_izq[BUFF_MAX];

    pop(go_to);
    pop(comparator);
    pop(func);
    pop(op_der);
    pop(op_izq);

    if (isConst(op_der) == 1) {
        sprintf(buffer_der, "%s", op_der);
    } else {
        if (isAux(op_der) == 1) {
            sprintf(buffer_der, "%s", op_der);    
        } else {
            sprintf(buffer_der, "_%s", op_der);
        }
    }

    if (isConst(op_izq) == 1) {
        sprintf(buffer_izq, "%s", op_izq);
    } else {
        if (isAux(op_izq) == 1) {
            sprintf(buffer_izq, "%s", op_izq);    
        } else {
            sprintf(buffer_izq, "_%s", op_izq);
        }
    }

    if(strcmp(comparator, "BEQ") == 0) {
        strcpy(cmp_assembler, "JE");
    }
    if(strcmp(comparator, "BNE") == 0) {
        strcpy(cmp_assembler, "JNE");
    }   
    if(strcmp(comparator, "BGT") == 0) {
        strcpy(cmp_assembler, "JG");
    }  
    if(strcmp(comparator, "BLT") == 0) {
        strcpy(cmp_assembler, "JL");
    }      
    if(strcmp(comparator, "BGE") == 0) {
        strcpy(cmp_assembler, "JGE");
    }  
    if(strcmp(comparator, "BLE") == 0) {
        strcpy(cmp_assembler, "JLE");
    }      

    sprintf(buffer, "\tCMP %s, %s", buffer_izq, buffer_der);
    write_to_file(buffer);
    sprintf(buffer, "\t%s ET_%s\n", cmp_assembler, go_to);
    write_to_file(buffer);
    write_to_file("");
}

void genera_jump() {

    char go_to[BUFF_MAX];
    char func[BUFF_MAX];
    char buffer[BUFF_MAX];

    pop(go_to);
    pop(func);

    sprintf(buffer, "\tJMP ET_%s\n", go_to);
    write_to_file(buffer);
    write_to_file("");
}

/*
 * Genera una etiqueta
 */
void set_tag(int idx) {

    char buffer[BUFF_MAX];

    sprintf(buffer, "ET_%d:\n", idx);

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

//
// Funciones auxiliares
//

/*
 * Valida si es constante entera o float
 */ 
int isConst(const char* name) {

    if (name[0] >= '0' && name[0] <= '9' || name[0] == '-') {
        return 1;
    }
    return 0;
}

/*
 * Valida si variable auxiliar
 */ 
int isAux(const char* name) {

    if (name[0] == '@') {
        return 1;
    }
    return 0;
}

/*
 * Verifica si el indice se encuentra en una posicion que tiene salto
 */
int is_in_list(int i) {

    int j = 0;

    for(j = 0; j < branch_count; j++){
        if(branch_list[j] == i){

            return 1;
        }
    }

    for(j = 0; j < jump_count; j++){
        if(jump_list[j] == i){

            return 1;
        }
    }    

    return 0;
}