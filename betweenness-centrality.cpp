#include <fstream>
#include <iomanip>
#include "betweenness-centrality.h"

// Rispettivamente il numero di nodi e di vertici del grafo
static int nodes, edges;

/**
* @brief initGraph: alloca una matrice di adiacenza vuota,
*                   da ogni nodo non posso raggiungere nessun altro nodo tranne il nodo stesso
* @return matrice di adiacenza vuota
*/
static AdjacencyMatrix initGraph()
{
    // Allocazione della matrice di adiacenza
    AdjacencyMatrix graph = (int **)malloc(nodes * sizeof(int*));
    for (int i = 0; i < nodes; i++)
        graph[i] = (int *)malloc(nodes * sizeof(int));

    // Riempimento della matrice di adiacenza
    for (int i = 0; i<nodes; i++)
        for (int j = 0; j<nodes; j++)
            // Se non vi sono archi un nodo è connesso solo con se stesso (0)
            // e da esso non posso raggiungere nessuno degli altri (INF)
            graph[i][j] = (i == j) ? 0 : INF;

    return graph;
}

/**
* @brief nodePredContains: verifica la presenza di node all'interno di predNodes
* @param predNodes: puntatore al vettore di interi contenente i predecessori
* @param node: int da cercare tra i predecessori
* @return true se il nodo è presente, false in caso contrario
*/
static bool nodePredContains(vector<int> &predNodes, int node)
{
    for (size_t i = 0; i<predNodes.size(); i++)
        if (predNodes.at(i) == node) return true;
    return false;
}

/**
* @brief FloydWarshall: versione modificata dell'algoritmo di Floyd-Wharshall;
*                       crea in tempo O(|V|^3) una matrice di adiacenza contenente
*                       alla posizione (i,j) il peso del cammino minimo dal nodo i al nodo j
*                       ed una matrice di vettori contenente alla posizione (i,j)
*                       un vettore con i predecessori del nodo j in tutti i cammini minimi dal nodo i al nodo j
* @param graph: matrice di adiacenza del grafo di cui calcolare i cammini minimi tra tutte le coppie
* @return puntatore alla matrice di vettori contenente i predecessori, NULL se il grafo contiene cicli di peso negativo
*/
static Predecessors* FloydWarshall(AdjacencyMatrix graph)
{
    // Matrice di adiacenza che conterrà alla posizione (i,j) il peso del cammino minimo da i a j
    AdjacencyMatrix w = initGraph();

    // Matrice di vettori da aggiornare con i predecessori
    static Predecessors pred;

    // Alloco spazio in memoria per i predecessori
    pred.resize(nodes);
    for (int i = 0; i<nodes; i++)
    {
        pred[i].resize(nodes);
    }

    // Inizializzazione
    for (int i = 0; i < nodes; i++)
    {
        for (int j = 0; j < nodes; j++)
        {
            w[i][j] = graph[i][j]; // I cammini minimi inizialmente sono gli archi del grafo
            if ((w[i][j] != 0 && w[i][j] != INF)) pred[i][j].push_back(i); // Se vi è un arco il predecessore di j è i...
            else pred[i][j].push_back(-1); // ...altrimenti non c'è un predecessore
        }
    }

    for (int k = 0; k < nodes; k++)
    {
        for (int i = 0; i < nodes; i++)
        {
            for (int j = 0; j < nodes; j++)
            {
                // Se un cammino minimo passante per un nodo k è minore del cammino minimo attuale
                // aggiorno la matrice dei pesi e sostituisco i predecessori attuali (i,j) con quelli alla posizione (k,j)
                if (w[i][k] != INF && w[k][j] != INF && w[i][j] > w[i][k] + w[k][j])
                {
                    w[i][j] = w[i][k] + w[k][j];

                    pred[i][j].clear();

                    int size = (int)pred[k][j].size();
                    for (int l = 0; l<size; l++)
                        pred[i][j].push_back(pred[k][j][l]);
                }
                // Nel caso in cui il peso minimo attuale coincide con il nuovo peso da verificare
                // siamo in presenza di un cammino minimo alternativo
                else if (i != k && j != k && w[i][j] == w[i][k] + w[k][j])
                {
                    int size = (int)pred[k][j].size();
                    for (int l = 0; l<size; l++)
                    {
                        // Se due o più cammini minimi distinti da un nodo i ad un nodo j passano per lo stesso nodo
                        // questo deve essere contato tra i predecessori soltanto una volta
                        if (!nodePredContains(pred[i][j], pred[k][j][l]))
                            pred[i][j].push_back(pred[k][j][l]);
                    }
                }
            }
        }
    }

    // Verifico la presenza di cicli negativi controllando
    // che il cammino minimo da ogni nodo a se stesso sia ancora 0
    for (int i = 0; i<nodes; i++)
        if (w[i][i] < 0)
        {
            delGraph(w);
            return NULL;
        }

    delGraph(w);

    return &pred;
}

