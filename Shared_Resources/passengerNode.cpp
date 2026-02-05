#pragma once
#include <iostream>
#include <string>
#include <string>
#include <array>
using namespace std;

// This structure represents a single passenger reservation node in the linked list.
struct PassengerNode {
    string passengerId;
    string passengerName;
    int seatRow;
    int seatColumn;
    int planeNum;
    string passengerClass;
    PassengerNode* next;

    // ADD THIS DEFAULT CONSTRUCTOR
        PassengerNode() 
            : passengerId(""), passengerName(""), seatRow(0), seatColumn(0), 
            planeNum(0), passengerClass(""), next(nullptr) {}

    // Constructor to initialize a passenger node with provided details.
    PassengerNode(string id, string name, int row, int column, int planeNumber, string passengerClassType = "Economy") {
        passengerId = id;
        passengerName = name;
        seatRow = row;
        seatColumn = column;
        passengerClass = passengerClassType;
        planeNum = planeNumber;
        next = nullptr;
    }
};

int PassengerconvertColumnCharToIndex(char columnChar) {
    return columnChar - 'A';
}

// This function converts a zero-based column index (0-5) to a seat column character (A-F).
char PassengerconvertColumnIndexToChar(int columnIndex) {
    return 'A' + columnIndex;
}


class PassengerLinkedList {
private: 
    PassengerNode* head;
    int totalPlanes;

    // Helper function to render seating sections
    void renderSeatingRows(string sectionName, int startRow, int endRow, PassengerNode passengerList[30][6]) {
        cout << "---------- " << sectionName << " ----------" << endl;
        for (int i = startRow; i <= endRow; i++) {
            if (i + 1 < 10) cout << (i + 1) << "   ";
            else cout << (i + 1) << "  ";

            for (int j = 0; j < 6; j++) {
                if (passengerList[i][j].passengerName != "") {
                    cout << "X   ";
                } else {
                    cout << "O   ";
                }
            }
            cout << endl;
        }
    }

public: 
    PassengerLinkedList() {
        head = nullptr;
        totalPlanes = 0;
    }

    void init(string id, string name, int row, int column, int planeNum, string passengerClassType = "Economy") {
        PassengerNode* newNode = new PassengerNode(id, name, row, column, planeNum, passengerClassType);

        if (head == nullptr) {
            head = newNode;
            return;
        }

        // Find last node
        PassengerNode* current = head;
        while (current->next != nullptr) {
            current = current->next; // Move to the next Passenger Node
        }

        current->next = newNode;
    }

    int getSize() {
        int count = 0;

        PassengerNode* current = head;
        
        while (current != nullptr) {
            count++;
            current = current-> next;
        }
        return count;
    }

    bool doesPassengerExists(string id) {
        PassengerNode* current = head;

        while (current != nullptr) {
            if (current->passengerId == id) {
                return true;
            }
            current = current->next;
        }

        return false;
    }

    // Check if a specific seat on a plane is occupied.
    bool isSeatOccupied(int seatRow, int columnChar, int planeNumber) {
        PassengerNode* current = head;
        
        while (current != nullptr) {
            if (current->seatRow == seatRow && current->seatColumn == columnChar && current->planeNum == planeNumber) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Display all the information about the passengers in the whole dataset
    void displayAllPassengers() {
        PassengerNode* current = head;

        cout << "==========PASSENGER MANIFEST==========" << endl;
        cout << "ID        Name                     Seat     Class" << endl;
        while (current != nullptr) {
            cout << current->passengerId << "    ";
            cout << current->passengerName << string(25 - current->passengerName.length(), ' ');
            cout << (current->seatRow+ 1)<< PassengerconvertColumnIndexToChar(current->seatColumn) << string(8 - to_string(current->seatRow).length(), ' ');
            cout << current->passengerClass;
            cout << endl;

            current = current -> next;
        }

    }

    // This function will return a 2D array containing the passengers in a specific plane number
    void getPassengersFromPlane(PassengerNode passengerList[][6], int planeNumber) {
        // Initialize passengerList with default nodes
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 6; j++) {
                passengerList[i][j] = PassengerNode();
            }
        }

        PassengerNode* current = head;
        while (current != nullptr) {
            if (current->planeNum == planeNumber) {
                passengerList[current->seatRow][current->seatColumn] = *current;
            }
            current = current->next;
        }
    }

    // Search for a passenger by ID
    PassengerNode* searchPassenger(string id) {
        PassengerNode* current = head;
        while (current != nullptr) {
            if (current->passengerId == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Display seating grid and manifest for a specific plane
    void displayPlaneManifest(int planeNumber) {
        PassengerNode passengerList[30][6];
        getPassengersFromPlane(passengerList, planeNumber);

        cout << "\n===============================" << endl;
        cout << "== PLANE " << planeNumber << endl;
        cout << "===============================" << endl;
        cout << "    A   B   C   D   E   F" << endl;

        // Render sections using the normal helper function
        renderSeatingRows("First Class (Rows 1-3)", 0, 2, passengerList);
        renderSeatingRows("Business Class (Rows 4-10)", 3, 9, passengerList);
        renderSeatingRows("Economy Class (Rows 11-30)", 10, 29, passengerList);

        cout << "\nLegend: O = Available, X = Occupied" << endl;

        cout << "\n========================================" << endl;
        cout << "== PASSENGER LIST - PLANE " << planeNumber << endl;
        cout << "========================================" << endl;
        cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(8) << "Seat" << setw(12) << "Class" << endl;
        cout << string(55, '-') << endl;

        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 6; j++) {
                if (passengerList[i][j].passengerName != "") {
                    cout << left << setw(10) << passengerList[i][j].passengerId
                         << setw(25) << passengerList[i][j].passengerName
                         << setw(8) << (to_string(i+1) + PassengerconvertColumnIndexToChar(j))
                         << setw(12) << passengerList[i][j].passengerClass << endl;
                }
            }
        }
    }

    void setTotalPlanes(int total) {
        totalPlanes = total;
    }

    int getTotalPlanes() {
        return totalPlanes;
    }

};
