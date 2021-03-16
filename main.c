#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


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
    //char* buff_tmp = calloc(size_of_file, sizeof(char));
    fgets(buff, size_of_file, city_file);
    int amnt_nodes = strtol(buff, &endptr, 10);
    *size = amnt_nodes;
    //printf("%d", amnt_nodes);
    int** return_graph = (int**) calloc(amnt_nodes, sizeof(int*));
    int i, j, tmp;
    for(i = 0; i < amnt_nodes; i++)
    {
        return_graph[i] = (int*) calloc(amnt_nodes, sizeof(int*));
        for(j = 0; j < amnt_nodes; j++)
        {
            return_graph[i][j] = 0;
        }
    }
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

int main(int argc, char **argv)
{
    int amnt_nodes;
    char* file_name = "test.txt";
    int **t = get_graph(file_name, &amnt_nodes);
}