/**
* @brief shortestPathVisit: aggiorna ricorsivamente l'array dinamico nodesPathCount in base a quante volte
*                           un cammino minimo dal nodo corrispondente all'indice della riga a c
*                           presenta il nodo al relativo indice dell'array
* @param predRow: puntatore alla riga contenente i predecessori nei cammini minimi
*                 dal nodo corrispondente all'indice della riga a tutti gli altri
* @param c: nodo di cui controllare i predecessori nella riga
* @param nodesPathCount: puntatore all'array da aggiornare
* @return il numero di cammini minimi dal nodo corrispondente all'indice della riga a c
*/
static int shortestPathVisit(vector< vector<int> > &predRow, int c, int *nodesPathCount)
{
    // Incremento di 1 il nodo di partenza ogni volta che finisce il backtracking.
    // In questo modo alla posizione corrispondente al nodo di partenza dell'array nodesPathCount
    // sarà contenuto il numero di cammini minimi da i a j
    if (predRow[c][0] == -1) return 1;

    int tmp = 0, ret = 0;
    for (size_t k = 0; k < predRow[c].size(); k++)
    {
        // Alla fine della ricorsione tmp è il numero di cammini minimi dal nodo di partenza al nodo c
        tmp = shortestPathVisit(predRow, predRow[c][k], nodesPathCount);
        // Incremento di tmp volte nodesPathCount alle posizioni corrispondenti ai predecessori di c
        nodesPathCount[predRow[c][k]] += tmp;
        // Incremento il valore di ritorno della funzione in quanto
        // ogni cammino passante per un predecessore di c passa anche per c stesso
        ret += tmp;
    }

    return ret;
}

AdjacencyMatrix readGraph(string file)
{
    ifstream fin(file); // Apre file in modalità read-only

    if (!fin.is_open()) return NULL;

    fin >> nodes >> edges;

    AdjacencyMatrix graph = initGraph();

    // Aggiorno la matrice di adiacenza vuota modificando le posizioni (i,j)
    // con il peso dell'arco da i a j
    int node_i, node_j, tmp;
    for (int e = 0; e < edges; e++)
    {
        fin >> node_i >> node_j >> tmp;

        // Nel caso vi sia più di un arco da un nodo i ad un nodo j considero solo l'arco di peso minore
        // un arco che va da un nodo in se stesso è un arco alternativo a quello implicito di peso 0 del nodo
        if (tmp < graph[node_i - 1][node_j - 1]) graph[node_i - 1][node_j - 1] = tmp;
    }

    fin.close();

    return graph;
}

void writeNodesBetCentrality(string file, float* betCentrality)
{
    ofstream fout(file); // Apre file in modalità write-only

    fout << fixed; // setprecision indica il numero di cifre decimali invece che quello di cifre significative
    for (int i = 0; i<nodes; i++)
        fout << i + 1 << " " << betCentrality[i] << setprecision(6) << endl;

    fout.close();
}

float* betweennessCentrality(AdjacencyMatrix graph)
{
    // Puntatore alla matrice dei predecessori calcolata con Floyd-Wharshall
    Predecessors* pred = FloydWarshall(graph);

    if (pred == NULL) return NULL;

    // Puntatore all'array dinamico contenente all'i-esima posizione l'indice di betweenness centrality del nodo i
    float *betCentrality = (float*)malloc(nodes * sizeof(float));

    // Array dinamico da aggiornare per ogni coppia di vertici (i,j)
    // contenente alla k-esima posizione il numero di volte
    // che un qualsiasi cammino minimo da i a j è passato per k
    int *nodesPathsCount = (int*)malloc(nodes * sizeof(int));

    // Inizializzazione
    for (int l = 0; l<nodes; l++)
    {
        betCentrality[l] = 0;
        nodesPathsCount[l] = 0;
    }

    for (int i = 0; i < nodes; i++)
    {
        for (int j = 0; j < nodes; j++)
        {
            // Non considero i cammini da un nodo a se stesso o da un nodo ad uno non raggiungibile
            if (pred->at(i)[j][0] == -1) continue;

            // Ricostruisco tutti i cammini minimi dal nodo i al nodo j aggiornando nodesPathCount
            shortestPathVisit(pred->at(i), j, nodesPathsCount);

            for (int k = 0; k<nodes; k++)
            {
                // La betweenness centrality del nodo i non deve essere incrementata in quanto i è il nodo di partenza
                if (i != k)
                {
                    // Il nodo i è l'ultimo dei predecessori per ogni cammino minimo distinto
                    // nodesPathCount[i] perciò è il numero di cammini minimi distinti da i a j.
                    betCentrality[k] += (float)nodesPathsCount[k] / (float)nodesPathsCount[i];
                    // Azzero l'array nodesPathsCount per riutilizzarlo nei cammini minimi da i (o i+1) a j+1 (o j=0)
                    nodesPathsCount[k] = 0;
                }
            }

            nodesPathsCount[i] = 0;
        }
    }

    free(nodesPathsCount);

    return betCentrality;
}

void delGraph(AdjacencyMatrix graph)
{
    for (int i = 0; i<nodes; i++)
        free(graph[i]);
    free(graph);
}
