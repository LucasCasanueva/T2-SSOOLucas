#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

void update_waiting(int t, Queue *q)
{
	Node *curr_node = q->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "WAITING"))
		{
			if ((t - curr_node->value->io_init_time) % curr_node->value->waiting_delay == 0)
			{
				curr_node->value->state = "READY";
			}
		}
		curr_node = curr_node->next;
	}
}

/* Devuelve el nodo(proceso) que esta corriendo en la CPU o NULL en caso contrario  */
struct Node *running_node(Queue *q_fifo1, Queue *q_fifo2, Queue *q_sjf)
{
	struct Node *curr_node = q_fifo1->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "RUNNING"))
		{
			return curr_node;
		}
		curr_node = curr_node->next;
	}
	curr_node = q_fifo2->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "RUNNING"))
		{
			return curr_node;
		}
		curr_node = curr_node->next;
	}
	curr_node = q_sjf->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "RUNNING"))
		{
			return curr_node;
		}
		curr_node = curr_node->next;
	}
	return NULL;
}

void update_running(int t, int change_cpu, Node *running_node, Queue *q_fifo1, Queue *q_fifo2, Queue *q_sjf)
{
	running_node->value->burst--;
	if (running_node->value->burst == 0)
	{
		running_node->value->state = "FINISHED";
		change_cpu = 2;
		if (running_node->value->priority == 1)
		{
			dequeue(q_fifo1, running_node->value);
		}
		else if (running_node->value->priority == 2)
		{
			dequeue(q_fifo2, running_node->value);
		}
		else if (running_node->value->priority == 3)
		{
			dequeue(q_sjf, running_node->value);
		}
		
	}
	else if ((t - running_node->value->cpu_init_time) % running_node->value->wait == 0)
	{
		running_node->value->state = "WAITING";
		running_node->value->io_init_time = t;
		running_node->value->interrupts++;
		change_cpu = 1;
		if (running_node->value->priority == 1)
		{
			dequeue(q_fifo1, running_node->value);
			enqueue(q_fifo1, running_node->value);
		}
		else if (running_node->value->priority == 2)
		{
			running_node->value->priority = 1;
			dequeue(q_fifo2, running_node->value);
			enqueue(q_fifo1, running_node->value);
		}
		else
		{
			dequeue(q_sjf, running_node->value);
			sjf_final_enqueue(q_sjf, running_node->value);
		}
	}
	else
	{
		if (running_node->value->priority == 1)
		{
			if ((t - running_node->value->cpu_init_time) % q_fifo1->quantum == 0)
			{
				change_cpu = 1;
				running_node->value->state = "READY";
				running_node->value->priority = 2;
				running_node->value->interrupts++;
				dequeue(q_fifo1, running_node->value);
				enqueue(q_fifo2, running_node->value);
			}
		}
		else if (running_node->value->priority == 2)
		{
			if ((t - running_node->value->cpu_init_time) % q_fifo2->quantum == 0)
			{
				change_cpu = 1;
				running_node->value->state = "READY";
				running_node->value->priority = 3;
				running_node->value->interrupts++;
				dequeue(q_fifo2, running_node->value);
				order_enqueue(q_sjf, running_node->value);
			}
		}
	}
}

/* Ingresa los procesos correspondientes a la primera cola */
void enqueue_processes_q1(Process **process_array, int len_array, int t, Queue *q_fifo1)
{
	for (int i = 0; i < len_array; i++)
	{
		if (process_array[i]->init_time == t)
		{
			enqueue(q_fifo1, process_array[i]);
		}
	}
}

void enqueue_aging(int t, Queue *q_fifo1, Queue *q_fifo2, Queue *q_sjf)
{
	Node *curr_node = q_fifo2->head;
	while (curr_node != NULL)
	{
		if ((t - curr_node->value->init_time) % curr_node->value->aging == 0)
		{
			dequeue(q_fifo2, curr_node->value);
			enqueue(q_fifo1, curr_node->value);
			curr_node = curr_node->next;
		}
	}
	curr_node = q_sjf->head;
	while (curr_node != NULL)
	{
		if ((t - curr_node->value->init_time) % curr_node->value->aging == 0)
		{
			dequeue(q_sjf, curr_node->value);
			enqueue(q_fifo1, curr_node->value);
			curr_node = curr_node->next;
		}
	}
}

