#include <string>
#include <iostream>
#include "..\..\Shared_Resources\passengerRepo.cpp"
#include "..\..\Shared_Resources\passengerNode.cpp"

using namespace std;

PassengerLinkedList passengerLinkedList;
const char availableSeatMarker = 'O';
const char occupiedSeatMarker = 'X';

char checkOccupiedSeat(PassengerNode passenger) {
    if (passenger.passengerName != "") {
        return occupiedSeatMarker;
    }
    return availableSeatMarker;

}

void linkedListSeatManifest(int currentPlane) {
    int totalPlanes = 79;
    PassengerNode passengerList[30][6];

    // Populating the passengerList 2D array
    passengerLinkedList.getPassengersFromPlane(passengerList, currentPlane);

    cout << "===============================" << endl;
    cout << "== PLANE " << currentPlane << endl;
    cout << "===============================" << endl;
    cout << "   ";
    for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
        cout << " " << convertColumnIndexToChar(columnIndex) << "  ";
    }
    cout << endl;
    cout << "---------- First Class (Rows 1 - 3) ----------" << endl;
    for (int rowIndex=0; rowIndex<3; rowIndex++) {
        cout << (rowIndex+1) << "   ";
        for (int columnIndex=0; columnIndex < totalColumns; columnIndex++) {
            // Checking to see if there is someone occupying the seat.
            cout  << checkOccupiedSeat(passengerList[rowIndex][columnIndex]) << "   ";
        }
        cout << endl;
    }
    cout << "---------- Business Class (Rows 4 - 10) ----------" << endl;
    for (int rowIndex=3; rowIndex<10; rowIndex++) {
        if ((rowIndex+1) == 10) {
            cout << (rowIndex+1) << "  ";
        } else { cout << rowIndex+1 << "   "; }
        for (int columnIndex=0; columnIndex < totalColumns; columnIndex++) {
            // Checking to see if there is someone occupying the seat.
            cout << checkOccupiedSeat(passengerList[rowIndex][columnIndex]) << "   ";
        }
        cout << endl;
    }
    cout << "---------- Economy Class (Rows 11 - 30) ----------" << endl;
    for (int rowIndex=10; rowIndex<30; rowIndex++) {
        cout << (rowIndex+1) << "  ";
        for (int columnIndex=0; columnIndex < totalColumns; columnIndex++) {
            // Checking to see if there is someone occupying the seat.
            cout << checkOccupiedSeat(passengerList[rowIndex][columnIndex]) << "   ";
        }
        cout << endl;
    }
}

int main() {
    passengerLinkedList = readPassengerCSV();
    linkedListSeatManifest(12);
    return 0;
}
