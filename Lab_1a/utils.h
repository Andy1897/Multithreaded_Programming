#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

namespace utils {
    void createFile(const string &filename, int rows, int columns) {
        ofstream fileout(filename, ios_base::out | ios_base::trunc);
        fileout << rows << " " << columns << endl;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                fileout << (double) (rand()) / rand() << " ";
            }
            fileout << endl;
        }
        fileout.close();
    }

    vector<vector<double>> loadMatrix(const string &filename) {
        vector<vector<double>> matrix;
        int rows, columns;
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << ".\n";
            return matrix;
        }
        file >> rows >> columns;
        if (rows < 1 || columns < 1) {
            cerr << "The number of rows and columns must be greater than 0.\n";
            return matrix;
        }
        matrix.resize(rows);
        for (auto &row : matrix) {
            row.resize(columns);
        }
        for (auto &row : matrix) {
            for (auto &num : row) {
                file >> num;
            }
        }
        file.close();
        return matrix;
    }
}