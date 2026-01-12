/*
===============================================================================
FLIGHT RESERVATION & CANCELLATION SYSTEM - ARRAY VERSION
===============================================================================
Student ID: TP082578
Assignment: Data Structures Assignment Task 1
Component: Reservation & Cancellation (Array Version)
Date: January 2026

Description:
This program implements a flight reservation and cancellation system using
ARRAY data structures. It manages passenger bookings with a fixed-size array
and maintains seat allocation in a 2D seating grid.

Key Features:
- Array-based passenger storage (fixed capacity)
- Reservation (insertion) with array management
- Cancellation (deletion) with array shifting
- Duplicate ID prevention
- Seat conflict detection
- Time complexity analysis included
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

// Seating grid dimensions (30 rows x 6 columns = 180 seats)
const int totalRows = 30;
const int totalColumns = 6;
const int totalSeats = totalRows * totalColumns; // 180 seats

// Array capacity for passenger storage
const int maxPassengers = 180; // Maximum passengers that can be stored

// Seating grid markers
const char availableSeatMarker = 'O'; // Open seat
const char occupiedSeatMarker = 'X';  // Taken seat

// Seat class boundaries (row-based)
const int firstClassStartRow = 0; // Rows 1-3 (indices 0-2)
const int firstClassEndRow = 2;
const int businessClassStartRow = 3; // Rows 4-10 (indices 3-9)
const int businessClassEndRow = 9;
const int economyClassStartRow = 10; // Rows 11-30 (indices 10-29)
const int economyClassEndRow = 29;

// Column labels for seat notation
const string columnLabels = "ABCDEF";

// CSV file path
const string csvFilePath = "../Dataset/flight_passenger_data.csv.csv";

// ============================================================================
// PASSENGER STRUCTURE
// ============================================================================

/**
 * Structure to store passenger information.
 * Used in the array-based storage system.
 */
struct Passenger
{
    string passengerId;    // Unique identifier (e.g., "100000")
    string passengerName;  // Full name
    int seatRow;           // Row index (0-based, 0-29)
    int seatColumn;        // Column index (0-based, 0-5)
    string passengerClass; // "First", "Business", or "Economy"
    bool isActive;         // true = active reservation, false = deleted/empty slot

    // Default constructor
    Passenger() : passengerId(""), passengerName(""), seatRow(-1),
                  seatColumn(-1), passengerClass(""), isActive(false) {}
};

// ============================================================================
// GLOBAL DATA STRUCTURES
// ============================================================================

// Array to store passengers (fixed size)
Passenger passengerArray[maxPassengers];

// Number of active passengers currently in the array
int activePassengerCount = 0;

// 2D seating grid to track seat availability
char seatingGrid[totalRows][totalColumns];

// ============================================================================
// UTILITY FUNCTIONS - INPUT & VALIDATION
// ============================================================================

/**
 * Clears the input buffer to prevent cin errors.
 */
void clearInputBuffer()
{
    cin.clear();
    cin.ignore(10000, '\n');
}

/**
 * Pauses execution and waits for user to press Enter.
 */
void pauseForUserInput()
{
    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
}

/**
 * Clears the terminal screen.
 */
void clearScreen()
{
    system("cls"); // Windows-specific
}

/**
 * Converts a string to uppercase.
 */
