#ifndef PARALLEL_PROGRAMMING_2_JACOBIMPI_H
#define PARALLEL_PROGRAMMING_2_JACOBIMPI_H

#include <vector>
#include "mpi.h"
#include <string>
#include <fstream>

typedef long double ld;

class QuickSortMPI {
public:
    QuickSortMPI(int argc, char **argv);

    ~QuickSortMPI();

    double run();

private:

    enum consts {
        MAIN_PROCESS = 0,
    };

    enum tags {
        ARRAY,
        SIZE,
        INIT,
        ORDER,
    };

    int full_array_size = 0;
    int *full_array = nullptr;
    int MPI_initial_size;
    int MPI_initial_rank;
    int *array_working_part = nullptr;
    int array_size = 0;
    int split_pos = 0;
    double calc_time = -1;
    int order = 0;

    int argc;
    char **argv;

    std::ofstream log;

    void readInitialData();

    void rearrangePart(int pivot);

    void startSolve(MPI_Comm current_communicator);

    void printInfo();

    void prepareMPI();

    void stopMPI();

    void mainProcessRun();

    void otherProcessRun();

    void writeResult();

    void sendInitialData();

    void copyInitialData();

    void waitInitialData();

    void sendResult();

    void recvResult();

    void quickSort(int from, int to);

    int getPivot(int from, int to);

    void broadcastPivot(int &pivot, MPI_Comm current_communicator);

    void updateArr(int rank, MPI_Comm communicator);
};
