#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include <chrono>

#define num_threads 11

using namespace std;
using namespace std::chrono;

// Sudoku puzzle to be validated
int sudoku[9][9] = {
        {4, 3, 5, 2, 6, 9, 7, 8, 1},
        {6, 8, 2, 5, 7, 1, 4, 9, 3},
        {1, 9, 7, 8, 3, 4, 5, 6, 2},
        {8, 2, 6, 1, 9, 5, 3, 4, 7},
        {3, 7, 4, 6, 8, 2, 9, 1, 5},
        {9, 5, 1, 7, 4, 3, 6, 2, 8},
        {5, 1, 9, 3, 2, 6, 8, 7, 4},
        {2, 4, 8, 9, 5, 7, 1, 3, 6},
        {7, 6, 3, 4, 1, 8, 2, 5, 9}
};

// Declaration of array of thread IDs
pthread_t threads[num_threads];
// Final array containing results of 15 validations
int thread_result[num_threads] = {0};

// Struct that stores thread arguments
typedef struct{
    int row;
    int column;
} inputParams;

int threadId = 0;

// Function to determine if a row contains numbers 1-9 without duplicates.
void *checkRow(void* rowData) {
    // Confirm that parameters indicate a valid row subsection
    auto *params = (inputParams*) rowData;
    int row = params->row;
    int col = params->column;
    if (col != 0 && row != 0) {
        cerr << "Invalid row or column for row subsection! Row: " << row << " Column: " << col;
        pthread_exit(nullptr);
    }

    // Check if numbers 1-9 only appear once in the row
    for (int i = row; i < 9; i++) {
        int rowValidity[9] = {0}; // Contains 1 if valid, 0 if not.
        for (int j = 0; j < 9; j++) {
            int value = sudoku[i][j];
            if (value < 1 || value > 9 || rowValidity[value - 1] == 1) {
                pthread_exit(nullptr);
            } else {
                rowValidity[value - 1] = 1;
            }
        }
    }

    // Row subsection is valid if this point is reached.
    thread_result[9] = 1; // Maps result of each row validation to indices 9-11 of the validity array
    pthread_exit(nullptr);
}

// Function to determine if a column contains numbers 1-9 without duplicates.
void *checkColumn(void* colData) {
    // Confirm that parameters indicate a valid col subsection
    auto *params = (inputParams*) colData;
    int row = params->row;
    int col = params->column;
    if (row != 0 && col != 0) {
        cerr << "Invalid row or column for column subsection! Row: " << row << " Column: " << col;
        pthread_exit(nullptr);
    }

    // Check if numbers 1-9 only appear once in the column
    for (int i = col; i < 9; i++) {
        int columnValidity[9] = {0}; // Contains 1 if valid, 0 if not.
        for (int j = 0; j < 9; j++) {
            int value = sudoku[j][i];
            if (value < 1 || value > 9 || columnValidity[value - 1] == 1) {
                pthread_exit(nullptr);
            } else {
                columnValidity[value - 1] = 1;
            }
        }
    }

    // Column subsection is valid if this point is reached.
    thread_result[10] = 1; // Maps result of each column validation to indices 12-14 of the validity array
    pthread_exit(nullptr);
}

// Function to determine if a 3x3 subgrid contains numbers 1-9 without duplicates.
void *checkSubGrid(void* gridData) {
    // Confirm that parameters indicate a valid 3x3 subgrid
    auto *params = (inputParams*) gridData;
    int row = params->row;
    int col = params->column;
    if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
        cerr << "Invalid row or column for row subsection! Row: " << row << " Column: " << col;
        pthread_exit(nullptr);
    }
    int subGridValidity[9] = {0}; // Contains 1 if valid, 0 if not.
    for (int i = row; i < row + 3; i++) {
        for (int j = col; j < col + 3; j++) {
            int value = sudoku[i][j];
            if (value < 1 || value > 9 || subGridValidity[value - 1] == 1) {
                pthread_exit(nullptr);
            } else {
                subGridValidity[value - 1] = 1;
            }
        }
    }

    // 3x3 subgrid is valid if this point is reached 
    thread_result[row + col/3] = 1; // Maps result of each subgrid validation to indices 0-8 of the validity array
    pthread_exit(nullptr);
}

int main() {

    // Start clock
    steady_clock::time_point start_time_threads = steady_clock::now();

    // Creating 11 threads

    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(i == 0 && j == 0){
                auto *rowData = new inputParams[sizeof(inputParams)];
                rowData->row = i;
                rowData->column = j;
                pthread_create(&threads[threadId++], nullptr, checkRow, rowData); // 1 row thread
            }
            if(i == 0 && j == 0){
                auto *columnData = new inputParams[sizeof(inputParams)];
                columnData->row = i;
                columnData->column = j;
                pthread_create(&threads[threadId++], nullptr, checkColumn, columnData); // 1 column thread
            }
            if(i%3 == 0 && j%3 == 0){
                auto *gridData = new inputParams[sizeof(inputParams)];
                gridData->row = i;
                gridData->column = j;
                pthread_create(&threads[threadId++], nullptr, checkSubGrid, gridData); // 9 grid threads
            }
        }
    }
    
    // Wait for all threads to finish execution
    for (auto & thread : threads) {
        pthread_join(thread, nullptr);      
    }

    // If the result of any row/column/grid validation is 0, then the Sudoku solution is invalid.
    for (int i : thread_result) {
        if (i == 0) {
            cout << "The solution for this Sudoku puzzle is invalid!" << endl;
            // Compute and return the elapsed time in milliseconds.
            steady_clock::time_point end_time_threads = steady_clock::now();
            duration<double> elapsed_time_threads = duration_cast<duration<double>>(end_time_threads - start_time_threads);

            cout << endl << "Total time using 11 threads: " << elapsed_time_threads.count() << " seconds" << endl;
            return 0;
        }
    }
    cout << "The solution for this Sudoku puzzle is valid!" << endl;
    // Compute and return the elapsed time in milliseconds.
            steady_clock::time_point end_time_threads = steady_clock::now();
            duration<double> elapsed_time_threads = duration_cast<duration<double>>(end_time_threads - start_time_threads);

            cout << endl << "Total time using 11 threads: " << elapsed_time_threads.count() << " seconds" << endl;
    return 0;
}