string toUpperCase(const string &input)
{
    string result = input;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

/**
 * Trims leading and trailing whitespace from a string.
 */
string trimWhitespace(const string &input)
{
    size_t start = input.find_first_not_of(" \t\n\r");
    size_t end = input.find_last_not_of(" \t\n\r");

    if (start == string::npos)
    {
        return ""; // String is all whitespace
    }

    return input.substr(start, end - start + 1);
}

/**
 * Validates passenger class input.
 * Returns true if valid (First, Business, or Economy).
 */
bool isValidClass(const string &passengerClass)
{
    string upperClass = toUpperCase(passengerClass);
    return (upperClass == "FIRST" || upperClass == "BUSINESS" || upperClass == "ECONOMY");
}

/**
 * Converts column letter (A-F) to index (0-5).
 * Returns -1 if invalid.
 */
int columnLetterToIndex(char columnLetter)
{
    char upperColumn = toupper(columnLetter);
    if (upperColumn >= 'A' && upperColumn <= 'F')
    {
        return upperColumn - 'A';
    }
    return -1;
}

/**
 * Converts column index (0-5) to letter (A-F).
 */
char columnIndexToLetter(int columnIndex)
{
    return columnLabels[columnIndex];
}

// ============================================================================
// SEATING GRID FUNCTIONS
// ============================================================================

/**
 * Initializes the seating grid with all seats available.
 * Time Complexity: O(rows × columns) = O(180)
 */
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

/**
 * Marks a seat as occupied in the seating grid.
 * Time Complexity: O(1) - direct array access
 */
void allocateSeat(int seatRow, int seatColumn)
{
    seatingGrid[seatRow][seatColumn] = occupiedSeatMarker;
}

/**
 * Marks a seat as available in the seating grid.
 * Time Complexity: O(1) - direct array access
 */
void deallocateSeat(int seatRow, int seatColumn)
{
    seatingGrid[seatRow][seatColumn] = availableSeatMarker;
}

/**
 * Checks if a seat is available.
 * Time Complexity: O(1) - direct array access
 */
bool isSeatAvailable(int seatRow, int seatColumn)
{
    return seatingGrid[seatRow][seatColumn] == availableSeatMarker;
}

/**
 * Counts the number of available seats.
 * Time Complexity: O(rows × columns) = O(180)
 */
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

/**
 * Displays the seating grid with class sections labeled.
 * Time Complexity: O(rows × columns) = O(180)
 */
void displaySeatingGrid()
{
    cout << "\n========================================\n";
    cout << "         SEATING GRID (30x6)\n";
    cout << "========================================\n\n";

    // Column headers
    cout << "     ";
    for (int col = 0; col < totalColumns; col++)
    {
        cout << columnIndexToLetter(col) << "  ";
    }
    cout << "\n";
    cout << "   ----------------------\n";

    // Display rows with class labels
    for (int row = 0; row < totalRows; row++)
    {
        // Row number (1-indexed for display)
        cout << setw(2) << (row + 1) << " | ";

        // Seat markers
        for (int column = 0; column < totalColumns; column++)
        {
            cout << seatingGrid[row][column] << "  ";
        }

        // Class label on the right
        if (row >= firstClassStartRow && row <= firstClassEndRow)
        {
            cout << " | FIRST";
        }
        else if (row >= businessClassStartRow && row <= businessClassEndRow)
        {
            cout << " | BUSINESS";
        }
        else
        {
            cout << " | ECONOMY";
        }

        cout << "\n";
    }

    cout << "\nLegend: O = Available, X = Occupied\n";
    cout << "Total Available Seats: " << countAvailableSeats() << "/" << totalSeats << "\n";
}

// ============================================================================
// ARRAY MANAGEMENT FUNCTIONS
// ============================================================================

/**
 * Initializes the passenger array with default values.
 * Time Complexity: O(n) where n = maxPassengers = 180
 */
void initializePassengerArray()
{
    for (int i = 0; i < maxPassengers; i++)
    {
        passengerArray[i].passengerId = "";
        passengerArray[i].passengerName = "";
        passengerArray[i].seatRow = -1;
        passengerArray[i].seatColumn = -1;
        passengerArray[i].passengerClass = "";
        passengerArray[i].isActive = false;
    }
    activePassengerCount = 0;
}

/**
 * Searches for a passenger by ID in the array.
 * Returns the index if found, -1 if not found.
 * Time Complexity: O(n) - linear search through active passengers
 */
int findPassengerByID(const string &passengerId)
{
    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive && passengerArray[i].passengerId == passengerId)
        {
            return i;
        }
    }
    return -1; // Not found
}

/**
 * Searches for a passenger by seat position.
 * Returns the index if found, -1 if not found.
 * Time Complexity: O(n) - linear search through active passengers
 */
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
    return -1; // Not found
}

