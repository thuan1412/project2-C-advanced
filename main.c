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
   JRB mapping;
} Graph;

Graph createGraph();
void addVertex(Graph g,  char *station_name, char *bus_list);
char *getVertex(Graph graph, char *station_name);
double getEdgeValue(Graph graph, char *station1, char *station2);
void addEdge(Graph g, char *station1, char *station2, double distance);
int indegree(Graph graph, int v, int* output);
int outdegree(Graph graph, int v, int* output);
void dropGraph(Graph graph);
double shortestPath(Graph graph, int s, int t, int* path, int*length);
int compareBusStation(Jval j1, Jval j2);
int updateVertex(Graph g, char *station_name, char *new_bus);
Graph dataToGraph(char *filename);
int getMapping(Graph g, char *station_name);
int addMapping(Graph g, char *staion_name, int index);
int getAdjacentVertices(Graph graph, int vertex, int* output);
int adjacent(Graph graph, int v1, int v2);
void BFS(Graph g, int start, int stop);

void trim(char * str);
int endsWith(char *s1, char *s2);

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
    Graph g = createGraph();

    g = dataToGraph("bus_full.txt");

    char * result = getVertex(g, "Giai Phong");
    if (result) {
        printf("1.%s\n", result);
    }
    double distance = getEdgeValue(g, "Giai Phong",  "Kim Dong");
    printf("2.%.2f\n", distance);
    int index = getMapping(g, "Giai Phong");
    printf("3.%d\n", index);
    
    int path[1000];
    int length[1000];
    int n = getMapping(g, "Tran Dai Nghia");
    
    int m = getMapping(g, "Tran Nhat Duat");
    printf("%d %d\n", m, n);
    // int a = shortestPath(g, m, n, path, length);
    // printf("4. %d\n", a);
    int output[100];
    n = getAdjacentVertices(g, n, output);
    printf("%d\n", n);
    return 0;
}


Graph createGraph() {
    Graph g;
    g.edges = make_jrb();
    g.vertices = make_jrb();
    g.mapping = make_jrb();
    return g;
}


void BFS(Graph g, int start, int stop) {
    int visited[1000] = {};
    int n, output[100], i, u, v;
    Dllist node, queue;
    
    queue = new_dllist();
    dll_append(queue, new_jval_i(start));

    while ( !dll_empty(queue) ) {
        node = dll_first(queue);
        u = jval_i(node->val);
        dll_delete_node(node);
        if (!visited[u]) {
            printf("%d \n", u);
            if (u == stop) return;
            n = getAdjacentVertices(g, u, output);
        }
        
    }
}

Graph dataToGraph(char *filename) {
    FILE *bus_data = fopen(filename, "r");
    Graph g = createGraph();
    int count = 0;
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
            for (i=0; *(bus_stations+i+1); i++) {
                trim(*(bus_stations + i));
                trim(*(bus_stations + i+1)); 
                
                char *station=0;
                station = malloc(100);
                strcpy(station, *(bus_stations + i));
                
                char *next_station=0;
                next_station = malloc(200);
                strcpy(next_station, *(bus_stations + i+1));
                
                if (!getVertex(g, station)) {
                    // if (getMapping(g, station)!=-1) 
                    if (addMapping(g, station, count)) count++;

                    // if (getMapping(g, next_station)!=-1) 
                    if (addMapping(g, next_station, count)) count++;
                    
                    addVertex(g, station, bus_name);
                } else {
                    updateVertex(g, station, bus_name);
                }
                addEdge(g, station, next_station, 1);
                free(*(bus_stations+i));
            }
            free(bus_stations);
        }
    }
    printf("%d\n", count);
    return g;
}

int addMapping(Graph g, char *staion_name, int index) {
    JRB node = jrb_find_str(g.mapping, staion_name);
    if (node == NULL) {
        jrb_insert_str(g.mapping, staion_name, new_jval_i(index));
        return 1;
    }
    return 0;
}

int getMapping(Graph g, char *station_name) {
    JRB node = jrb_find_str(g.mapping, station_name);
    if (node==NULL) {
        return -1;
    } else {
        int index = jval_i(node->val);
        return index;
    }
}

void addVertex(Graph g,  char *station_name, char *bus_list) {
    int index = getMapping(g, station_name);
    JRB node = jrb_find_int(g.vertices, index);
    if (node == NULL) {
        jrb_insert_int(g.vertices, index, new_jval_s(bus_list));
    }
}


char *getVertex(Graph g, char *station_name) {
    int index = getMapping(g, station_name);
    JRB node = jrb_find_int(g.vertices, index);
    if (node==NULL) {
        return NULL;
    } else {
        return jval_s(node->val);
    }
}

