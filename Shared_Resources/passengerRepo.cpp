#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include ".\passengerNode.cpp"

using namespace std;

// System constants
const string csvFilePath = "C:\\Users\\User\\Dev\\C++\\Assignment-Data-Structure\\Dataset\\flight_passenger_data.csv.csv";
const int totalRows = 30;
const int totalColumns = 6;


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

// This function converts a zero-based column index (0-5) to a seat column character (A-F).
char convertColumnIndexToChar(int columnIndex) {
    return 'A' + columnIndex;
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
    int currentPlaneIndex = 1;
    int newPlaneIndex = currentPlaneIndex;
    int totalPlanes = -11111;

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

        // This will check whether is already someone in that seat
        // if there is then the algorithm will keep on increasing the plane index
        // until the seat on the plane is free
        while (passengerLinkedList.isSeatOccupied(seatRow, seatColumn, newPlaneIndex)) {
            newPlaneIndex++;
        }

        // Just to see how many planes are there in total
        if (newPlaneIndex > totalPlanes) { 
            totalPlanes = newPlaneIndex;
        }
        // Initializing the data of the linked list
        passengerLinkedList.init(passengerId, passengerName, seatRow, seatColumn, newPlaneIndex, passengerClass);
        newPlaneIndex = currentPlaneIndex;
       
    }
    csvInputFile.close();
    passengerLinkedList.setTotalPlanes(totalPlanes);

    return passengerLinkedList;
}
