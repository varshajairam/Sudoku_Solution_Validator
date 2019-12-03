#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <chrono>
#define num_threads 27
using namespace std;
using namespace std::chrono;
int sudoku[9][9] = {
{6, 2, 4, 5, 3, 9, 1, 8, 7},
{5, 1, 9, 7, 2, 8, 6, 3, 4},
{8, 3, 7, 6, 1, 4, 2, 9, 5},
{1, 4, 3, 8, 6, 5, 7, 2, 9},
{9, 5, 8, 2, 4, 7, 3, 6, 1},
{7, 6, 2, 3, 9, 1, 4, 5, 8},
{3, 7, 1, 9, 5, 6, 8, 4, 2},
{4, 9, 6, 1, 8, 2, 5, 7, 3},
{2, 8, 5, 4, 7, 3, 9, 1, 6}
};
pthread_t threads[num_threads];
int thread_check[num_threads] = {0};
typedef struct {
int row;
int column;
} inputParams;
int threadId = 0;
void *checkRow(void* rowData) {
auto *params = (inputParams*) rowData;
int row = params->row;
int col = params->column;
if (col != 0 || row > 8) {
cerr << "Invalid row or column for subsection! row= "<< row << "column= "<< col;
pthread_exit(nullptr);
}
int rowValidity[9] = {0};
int i;
for (i = 0; i < 9; i++) {
int value = sudoku[row][i];
if (value < 1 || value > 9 || rowValidity[value - 1] == 1) {
pthread_exit(nullptr);
} else {
rowValidity[value - 1] = 1;
}
}
thread_check[9 + row] = 1;
pthread_exit(nullptr);
}
void *checkColumn(void* colData) {
auto *params = (inputParams*) colData ;
int row = params->row;
int col = params->column;
if (row != 0 || col > 8) {
cerr << "Invalid row or column for subsection! row= "<< row << "column= "<< col;
pthread_exit(nullptr);
}
int columnValidity[9] = {0};
int i;
for (i = 0; i < 9; i++) {
int value = sudoku[i][col];
if (value < 1 || value > 9 || columnValidity[value - 1] == 1) {
pthread_exit(nullptr);
} else {
columnValidity[value - 1] = 1;
}
}
thread_check[18 + col] = 1;
pthread_exit(nullptr);
}
void *checkSubGrid(void* gridData) {
auto *params = (inputParams*) gridData;
int row = params->row;
int col = params->column;
if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
cerr << "Invalid row or column for subsection! row= "<< row << "column= "<< col;
pthread_exit(nullptr);
}
int subGridValidity[9] = {0};
int i, j;
for (i = row; i < row + 3; i++) {
for (j = col; j < col + 3; j++) {
int value = sudoku[i][j];
if (value < 1 || value > 9 || subGridValidity[value - 1] == 1) {
pthread_exit(nullptr);
} else {
subGridValidity[value - 1] = 1;
}
}
}
thread_check[row + col/3] = 1;
pthread_exit(nullptr);
}
int main() {
steady_clock::time_point start_time_threads = steady_clock::now();
for (int i = 0; i < 9; i++) {
for (int j = 0; j < 9; j++) {
if (i%3 == 0 && j%3 == 0) {
auto *gridData = new inputParams[sizeof(inputParams)];
gridData->row = i; gridData->column = j;
pthread_create(&threads[threadId++], nullptr, checkSubGrid, gridData);
}
if (i == 0) {
auto *columnData = new inputParams[sizeof(inputParams)];
columnData->row = i;
columnData->column = j;
pthread_create(&threads[threadId++], nullptr, checkColumn, columnData); }
if (j == 0) {
auto *rowData = new inputParams[sizeof(inputParams)];
rowData->row = i;
rowData->column = j;
pthread_create(&threads[threadId++], nullptr, checkRow, rowData); // row threads
}
}
}
for (auto & thread : threads) {
pthread_join(thread, nullptr);
}
for (int i : thread_check) {
if (i == 0) {
cout << "The solution for this Sudoku puzzle is invalid!" << endl;
// Compute and return the elapsed time in milliseconds.
steady_clock::time_point end_time_threads = steady_clock::now();
duration<double> elapsed_time_threads = duration_cast<duration<double>>(end_time_threads - start_time_threads);
cout << endl << "Total time using 27 threads: " << elapsed_time_threads.count() << " seconds" << endl;
return 0;
}
}
cout << "The solution for this Sudoku puzzle is valid!" << endl;
steady_clock::time_point end_time_threads = steady_clock::now();
duration<double> elapsed_time_threads = duration_cast<duration<double>>(end_time_threads - start_time_threads);
cout << endl << "Total time using 27 threads: " << elapsed_time_threads.count() << " seconds" << endl;
return 0;
}