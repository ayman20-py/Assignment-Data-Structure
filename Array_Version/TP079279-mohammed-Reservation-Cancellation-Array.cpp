/*
===============================================================================
FLIGHT RESERVATION & CANCELLATION SYSTEM - ARRAY VERSION
===============================================================================
Student ID: TP079279
Assignment: Data Structures Assignment Task 1
Component: Reservation & Cancellation (Array Version)
Date: January 2026

FINAL VERSION - All fixes applied:
- Mac-compatible screen clearing
- Seating grid after passenger input
- Inline input (type on same line as prompt)
===============================================================================
*/

// ============================================================================
// INCLUDES & NAMESPACE
// ============================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

// ============================================================================
// CONSTANTS & CONFIGURATION
// ============================================================================

const int totalRows = 30;
const int totalColumns = 6;
const int totalSeats = totalRows * totalColumns;
const int maxPassengers = 180;

const char availableSeatMarker = 'O';
const char occupiedSeatMarker = 'X';

const int firstClassStartRow = 0;
const int firstClassEndRow = 2;
const int businessClassStartRow = 3;
const int businessClassEndRow = 9;
const int economyClassStartRow = 10;
const int economyClassEndRow = 29;

const string columnLabels = "ABCDEF";
const string csvFilePath = "../Dataset/flight_passenger_data.csv.csv";

// ============================================================================
// PASSENGER STRUCTURE
// ============================================================================

struct Passenger
{
    string passengerId;
    string passengerName;
    int seatRow;
    int seatColumn;
    string passengerClass;
    bool isActive;

    Passenger() : passengerId(""), passengerName(""), seatRow(-1),
                  seatColumn(-1), passengerClass(""), isActive(false) {}
};

// ============================================================================
// GLOBAL DATA STRUCTURES
// ============================================================================

Passenger passengerArray[maxPassengers];
int activePassengerCount = 0;
char seatingGrid[totalRows][totalColumns];

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void clearInputBuffer()
{
    cin.clear();
    cin.ignore(10000, '\n');
}

void pauseForUserInput()
{
    cout << "\nPress Enter to continue...";
    cin.sync();
    cin.get();
}

void clearScreen()
{
    system("clear");
}

string toUpperCase(const string &input)
{
    string result = input;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

string trimWhitespace(const string &input)
{
    size_t start = input.find_first_not_of(" \t\n\r");
    size_t end = input.find_last_not_of(" \t\n\r");

    if (start == string::npos)
    {
        return "";
    }

    return input.substr(start, end - start + 1);
}

bool isValidClass(const string &passengerClass)
{
    string upperClass = toUpperCase(passengerClass);
    return (upperClass == "FIRST" || upperClass == "BUSINESS" || upperClass == "ECONOMY");
}

int columnLetterToIndex(char columnLetter)
{
    char upperColumn = toupper(columnLetter);
    if (upperColumn >= 'A' && upperColumn <= 'F')
    {
        return upperColumn - 'A';
    }
    return -1;
}

char columnIndexToLetter(int columnIndex)
{
    return columnLabels[columnIndex];
}

// ============================================================================
// SEATING GRID FUNCTIONS
// ============================================================================

void initializeSeatingGrid()
{
    for (int row = 0; row < totalRows; row++)
    {
        for (int column = 0; column < totalColumns; column++)
        {
            seatingGrid[row][column] = availableSeatMarker;
        }
    }
}

void allocateSeat(int seatRow, int seatColumn)
{
    seatingGrid[seatRow][seatColumn] = occupiedSeatMarker;
}

void deallocateSeat(int seatRow, int seatColumn)
{
    seatingGrid[seatRow][seatColumn] = availableSeatMarker;
}

bool isSeatAvailable(int seatRow, int seatColumn)
{
    return seatingGrid[seatRow][seatColumn] == availableSeatMarker;
}

int countAvailableSeats()
{
    int availableCount = 0;
    for (int row = 0; row < totalRows; row++)
    {
        for (int column = 0; column < totalColumns; column++)
        {
            if (seatingGrid[row][column] == availableSeatMarker)
            {
                availableCount++;
            }
        }
    }
    return availableCount;
}

void displaySeatingGrid()
{
    cout << "\n========================================\n";
    cout << "         SEATING GRID (30x6)\n";
    cout << "========================================\n\n";

    cout << "     ";
    for (int col = 0; col < totalColumns; col++)
    {
        cout << " " << columnIndexToLetter(col) << "  ";
    }
    cout << "\n\n";

    cout << "======== FIRST CLASS (Rows 1-3) ========\n";
    for (int row = firstClassStartRow; row <= firstClassEndRow; row++)
    {
        cout << setw(2) << (row + 1) << "   ";
        for (int column = 0; column < totalColumns; column++)
        {
            if (seatingGrid[row][column] == availableSeatMarker)
            {
                cout << " 0  ";
            }
            else
            {
                cout << " X  ";
            }
        }
        cout << "\n";
    }

    cout << "\n";
    cout << "====== BUSINESS CLASS (Rows 4-10) ======\n";
    for (int row = businessClassStartRow; row <= businessClassEndRow; row++)
    {
        cout << setw(2) << (row + 1) << "   ";
        for (int column = 0; column < totalColumns; column++)
        {
            if (seatingGrid[row][column] == availableSeatMarker)
            {
                cout << " 0  ";
            }
            else
            {
                cout << " X  ";
            }
        }
        cout << "\n";
    }

    cout << "\n";
    cout << "====== ECONOMY CLASS (Rows 11-30) ======\n";
    for (int row = economyClassStartRow; row <= economyClassEndRow; row++)
    {
        cout << setw(2) << (row + 1) << "   ";
        for (int column = 0; column < totalColumns; column++)
        {
            if (seatingGrid[row][column] == availableSeatMarker)
            {
                cout << " 0  ";
            }
            else
            {
                cout << " X  ";
            }
        }
        cout << "\n";
    }

    cout << "\n";
    cout << "Legend: 0 = Available, X = Occupied\n";
    cout << "Total Available Seats: " << countAvailableSeats() << "/" << totalSeats << "\n";
}

// ============================================================================
// ARRAY MANAGEMENT FUNCTIONS
// ============================================================================

void initializePassengerArray()
{
    for (int i = 0; i < maxPassengers; i++)
    {
        passengerArray[i] = Passenger();
    }
    activePassengerCount = 0;
}

int findPassengerByID(const string &passengerId)
{
    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive && passengerArray[i].passengerId == passengerId)
        {
            return i;
        }
    }
    return -1;
}

