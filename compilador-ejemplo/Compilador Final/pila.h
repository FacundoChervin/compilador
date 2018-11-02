#include <stdio.h>
#include <stdlib.h>
 
/* declaracion */
struct tpila{
  int clave;
  struct tpila *sig;
};

/* prototipos e implementacion */
int crearPila(struct tpila **pila);
int vacia(struct tpila *pila);
void apilar(struct tpila *pila, int elem);
int desapilar(struct tpila *pila);

void mostrarPila(struct tpila *pila);
 
 
int crearPila(struct tpila **pila)
{  
   if ((*pila = (struct tpila *) malloc(sizeof(struct tpila))) == NULL )
   {
      return -1;
   }
   (*pila)->sig = NULL;
   return 0;
}
 
int vacia(struct tpila *pila)
{
  return (pila->sig == NULL);
}
 
void apilar(struct tpila *pila, int elem)
{
  struct tpila *nuevo;
 
  nuevo = (struct tpila *) malloc(sizeof(struct tpila));
  nuevo->clave = elem;  
  nuevo->sig = pila->sig;
  pila->sig = nuevo;
  
} 

void mostrarPila(struct tpila *pila)
{

   
  struct tpila *aux = pila;
  int valor;
  
   printf("\n----------PILA-----------");
  
  while (aux != NULL){
        valor = aux->clave;  
        printf("\n Elemento: %d", valor);
        aux = aux->sig;
        }
    printf("\n--------FIN DE PILA---------\n\n");
}
 
int desapilar(struct tpila *pila)
{
  struct tpila *aux;
  int valor;
  
  aux = pila->sig;
  valor = aux->clave;  
  pila->sig = aux->sig;
  free(aux);
  return valor;
   
}


