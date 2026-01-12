/* ===========================================================
                    INCLUDES AND LIBRARIES
   =========================================================== */

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

/* ===========================================================
                    CONSTANTS AND CONFIGURATION
   =========================================================== */

// CSV file path for flight passenger data.
const string csvFilePath = "Assignment\\flight_passenger_data.csv";

// Seating grid dimensions for the airplane.
const int totalRows = 30;
const int totalColumns = 6;

// Seat availability markers.
const char availableSeatMarker = 'O';
const char occupiedSeatMarker = 'X';

// Menu option constants for clarity.
const int menuOptionReserve = 1;
const int menuOptionCancel = 2;
const int menuOptionDisplay = 3;
const int menuOptionSaveAndExit = 4;

/* ===========================================================
                    DATA STRUCTURES
   =========================================================== */

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

/* ===========================================================
                    GLOBAL VARIABLES
   =========================================================== */

// Head pointer for the linked list of passenger reservations.
PassengerNode* headPassengerNode = nullptr;

// Two-dimensional array representing the airplane seating grid.
char seatingGrid[totalRows][totalColumns];

// Temporary structure to store the most recent passenger for each seat during CSV loading.
struct SeatBookingInfo {
    string passengerId;
    string passengerName;
    string passengerClass;
    bool isOccupied;
};

SeatBookingInfo mostRecentBookings[totalRows][totalColumns];

/* ===========================================================
                    UTILITY FUNCTIONS
   =========================================================== */

// This function clears the terminal screen for a cleaner user experience.
void clearScreen() {
    system("cls");
}

// This function pauses execution and waits for the user to press Enter.
void pauseForUserInput() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

// This function converts a seat column character (A-F) to a zero-based column index (0-5).
int convertColumnCharToIndex(char columnChar) {
    return columnChar - 'A';
}

// This function converts a zero-based column index (0-5) to a seat column character (A-F).
char convertColumnIndexToChar(int columnIndex) {
    return 'A' + columnIndex;
}

// This function clears the input buffer and error state when invalid input is detected.
void clearInputBuffer() {
    cin.clear();
    cin.ignore(10000, '\n');
}

// This function validates that a string is not empty or just whitespace.
bool isValidNonEmptyString(const string& input) {
    if (input.empty()) {
        return false;
    }

    // Check if string contains only whitespace.
    for (char characterToCheck : input) {
        if (!isspace(characterToCheck)) {
            return true;
        }
    }

    return false;
}

// This function validates and normalizes passenger class input.
bool isValidPassengerClass(string& passengerClass) {
    // Convert to lowercase for comparison.
    string lowerCaseClass = passengerClass;
    for (char& characterToConvert : lowerCaseClass) {
        characterToConvert = tolower(characterToConvert);
    }

    // Check against valid classes and normalize the input.
    if (lowerCaseClass == "first") {
        passengerClass = "First";
        return true;
    } else if (lowerCaseClass == "business") {
        passengerClass = "Business";
        return true;
    } else if (lowerCaseClass == "economy") {
        passengerClass = "Economy";
        return true;
    }

    return false;
}

// This function validates that a character is a valid seat column (A-F, case-insensitive).
bool isValidSeatColumn(char& seatColumn) {
    // Convert to uppercase.
    seatColumn = toupper(seatColumn);

    // Check if it's between A and F.
    return (seatColumn >= 'A' && seatColumn <= 'F');
}

/* ===========================================================
                    SEATING GRID FUNCTIONS
   =========================================================== */

// This function initializes all seats in the seating grid to available status.
void initializeSeatingGrid() {
    for (int rowIndex = 0; rowIndex < totalRows; rowIndex++) {
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            seatingGrid[rowIndex][columnIndex] = availableSeatMarker;
        }
    }
}