int findPassengerBySeat(int seatRow, int seatColumn)
{
    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive &&
            passengerArray[i].seatRow == seatRow &&
            passengerArray[i].seatColumn == seatColumn)
        {
            return i;
        }
    }
    return -1;
}

int findPassengerByName(const string &passengerName)
{
    string searchNameUpper = toUpperCase(passengerName);

    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive)
        {
            string storedNameUpper = toUpperCase(passengerArray[i].passengerName);
            if (storedNameUpper == searchNameUpper)
            {
                return i;
            }
        }
    }
    return -1;
}

string generateUniquePassengerID()
{
    int maxID = 99999;

    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive && !passengerArray[i].passengerId.empty())
        {
            int currentID = stoi(passengerArray[i].passengerId);
            if (currentID > maxID)
            {
                maxID = currentID;
            }
        }
    }

    int newID = maxID + 1;
    return to_string(newID);
}

bool insertReservation(const string &passengerId, const string &passengerName,
                       int seatRow, int seatColumn, const string &passengerClass)
{
    if (activePassengerCount >= maxPassengers)
    {
        cout << "\n[ERROR] Cannot add reservation - array is full (max " << maxPassengers << " passengers).\n";
        return false;
    }

    if (findPassengerByID(passengerId) != -1)
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' already exists.\n";
        return false;
    }

    int duplicateNameIndex = findPassengerByName(passengerName);
    if (duplicateNameIndex != -1)
    {
        cout << "\n[ERROR] A passenger with name '" << passengerName << "' already exists.\n";
        cout << "Existing passenger details:\n";
        cout << "  - ID: " << passengerArray[duplicateNameIndex].passengerId << "\n";
        cout << "  - Seat: " << (passengerArray[duplicateNameIndex].seatRow + 1)
             << columnIndexToLetter(passengerArray[duplicateNameIndex].seatColumn) << "\n";
        cout << "  - Class: " << passengerArray[duplicateNameIndex].passengerClass << "\n";
        cout << "\nPlease use a different name or cancel the existing reservation first.\n";
        return false;
    }

    if (!isSeatAvailable(seatRow, seatColumn))
    {
        cout << "\n[ERROR] Seat " << (seatRow + 1) << columnIndexToLetter(seatColumn)
             << " is already occupied.\n";
        return false;
    }

    passengerArray[activePassengerCount].passengerId = passengerId;
    passengerArray[activePassengerCount].passengerName = passengerName;
    passengerArray[activePassengerCount].seatRow = seatRow;
    passengerArray[activePassengerCount].seatColumn = seatColumn;
    passengerArray[activePassengerCount].passengerClass = passengerClass;
    passengerArray[activePassengerCount].isActive = true;

    allocateSeat(seatRow, seatColumn);
    activePassengerCount++;

    cout << "\n[SUCCESS] Reservation added successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn)
         << " (" << passengerClass << " Class)\n";

    return true;
}

