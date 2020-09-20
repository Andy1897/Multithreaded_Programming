#include "quickSortMPI.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cstring>

void readInitial(ld *&init, const std::string &path) {
    std::ifstream in(path);
    int n;
    in >> n;
    init = new ld[n];
    for (int i = 0; i < n; ++i) {
        in >> init[i];
    }
}

template<typename T>
void printArr(T *arr, int size, const std::string arrname = "", int start = 0) {
    if (arrname != "") {
        std::cout << arrname << std::endl;
    }
    for (int i = start; i < start + size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void printArr(T *arr, int size, std::ofstream &out, const std::string arrname = "", int start = 0) {
    if (arrname != "") {
        out << arrname << std::endl;
    }
    for (int i = start; i < start + size; i++) {
        out << arr[i] << " ";
    }
    out << std::endl << std::endl;
}

QuickSortMPI::QuickSortMPI(int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;
    prepareMPI();
}

QuickSortMPI::~QuickSortMPI() {
    delete[] full_array;
    delete[] array_working_part;
    stopMPI();
}

void QuickSortMPI::prepareMPI() {
    MPI_Init(&argc, &argv);
}

void QuickSortMPI::stopMPI() {
    MPI_Finalize();
}

double QuickSortMPI::run() {

    MPI_Comm_size(MPI_COMM_WORLD, &MPI_initial_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_initial_rank);

    if (__builtin_popcount(MPI_initial_size) != 1) {
        std::cout << "number of processes should be power of two.\n";
        exit(0);
    }

    if(MPI_initial_rank == MAIN_PROCESS){
        if (argc < 3) {
            std::cout << "usage: path1 path2\n"
                         "path1 - path to file which data has to be sorted,\n"
                         "path2 - path to write sorted data,\n";
            exit(0);
        }
        mainProcessRun();
    } else {
        otherProcessRun();
    }
    return calc_time;
}

void QuickSortMPI::mainProcessRun() {
    readInitialData();
    // calculation starts
    double start_time = MPI_Wtime();
    sendInitialData();
    copyInitialData();
    startSolve(MPI_COMM_WORLD);
    recvResult();
    double end_time = MPI_Wtime();
    calc_time = end_time - start_time;
    writeResult();
}

void QuickSortMPI::otherProcessRun() {
    waitInitialData();
    startSolve(MPI_COMM_WORLD);
    sendResult();
}

void QuickSortMPI::startSolve(MPI_Comm current_communicator) {

    int cur_size;
    int cur_rank;
    MPI_Comm_rank(current_communicator, &cur_rank);
    MPI_Comm_size(current_communicator, &cur_size);
    bool main_process = cur_rank == MAIN_PROCESS;

    if (cur_size == 1) {
        quickSort(0, array_size);

        return;
    }

    int pivot;
    if (main_process) {
        pivot = getPivot(0, array_size);
    }
    broadcastPivot(pivot, current_communicator);
    rearrangePart(pivot);
    updateArr(cur_rank, current_communicator);

    MPI_Comm new_comm;
    MPI_Comm_split(current_communicator, cur_rank & 1, 0, &new_comm);

    order = 2 * order + (cur_rank & 1);
    startSolve(new_comm);
}

void QuickSortMPI::rearrangePart(int pivot) {
    int i, j;
    for (i = 0, j = array_size - 1; i < j;) {
        while (i < j && array_working_part[i] < pivot) i++;
        while (i < j && array_working_part[j] >= pivot) j--;
        std::swap(array_working_part[i], array_working_part[j]);
    }
    split_pos = i;
}

void QuickSortMPI::quickSort(int from, int to) {
    std::qsort(array_working_part, to - from, sizeof(int),
               [](const void *p1, const void *p2) { return *(int *) p1 - *(int *) p2; }
    );
}

int QuickSortMPI::getPivot(int from, int to) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(from, to - 1);
    int rand_index = distribution(generator);
    return array_working_part[rand_index];
}

void QuickSortMPI::writeResult() {
    std::ofstream out(argv[2]);
    out << full_array_size << "\n";
    for (int i = 0; i < full_array_size; i++) {
        out << full_array[i] << " ";
    }
    out << "\n";
}

void QuickSortMPI::printInfo() {
    log << "rank: " << MPI_initial_rank << " - started" << std::endl;
    log << "array_working_part size: " << array_size << std::endl;
    printArr(array_working_part, array_size, log, "array_working_part");
}

void QuickSortMPI::readInitialData() {
    std::ifstream in(argv[1]);

    in >> full_array_size;

    full_array = new int[full_array_size];
    for (int i = 0; i < full_array_size; i++) {
        in >> full_array[i];
    }
    std::cerr << full_array_size << " " << MPI_initial_size << " ";
}

void QuickSortMPI::sendInitialData() {
    MPI_Request r;
    int part_size = full_array_size / MPI_initial_size;

    for (int process = 0; process < MPI_initial_size; process++) {
        if (process == MAIN_PROCESS) {
            continue;
        }
        int first = process * part_size;
        int last = (process == MPI_initial_size - 1 ? full_array_size : first + part_size);
        int size = last - first;
        // cant Isend here, because we shouldn't rewrite or delete 'size' untill it is copied to MPI buffer
        MPI_Send(&size, 1, MPI_INT, process, SIZE, MPI_COMM_WORLD);
        MPI_Isend(full_array + first, size, MPI_INT, process, INIT, MPI_COMM_WORLD, &r);
    }
}

void QuickSortMPI::waitInitialData() {
    MPI_Recv(&array_size, 1, MPI_INT, MAIN_PROCESS, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    array_working_part = new int[array_size];
    MPI_Recv(array_working_part, array_size, MPI_INT, MAIN_PROCESS, INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void QuickSortMPI::copyInitialData() {
    int part_size = full_array_size / MPI_initial_size;
    int first = MPI_initial_rank * part_size;
    int last = (MAIN_PROCESS == MPI_initial_size - 1 ? full_array_size : first + part_size);
    array_size = last - first;
    array_working_part = new int[array_size];
    memcpy(array_working_part, full_array, array_size * sizeof(int));
}

void QuickSortMPI::broadcastPivot(int &pivot, MPI_Comm current_communicator) {
    MPI_Bcast(&pivot, 1, MPI_INT, MAIN_PROCESS, current_communicator);
}

void QuickSortMPI::updateArr(int rank, MPI_Comm communicator) {
    int neighbour_diff = 1;
    bool high = rank & neighbour_diff;
    int send_size;
    int neighbour;
    int copy_from;
    int copy_to;
    int send_from;

    if (high) {
        neighbour = rank - neighbour_diff;
        send_size = split_pos;
        send_from = 0;
        copy_from = split_pos;
        copy_to = array_size;
    } else {
        neighbour = rank + neighbour_diff;
        send_size = array_size - split_pos;
        send_from = split_pos;
        copy_from = 0;
        copy_to = split_pos;
    }

    MPI_Request reqs[3];

    MPI_Isend(&send_size, 1, MPI_INT, neighbour, SIZE, communicator, &reqs[0]);
    MPI_Isend(array_working_part + send_from, send_size, MPI_INT, neighbour, ARRAY, communicator, &reqs[1]);

    int recv_size;
    MPI_Recv(&recv_size, 1, MPI_INT, neighbour, SIZE, communicator, MPI_STATUS_IGNORE);

    int *temp_array = new int[recv_size + array_size - send_size];
    MPI_Irecv(temp_array, recv_size, MPI_INT, neighbour, ARRAY, communicator, &reqs[2]);
    for (int i = copy_from; i < copy_to; i++) {
        temp_array[i - copy_from + recv_size] = array_working_part[i];
    }

    MPI_Waitall(3, reqs, MPI_STATUSES_IGNORE); // have to wait all send operations or buffers may corrupt
    delete[] array_working_part;
    array_working_part = temp_array;
    array_size += recv_size - send_size;
}

void QuickSortMPI::recvResult() {

    int size = MPI_initial_size;
    int *counts = new int[size];
    int *displacements = new int[size];
    int *orders = new int[size];
    orders[MAIN_PROCESS] = order;
    counts[MAIN_PROCESS] = array_size;
    displacements[MAIN_PROCESS] = 0;

    for (int process = 0, p_size, p_order; process < size; process++) {
        if (process == MAIN_PROCESS) {
            continue;
        }
        MPI_Recv(&p_size, 1, MPI_INT, process, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&p_order, 1, MPI_INT, process, ORDER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        counts[process] = p_size;
        orders[process] = p_order;
    }

    for (int p_order = 0, offset = 0; p_order < size; p_order++) {
        for (int process = 0; process < size; process++) {
            if (orders[process] == p_order) {
                displacements[process] = offset;

                offset += counts[process];
                break;
            }
        }
    }

    MPI_Gatherv(array_working_part, array_size, MPI_INT, full_array, counts, displacements, MPI_INT, MAIN_PROCESS,
                MPI_COMM_WORLD);

    delete[] orders;
    delete[] counts;
    delete[] displacements;
}

void QuickSortMPI::sendResult() {
    MPI_Request r;
    MPI_Isend(&array_size, 1, MPI_INT, MAIN_PROCESS, SIZE, MPI_COMM_WORLD, &r);
    MPI_Isend(&order, 1, MPI_INT, MAIN_PROCESS, ORDER, MPI_COMM_WORLD, &r);

    MPI_Gatherv(array_working_part, array_size, MPI_INT, nullptr, nullptr, nullptr, MPI_INT, MAIN_PROCESS,
                MPI_COMM_WORLD);
}
