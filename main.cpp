#include <iostream>
#include <time.h>
#include <sys/resource.h>
#include "betweenness-centrality.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: betweenness-centrality.exe file_input.dat file_output.dat" << endl;
        return -1;
    }

    string input = argv[1], output = argv[2];
    AdjacencyMatrix G = readGraph(input);
    if (G == NULL)
    {
        cout << "Error: input file not found!" << endl;
        return -1;
    }

    size_t start, end;
    start = clock();
    float *betCentrality = betweennessCentrality(G);
    end = clock();
    if (betCentrality == NULL)
    {
        cout << "Error: graph has a negative weight cycle" << endl;
        delGraph(G);
        return -1;
    }

    cout << "Exec. time: " << (float)(end - start) / CLOCKS_PER_SEC << "s" << endl;

    struct rusage r_usage;
    getrusage(RUSAGE_SELF,&r_usage);
    printf("Memory usage = %ldkB\n",r_usage.ru_maxrss);

    writeNodesBetCentrality(output, betCentrality);
    free(betCentrality);
    delGraph(G);

    return 0;
}
