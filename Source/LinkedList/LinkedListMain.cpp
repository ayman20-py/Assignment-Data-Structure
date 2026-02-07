/*
===============================================================================
PLANE FLIGHT RESERVATION SYSTEM - LINKED LIST VERSION
===============================================================================
Student ID: TP082578 (Insertion & Deletion)
Student ID: TP081462 (Seat Lookup & Manifest)
Assignment: Data Structures Assignment Task 1
Component: Plane Reservation System (Linked List Version)
Date: Feb 2026

CODE ORGANIZATION:
- Section 1: Shared/Common Functions (TP082578 + TP081462)
- Section 2: TP082578 - Reservation (Insertion) Functions
- Section 3: TP082578 - Cancellation (Deletion) Functions
- Section 4: TP081462 - Seat Lookup Functions
- Section 5: TP081462 - Seating Grid Functions
===============================================================================
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <array>

using namespace std;

/* ===========================================================
                 SECTION 1: SHARED/COMMON FUNCTIONS
   =========================================================== */

// System constants
const string csvFilePath = "C:\\Users\\User\\Dev\\C++\\Assignment-Data-Structure\\Source\\LinkedList\\FlightPassengerDataLinkedList.csv";
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
    PassengerNode* tail;
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
        tail = nullptr;
        totalPlanes = 0;
    }

    PassengerNode* getHead() const {
        return head;
    }

    void init(string id, string name, int row, int column, int planeNum, string passengerClassType = "Economy") {
        PassengerNode* newNode = new PassengerNode(id, name, row, column, planeNum, passengerClassType);

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            return;
        }

        tail->next = newNode;
        tail = newNode;
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

    // Display all the information about the passengers in the whole dataset with optional class filter
    void displayAllPassengersFiltered(const string& filterClass = "") {
        PassengerNode* current = head;
        string filterUpper = filterClass;
        transform(filterUpper.begin(), filterUpper.end(), filterUpper.begin(), ::toupper);

        string title = "========== PASSENGER MANIFEST (ALL) ==========";
        if (!filterClass.empty()) {
            title = "========== PASSENGER MANIFEST (" + filterClass + " ONLY) ==========";
        }

        cout << "\n" << title << endl;
        cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(8) << "Plane" << setw(8) << "Seat" << setw(12) << "Class" << endl;
        cout << string(63, '-') << endl;

        int count = 0;
        while (current != nullptr) {
            string currentClassUpper = current->passengerClass;
            transform(currentClassUpper.begin(), currentClassUpper.end(), currentClassUpper.begin(), ::toupper);

            if (filterClass.empty() || currentClassUpper == filterUpper) {
                cout << left << setw(10) << current->passengerId
                     << setw(25) << current->passengerName.substr(0, 23)
                     << setw(8) << ("#" + to_string(current->planeNum))
                     << setw(8) << (to_string(current->seatRow + 1) + PassengerconvertColumnIndexToChar(current->seatColumn))
                     << setw(12) << current->passengerClass << endl;
                count++;
            }
            current = current->next;

            if (count > 0 && count % 20 == 0 && (filterClass.empty() || current != nullptr)) {
                // Pause if needed, but for performance timing we might want to avoid interactive pauses in the runner itself
                // However, for user display, we should keep it readable.
                // For joint display, Array will handle the output.
            }
        }

        cout << string(63, '-') << endl;
        cout << "Total Passengers Displayed: " << count << endl;
        cout << string(63, '=') << endl;
    }

    void displayAllPassengers() {
        displayAllPassengersFiltered("");
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

    string getNextPassengerId() {
        int highestPassengerId = 110000;
        PassengerNode* currentPassenger = head;

        while (currentPassenger != nullptr) {
            bool isNumericId = !currentPassenger->passengerId.empty();
            for (char c : currentPassenger->passengerId) {
                if (!isdigit(static_cast<unsigned char>(c))) {
                    isNumericId = false;
                    break;
                }
            }

            if (isNumericId) {
                int currentIdValue = stoi(currentPassenger->passengerId);
                if (currentIdValue > highestPassengerId) {
                    highestPassengerId = currentIdValue;
                }
            }

            currentPassenger = currentPassenger->next;
        }

        return to_string(highestPassengerId + 1);
    }

    bool removePassengerById(const string& passengerId, PassengerNode& removedPassenger) {
        if (head == nullptr) {
            return false;
        }

        if (head->passengerId == passengerId) {
            PassengerNode* removedNode = head;
            removedPassenger = *removedNode;
            removedPassenger.next = nullptr;
            head = head->next;
            if (removedNode == tail) {
                tail = head;
            }
            delete removedNode;
            return true;
        }

        PassengerNode* previous = head;
        PassengerNode* current = head->next;

        while (current != nullptr) {
            if (current->passengerId == passengerId) {
                removedPassenger = *current;
                removedPassenger.next = nullptr;
                previous->next = current->next;
                if (current == tail) {
                    tail = previous;
                }
                delete current;
                return true;
            }
            previous = current;
            current = current->next;
        }

        return false;
    }

    void writeToCSV(const string& filePath) const {
        ofstream csvOutputFile(filePath);
        if (!csvOutputFile.is_open()) {
            cout << "Could not open the file for writing: " << filePath << endl;
            return;
        }

        csvOutputFile << "PassengerID,Name,SeatRow,SeatColumn,Class" << endl;

        PassengerNode* currentPassenger = head;
        while (currentPassenger != nullptr) {
            csvOutputFile << currentPassenger->passengerId << ","
                         << currentPassenger->passengerName << ","
                         << (currentPassenger->seatRow + 1) << ","
                         << PassengerconvertColumnIndexToChar(currentPassenger->seatColumn) << ","
                         << currentPassenger->passengerClass << endl;
            currentPassenger = currentPassenger->next;
        }

        csvOutputFile.close();
    }

};

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