bool cancelReservation(const string &passengerId)
{
    int passengerIndex = findPassengerByID(passengerId);

    if (passengerIndex == -1)
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' not found!\n";
        return false;
    }

    int seatRow = passengerArray[passengerIndex].seatRow;
    int seatColumn = passengerArray[passengerIndex].seatColumn;
    string passengerName = passengerArray[passengerIndex].passengerName;

    deallocateSeat(seatRow, seatColumn);

    for (int i = passengerIndex; i < activePassengerCount - 1; i++)
    {
        passengerArray[i] = passengerArray[i + 1];
    }

    passengerArray[activePassengerCount - 1] = Passenger();
    activePassengerCount--;

    cout << "\n[SUCCESS] Reservation cancelled successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Freed Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn) << "\n";
    cout << "Active Passengers Remaining: " << activePassengerCount << "\n";

    return true;
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void displayAllPassengers()
{
    cout << "\n========================================\n";
    cout << "     ACTIVE RESERVATIONS (" << activePassengerCount << ")\n";
    cout << "========================================\n\n";

    if (activePassengerCount == 0)
    {
        cout << "No active reservations.\n";
        return;
    }

    cout << left << setw(10) << "ID"
         << setw(25) << "Name"
         << setw(8) << "Seat"
         << setw(12) << "Class" << "\n";
    cout << string(55, '-') << "\n";

    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive)
        {
            string seatNotation = to_string(passengerArray[i].seatRow + 1) +
                                  columnIndexToLetter(passengerArray[i].seatColumn);

            string displayName = passengerArray[i].passengerName;
            if (displayName.length() > 22)
            {
                displayName = displayName.substr(0, 22) + "...";
            }

            cout << left << setw(10) << passengerArray[i].passengerId
                 << setw(25) << displayName
                 << setw(8) << seatNotation
                 << setw(12) << passengerArray[i].passengerClass << "\n";
        }
    }

    cout << "\nTotal Active: " << activePassengerCount << "/" << maxPassengers << "\n";
}

// ============================================================================
// CSV FILE I/O FUNCTIONS
// ============================================================================

bool loadPassengerDataFromCSV()
{
    ifstream inputFile(csvFilePath);

    if (!inputFile.is_open())
    {
        cout << "[WARNING] Could not open file '" << csvFilePath << "'.\n";
        cout << "Starting with empty passenger list.\n";
        return false;
    }

    string line;
    int lineNumber = 0;
    int recordsLoaded = 0;
    int duplicateSeats = 0;
    int duplicateNames = 0;

    getline(inputFile, line);
    lineNumber++;

    while (getline(inputFile, line) && activePassengerCount < maxPassengers)
    {
        lineNumber++;

        if (line.empty())
            continue;

        stringstream ss(line);
        string passengerId, passengerName, seatRowStr, seatColumnStr, passengerClass;

        getline(ss, passengerId, ',');
        getline(ss, passengerName, ',');
        getline(ss, seatRowStr, ',');
        getline(ss, seatColumnStr, ',');
        getline(ss, passengerClass, ',');

        try
        {
            int seatRow = stoi(seatRowStr) - 1;
            int seatColumn = columnLetterToIndex(seatColumnStr[0]);

            if (seatRow < 0 || seatRow >= totalRows || seatColumn < 0 || seatColumn >= totalColumns)
            {
                continue;
            }

            int existingIndex = findPassengerBySeat(seatRow, seatColumn);
            if (existingIndex != -1)
            {
                passengerArray[existingIndex].passengerId = passengerId;
                passengerArray[existingIndex].passengerName = passengerName;
                passengerArray[existingIndex].passengerClass = passengerClass;
                duplicateSeats++;
            }
            else
            {
                int duplicateNameIndex = findPassengerByName(passengerName);
                if (duplicateNameIndex != -1)
                {
                    cout << "[WARNING] Skipping passenger '" << passengerName
                         << "' (ID: " << passengerId << ") - duplicate name.\n";
                    duplicateNames++;
                    continue;
                }

                passengerArray[activePassengerCount].passengerId = passengerId;
                passengerArray[activePassengerCount].passengerName = passengerName;
                passengerArray[activePassengerCount].seatRow = seatRow;
                passengerArray[activePassengerCount].seatColumn = seatColumn;
                passengerArray[activePassengerCount].passengerClass = passengerClass;
                passengerArray[activePassengerCount].isActive = true;

                allocateSeat(seatRow, seatColumn);
                activePassengerCount++;
                recordsLoaded++;
            }
        }
        catch (...)
        {
            continue;
        }
    }

    inputFile.close();

    cout << "\n[SUCCESS] CSV data loaded.\n";
    cout << "Records Loaded: " << recordsLoaded << "\n";
    cout << "Duplicate Seats Replaced: " << duplicateSeats << "\n";
    cout << "Duplicate Names Skipped: " << duplicateNames << "\n";
    cout << "Active Passengers: " << activePassengerCount << "\n";

    return true;
}

