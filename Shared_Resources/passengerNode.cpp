#pragma once
#include <iostream>
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

    void displayAll() {
        PassengerNode* current = head;
        int index = 0;

        while (current != nullptr) {
            cout << current->passengerId << ". " << current->passengerName << " | " << "Passenger Class: " << current->passengerClass << " | Plane number: " << current->planeNum << endl;
            current = current->next;
            index++;
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
