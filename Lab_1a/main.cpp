#include <iostream>
#include <windows.h>
#include <omp.h>
#include <vector>
#include "utils.h"
#include "multiplier.h"

using namespace std;
using namespace utils;
using namespace multiplier;

double multiply(int rowsA, int columnsA_rowsB, int columnsB, int mode, int chunkSize) {

    string file1 = "matrix1.txt";
    string file2 = "matrix2.txt";

    createFile(file1, rowsA, columnsA_rowsB);
    createFile(file2, columnsA_rowsB, columnsB);

    int maxThreadNum = 4;
    omp_set_num_threads(maxThreadNum);
    vector<vector<double>> a = loadMatrix(file1);
    vector<vector<double>> b = loadMatrix(file2);

    vector<vector<double>> result;
    int rows = a.size();
    int inter = b.size();
    int columns = b[0].size();
    char buf[1000];
    int res = -1;
    cout << "Begin multiply" << endl;
    ULONGLONG startTime = GetTickCount(), timeMultiply;
    switch (mode) {
        case (0): {
            result = multiplyInOneThead(a, b);
            timeMultiply = GetTickCount() - startTime;
            res = snprintf(buf, sizeof(buf), "ONE THREAD MODE: %I64dx%I64d on %I64dx%I64d, Time: %I64d milliseconds\n",
                           rows, inter, inter, columns, timeMultiply);
            break;
        }
        case (1): {
            result = multiplyParallelStatic(a, b, chunkSize);
            timeMultiply = GetTickCount() - startTime;
            res = snprintf(buf, sizeof(buf),
                           "STATIC MODE (chunkSize = %d) : %I64dx%I64d on %I64dx%I64d, Time: %I64d milliseconds\n",
                           chunkSize, rows, inter, inter, columns, timeMultiply);
            break;
        }
        case (2): {
            result = multiplyParallelDynamic(a, b, chunkSize);
            timeMultiply = GetTickCount() - startTime;
            res = snprintf(buf, sizeof(buf),
                           "DYNAMIC MODE (chunkSize = %d) : %I64dx%I64d on %I64dx%I64d, Time: %I64d milliseconds\n",
                           chunkSize, rows, inter, inter, columns, timeMultiply);
            break;
        }
        case (3): {
            result = multiplyParallelGuided(a, b, chunkSize);
            timeMultiply = GetTickCount() - startTime;
            res = snprintf(buf, sizeof(buf),
                           "GUIDED MODE (chunkSize = %d) : %I64dx%I64d on %I64dx%I64d, Time: %I64d milliseconds\n",
                           chunkSize, rows, inter, inter, columns, timeMultiply);
            break;
        }
        default:
            break;
    }

    string out = "error!";
    if (res >= 0 && res < sizeof(buf))
        out = buf;
    cout << out << endl;
    return timeMultiply;
};

int main() {

    int rowsA = 1000;
    int columnsA_rowsB = 1000;
    int columnsB = 2000;
    int mode = 3;
    int chunkSize = 1;
    int execute_count = 5;
    ULONGLONG time = 0;

    for (int i = 0; i < execute_count; ++i) {
        time += multiply(rowsA, columnsA_rowsB, columnsB, mode, chunkSize);
    }

    time = time / execute_count;
    cout << "average time: " << time << endl;

    return 0;
}