// This function displays the current state of the seating grid with row and column labels.
void displaySeatingGrid() {
    cout << "\n========================================\n";
    cout << "         SEATING GRID STATUS\n";
    cout << "========================================\n\n";

    // Display column headers.
    cout << "       ";
    for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
        cout << " " << convertColumnIndexToChar(columnIndex) << "  ";
    }
    cout << "\n";

    // Display First Class section (Rows 1-3).
    cout << "\n--- FIRST CLASS (Rows 1-3) ---\n";
    for (int rowIndex = 0; rowIndex < 3; rowIndex++) {
        cout << "Row " << setw(2) << rowIndex + 1 << "  ";
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            cout << seatingGrid[rowIndex][columnIndex] << "   ";
        }
        cout << "\n";
    }

    // Display Business Class section (Rows 4-10).
    cout << "\n--- BUSINESS CLASS (Rows 4-10) ---\n";
    for (int rowIndex = 3; rowIndex < 10; rowIndex++) {
        cout << "Row " << setw(2) << rowIndex + 1 << "  ";
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            cout << seatingGrid[rowIndex][columnIndex] << "   ";
        }
        cout << "\n";
    }

    // Display Economy Class section (Rows 11-30).
    cout << "\n--- ECONOMY CLASS (Rows 11-30) ---\n";
    for (int rowIndex = 10; rowIndex < totalRows; rowIndex++) {
        cout << "Row " << setw(2) << rowIndex + 1 << "  ";
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            cout << seatingGrid[rowIndex][columnIndex] << "   ";
        }
        cout << "\n";
    }

    cout << "\nLegend: " << availableSeatMarker << " = Available, "
         << occupiedSeatMarker << " = Occupied\n";
    cout << "========================================\n\n";
}

// This function checks if a specific seat is available for reservation.
bool isSeatAvailable(int row, int column) {
    // Validate that the row and column are within valid bounds.
    if (row < 0 || row >= totalRows || column < 0 || column >= totalColumns) {
        return false;
    }

    return seatingGrid[row][column] == availableSeatMarker;
}

// This function marks a seat as occupied in the seating grid.
void allocateSeat(int row, int column) {
    seatingGrid[row][column] = occupiedSeatMarker;
}

// This function marks a seat as available in the seating grid.
void deallocateSeat(int row, int column) {
    seatingGrid[row][column] = availableSeatMarker;
}

// This function checks if there are any available seats on the entire plane.
bool hasAvailableSeats() {
    for (int rowIndex = 0; rowIndex < totalRows; rowIndex++) {
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            if (seatingGrid[rowIndex][columnIndex] == availableSeatMarker) {
                return true;
            }
        }
    }
    return false;
}

/* ===========================================================
                    LINKED LIST FUNCTIONS
   =========================================================== */

// This function inserts a new passenger reservation at the end of the linked list.
// It handles three cases: empty list, single node, and multiple nodes.
void insertReservation(string passengerId, string passengerName, int seatRow, int seatColumn, string passengerClass = "Economy", bool isNewReservation = true) {
    // Create a new passenger node with the provided details.
    PassengerNode* newPassengerNode = new PassengerNode(passengerId, passengerName, seatRow, seatColumn, passengerClass, isNewReservation);

    // Case 1: The linked list is empty (no existing reservations).
    if (headPassengerNode == nullptr) {
        headPassengerNode = newPassengerNode;
        return;
    }

    // Case 2: The linked list has one or more nodes (traverse to the end).
    PassengerNode* currentNode = headPassengerNode;
    while (currentNode->nextNode != nullptr) {
        currentNode = currentNode->nextNode;
    }

    // Insert the new node at the end of the list.
    currentNode->nextNode = newPassengerNode;
}

