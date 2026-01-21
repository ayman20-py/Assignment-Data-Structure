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
public: 
    PassengerLinkedList() {
        head = nullptr;
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
        PassengerNode* current = head;

        while (current != nullptr) {
            if (current->planeNum == planeNumber) {
                passengerList[current->seatRow][current->seatColumn] = *current;
            }
            current = current->next;
        }
    }

};