bool savePassengerDataToCSV()
{
    ofstream outputFile(csvFilePath);

    if (!outputFile.is_open())
    {
        cout << "\n[ERROR] Could not open file '" << csvFilePath << "' for writing.\n";
        return false;
    }

    outputFile << "PassengerID,Name,SeatRow,SeatColumn,Class\n";

    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive)
        {
            outputFile << passengerArray[i].passengerId << ","
                       << passengerArray[i].passengerName << ","
                       << (passengerArray[i].seatRow + 1) << ","
                       << columnIndexToLetter(passengerArray[i].seatColumn) << ","
                       << passengerArray[i].passengerClass << "\n";
        }
    }

    outputFile.close();

    cout << "\n[SUCCESS] Passenger data saved to '" << csvFilePath << "'.\n";
    cout << "Records Saved: " << activePassengerCount << "\n";

    return true;
}

// ============================================================================
// USER INTERACTION FUNCTIONS
// ============================================================================

void handleReservation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "          NEW RESERVATION\n";
    cout << "========================================\n";

    if (activePassengerCount >= maxPassengers)
    {
        cout << "\n[ERROR] Cannot add reservation - flight is fully booked!\n";
        cout << "Maximum capacity: " << maxPassengers << " passengers\n";
        cout << "Current passengers: " << activePassengerCount << "\n";
        pauseForUserInput();
        return;
    }

    int availableSeats = countAvailableSeats();
    if (availableSeats == 0)
    {
        cout << "\n[ERROR] No available seats remaining.\n";
        pauseForUserInput();
        return;
    }

    cout << "\nAvailable Seats: " << availableSeats << "/" << totalSeats << "\n";

    // Get passenger name - INLINE INPUT
    string passengerName;
    cout << "\nEnter Passenger Name: ";
    cin.ignore(10000, '\n');
    getline(cin, passengerName);
    passengerName = trimWhitespace(passengerName);

    if (passengerName.empty())
    {
        cout << "[ERROR] Name cannot be empty.\n";
        pauseForUserInput();
        return;
    }

    // Get passenger class - INLINE INPUT
    string passengerClass;
    cout << "Enter Class (First/Business/Economy): ";
    getline(cin, passengerClass);
    passengerClass = trimWhitespace(passengerClass);

    if (!isValidClass(passengerClass))
    {
        cout << "[ERROR] Invalid class. Must be First, Business, or Economy.\n";
        pauseForUserInput();
        return;
    }

    passengerClass = toUpperCase(passengerClass);
    if (passengerClass == "FIRST")
        passengerClass = "First";
    else if (passengerClass == "BUSINESS")
        passengerClass = "Business";
    else if (passengerClass == "ECONOMY")
        passengerClass = "Economy";

    // NOW SHOW SEATING GRID
    displaySeatingGrid();

    // Get seat row
    int seatRow;
    cout << "\nEnter Seat Row (1-30): ";
    if (!(cin >> seatRow))
    {
        clearInputBuffer();
        cout << "[ERROR] Invalid input. Row must be a number.\n";
        pauseForUserInput();
        return;
    }
    clearInputBuffer();

    if (seatRow < 1 || seatRow > totalRows)
    {
        cout << "[ERROR] Row must be between 1 and " << totalRows << ".\n";
        pauseForUserInput();
        return;
    }

    seatRow--;

    // Get seat column
    char seatColumnLetter;
    cout << "Enter Seat Column (A-F): ";
    cin >> seatColumnLetter;
    clearInputBuffer();

    int seatColumn = columnLetterToIndex(seatColumnLetter);
    if (seatColumn == -1)
    {
        cout << "[ERROR] Column must be between A and F.\n";
        pauseForUserInput();
        return;
    }

    string passengerId = generateUniquePassengerID();

    bool success = insertReservation(passengerId, passengerName, seatRow, seatColumn, passengerClass);

    if (success)
    {
        cout << "\nReservation Summary:\n";
        cout << "-------------------\n";
        cout << "ID: " << passengerId << "\n";
        cout << "Name: " << passengerName << "\n";
        cout << "Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn) << "\n";
        cout << "Class: " << passengerClass << "\n";
        cout << "Active Passengers: " << activePassengerCount << "/" << maxPassengers << "\n";
    }

    pauseForUserInput();
}