double getEdgeValue(Graph g, char *station1, char *station2) {
    JRB node, tree;
    int index1, index2;
    index1 = getMapping(g, station1);
    index2 = getMapping(g, station2);
    node = jrb_find_int(g.edges, index1);
    if (node == NULL) {
        return INFINITIVE_VALUE;
    }
    tree = (JRB) jval_v(node->val);
    node = jrb_find_int(tree, index2);
    if (!node) {
        return INFINITIVE_VALUE;
    } else {
        return jval_d(node->val);
    }
} 

void addEdge(Graph g, char *station1, char *station2, double distance) {
    JRB node, tree;
    int index1, index2;
    index1 = getMapping(g, station1);
    index2 = getMapping(g, station2);
    if (getEdgeValue(g, station1, station2) == INFINITIVE_VALUE) {
        node = jrb_find_int(g.edges, index1);
        if (node == NULL) {
            tree = make_jrb();
            jrb_insert_int(g.edges, index1, new_jval_v(tree));
        } else {
            tree = (JRB) jval_v(node->val);
        }
        jrb_insert_int(tree, index2, new_jval_d(distance));
    }

    if (getEdgeValue(g, station2, station1) == INFINITIVE_VALUE) {
        node = jrb_find_int(g.edges, index2);
        if (node == NULL) {
            tree = make_jrb();
            jrb_insert_int(g.edges, index2, new_jval_v(tree));
        } else {
            tree = (JRB) jval_v(node->val);
        }
        jrb_insert_int(tree, index1, new_jval_d(distance));
    }
}


int getAdjacentVertices (Graph graph, int v, int* output)
{
    JRB tree, node;
    int total;
    node = jrb_find_int(graph.edges, v);
    if (node==NULL)
       return 0;
    tree = (JRB) jval_v(node->val);
    total = 0;   
    jrb_traverse(node, tree)
    {
       output[total] = jval_i(node->key);
       total++;                
    }
    return total;   
}

int updateVertex(Graph g, char *station_name, char new_bus[10]) {
    int index = getMapping(g, station_name);
    JRB node = jrb_find_int(g.vertices, index);
    if (node==NULL) {
        return -1;
    } else {
        jrb_delete_node(node);
        char *temp;
        temp = malloc (sizeof(char) * 300);
        strcpy(temp, jval_s(node->val));
        if (!endsWith(temp, new_bus)) {
            strcat(temp, " ");
            strcat(temp, new_bus);
        }
        jrb_insert_int(g.vertices, index, new_jval_s(temp));
        return 1;
    }
    return 0;
}

void trim(char * str) {
    int index, i;
    index = 0;
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n') {
        index++;
    }
    i = 0;
    while(str[i + index] != '\0') {
        str[i] = str[i + index];
        i++;
    }
    str[i] = '\0'; // Terminate string with NULL

    i = 0;
    index = -1;
    while(str[i] != '\0') {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
            index = i;
        }

        i++;
    }
    if (str[index] == '.')
        str[index] = '\0';
    str[index + 1] = '\0';
}

int endsWith(char *s1, char *s2) {
    return strcmp(s1+strlen(s1) - strlen(s2), s2) == 0;
}

double shortestPath(Graph g, char *start_station, char *end_station, int *path, int *length) {
    int s = getMapping(g, start_station);
    int t = getMapping(g, end_station);
    double distance[1000];
    int previous[1000], u, visit[1000];
    
    for (int i=0; i<1000; i++){
        distance[i] = INFINITIVE_VALUE;
        visit[i] = 0;
        previous[i] = 0;
    }
    distance[s] = 0;
    previous[s] = s;
    visit[s] = 1;
    
    Dllist ptr, queue, node;
    queue = new_dllist();
    dll_append(queue, new_jval_i(s));
    
    // Duyet Queue
    while (!dll_empty(queue)){
        node = dll_first(queue);
        int u = jval_i(node->val);
        dll_delete_node(node);
        int output[100];
        int number = outdegree(graph,u,output);
        for (int i =0; i<number; i++) {
            if (visit[output[i]]==0) {
                visit[output[i]] = 1;
                dll_append(queue,new_jval_i(output[i]));
            }
            if ((getEdgeValue(graph,u,output[i])+distance[u])<distance[output
                                                                       [i]]) {
                distance[output[i]]= getEdgeValue(graph,u,output[i])+distance[u];
                previous[output[i]] = u;
                
            }
             
          
        }
    }
     path[0] = t;
    int length1 = 1;
    int cur = t;
    while (cur != s){
        path[length1] = previous[cur];
        length1 = length1+1;
        cur = previous[cur];
    }
    

    return distance[t];
}


int adjacent(Graph graph, int v1, int v2)
{
    JRB node, tree;
    node = jrb_find_int(graph.edges, v1);
    if (node==NULL)
       return 0;
    tree = (JRB) jval_v(node->val);
    if (jrb_find_int(tree, v2)==NULL)
       return 0;
    else
       return 1;       
}