/**
 * Generates a unique passenger ID.
 * Scans existing IDs and returns the next sequential number.
 * Time Complexity: O(n) - scan all passengers to find max ID
 */
string generateUniquePassengerID()
{
    int maxID = 99999; // Starting point

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

/**
 * ARRAY INSERTION FUNCTION
 * Inserts a new passenger into the array.
 *
 * Time Complexity: O(1) - insert at the end of the array
 *
 * Error Handling:
 * - Array full (capacity reached)
 * - Duplicate passenger ID
 * - Seat already taken
 *
 * Returns: true if successful, false if failed
 */
bool insertReservation(const string &passengerId, const string &passengerName,
                       int seatRow, int seatColumn, const string &passengerClass)
{

    // ERROR CHECK 1: Array full
    if (activePassengerCount >= maxPassengers)
    {
        cout << "\n[ERROR] Cannot add reservation - array is full (max " << maxPassengers << " passengers).\n";
        return false;
    }

    // ERROR CHECK 2: Duplicate ID
    if (findPassengerByID(passengerId) != -1)
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' already exists.\n";
        return false;
    }

    // ERROR CHECK 3: Seat already taken
    if (!isSeatAvailable(seatRow, seatColumn))
    {
        cout << "\n[ERROR] Seat " << (seatRow + 1) << columnIndexToLetter(seatColumn)
             << " is already occupied.\n";
        return false;
    }

    // INSERTION LOGIC - Add at the end of active passengers
    passengerArray[activePassengerCount].passengerId = passengerId;
    passengerArray[activePassengerCount].passengerName = passengerName;
    passengerArray[activePassengerCount].seatRow = seatRow;
    passengerArray[activePassengerCount].seatColumn = seatColumn;
    passengerArray[activePassengerCount].passengerClass = passengerClass;
    passengerArray[activePassengerCount].isActive = true;

    // Allocate seat in grid
    allocateSeat(seatRow, seatColumn);

    // Increment active count
    activePassengerCount++;

    cout << "\n[SUCCESS] Reservation added successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn)
         << " (" << passengerClass << " Class)\n";

    return true;
}

/**
 * ARRAY DELETION FUNCTION (WITH SHIFTING)
 * Removes a passenger from the array and shifts remaining elements left.
 *
 * Time Complexity: O(n) - must shift all elements after deleted position
 *
 * Deletion Logic:
 * 1. Find passenger by ID (linear search)
 * 2. Free the allocated seat
 * 3. Shift all passengers after the deleted position left by one
 * 4. Decrement active passenger count
 *
 * Error Handling:
 * - Passenger ID not found
 *
 * Returns: true if successful, false if failed
 */
bool cancelReservation(const string &passengerId)
{

    // STEP 1: Find passenger by ID
    int passengerIndex = findPassengerByID(passengerId);

    // ERROR CHECK: Passenger not found
    if (passengerIndex == -1)
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' not found.\n";
        return false;
    }

    // Store seat information before deletion
    int seatRow = passengerArray[passengerIndex].seatRow;
    int seatColumn = passengerArray[passengerIndex].seatColumn;
    string passengerName = passengerArray[passengerIndex].passengerName;

    // STEP 2: Free the allocated seat
    deallocateSeat(seatRow, seatColumn);

    // STEP 3: SHIFT ELEMENTS LEFT
    // Time Complexity: O(n) - worst case when deleting first element
    for (int i = passengerIndex; i < activePassengerCount - 1; i++)
    {
        passengerArray[i] = passengerArray[i + 1];
    }

    // STEP 4: Clear the last element (now duplicated)
    passengerArray[activePassengerCount - 1] = Passenger(); // Reset to default

    // STEP 5: Decrement active count
    activePassengerCount--;

    cout << "\n[SUCCESS] Reservation cancelled successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Freed Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn) << "\n";
    cout << "Active Passengers Remaining: " << activePassengerCount << "\n";

    return true;
}

/**
 * Alternative: LOGICAL DELETION (without shifting)
 * Marks passenger as inactive without shifting array elements.
 *
 * Time Complexity: O(1) - no shifting required
 *
 * Note: This approach is faster but wastes array space.
 * Use when performance is critical and memory is not a concern.
 */
