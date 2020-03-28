#ifndef BETWEENNESSCENTRALITY_H
#define BETWEENNESSCENTRALITY_H

#include <iostream>
#include <vector>
#include <limits>

#define INF INT32_MAX

using namespace std;

// Definisco il tipo AdjacencyMatrix: una matrice di num_nodi*num_nodi int
// in cui alla posizione (i,j) vi è il peso dell'arco che va dal nodo i al nodo j
typedef int** AdjacencyMatrix;

// Definisco il tipo Predecessors: una matrice di vettori di int
// in cui alla posizione (i,j) vi sono tutti i predecessori del nodo j
// in un qualsiasi cammino minimo da i a j
typedef vector< vector< vector<int> > > Predecessors;

/**
 * @brief readGraph: legge il file di input e alloca la matrice di adiacenza ad esso associata
 * @param file: nome del file di input
 * @return matrice di adiacenza del grafo
 */
AdjacencyMatrix readGraph(string file);

/**
 * @brief writeNodesBetCentrality: scrive su un file di output i nodi e gli indici di betweenness centrality ad essi associati
 * @param file: nome del file di output
 * @param betCentrality: puntatore all'array contenente gli indici di betweenness centrality
 */
void writeNodesBetCentrality(string file, float* betCentrality);

/**
 * @brief betweennessCentrality: calcola per ogni nodo il suo indice di betweenness centrality
 * @param graph: matrice di adiacenza del grafo di cui calcolare la centralità
 * @return puntatore all'array degli indici di betweenness centrality
 */
float* betweennessCentrality(AdjacencyMatrix graph);

/**
 * @brief delGraph: dealloca la matrice di adiacenza associata al grafo
 * @param graph: matrice di adiacenza
 */
void delGraph(AdjacencyMatrix graph);

#endif // BETWEENNESSCENTRALITY_H