// This function removes a passenger reservation from the linked list by passenger ID.
// It handles three deletion cases: head node, middle node, and tail node.
bool cancelReservation(string passengerId) {
    // Case 1: The linked list is empty (no reservations to cancel).
    if (headPassengerNode == nullptr) {
        cout << "\n[ERROR] No reservations found. Cannot cancel.\n";
        return false;
    }

    // Case 2: The head node contains the passenger ID to be deleted.
    if (headPassengerNode->passengerId == passengerId) {
        PassengerNode* nodeToDelete = headPassengerNode;

        // Free the seat in the seating grid.
        deallocateSeat(nodeToDelete->seatRow, nodeToDelete->seatColumn);

        cout << "\n[SUCCESS] Reservation cancelled for " << nodeToDelete->passengerName
             << " (ID: " << passengerId << "). Seat Row " << nodeToDelete->seatRow + 1
             << ", Column " << convertColumnIndexToChar(nodeToDelete->seatColumn) << " is now available.\n";

        // Move the head pointer to the next node.
        headPassengerNode = headPassengerNode->nextNode;

        // Delete the old head node.
        delete nodeToDelete;
        return true;
    }

    // Case 3: The passenger ID is in a middle or tail node.
    PassengerNode* currentNode = headPassengerNode;

    // Traverse the list to find the node before the one to be deleted.
    while (currentNode->nextNode != nullptr && currentNode->nextNode->passengerId != passengerId) {
        currentNode = currentNode->nextNode;
    }

    // If the passenger ID was not found in the list.
    if (currentNode->nextNode == nullptr) {
        cout << "\n[ERROR] Passenger ID " << passengerId << " not found. Cannot cancel.\n";
        return false;
    }

    // Node to delete is found at currentNode->nextNode.
    PassengerNode* nodeToDelete = currentNode->nextNode;

    // Free the seat in the seating grid.
    deallocateSeat(nodeToDelete->seatRow, nodeToDelete->seatColumn);

    cout << "\n[SUCCESS] Reservation cancelled for " << nodeToDelete->passengerName
         << " (ID: " << passengerId << "). Seat Row " << nodeToDelete->seatRow + 1
         << ", Column " << convertColumnIndexToChar(nodeToDelete->seatColumn) << " is now available.\n";

    // Bypass the node to delete by linking current node to the next-next node.
    currentNode->nextNode = nodeToDelete->nextNode;

    // Delete the node.
    delete nodeToDelete;
    return true;
}

// This function displays all current passenger reservations occupying seats in a compact multi-column format.
void displayReservations() {
    cout << "\n===============================================================================\n";
    cout << "                         CURRENT ACTIVE RESERVATIONS\n";
    cout << "===============================================================================\n\n";

    // Check if the list is empty.
    if (headPassengerNode == nullptr) {
        cout << "No reservations found.\n\n";
        return;
    }

    // Traverse the linked list and count total and new reservations.
    PassengerNode* currentNode = headPassengerNode;
    int totalReservationCount = 0;
    int newReservationCount = 0;

    while (currentNode != nullptr) {
        totalReservationCount++;
        if (currentNode->isNewReservation) {
            newReservationCount++;
        }
        currentNode = currentNode->nextNode;
    }

    // Display summary.
    cout << "Total Active Reservations: " << totalReservationCount << "\n\n";

    // Display column headers.
    cout << "ID       Name                    Seat  Class\n";
    cout << "-------  ----------------------  ----  ---------\n";

    // Traverse the linked list and display all reservations in compact format.
    currentNode = headPassengerNode;

    while (currentNode != nullptr) {
        // Format seat as "5B", "12A", etc.
        string seatInfo = to_string(currentNode->seatRow + 1) +
                         convertColumnIndexToChar(currentNode->seatColumn);

        // Truncate name if too long.
        string displayName = currentNode->passengerName;
        if (displayName.length() > 22) {
            displayName = displayName.substr(0, 19) + "...";
        }

        // Print the row with proper formatting.
        cout << left << setw(9) << currentNode->passengerId
             << setw(24) << displayName
             << setw(6) << seatInfo
             << currentNode->passengerClass << "\n";

        currentNode = currentNode->nextNode;
    }

    cout << "\n===============================================================================\n\n";
}

// This function checks if a passenger ID already exists in the reservation list.
bool isPassengerIdExists(string passengerId) {
    PassengerNode* currentNode = headPassengerNode;

    while (currentNode != nullptr) {
        if (currentNode->passengerId == passengerId) {
            return true;
        }
        currentNode = currentNode->nextNode;
    }

    return false;
}