void handleCancellation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "       CANCEL RESERVATION\n";
    cout << "========================================\n";

    if (activePassengerCount == 0)
    {
        cout << "\n[ERROR] No reservations to cancel.\n";
        pauseForUserInput();
        return;
    }

    displayAllPassengers();

    string passengerId;
    cout << "\nEnter Passenger ID to cancel: ";
    cin.ignore(10000, '\n');
    getline(cin, passengerId);
    passengerId = trimWhitespace(passengerId);

    if (passengerId.empty())
    {
        cout << "[ERROR] Passenger ID cannot be empty.\n";
        pauseForUserInput();
        return;
    }

    bool success = cancelReservation(passengerId);

    if (success)
    {
        cout << "\nCurrent Status:\n";
        cout << "Active Passengers: " << activePassengerCount << "/" << maxPassengers << "\n";
        cout << "Available Seats: " << countAvailableSeats() << "/" << totalSeats << "\n";
    }

    pauseForUserInput();
}

// ============================================================================
// MAIN MENU & PROGRAM CONTROL
// ============================================================================

void displayMainMenu()
{
    cout << "\n========================================\n";
    cout << "  FLIGHT RESERVATION SYSTEM (ARRAY)\n";
    cout << "========================================\n";
    cout << "  Student ID: TP079279\n";
    cout << "  Active Passengers: " << activePassengerCount << "/" << maxPassengers << "\n";
    cout << "  Available Seats: " << countAvailableSeats() << "/" << totalSeats << "\n";
    cout << "========================================\n\n";
    cout << "1. Make a Reservation (Insert)\n";
    cout << "2. Cancel a Reservation (Delete)\n";
    cout << "3. View All Passengers\n";
    cout << "4. View Seating Grid\n";
    cout << "5. Save and Exit\n\n";
    cout << "Enter your choice (1-5): ";
}

int main()
{
    initializePassengerArray();
    initializeSeatingGrid();

    cout << "========================================\n";
    cout << "  FLIGHT RESERVATION SYSTEM - ARRAY\n";
    cout << "========================================\n\n";
    cout << "Loading passenger data from CSV...\n";

    loadPassengerDataFromCSV();

    pauseForUserInput();

    int choice;
    bool running = true;

    while (running)
    {
        clearScreen();
        displayMainMenu();

        if (!(cin >> choice))
        {
            clearInputBuffer();
            cout << "\n[ERROR] Invalid input. Please enter a number.\n";
            pauseForUserInput();
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            handleReservation();
            break;

        case 2:
            handleCancellation();
            break;

        case 3:
            clearScreen();
            displayAllPassengers();
            pauseForUserInput();
            break;

        case 4:
            clearScreen();
            displaySeatingGrid();
            pauseForUserInput();
            break;

        case 5:
            clearScreen();
            cout << "\n========================================\n";
            cout << "            SAVING & EXIT\n";
            cout << "========================================\n";
            savePassengerDataToCSV();
            cout << "\nThank you for using the Flight Reservation System!\n";
            cout << "Goodbye!\n\n";
            running = false;
            break;

        default:
            cout << "\n[ERROR] Invalid choice. Please enter 1-5.\n";
            pauseForUserInput();
        }
    }

    return 0;
}
