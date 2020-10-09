#include <vector>
#include <iostream>
#include <omp.h>

namespace multiplier {
    vector<vector<double>> multiplyInOneThead(vector<vector<double>> &a, vector<vector<double>> &b) {
        int rows1 = a.size();
        int inter21 = b.size();
        int columns2 = b[0].size();
        vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
        for (int row = 0; row < rows1; row++) {
            for (int column = 0; column < columns2; column++) {
                double sum = 0;
                for (int inter = 0; inter < inter21; inter++) {
                    sum += a[row][inter] * b[inter][column];
                }
                result[row][column] = sum;
            }
        }
        return result;
    }

    vector<vector<double>> multiplyParallelStatic(vector<vector<double>> &a, vector<vector<double>> &b, int chunkSize) {
        int rows1 = a.size();
        int inter21 = b.size();
        int columns2 = b[0].size();
        vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
        if (rows1 == 1 && columns2 == 1) {
            double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(static, chunkSize)
            for (int inner = 0; inner < a[0].size(); inner++) {
                sum += a[0][inner] * b[inner][0];
            }
            result[0][0] = sum;
        } else if (rows1 < 4) {
#pragma omp parallel for schedule(static, chunkSize) shared(a, b)
            for (int i = 0; i < rows1 * columns2; i++) {
                int column = i % columns2;
                int row = i / columns2;
                int sum = 0;
                for (int inner = 0; inner < inter21; inner++) {
                    sum += a[row][inner] * b[inner][column];
                }
                result[row][column] = sum;
            }
        } else {
#pragma omp parallel for schedule(static, chunkSize) shared(a, b)
            for (int row = 0; row < rows1; row++) {
                for (int column = 0; column < columns2; column++) {
                    for (int inter = 0; inter < inter21; inter++) {
                        result[row][column] += a[row][inter] * b[inter][column];
                    }
                }
            }
        }
        return result;
    }

    vector<vector<double>>
    multiplyParallelDynamic(vector<vector<double>> &a, vector<vector<double>> &b, int chunkSize) {
        int rows1 = a.size();
        int inter21 = b.size();
        int columns2 = b[0].size();
        vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
        if (rows1 == 1 && columns2 == 1) {
            double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(dynamic, chunkSize)
            for (int inner = 0; inner < inter21; inner++) {
                sum += a[0][inner] * b[inner][0];
            }
            result[0][0] = sum;
        } else if (rows1 < 4) {
#pragma omp parallel for schedule(dynamic, chunkSize) shared(a, b)
            for (int i = 0; i < rows1 * columns2; i++) {
                int column = i % columns2;
                int row = i / columns2;
                int sum = 0;
                for (int inner = 0; inner < inter21; inner++) {
                    sum += a[row][inner] * b[inner][column];
                }
                result[row][column] = sum;
            }
        } else {
#pragma omp parallel for schedule(dynamic, chunkSize) shared(a, b)
            for (int row = 0; row < rows1; row++) {
                for (int column = 0; column < columns2; column++) {
                    for (int inter = 0; inter < inter21; inter++) {
                        result[row][column] += a[row][inter] * b[inter][column];
                    }
                }
            }
        }
        return result;
    }

    vector<vector<double>> multiplyParallelGuided(vector<vector<double>> &a, vector<vector<double>> &b, int chunkSize) {
        int rows1 = a.size();
        int inter21 = b.size();
        int columns2 = b[0].size();
        vector<vector<double>> result(rows1, vector<double>(columns2, 0.0));
        if (rows1 == 1 && columns2 == 1) {
            double sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(guided, chunkSize)
            for (int inner = 0; inner < inter21; inner++) {
                sum += a[0][inner] * b[inner][0];
            }
            result[0][0] = sum;
        } else if (rows1 < 4) {
#pragma omp parallel for schedule(guided, chunkSize) shared(a, b)
            for (int i = 0; i < rows1 * columns2; i++) {
                int column = i % columns2;
                int row = i / columns2;
                int sum = 0;
                for (int inner = 0; inner < inter21; inner++) {
                    sum += a[row][inner] * b[inner][column];
                }
                result[row][column] = sum;
            }
        } else {
#pragma omp parallel for schedule(guided, chunkSize) shared(a, b)
            for (int row = 0; row < rows1; row++) {
                for (int column = 0; column < columns2; column++) {
                    for (int inter = 0; inter < inter21; inter++) {
                        result[row][column] += a[row][inter] * b[inter][column];
                    }
                }
            }
        }
        return result;
    }
}