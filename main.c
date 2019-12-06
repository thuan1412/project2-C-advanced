#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "jrb.h"
#include "dllist.h"
#include "jval.h"

#define INFINITIVE_VALUE  10000000

typedef struct {
    char bus_name[10];
    char **bus_stations;
} Bus;

typedef struct {
    char **bus_list;
    char station_name;
} BusStation;

typedef struct {
   JRB edges;
   JRB vertices;
} Graph;

Graph createGraph();
void addVertex(Graph g,  char *station_name, char *bus_list);
char *getVertex(Graph graph, char *station_name);
void addEdge(Graph g, char *station1, char *station2, double distance);
double getEdgeValue(Graph graph, char *station1, char *station2);
int indegree(Graph graph, int v, int* output);
int outdegree(Graph graph, int v, int* output);
void dropGraph(Graph graph);
double shortestPath(Graph graph, int s, int t, int* path, int*length);
int compareBusStation(Jval j1, Jval j2);
int updateVertex(Graph g, char *station_name, char *new_bus);
Graph dataToGraph(char *filename);

void trim(char * str);


Bus bus_list[150];
int bus_count = 0;

char** stringSplit(char* str, const char delimiter) {
    char** result = 0;
    size_t count = 0;
    char* temp = str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = delimiter;
    delim[1] = 0;

    // count how many elements will be extracted
    while (*temp) {
        if (delimiter == *temp) {
            count++;
            last_comma = temp;
        }
        temp++;
    }
    
    // add space for traling token
    count += last_comma < (str + strlen(str) - 1);

    count++;

    result = malloc(sizeof(char *) * count);
    
    if (result) {
        size_t idx = 0;
        char* token = strtok(str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count-1);
        *(result + idx) = 0;
    }
    return result;
}


int main() {
    // FILE *bus_data = fopen("bus_test.txt", "r");
    Graph g = createGraph();

    g = dataToGraph("bus_full.txt");
    char *array1 = "1 2 2";
    addVertex(g, "1", array1);

    // updateVertex(g, "1", "3");
    char * result = getVertex(g, "1");
    if (result) {
        printf("%s\n", result);
    }
    return 0;

}


Graph createGraph() {
    Graph g;
    g.edges = make_jrb();
    g.vertices = make_jrb();
    return g;
}


Graph dataToGraph(char *filename) {
    FILE *bus_data = fopen(filename, "r");
    Graph g = createGraph();

    while (!feof(bus_data)) {
        char *s;
        s = (char *)(malloc(1000));
        fgets(s, 1000, bus_data);    
        s[strlen(s)-1] = '\0';    
        char ** token = 0;
        char *bus_name=0;
        bus_name = malloc(5);

        token = stringSplit(s, ':');
        strcpy(bus_name, *(token + 0));
        char *station_lines = *(token + 1);

        char **bus_stations;
        
        bus_stations = stringSplit(*(token + 1), '-');

        if (bus_stations) {
            int i;
            for (i=0; *(bus_stations+i); i++) {
                trim(*(bus_stations + i)); 
                char *station=0;
                station = malloc(100);
                strcpy(station, *(bus_stations + i));
                addVertex(g, station, bus_name);
                free(*(bus_stations+i));
            }
            free(bus_stations);
        }
    }
    return g;
}


void addVertex(Graph g,  char *station_name, char *bus_list) {
    JRB node = jrb_find_str(g.vertices, station_name);
    if (node == NULL) {
        jrb_insert_str(g.vertices, station_name, new_jval_s(bus_list));
    }
}

void addEdge(Graph g, char *station1, char *station2, double distance) {

}

char *getVertex(Graph g, char *station_name) {
    JRB node = jrb_find_str(g.vertices, station_name);
    if (node==NULL) {
        return NULL;
    } else {
        return jval_s(node->val);
    }
}


int updateVertex(Graph g, char *station_name, char new_bus[10]) {
    JRB node = jrb_find_str(g.vertices, station_name);
    if (node==NULL) {
        return -1;
    } else {
        jrb_delete_node(node);
        char *temp;
        temp = malloc (sizeof(char) * 300);
        strcpy(temp, jval_s(node->val));
        strcat(temp, " ");
        strcat(temp, new_bus);
        jrb_insert_str(g.vertices, station_name, new_jval_s(temp));
        return 1;
    }
    return 0;
}


void trim(char * str) {
    int index, i;
    index = 0;
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    i = 0;
    while(str[i + index] != '\0')
    {
        str[i] = str[i + index];
        i++;
    }
    str[i] = '\0'; // Terminate string with NULL

    i = 0;
    index = -1;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index = i;
        }

        i++;
    }
    str[index + 1] = '\0';
}