/* ===========================================================
        SECTION 2: TP082578 - RESERVATION (INSERTION)
   =========================================================== */

struct ReservationInsertionResult {
    bool isSuccessful;
    string errorMessage;
    int planeNumber;
    int seatRowIndex;
    int seatColumnIndex;
};

struct ReservationDeletionResult {
    bool isSuccessful;
    string errorMessage;
    string passengerId;
    string passengerName;
    string passengerClass;
    int planeNumber;
    int seatRowIndex;
    int seatColumnIndex;
};

bool normalizePassengerClass(const string& passengerClassInput, string& normalizedClass) {
    string uppercaseClass = passengerClassInput;
    transform(uppercaseClass.begin(), uppercaseClass.end(), uppercaseClass.begin(), ::toupper);

    if (uppercaseClass == "FIRST") {
        normalizedClass = "First";
        return true;
    }
    if (uppercaseClass == "BUSINESS") {
        normalizedClass = "Business";
        return true;
    }
    if (uppercaseClass == "ECONOMY") {
        normalizedClass = "Economy";
        return true;
    }

    normalizedClass.clear();
    return false;
}

bool hasNonWhitespaceContent(const string& value) {
    for (char c : value) {
        if (!isspace(static_cast<unsigned char>(c))) {
            return true;
        }
    }
    return false;
}

void getSeatClassRowRange(const string& passengerClass, int& startRowIndex, int& endRowIndex) {
    if (passengerClass == "First") {
        startRowIndex = 0;
        endRowIndex = 2;
        return;
    }
    if (passengerClass == "Business") {
        startRowIndex = 3;
        endRowIndex = 9;
        return;
    }

    startRowIndex = 10;
    endRowIndex = 29;
}

bool isSeatIndexValid(int seatRowIndex, int seatColumnIndex) {
    if (seatRowIndex < 0 || seatRowIndex >= totalRows) {
        return false;
    }
    if (seatColumnIndex < 0 || seatColumnIndex >= totalColumns) {
        return false;
    }
    return true;
}

string generateNextPassengerId(PassengerLinkedList& linkedList) {
    return linkedList.getNextPassengerId();
}

