#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


Queue* init_queue(int qid, int type, int p, int q) {
    Queue * new_queue = malloc(sizeof(Queue));
    new_queue->qid = qid;
    new_queue->quantum = p*q;
    new_queue->head = NULL;
    new_queue->tail = NULL;
    return new_queue;
};

// this is the method to add a node in a FIFO LIST
bool enqueue(Queue *q, struct Process *new_process){
    Node * new_node = malloc(sizeof(Node));
    // if there is no memory to another node
    if (new_node == NULL ) return false;
    new_node->value = new_process;
    new_node->next = NULL;
    if (q->tail != NULL)
    {
        q->tail->next = new_node;
    };
    q->tail = new_node;
    if (q->head == NULL)
    {
        q->head = new_node;
    }
    return true;
}

/*
struct Process* dequeue(Queue *q) {
    // the scheduler need to verify if the q->head is empty
    Node *tmp = q->head;
    struct Process *res = tmp->value;
    q->head = q->head->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
    free(tmp);
    return res;    
}*/

void dequeue(Queue *q, struct Process *process) {
    // the scheduler need to verify if the q->head is empty
    printf("id de la cabeza es %d\n", q->head->value->pid);
    if (q->head->next == NULL)
    {
        printf("next es null\n");
    }
    Node *curr_node = q->head;
    Node *prev_node = NULL;
    while (1)
    {
        printf("Entre al while del dequeue\n");
        printf("curr_node process id es %d\n", curr_node->value->pid);
        printf("input process id es %d\n", process->pid);
        if (curr_node->value == process)
        {
            printf("BINGO\n");
            break;
        }
        else
        {
            prev_node = curr_node;
            curr_node = curr_node->next;
        }
    }
    printf("Sali del while del dequeue\n");
    if (curr_node == NULL)
    {
        printf("el curr node es null\n");
    }
    if (curr_node == q->head)
    {
        q->head = q->head->next;
        if (q->head == NULL)
        {
            q->tail = NULL;
        }
    }
    else if (curr_node == q->tail)
    {
        q->tail = prev_node;
    }
    else
    {
        prev_node->next = curr_node->next;
    }
    printf("Termine el dequeue\n");
    free(curr_node);
}

// this is the method to add a new node in a SFJ LIST
bool order_enqueue(Queue *q, struct Process *new_process){
    Node * new_node;
    Node * currNode;
    Node * prevNode;
    new_node = malloc(sizeof(Node));
    if (new_node == NULL) return false;
    new_node->value = new_process;
    new_node->next = NULL;
    // si la cola esta vacia, el nuevo nodo es cola y cabeza
    if (!(q->head))
    {
        q->head = new_node;
        q->tail = new_node;
    }
    // si la cola no esta vacia entonces entra aca
    else {
        // se revisa el nodo cabeza
        currNode = q->head;
        // si el valor de burst(tiempo de ejecucion) es menor que el del nodo revisado, se guarda el currNode como siguiente al nuevo
        if (new_node->value->burst < q->head->value->burst)
        {
            // se guarda el nodo cabeza como el siguente al nuevo
            new_node->next = q->head;
            // se actualiza la nueva cabeza con el nuevo nodo
            q->head = new_node;
        }
        else {
            // si el tiempo es mayor al de la cabeza debe revisar la cola iterativamente
            prevNode = currNode;
            // guarda el nodo previo y recorre iterativamente
            while (currNode && currNode->value->burst <= new_node->value->burst)
            {
                // se actualiza el nodo que se esta revisando, asi como el previo
                prevNode = currNode;
                currNode = currNode->next;
            }
            // si llega alfinal de la cola entonces el neuvo nodo tiene que se la nueva cola
            if (!currNode)
            {
                prevNode->next = new_node;
                q->tail = new_node;
            }
            // si queda entre medio de dos nodos intermedios, entonces se guarda actulizando los next de cada uno
            else {
                // al nuevo nodo se le establece el currNode como next
                new_node->next = currNode;
                // al nodo previo se le extablece el nuevo nodo como next
                prevNode->next = new_node;
            }
            
        }
        
    }
    return true;
    
}

void sjf_final_enqueue(Queue *q, struct Process *process)
{
    // esta funcion tira error 
    printf("get inside the sfj final funciton with process %d...\n", process->pid);
    printf("the id of the head is  %d...\n", q->head->value->pid);
    Node *new_node = malloc(sizeof(Node));
    new_node->value = process;
    new_node->next = NULL;
    printf("info of the queue %d\n", q->tail->value->pid);
    // el error era que si no hay cola entonces tira error
    if (!(q->head))
    {
        printf("inside the if q head\n");
        q->head = new_node;
        q->tail = new_node;
    }
    else {
        q->tail->next = new_node; 
        q->tail = new_node;  
    }
    printf("info of the queue %d\n", q->tail->value->pid);
}