bool ingresar_cpu(int t, Queue *q_fifo1, Queue *q_fifo2, Queue *q_sjf)
{
	Node *curr_node = q_fifo1->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "READY"))
		{
			curr_node->value->t_first = t;
			curr_node->value->state = "RUNNING";
			curr_node->value->cpu_init_time = t;
			curr_node->value->cpu_turns++;
			return true;
		}
		else
		{
			curr_node = curr_node->next;
		}
	}
	curr_node = q_fifo2->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "READY"))
		{
			curr_node->value->t_first = t;
			curr_node->value->state = "RUNNING";
			curr_node->value->cpu_init_time = t;
			curr_node->value->cpu_turns++;
			return true;
		}
		else
		{
			curr_node = curr_node->next;
		}
	}
	curr_node = q_sjf->head;
	while (curr_node != NULL)
	{
		if (strcmp(curr_node->value->state, "READY"))
		{
			curr_node->value->t_first = t;
			curr_node->value->state = "RUNNING";
			curr_node->value->cpu_init_time = t;
			curr_node->value->cpu_turns++;
			return true;
		}
		else
		{
			curr_node = curr_node->next;
		}
	}
	return false;
}

void generate_output(int t, Process **finished_process_array, int len, FILE *output_file)
{
	for (int i = 0; i < len; i++)
	{
		Process *p = finished_process_array[i];
		p->turnaround_time = p->t_out - p->init_time;
		p->response_time = p->t_first - p->init_time;
		p->waiting_time = p->turnaround_time - p->burst;
		fprintf(output_file, "%s, %i, %i, %i, %i, %i\n", p->name_process, p->cpu_turns, p->interrupts, p->turnaround_time, p->response_time, p->waiting_time);
	}
}

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	FILE *output_file = fopen(argv[2], "w");
	int q = atoi((char *)argv[3]);
	InputFile *input_file = read_file(file_name);
	Process **process_array = calloc(input_file->len, sizeof(Process*));
	Process **finished_process_array = calloc(input_file->len, sizeof(Process*));
	Queue *q_fifo1 = init_queue(1, 0, 2, q);
	Queue *q_fifo2 = init_queue(2, 0, 1, q);
	Queue *q_sjf = init_queue(3, 1, 0, 0);

	/*Mostramos el archivo de input en consola*/
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	for (int i = 0; i < input_file->len; ++i)
	{
		printf("HOLA");
		char *name =  input_file->lines[i][0];
		int pid = atoi(input_file->lines[i][1]);
		int priority = 1;
		int init_time = atoi(input_file->lines[i][2]);
		int burst = atoi(input_file->lines[i][3]);
		int wait = atoi(input_file->lines[i][4]);
		int waiting_delay = atoi(input_file->lines[i][5]);
		int aging = atoi(input_file->lines[i][6]);
		Process *p = init_process(pid, name, priority, init_time, burst, wait, waiting_delay, aging);
		printf("HOOLA");
		process_array[i] = p;
	}
	printf("PASE EL FOR");
	int change_cpu = 0;
	int counter = 0;
	int t = 0;
	while (1)
	{
		update_waiting(t, q_fifo1);
		update_waiting(t, q_fifo2);
		update_waiting(t, q_sjf);
		struct Node *runnin_node = running_node(q_fifo1, q_fifo2, q_sjf);
		if (runnin_node != NULL)
		{
			update_running(t, change_cpu, runnin_node, q_fifo1, q_fifo2, q_sjf);
			if (change_cpu == 2)
			{
				runnin_node->value->t_out = t;
				finished_process_array[counter] = runnin_node->value;
				counter++;
				if (counter + 1 == input_file->len)
				{
					break;
				}
			}
		}
		enqueue_processes_q1(process_array, input_file->len, t, q_fifo1);
		enqueue_aging(t, q_fifo1, q_fifo2, q_sjf);
		if (change_cpu == 1 || change_cpu == 2)
		{
			ingresar_cpu(t, q_fifo1, q_fifo2, q_sjf);
		}
		t++;
	}
	printf("LLEGUE AL OUTPUT");
	generate_output(t, finished_process_array, counter, output_file);

	input_file_destroy(input_file);
}