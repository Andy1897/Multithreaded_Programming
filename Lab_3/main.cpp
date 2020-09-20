#include <iostream>
#include "quickSortMPI.h"

int main(int argc, char **argv) {

    QuickSortMPI qsmpi(argc, argv);
    double time = qsmpi.run();

    if (time >= 0) {
        std::cout << "calculation time: " << time << std::endl;
        std::cerr << time << std::endl;
    }
    return 0;
}
