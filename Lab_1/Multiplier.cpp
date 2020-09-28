#pragma once

#include "Matrix.cpp"

class Multiplier {
public:
    Matrix *a;
    Matrix *b;

    Multiplier(Matrix *a, Matrix *b) : a(a), b(b) {}

    virtual Matrix *multiply() {
        auto result = new Matrix(a->rows, b->columns);
        for (int i = 0; i < result->rows; ++i) {
            for (int j = 0; j < result->columns; ++j) {
                result->values[i][j] = getElement(i, j);
            }
        }
        return result;
    }

protected:
    int getElement(int row, int column) const {
        int element = 0;
        for (int i = 0; i < a->columns; ++i) {
            element += a->values[row][i] * b->values[i][column];
        }
        return element;
    }
};

class StaticScheduleMultiplier : public Multiplier {
public:
    StaticScheduleMultiplier(Matrix *a, Matrix *b) : Multiplier(a, b) {}

    Matrix *multiply() override {
        int rows = a->rows;
        int columns = b->columns;
        int tasks = rows * columns;
        auto result = new Matrix(a->rows, b->columns);

#pragma omp parallel for schedule(static)
        for (int task = 0; task < tasks; ++task) {
            int row = task / columns;
            int column = task % columns;
            result->values[row][column] = getElement(row, column);
        }
        return result;
    }
};

class DynamicScheduleMultiplier : public Multiplier {
public:
    DynamicScheduleMultiplier(Matrix *a, Matrix *b) : Multiplier(a, b) {}

    Matrix *multiply() override {
        int rows = a->rows;
        int columns = b->columns;
        int tasks = rows * columns;
        auto result = new Matrix(a->rows, b->columns);

#pragma omp parallel for schedule(dynamic)
        for (int task = 0; task < tasks; ++task) {
            int row = task / columns;
            int column = task % columns;
            result->values[row][column] = getElement(row, column);
        }
        return result;
    }
};

class GuidedScheduleMultiplier : public Multiplier {
public:
    GuidedScheduleMultiplier(Matrix *a, Matrix *b) : Multiplier(a, b) {}

    Matrix *multiply() override {
        int rows = a->rows;
        int columns = b->columns;
        int tasks = rows * columns;
        auto *result = new Matrix(a->rows, b->columns);

#pragma omp parallel for schedule(guided)
        for (int task = 0; task < tasks; ++task) {
            int row = task / columns;
            int column = task % columns;
            result->values[row][column] = getElement(row, column);
        }
        return result;
    }
};
