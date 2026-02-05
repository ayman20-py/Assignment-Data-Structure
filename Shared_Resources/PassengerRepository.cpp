#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include ".\PassengerNode.cpp"

using namespace std;

// System constants
const string csvFilePath = "Shared_Resources\\FlightPassengerData.csv";
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

    const size_t fileBufferSize = 1 << 20;
    static char fileBuffer[fileBufferSize];
    csvInputFile.rdbuf()->pubsetbuf(fileBuffer, fileBufferSize);

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

    while (getline(csvInputFile, currentLine)) {
        size_t firstCommaIndex = currentLine.find(',');
        if (firstCommaIndex == string::npos) {
            continue;
        }

        size_t secondCommaIndex = currentLine.find(',', firstCommaIndex + 1);
        if (secondCommaIndex == string::npos) {
            continue;
        }

        size_t thirdCommaIndex = currentLine.find(',', secondCommaIndex + 1);
        if (thirdCommaIndex == string::npos) {
            continue;
        }

        size_t fourthCommaIndex = currentLine.find(',', thirdCommaIndex + 1);
        if (fourthCommaIndex == string::npos) {
            continue;
        }

        string passengerId = currentLine.substr(0, firstCommaIndex);
        string passengerName = currentLine.substr(firstCommaIndex + 1, secondCommaIndex - firstCommaIndex - 1);
        string seatRowString = currentLine.substr(secondCommaIndex + 1, thirdCommaIndex - secondCommaIndex - 1);
        string seatColumnString = currentLine.substr(thirdCommaIndex + 1, fourthCommaIndex - thirdCommaIndex - 1);
        string passengerClass = currentLine.substr(fourthCommaIndex + 1);

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

void savePassengerCSV(const PassengerLinkedList& passengerLinkedList) {
    passengerLinkedList.writeToCSV(csvFilePath);
}