bool cancelReservationLogical(const string &passengerId)
{
    int passengerIndex = findPassengerByID(passengerId);

    if (passengerIndex == -1)
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' not found.\n";
        return false;
    }

    // Mark as inactive (logical deletion)
    passengerArray[passengerIndex].isActive = false;

    // Free the seat
    deallocateSeat(passengerArray[passengerIndex].seatRow,
                   passengerArray[passengerIndex].seatColumn);

    cout << "\n[SUCCESS] Reservation cancelled (logical deletion).\n";
    return true;
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

/**
 * Displays all active passengers in a formatted table.
 * Time Complexity: O(n) - iterate through all active passengers
 */
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

    // Table header
    cout << left << setw(10) << "ID"
         << setw(25) << "Name"
         << setw(8) << "Seat"
         << setw(12) << "Class" << "\n";
    cout << string(55, '-') << "\n";

    // Display each passenger
    for (int i = 0; i < activePassengerCount; i++)
    {
        if (passengerArray[i].isActive)
        {
            string seatNotation = to_string(passengerArray[i].seatRow + 1) +
                                  columnIndexToLetter(passengerArray[i].seatColumn);

            // Truncate long names
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

/**
 * Loads passenger data from CSV file.
 * Handles duplicate seats by keeping the most recent booking.
 * Time Complexity: O(n×m) where n = CSV records, m = active passengers
 */
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

    // Skip header row
    getline(inputFile, line);
    lineNumber++;

    // Read all passenger records
    while (getline(inputFile, line) && activePassengerCount < maxPassengers)
    {
        lineNumber++;

        // Skip empty lines
        if (line.empty())
            continue;

        // Parse CSV line
        stringstream ss(line);
        string passengerId, passengerName, seatRowStr, seatColumnStr, passengerClass;

        getline(ss, passengerId, ',');
        getline(ss, passengerName, ',');
        getline(ss, seatRowStr, ',');
        getline(ss, seatColumnStr, ',');
        getline(ss, passengerClass, ',');

        // Validate and convert data
        try
        {
            int seatRow = stoi(seatRowStr) - 1; // Convert to 0-indexed
            int seatColumn = columnLetterToIndex(seatColumnStr[0]);

            if (seatRow < 0 || seatRow >= totalRows || seatColumn < 0 || seatColumn >= totalColumns)
            {
                continue; // Skip invalid seat
            }

            // Check if seat already has a passenger (keep most recent)
            int existingIndex = findPassengerBySeat(seatRow, seatColumn);
            if (existingIndex != -1)
            {
                // Replace with newer booking
                passengerArray[existingIndex].passengerId = passengerId;
                passengerArray[existingIndex].passengerName = passengerName;
                passengerArray[existingIndex].passengerClass = passengerClass;
                duplicateSeats++;
            }
            else
            {
                // Add new passenger
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
            // Skip malformed records
            continue;
        }
    }

    inputFile.close();

    cout << "\n[SUCCESS] CSV data loaded.\n";
    cout << "Records Loaded: " << recordsLoaded << "\n";
    cout << "Duplicate Seats Replaced: " << duplicateSeats << "\n";
    cout << "Active Passengers: " << activePassengerCount << "\n";

    return true;
}

/**
 * Saves current passenger data to CSV file.
 * Overwrites the file with current state.
 * Time Complexity: O(n) - write all active passengers
 */
bool savePassengerDataToCSV()
{
    ofstream outputFile(csvFilePath);

    if (!outputFile.is_open())
    {
        cout << "\n[ERROR] Could not open file '" << csvFilePath << "' for writing.\n";
        return false;
    }

    // Write header
    outputFile << "PassengerID,Name,SeatRow,SeatColumn,Class\n";

    // Write all active passengers
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

/**
 * Handles the reservation process with user input.
 * Time Complexity: O(n) - dominated by ID generation and duplicate checks
 */
void handleReservation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "          NEW RESERVATION\n";
    cout << "========================================\n";

    // Check if array is full
    if (activePassengerCount >= maxPassengers)
    {
        cout << "\n[ERROR] Cannot add reservation - flight is fully booked!\n";
        cout << "Maximum capacity: " << maxPassengers << " passengers\n";
        cout << "Current passengers: " << activePassengerCount << "\n";
        pauseForUserInput();
        return;
    }

    // Check available seats
    int availableSeats = countAvailableSeats();
    if (availableSeats == 0)
    {
        cout << "\n[ERROR] No available seats remaining.\n";
        pauseForUserInput();
        return;
    }

    cout << "\nAvailable Seats: " << availableSeats << "/" << totalSeats << "\n";

    // Get passenger name
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

    // Get passenger class
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

    // Normalize class name
    passengerClass = toUpperCase(passengerClass);
    if (passengerClass == "FIRST")
        passengerClass = "First";
    else if (passengerClass == "BUSINESS")
        passengerClass = "Business";
    else if (passengerClass == "ECONOMY")
        passengerClass = "Economy";

    // Display seating grid
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

    seatRow--; // Convert to 0-indexed

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

    // Generate unique passenger ID
    string passengerId = generateUniquePassengerID();

    // Attempt insertion
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

/**
 * Handles the cancellation process with user input.
 * Time Complexity: O(n) - dominated by array shifting in deletion
 */
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

    // Display current passengers
    displayAllPassengers();

    // Get passenger ID to cancel
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

    // Attempt deletion (with shifting)
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

/**
 * Displays the main menu.
 */
void displayMainMenu()
{
    cout << "\n========================================\n";
    cout << "  FLIGHT RESERVATION SYSTEM (ARRAY)\n";
    cout << "========================================\n";
    cout << "  Student ID: TP082578\n";
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

/**
 * Main program entry point.
 */
int main()
{
    // Initialize data structures
    initializePassengerArray();
    initializeSeatingGrid();

    // Load existing data from CSV
    cout << "========================================\n";
    cout << "  FLIGHT RESERVATION SYSTEM - ARRAY\n";
    cout << "========================================\n\n";
    cout << "Loading passenger data from CSV...\n";

    loadPassengerDataFromCSV();

    pauseForUserInput();

    // Main program loop
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

/*
===============================================================================
TIME COMPLEXITY ANALYSIS
===============================================================================

INSERTION (insertReservation):
- Best Case: O(1)
  * Direct insertion at end of array
  * No duplicate checks needed
  * Seat availability check is O(1)

- Worst Case: O(n) where n = activePassengerCount
  * Duplicate ID check requires linear search: O(n)
  * ID generation scans all passengers: O(n)
  * Actual insertion is still O(1)

DELETION (cancelReservation):
- Best Case: O(n)
  * Even if passenger is at the end, still need to search: O(n)
  * If at end, no shifting needed: O(1)
  * Total: O(n)

- Worst Case: O(n)
  * Search for passenger by ID: O(n)
  * Shift all remaining elements: O(n)
  * Total: O(n) + O(n) = O(2n) = O(n)

SEARCH (findPassengerByID):
- Always O(n)
  * Must potentially check all passengers
  * Linear search through array

DISPLAY (displayAllPassengers):
- Always O(n)
  * Must iterate through all active passengers

===============================================================================
ARRAY VS LINKED LIST COMPARISON
===============================================================================

ARRAYS:
+ Fast random access: O(1)
+ Better cache locality (contiguous memory)
+ Simpler to implement
+ Lower memory overhead (no pointers)
- Fixed size (cannot grow)
- Deletion requires shifting: O(n)
- Insertion at beginning/middle: O(n)

LINKED LISTS:
+ Dynamic size (can grow indefinitely)
+ Fast insertion/deletion at known position: O(1)
+ No wasted space from unused capacity
- Slow search: O(n)
- Poor cache locality (scattered memory)
- Extra memory for pointers

FOR THIS APPLICATION:
- Array is suitable because:
  * Maximum 180 passengers (known fixed capacity)
  * Mostly append operations (end of array)
  * Random access needed for seat checks
  * Memory efficiency (180 slots vs unlimited nodes)

===============================================================================
*/