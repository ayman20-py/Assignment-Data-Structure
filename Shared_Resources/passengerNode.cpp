#pragma once
#include <iostream>
#include <string>
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

    // Constructor to initialize a passenger node with provided details.
    PassengerNode(string id, string name, int row, int column, int planeNum, string passengerClassType = "Economy") {
        passengerId = id;
        passengerName = name;
        seatRow = row;
        seatColumn = column;
        passengerClass = passengerClassType;
        planeNum = planeNum;
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

    void append(string id, string name, int row, int column, int planeNum, string passengerClassType = "Economy") {
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
};
