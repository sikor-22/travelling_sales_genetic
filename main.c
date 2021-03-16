#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define PRESERVE_CONST 0.92 //in final implementation as argument from command line
#define MUTATION_CONST 0.7
#define MIX_CONST 0.4


struct agent
{
    int* genes;
    int distance;
};



void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

//utility function to create random inversion of array [0, 1, 2, ... , size - 1]
int* create_inversion(int size)
{
    
    int i, j;
    int *inv = calloc(size, sizeof(int));
    for(i = 0; i < size; i++)
    {
        inv[i] = i;
    }
    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++)
        {
            if(rand()%100 < 25) //every par rolls twice
            {
                swap(&inv[i], &inv[j]);
            }
        }
    }
    return inv;
}

//utility function to calculate total distance given graph matrix "graph" of size "size" and inversion of nodes "order"
int calc_distance(int* order,int** graph, int size)
{
    int i, current_node, next_node;
    int distance = 0;
    for(i = 0; i < size-1; i++)
    {
        current_node = order[i];
        next_node = order[i+1];
        distance += graph[current_node][next_node];
    }
    return (distance + graph[order[size-1]][order[0]]);
}



//utility function to create graph from given file
int** get_graph(char* entry_file, int* size)
{
    FILE* city_file = fopen(entry_file, "r");
    if(city_file == NULL)
    {
        exit(-1);
    }


    fseek(city_file, 0L, SEEK_END);
    int size_of_file = ftell(city_file);
    fseek(city_file, 0L, SEEK_SET);
    
    char* buff = calloc(size_of_file, sizeof(char));
    char* endptr;

    fgets(buff, size_of_file, city_file);
    int amnt_nodes = strtol(buff, &endptr, 10);
    *size = amnt_nodes;
    
    int** return_graph = (int**) calloc(amnt_nodes, sizeof(int*));
    int i, j, tmp;
    //memory alloc
    for(i = 0; i < amnt_nodes; i++)
    {
        return_graph[i] = (int*) calloc(amnt_nodes, sizeof(int*));
        for(j = 0; j < amnt_nodes; j++)
        {
            return_graph[i][j] = 0;
        }
    }

    //scan values
    int counter = 0;
    while(counter < (int)pow(amnt_nodes, 2))
    {
        fscanf(city_file, "%d", &tmp);
        return_graph[counter / amnt_nodes][counter % amnt_nodes] = tmp;
        counter++;
    }
    fclose(city_file);
    return return_graph;
}

struct agent* create_first_generation(int num, int size, int** graph)
{
    struct agent* return_arr = (struct agent*) calloc(num, sizeof(struct agent));
    int i;
    for(i = 0; i < num; i++)
    {
        return_arr[i].genes = create_inversion(size);
        return_arr[i].distance = calc_distance(return_arr[i].genes, graph, size);
    }
    return return_arr;
}


void mutate(struct agent* specimen, int size, int** graph)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(rand()%100 > (int)PRESERVE_CONST*100)
        {
            int k = rand()%size;
            swap(&specimen->genes[i], &specimen->genes[k]);
        }
    }
    specimen->distance = calc_distance(specimen->genes, graph, size);
}


struct agent mix(struct agent first_spec, struct agent sec_spec, int size, int**graph)
{
    if(rand()%100 < MUTATION_CONST*100)
    {
        mutate(&first_spec, size, graph);
    }
    if(rand()%100 < MUTATION_CONST*100)
    {
        mutate(&sec_spec, size, graph);
    }
    int *t = calloc(size, sizeof(int));
    int i;
    int tmp = rand()%100;
    for(i = 0; i < size; i++)
    {
        if(tmp < MIX_CONST*100)
        {
            t[i] = sec_spec.genes[i];
        }else{
            t[i] = first_spec.genes[i];
        }
    }
    int distance_loc = calc_distance(t, graph, size);
    struct agent ret_agent = {
        .genes = t,
        .distance = distance_loc
    };
    return ret_agent;
}





int* find_two_best(struct agent* generation, int num)
{
    int* ret_arr = (int*)calloc(2, sizeof(int));
    int min1 = generation[0].distance;
    int min_index1 = 0;
    int min2 = generation[1].distance;
    int min_index2 = 0;
    if(min1 > min2)
    {
        swap(&min1, &min2);
    }
    int i = 0;
    for(i = 2; i < num; i++)
    {
        if(generation[i].distance < min1)
        {
            min1 = generation[i].distance;
            min_index1 = i;
        }else if(generation[i].distance < min2)
        {
            min2 = generation[i].distance;
            min_index2 = i;
        }
    }
    ret_arr[0] = min_index1;
    ret_arr[1] = min_index2;
    return ret_arr;
}


struct agent* create_new_gen(struct agent* generation, int num, int size, int **graph)
{
   int* t = find_two_best(generation, num);
   struct agent* new_gen = calloc(num, sizeof(struct agent));
   int i;
   for(i = 0; i < num; i++)
   {
       new_gen[i] = mix(generation[t[0]], generation[t[1]], size, graph);
       if(rand()%100 < MUTATION_CONST *100)
       {
          mutate(&new_gen[i], size, graph);
       }
   }
   return new_gen;
}

int main(int argc, char **argv)
{
    srand(time(0));
    int amnt_nodes;
    char* file_name = "test.txt";
    int **t = get_graph(file_name, &amnt_nodes);
    struct agent* gen = create_first_generation((int)pow(amnt_nodes, 2), amnt_nodes, t);
    int i;
    int* t3 = find_two_best(gen, (int)pow(amnt_nodes, 2));
    for(i = 0; i < 36; i++)
    {
        struct agent* new_gen = create_new_gen(gen, (int)pow(amnt_nodes, 2), amnt_nodes, t);
        gen = new_gen;
        int *t2 = find_two_best(gen, (int)pow(amnt_nodes, 2));
        printf("Generation: %d | Distance: %d \n", i, gen[t2[0]].distance);
    }
}