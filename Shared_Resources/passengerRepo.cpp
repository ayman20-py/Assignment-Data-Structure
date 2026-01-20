#include <iostream>
#include <fstream>
#include <sstream>
#include ".\passengerNode.cpp"
using namespace std;

// System constants
const string csvFilePath = "C:\\Users\\User\\Dev\\C++\\Assignment-Data-Structure\\Dataset\\flight_passenger_data.csv.csv";
const int totalRows = 30;
const int totalColumns = 6;

const char availableSeatMarker = 'O';
const char occupiedSeatMarker = 'X';

struct SeatBookingInfo {
    string passengerId;
    string passengerName;
    string passengerClass;
    bool isOccupied;
};

SeatBookingInfo mostRecentBookings[totalRows][totalColumns];

struct PassengerCSV{
    string passengerID;
    string name;
    int seatRow;
    char searCol;
    string pClass;
};

int convertColumnCharToIndex(char columnChar) {
    return columnChar - 'A';
}

PassengerLinkedList readPassengerCSV() {
    ifstream csvInputFile(csvFilePath);
    PassengerLinkedList passengerLinkedList;

    if (!csvInputFile.is_open()) {
        cout << "Could not open the file: " << csvFilePath << endl;
        return passengerLinkedList;
    }

    // Initializing the mostRecentBookings array to all seats available
    for (int rowIndex =0; rowIndex < totalRows; rowIndex++){
        for (int columnIndex=0; columnIndex < totalColumns; columnIndex++) {
            mostRecentBookings[rowIndex][columnIndex].isOccupied = false;
        }
    }

    string currentLine;
    int totalRecordsRead = 0;

    // Skip the header line
    getline(csvInputFile, currentLine);

    while(getline(csvInputFile, currentLine)) {
        stringstream lineStream(currentLine);
        string passengerId;
        string passengerName; 
        string seatRowString;
        string seatColumnString;
        string passengerClass;

        getline(lineStream, passengerId, ',');
        getline(lineStream, passengerName, ',');
        getline(lineStream, seatRowString, ',');
        getline(lineStream, seatColumnString, ',');
        getline(lineStream, passengerClass, ',');

        // Convert seat row to int and making it as index
        int seatRow = stoi(seatRowString) - 1;
        int seatColumn = convertColumnCharToIndex(seatColumnString[0]);

        // Appending all of the data into the linked list
        passengerLinkedList.append(passengerId, passengerName, seatRow, seatColumn, 1, passengerClass);
       
    }
    csvInputFile.close();

    return passengerLinkedList;
}