// This function generates the next available unique passenger ID.
string generateNextPassengerId() {
    int highestIdNumber = 100000;

    // Traverse the linked list to find the highest passenger ID number.
    PassengerNode* currentNode = headPassengerNode;

    while (currentNode != nullptr) {
        // Convert passenger ID string to integer.
        int currentIdNumber = stoi(currentNode->passengerId);

        // Update highest if current is greater.
        if (currentIdNumber > highestIdNumber) {
            highestIdNumber = currentIdNumber;
        }

        currentNode = currentNode->nextNode;
    }

    // Return the next ID (highest + 1) as a string.
    return to_string(highestIdNumber + 1);
}

/* ===========================================================
                    CSV FILE OPERATIONS
   =========================================================== */

// This function loads passenger data from the CSV file into the linked list.
// It only keeps the most recent booking for each seat to handle historical data.
bool loadPassengerDataFromCsv() {
    ifstream csvInputFile(csvFilePath);

    // Check if the file opened successfully.
    if (!csvInputFile.is_open()) {
        cout << "\n[ERROR] Could not open file: " << csvFilePath << "\n";
        return false;
    }

    // Initialize the mostRecentBookings array to track the last booking per seat.
    for (int rowIndex = 0; rowIndex < totalRows; rowIndex++) {
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            mostRecentBookings[rowIndex][columnIndex].isOccupied = false;
        }
    }

    string currentLine;
    int totalRecordsRead = 0;

    // Skip the header line.
    getline(csvInputFile, currentLine);

    // Read each line from the CSV file and store only the last booking per seat.
    while (getline(csvInputFile, currentLine)) {
        stringstream lineStream(currentLine);
        string passengerId;
        string passengerName;
        string seatRowString;
        string seatColumnString;
        string passengerClass;

        // Parse CSV line (format: PassengerID,Name,SeatRow,SeatColumn,Class).
        getline(lineStream, passengerId, ',');
        getline(lineStream, passengerName, ',');
        getline(lineStream, seatRowString, ',');
        getline(lineStream, seatColumnString, ',');
        getline(lineStream, passengerClass, ',');

        // Convert seat row from string to integer and adjust to zero-based index.
        int seatRow = stoi(seatRowString) - 1;

        // Convert seat column from character to zero-based index.
        int seatColumn = convertColumnCharToIndex(seatColumnString[0]);

        // Store this booking as the most recent for this seat (overwrites any previous booking).
        mostRecentBookings[seatRow][seatColumn].passengerId = passengerId;
        mostRecentBookings[seatRow][seatColumn].passengerName = passengerName;
        mostRecentBookings[seatRow][seatColumn].passengerClass = passengerClass;
        mostRecentBookings[seatRow][seatColumn].isOccupied = true;

        totalRecordsRead++;
    }

    csvInputFile.close();

    // Now insert only the most recent bookings into the linked list.
    int activeReservationCount = 0;

    for (int rowIndex = 0; rowIndex < totalRows; rowIndex++) {
        for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
            if (mostRecentBookings[rowIndex][columnIndex].isOccupied) {
                // Allocate the seat in the seating grid.
                allocateSeat(rowIndex, columnIndex);

                // Insert the passenger reservation into the linked list as an existing (not new) reservation.
                insertReservation(
                    mostRecentBookings[rowIndex][columnIndex].passengerId,
                    mostRecentBookings[rowIndex][columnIndex].passengerName,
                    rowIndex,
                    columnIndex,
                    mostRecentBookings[rowIndex][columnIndex].passengerClass,
                    false
                );

                activeReservationCount++;
            }
        }
    }

    cout << "\n[SUCCESS] Processed " << totalRecordsRead << " historical records.\n";
    cout << "Loaded " << activeReservationCount << " active seat reservations (most recent per seat).\n";
    return true;
}

