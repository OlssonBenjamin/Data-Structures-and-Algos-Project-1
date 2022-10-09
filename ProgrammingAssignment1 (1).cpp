#include <iostream>
#include <sstream>
#include <list>

using namespace std;

struct Entry // Simple structure that contains the relevant information of an entry in the matrix
{
    pair<int, int> coords; /* coordinates are necessary for the calculation of a determinant,
                                   and allows the program to get the minor of a matrix relative to an entry
                                   by removing any nodes that are in the same column or row as the entry */
    int entry; //Value at the coordinate
    Entry(int y, int x, int num)
    {
        coords.first = y;
        coords.second = x;
        entry = num;
    }
};

struct Matrix
{
    list<Entry> matrixList_; //Store non-zero entries into a linked list
    int expectedRowSize_; //Use the expected row size to determine when the base case has been reached in the recursive computation of the determinant
    Matrix(list<Entry> matrix, int expectedRowSize)
    {
        matrixList_ = matrix;
        expectedRowSize_ = expectedRowSize;
    }
};

/* Helper function that loads the input into a linked list of Entry objects. This function was taken from Stepik Excercise 2.3
and then modified to load Entry objects into a linked list, but not creating an Entry object if the value is zero */
Matrix LoadMatrix(bool& squareMatrix)
{
    list<Entry> matrixList;
    string line;
    getline(cin, line); //Get the first line of input to determine how many numbers are in the first row of the matrix

    string token;

    int rowNum = 0;
    int size = 0;
    int currSize = 0;

    int test;


    //Get the length of the first row in the matrix by streaming the integer values into a token and incrementing size for each value
    stringstream lineStream(line);
    while(lineStream >> token)
    {
        size++;
    }

    for (int k = 0; k < size; k++)
    {
        stringstream lineStream2(line);
        currSize = 0;
        while(lineStream2 >> token)
        {
            test = stoi(token);
            if (test != 0) //Check that the number is not zero, if it is not, it can be added to the matrix
            {
                Entry temp(rowNum, currSize, test); //Add the entry to the list, using rowNum and currSize as its coordinates and test as the entry
                matrixList.push_back(temp);
            }
            currSize++;
        }
        //Matrix should be square, if it is not square, set squareMatrix to false so that main can handle the error
        if (currSize != size)
            squareMatrix = false;
        rowNum++;
        cin.clear(); //Flush buffer so line will be empty if there is no more input
        getline(cin, line);
        if (line.empty() && k != size - 1) //If the next input is empty and the loop has not reached its final value, then the matrix is missing at least one row and is not square
            squareMatrix = false;
    }
    Matrix matrix(matrixList, size);
    return matrix;
}

Matrix GetMinor(Matrix matrix, pair<int, int> removed)
{
    list<Entry> minorList; //Create a new linked list to function as the minor of the matrix in the argument
    auto iter = matrix.matrixList_.begin();
    while (iter != matrix.matrixList_.end())
    {
        bool insert = true; //Use a bool variable to determine if the entry should be added to the minor
        if (iter.operator*().coords.first == removed.first || iter.operator*().coords.second == removed.second)
            insert = false; //If the entry is in the same row or column as the entry that is being used to get the minor, do not add it to the list
        if (insert)
        {
            //Create new coordinates so that minor's determinant can be calculated correctly
            int y = iter.operator*().coords.first - 1; // The y variable will always be 1 greater than it should be because the top row of the matrix is always removed
            int x = iter.operator*().coords.second;
            if (x > removed.second)
                x = x - 1; //Only decrement the x by one if it is past the column that is being removed, otherwise its x coordinate is correct
            Entry inserted(y, x, iter.operator*().entry); //Create a new entry with the correct coordinates relative to the minor, but the same entry as before
            minorList.push_back(inserted);
        }
        iter++;
    }
    Matrix minor(minorList, matrix.expectedRowSize_ - 1);
    return minor;
}

int Determinant(Matrix matrix)
{
    int determinant = 0;
    if (matrix.expectedRowSize_ > matrix.matrixList_.size()) //An nxn matrix with less than n non-zero entries will always be zero
        return 0;
    if (matrix.expectedRowSize_ > 1) //The recursive step must continue until there is only a 1x1 matrix left
    {
        auto iter = matrix.matrixList_.begin();
        while (iter.operator*().coords.first == 0) //Iterate through the top row of the matrix and get each minor relative to that entry
        {
            int y = iter.operator*().coords.first;
            int x = iter.operator*().coords.second;
            pair<int, int> remove(y, x);
            Matrix minor = GetMinor(matrix, remove);
            if ((x + y) % 2 == 0)
                determinant += iter.operator*().entry * Determinant(minor);
            else
                determinant -= iter.operator*().entry * Determinant(minor);
            iter++;
        }
    }
    else
    {
        if (matrix.matrixList_.empty()) //If the list is empty then the value in the 1x1 matrix was zero
            return 0;
        else
            return matrix.matrixList_.front().entry; //The determinant of a 1x1 matrix is the single entry
    }
    return determinant;
}

int main()
{
    //Create boolean values to catch a non-square matrix error
    bool squareMatrix = true;
    Matrix matrix = LoadMatrix(squareMatrix); //Pass a boolean by reference so that its value can be changed without returning a boolean
    if (!squareMatrix)
        cout << "Error! Non-square matrix!" << endl;
    else
    {
        int determinant = Determinant(matrix);
        cout << determinant;
    }
    return 0;
}