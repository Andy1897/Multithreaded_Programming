#pragma once

#include <cstdlib>
#include <string>

class Matrix {
public:
    int rows;
    int columns;
    int **values;

    Matrix(int rows, int columns) : rows(rows), columns(columns) {
        values = new int *[rows];
        values[0] = new int[rows * columns];
        for (int i = 1; i != rows; ++i) {
            values[i] = new int[columns];
        }
    }

    bool fillRandom() const {
        if (rows == 0) {
            return false;
        }
        for (int i = 0; i != rows; ++i) {
            for (int j = 0; j != columns; ++j) {
                values[i][j] = rand() % 10;
            }
        }
        return true;
    }
};