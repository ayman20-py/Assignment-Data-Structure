#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include "../../Shared_Resources/PassengerNode.cpp"
#include "../../Shared_Resources/PassengerRepository.cpp"

using namespace std;

// Setting up the global variables
PassengerLinkedList passengerLinkedList;
const char availableSeatMarker = 'O';
const char occupiedSeatMarker = 'X';

// This function will return either 'O' or 'X' depending if the seat being checked is available or not
char checkOccupiedSeat(PassengerNode passenger) {
    if (passenger.passengerName != "") {
        return occupiedSeatMarker;
    }
    return availableSeatMarker;

}

// This function will only check the seating grid of a single plane at time
// Since there are a total of 79 planes there is an estimate of 2,700 lines
// which can be really hard to be readable on a terminal based interface.
void linkedListSeatingGrid(PassengerNode passengerList[][6], int currentPlane) {

    cout << "===============================" << endl;
    cout << "== PLANE " << currentPlane << endl;
    cout << "===============================" << endl;
    cout << "   ";
    for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
        cout << " " << convertColumnIndexToChar(columnIndex) << "  ";
    }
    cout << endl;
    // Rendering the first class passengers
    cout << "---------- First Class (Rows 1 - 3) ----------" << endl;
    for (int rowIndex=0; rowIndex<3; rowIndex++) {
        cout << (rowIndex+1) << "   ";
        for (int columnIndex=0; columnIndex < totalColumns; columnIndex++) {
            // Checking to see if there is someone occupying the seat.
            cout  << checkOccupiedSeat(passengerList[rowIndex][columnIndex]) << "   ";
        }
        cout << endl;
    }
    // Rendering the bussiness class passengers
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
    // Rendering the economy class passengers
    cout << "---------- Economy Class (Rows 11 - 30) ----------" << endl;
    for (int rowIndex=10; rowIndex<30; rowIndex++) {
        cout << (rowIndex+1) << "  ";
        for (int columnIndex=0; columnIndex < totalColumns; columnIndex++) {
            // Checking to see if there is someone occupying the seat.
            cout << checkOccupiedSeat(passengerList[rowIndex][columnIndex]) << "   ";
        }
        cout << endl;
    }

    // Including the legend so it is easier to cross-check in case of consfusion.
    cout << "-------Legend--------" << endl;
    cout << "Occupied Seat: " << occupiedSeatMarker << endl;
    cout << "Available Seat: " << availableSeatMarker << endl;
}