// This function saves all current passenger reservations to the CSV file.
// It overwrites the file with the current state to reflect any cancellations.
bool savePassengerDataToCsv() {
    // Open file in write mode to overwrite with current state.
    ofstream csvOutputFile(csvFilePath);

    // Check if the file opened successfully.
    if (!csvOutputFile.is_open()) {
        cout << "\n[ERROR] Could not open file for writing: " << csvFilePath << "\n";
        return false;
    }

    // Write the CSV header.
    csvOutputFile << "PassengerID,Name,SeatRow,SeatColumn,Class\n";

    // Traverse the linked list and write all current passenger records.
    PassengerNode* currentNode = headPassengerNode;
    int savedRecordCount = 0;

    while (currentNode != nullptr) {
        // Convert zero-based indices back to one-based and character format.
        int displaySeatRow = currentNode->seatRow + 1;
        char displaySeatColumn = convertColumnIndexToChar(currentNode->seatColumn);

        // Write the passenger record to the CSV file.
        csvOutputFile << currentNode->passengerId << ","
                     << currentNode->passengerName << ","
                     << displaySeatRow << ","
                     << displaySeatColumn << ","
                     << currentNode->passengerClass << "\n";

        savedRecordCount++;
        currentNode = currentNode->nextNode;
    }

    csvOutputFile.close();

    cout << "\n[SUCCESS] Saved " << savedRecordCount << " passenger record(s) to CSV file.\n";
    cout << "All reservations and cancellations have been saved.\n";
    return true;
}

/* ===========================================================
                    MENU AND USER INTERACTION
   =========================================================== */

// This function handles the reservation process by collecting user input and validating data.
void handleReservation() {
    clearScreen();

    cout << "\n========================================\n";
    cout << "        CREATE NEW RESERVATION\n";
    cout << "========================================\n\n";

    // Check if there are any available seats before proceeding.
    if (!hasAvailableSeats()) {
        cout << "[ERROR] Flight is fully booked. No available seats.\n";
        cout << "Please cancel an existing reservation to free up a seat.\n";
        pauseForUserInput();
        return;
    }

    string passengerName;
    int seatRow;
    char seatColumnChar;
    int seatColumn;
    string passengerClass;

    // Get and validate passenger name.
    cout << "Enter Passenger Name: ";
    cin.ignore();
    getline(cin, passengerName);

    if (!isValidNonEmptyString(passengerName)) {
        cout << "\n[ERROR] Passenger name cannot be empty.\n";
        pauseForUserInput();
        return;
    }

    // Get and validate passenger class.
    cout << "Enter Passenger Class (First/Business/Economy): ";
    getline(cin, passengerClass);

    if (!isValidPassengerClass(passengerClass)) {
        cout << "\n[ERROR] Invalid passenger class. Must be First, Business, or Economy.\n";
        pauseForUserInput();
        return;
    }

    // Display current seating grid.
    displaySeatingGrid();

    // Get and validate seat row.
    cout << "Enter Seat Row (1-" << totalRows << "): ";

    if (!(cin >> seatRow)) {
        cout << "\n[ERROR] Invalid input. Please enter a number.\n";
        clearInputBuffer();
        pauseForUserInput();
        return;
    }

    // Validate row range.
    if (seatRow < 1 || seatRow > totalRows) {
        cout << "\n[ERROR] Seat row must be between 1 and " << totalRows << ".\n";
        clearInputBuffer();
        pauseForUserInput();
        return;
    }

    seatRow--; // Convert to zero-based index.

    // Get and validate seat column.
    cout << "Enter Seat Column (A-F): ";
    cin >> seatColumnChar;

    if (!isValidSeatColumn(seatColumnChar)) {
        cout << "\n[ERROR] Seat column must be between A and F.\n";
        clearInputBuffer();
        pauseForUserInput();
        return;
    }

    seatColumn = convertColumnCharToIndex(seatColumnChar);

    // Validate seat availability.
    if (!isSeatAvailable(seatRow, seatColumn)) {
        cout << "\n[ERROR] Seat is either occupied or invalid. Please choose another seat.\n";
        clearInputBuffer();
        pauseForUserInput();
        return;
    }

    // Auto-generate a unique passenger ID.
    string passengerId = generateNextPassengerId();

    // Allocate the seat and insert the reservation.
    allocateSeat(seatRow, seatColumn);
    insertReservation(passengerId, passengerName, seatRow, seatColumn, passengerClass);

    cout << "\n[SUCCESS] Reservation created for " << passengerName << "\n";
    cout << "Auto-generated Passenger ID: " << passengerId << "\n";
    cout << "Seat: Row " << seatRow + 1 << ", Column " << seatColumnChar
         << " (" << passengerClass << ")\n";

    clearInputBuffer();
    pauseForUserInput();
}

