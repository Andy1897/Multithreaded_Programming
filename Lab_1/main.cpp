#include <iostream>
#include <chrono>
#include "Matrix.cpp"
#include "Multiplier.cpp"

using namespace std;

double multiply(int rows, int columns, int type) {

    auto A = new Matrix(rows, columns);
    auto B = new Matrix(rows, columns);
    A->fillRandom();
    B->fillRandom();
    Multiplier *multiplier;

    switch (type) {
        case 0:
            multiplier = new Multiplier(A, B);
            break;
        case 1:
            multiplier = new StaticScheduleMultiplier(A, B);
            break;
        case 2:
            multiplier = new DynamicScheduleMultiplier(A, B);
            break;
        case 3:
            multiplier = new GuidedScheduleMultiplier(A, B);
            break;
    }
    auto start = chrono::high_resolution_clock::now();
    multiplier->multiply();
    auto finish = chrono::high_resolution_clock::now();
    return chrono::duration<double>(finish - start).count();
}

int main() {
    int rows = 2000;
    int columns = 2000;
    int type = 0;
    int execute_count = 5;
    double time = 0;

    for (int i = 0; i < execute_count; ++i) {
        time += multiply(rows, columns, type);
    }

    time = time / execute_count;
    std::cout << "calculation time: " << time << std::endl;

    return 0;
}