ReservationInsertionResult insertPassengerReservation(
    PassengerLinkedList& linkedList,
    const string& passengerId,
    const string& passengerName,
    const string& passengerClassInput,
    bool hasPreferredSeat,
    int preferredSeatRowIndex,
    int preferredSeatColumnIndex,
    int forcedPlaneNumber = -1
) {
    ReservationInsertionResult result{};
    result.isSuccessful = false;
    result.errorMessage = "";
    result.planeNumber = -1;
    result.seatRowIndex = -1;
    result.seatColumnIndex = -1;

    if (!hasNonWhitespaceContent(passengerId)) {
        result.errorMessage = "Passenger ID cannot be empty.";
        return result;
    }
    if (linkedList.doesPassengerExists(passengerId)) {
        result.errorMessage = "Passenger ID already exists.";
        return result;
    }

    if (!hasNonWhitespaceContent(passengerName)) {
        result.errorMessage = "Passenger name cannot be empty.";
        return result;
    }

    string normalizedClass;
    if (!normalizePassengerClass(passengerClassInput, normalizedClass)) {
        result.errorMessage = "Passenger class must be First, Business, or Economy.";
        return result;
    }

    int startRowIndex = 0;
    int endRowIndex = 29;
    getSeatClassRowRange(normalizedClass, startRowIndex, endRowIndex);

    int totalPlanes = linkedList.getTotalPlanes();
    if (totalPlanes < 1) {
        totalPlanes = 1;
    }

    // Force targeting a specific plane (used by interactive reservation)
    if (forcedPlaneNumber > 0) {
        if (!isSeatIndexValid(preferredSeatRowIndex, preferredSeatColumnIndex)) {
            result.errorMessage = "Seat row or column is out of range.";
            return result;
        }
        if (preferredSeatRowIndex < startRowIndex || preferredSeatRowIndex > endRowIndex) {
            result.errorMessage = "Selected seat does not match the requested class.";
            return result;
        }
        if (linkedList.isSeatOccupied(preferredSeatRowIndex, preferredSeatColumnIndex, forcedPlaneNumber)) {
            result.errorMessage = "Seat is already occupied on Plane #" + to_string(forcedPlaneNumber);
            return result;
        }

        linkedList.init(passengerId, passengerName, preferredSeatRowIndex, preferredSeatColumnIndex, forcedPlaneNumber, normalizedClass);
        if (forcedPlaneNumber > totalPlanes) {
            linkedList.setTotalPlanes(forcedPlaneNumber);
        }
        result.isSuccessful = true;
        result.planeNumber = forcedPlaneNumber;
        result.seatRowIndex = preferredSeatRowIndex;
        result.seatColumnIndex = preferredSeatColumnIndex;
        return result;
    }

    if (hasPreferredSeat) {
        if (!isSeatIndexValid(preferredSeatRowIndex, preferredSeatColumnIndex)) {
            result.errorMessage = "Seat row or column is out of range.";
            return result;
        }
        if (preferredSeatRowIndex < startRowIndex || preferredSeatRowIndex > endRowIndex) {
            result.errorMessage = "Selected seat does not match the requested class.";
            return result;
        }

        for (int planeNumber = 1; planeNumber <= totalPlanes; planeNumber++) {
            if (!linkedList.isSeatOccupied(preferredSeatRowIndex, preferredSeatColumnIndex, planeNumber)) {
                linkedList.init(passengerId, passengerName, preferredSeatRowIndex, preferredSeatColumnIndex, planeNumber, normalizedClass);
                result.isSuccessful = true;
                result.planeNumber = planeNumber;
                result.seatRowIndex = preferredSeatRowIndex;
                result.seatColumnIndex = preferredSeatColumnIndex;
                return result;
            }
        }

        int newPlaneNumber = totalPlanes + 1;
        linkedList.init(passengerId, passengerName, preferredSeatRowIndex, preferredSeatColumnIndex, newPlaneNumber, normalizedClass);
        linkedList.setTotalPlanes(newPlaneNumber);
        result.isSuccessful = true;
        result.planeNumber = newPlaneNumber;
        result.seatRowIndex = preferredSeatRowIndex;
        result.seatColumnIndex = preferredSeatColumnIndex;
        return result;
    }

    for (int planeNumber = 1; planeNumber <= totalPlanes; planeNumber++) {
        for (int rowIndex = startRowIndex; rowIndex <= endRowIndex; rowIndex++) {
            for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
                if (!linkedList.isSeatOccupied(rowIndex, columnIndex, planeNumber)) {
                    linkedList.init(passengerId, passengerName, rowIndex, columnIndex, planeNumber, normalizedClass);
                    result.isSuccessful = true;
                    result.planeNumber = planeNumber;
                    result.seatRowIndex = rowIndex;
                    result.seatColumnIndex = columnIndex;
                    return result;
                }
            }
        }
    }

    int newPlaneNumber = totalPlanes + 1;
    int defaultRowIndex = startRowIndex;
    int defaultColumnIndex = 0;
    linkedList.init(passengerId, passengerName, defaultRowIndex, defaultColumnIndex, newPlaneNumber, normalizedClass);
    linkedList.setTotalPlanes(newPlaneNumber);
    result.isSuccessful = true;
    result.planeNumber = newPlaneNumber;
    result.seatRowIndex = defaultRowIndex;
    result.seatColumnIndex = defaultColumnIndex;
    return result;
}

/* ===========================================================
        SECTION 3: TP082578 - CANCELLATION (DELETION)
   =========================================================== */

ReservationDeletionResult deletePassengerReservation(
    PassengerLinkedList& linkedList,
    const string& passengerId
) {
    ReservationDeletionResult result{};
    result.isSuccessful = false;
    result.errorMessage = "";
    result.passengerId = "";
    result.passengerName = "";
    result.passengerClass = "";
    result.planeNumber = -1;
    result.seatRowIndex = -1;
    result.seatColumnIndex = -1;

    if (!hasNonWhitespaceContent(passengerId)) {
        result.errorMessage = "Passenger ID cannot be empty.";
        return result;
    }

    PassengerNode removedPassenger;
    if (!linkedList.removePassengerById(passengerId, removedPassenger)) {
        result.errorMessage = "Passenger ID not found.";
        return result;
    }

    result.isSuccessful = true;
    result.passengerId = removedPassenger.passengerId;
    result.passengerName = removedPassenger.passengerName;
    result.passengerClass = removedPassenger.passengerClass;
    result.planeNumber = removedPassenger.planeNum;
    result.seatRowIndex = removedPassenger.seatRow;
    result.seatColumnIndex = removedPassenger.seatColumn;
    return result;
}

/* ===========================================================
          SECTION 4: TP081462 - SEAT LOOKUP FUNCTIONS
   =========================================================== */

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

/* ===========================================================
         SECTION 5: TP081462 - SEATING GRID FUNCTIONS
   =========================================================== */

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