// This function handles the cancellation process by collecting the passenger ID.
void handleCancellation() {
    clearScreen();

    string passengerId;

    cout << "\n========================================\n";
    cout << "        CANCEL RESERVATION\n";
    cout << "========================================\n\n";

    // Display current reservations.
    displayReservations();

    // Get and validate passenger ID to cancel.
    cout << "Enter Passenger ID to cancel: ";
    cin >> passengerId;

    // Validate that passenger ID is not empty.
    if (!isValidNonEmptyString(passengerId)) {
        cout << "\n[ERROR] Passenger ID cannot be empty.\n";
        clearInputBuffer();
        pauseForUserInput();
        return;
    }

    // Attempt to cancel the reservation.
    cancelReservation(passengerId);
    clearInputBuffer();
    pauseForUserInput();
}

// This function displays the main menu and returns the user's choice with validation.
int displayMainMenu() {
    clearScreen();

    int userChoice;

    cout << "\n========================================\n";
    cout << "   AIRLINE RESERVATION SYSTEM\n";
    cout << "========================================\n";
    cout << "1. Reserve a Seat\n";
    cout << "2. Cancel a Reservation\n";
    cout << "3. Display All Reservations\n";
    cout << "4. Save and Exit\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";

    // Validate that input is an integer.
    if (!(cin >> userChoice)) {
        clearInputBuffer();
        return -1; // Return invalid choice indicator.
    }

    // Validate that choice is in valid range.
    if (userChoice < 1 || userChoice > 4) {
        return -1; // Return invalid choice indicator.
    }

    return userChoice;
}

/* ===========================================================
                    MAIN PROGRAM ENTRY POINT
   =========================================================== */

int main() {
    // Initialize the seating grid at the start of the program.
    initializeSeatingGrid();

    clearScreen();

    cout << "\n========================================\n";
    cout << "  WELCOME TO AIRLINE RESERVATION SYSTEM\n";
    cout << "========================================\n";
    cout << "Student ID: TP082578\n";
    cout << "Assignment: Linked List Reservation System\n";
    cout << "========================================\n\n";

    // Load passenger data from CSV file.
    cout << "Loading passenger data from CSV file...\n";
    if (!loadPassengerDataFromCsv()) {
        cout << "Failed to load passenger data. Starting with empty reservation list.\n";
    }

    pauseForUserInput();

    // Main program loop for menu-driven interaction.
    bool programRunning = true;

    while (programRunning) {
        int userChoice = displayMainMenu();

        // Handle invalid input from menu.
        if (userChoice == -1) {
            cout << "\n[ERROR] Invalid choice. Please enter a number between 1 and 4.\n";
            pauseForUserInput();
            continue;
        }

        switch (userChoice) {
            case menuOptionReserve:
                handleReservation();
                break;

            case menuOptionCancel:
                handleCancellation();
                break;

            case menuOptionDisplay:
                clearScreen();
                displayReservations();
                displaySeatingGrid();
                pauseForUserInput();
                break;

            case menuOptionSaveAndExit:
                clearScreen();
                cout << "\n========================================\n";
                cout << "         SAVING AND EXITING\n";
                cout << "========================================\n\n";

                // Save all changes to the CSV file.
                cout << "Saving passenger data to CSV file...\n";
                if (savePassengerDataToCsv()) {
                    cout << "\nAll changes have been saved successfully.\n";
                } else {
                    cout << "\nWarning: Failed to save changes to CSV file.\n";
                }

                cout << "\nThank you for using the Airline Reservation System.\n";
                cout << "Exiting program...\n";
                cout << "========================================\n\n";
                programRunning = false;
                break;

            default:
                cout << "\n[ERROR] Invalid choice. Please enter a number between 1 and 4.\n";
                pauseForUserInput();
                break;
        }
    }

    return 0;
}
