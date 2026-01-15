#include <string>
using namespace std;

// This structure represents a single passenger reservation node in the linked list.
struct PassengerNode {
    string passengerId;
    string passengerName;
    int seatRow;
    int seatColumn;
    string passengerClass;
    bool isNewReservation;
    PassengerNode* nextNode;

    // Constructor to initialize a passenger node with provided details.
    PassengerNode(string id, string name, int row, int column, string passengerClassType = "Economy", bool isNew = true) {
        passengerId = id;
        passengerName = name;
        seatRow = row;
        seatColumn = column;
        passengerClass = passengerClassType;
        isNewReservation = isNew;
        nextNode = nullptr;
    }
};
