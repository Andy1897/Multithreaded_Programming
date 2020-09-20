#include <iostream>
#include <vector>
#include <omp.h>
#include <string>
#include <fstream>

using namespace std;

#define INF INT32_MAX

vector <vector<int>> graph;
vector<int> dist;
vector<int> used;
int n;

void dijkstra(int start) {

    dist[start] = 0;

    for (int i = 0; i < n; i++) {

        vector<int> minDistPerThread(omp_get_max_threads(), INF);
        vector<int> nearestKPerThread(omp_get_max_threads(), -1);

#pragma omp parallel for default(none) shared(n, used, dist, minDistPerThread, nearestKPerThread)
        for (int k = 0; k < n; k++) {
            int threadNum = omp_get_thread_num();
            if (!used[k] && (nearestKPerThread[threadNum] < 0 || minDistPerThread[threadNum] > dist[k])) {
                minDistPerThread[threadNum] = dist[k];
                nearestKPerThread[threadNum] = k;
            }
        }

        int nearest_k = nearestKPerThread[0];
        int minDist = minDistPerThread[0];
        for (int j = 0; j < minDistPerThread.size(); j++) {
            if (minDist > minDistPerThread[j]) {
                minDist = minDistPerThread[j];
                nearest_k = nearestKPerThread[j];
            }
        }
        used[nearest_k] = 1;

#pragma omp parallel for default(none) shared(n, used, dist, graph, nearest_k)
        for (int v = 0; v < n; v++) {
            if (graph[nearest_k][v] >= 0) {
                dist[v] = min(dist[v], dist[nearest_k] + graph[nearest_k][v]);
            }
        }
    }
}

void readGraph(const string &in_path) {
    ifstream in(in_path);
    in >> n;
    graph.resize(n, vector<int>(n));
    dist.resize(n, INF);
    used.resize(n, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            in >> graph[i][j];
        }
    }
}

void writeDist(const string &out_path) {
    ofstream out(out_path);
    for (int i = 0; i < n; i++) {
        if (dist[i] == INF) {
            out << "INF ";
        } else {
            out << dist[i] << " ";
        }
    }
}

int main(int argc, char **argv) {

    if (argc < 4) {
        cout << "usage: path1 start path2\n"
                "path1 - path to graph\n"
                "start - first vertex for dijkstra\n"
                "path2 - path to output distance matrix\n";
        return 0;
    }

    string in(argv[1]);
    string out(argv[3]);
    int start = atoi(argv[2]);

    readGraph(in);

    double startTime = omp_get_wtime();
    dijkstra(start);
    double endTime = omp_get_wtime();

    writeDist(out);

    cerr << n << " " << endTime - startTime << endl;
    return 0;
}
