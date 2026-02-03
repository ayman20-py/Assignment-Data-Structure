/*
===============================================================================
PLANE FLIGHT RESERVATION SYSTEM - ARRAY VERSION
===============================================================================
Student ID: TP079279
Student ID: TP083605
Assignment: Data Structures Assignment Task 1
Component: Plane Reservation System (Array Version)
Date: January 2026

NEW FEATURES:
- Supports 10,000+ passengers across multiple planes
- Automatically creates new planes when current ones are full
- Each plane has 180 seats (30x6 seating grid)
- Can delete passengers from any specific plane
- Uses Array of Arrays (1D and 2D structure)
- 1D Array for passengers per plane
- 2D Array for seating grid per plane
===============================================================================
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>

using namespace std;

// ============================================================================
// CONSTANTS & CONFIGURATION
// ============================================================================

const int SEATS_PER_PLANE = 180; // Each plane: 30 rows × 6 columns
const int MAX_PLANES = 100;      // Support up to 100 planes (18,000 passengers)
const int ROWS_PER_PLANE = 30;
const int COLUMNS_PER_PLANE = 6;

const char AVAILABLE_SEAT = 'O';
const char OCCUPIED_SEAT = 'X';

const string COLUMN_LABELS = "ABCDEF";
const string CSV_FILE_PATH = "../Dataset/flight_passenger_data.csv_backup.csv";

// ============================================================================
// PASSENGER STRUCTURE
// ============================================================================

struct Passenger
{
    string passengerId;
    string passengerName;
    int planeNumber; // Which plane (0-99)
    int seatRow;     // Row in that plane (0-29)
    int seatColumn;  // Column in that plane (0-5)
    string passengerClass;
    bool isActive;

    Passenger() : passengerId(""), passengerName(""), planeNumber(-1),
                  seatRow(-1), seatColumn(-1), passengerClass(""), isActive(false) {}
};

// ============================================================================
// PLANE STRUCTURE
// ============================================================================

struct Plane
{
    int planeNumber;
    Passenger passengers[SEATS_PER_PLANE];               // Array of 180 passengers
    char seatingGrid[ROWS_PER_PLANE][COLUMNS_PER_PLANE]; // 2D seating grid
    int activePassengerCount;
    bool isActive;

    Plane() : planeNumber(-1), activePassengerCount(0), isActive(false) {}
};

// ============================================================================
// GLOBAL DATA STRUCTURES
// ============================================================================

Plane planes[MAX_PLANES]; // Array of planes
int activePlaneCount = 0; // How many planes are in use

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void handleReservation(); // Forward declaration for recursive call

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
        return "";
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
        return upperColumn - 'A';
    return -1;
}

char columnIndexToLetter(int columnIndex)
{
    return COLUMN_LABELS[columnIndex];
}

// ============================================================================
// PLANE MANAGEMENT FUNCTIONS
// ============================================================================

void initializePlane(int planeIndex)
{
    planes[planeIndex].planeNumber = planeIndex;
    planes[planeIndex].activePassengerCount = 0;
    planes[planeIndex].isActive = true;

    // Initialize seating grid
    for (int row = 0; row < ROWS_PER_PLANE; row++)
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
            planes[planeIndex].seatingGrid[row][col] = AVAILABLE_SEAT;

    // Initialize passengers
    for (int i = 0; i < SEATS_PER_PLANE; i++)
        planes[planeIndex].passengers[i] = Passenger();
}

int createNewPlane()
{
    if (activePlaneCount >= MAX_PLANES)
    {
        cout << "[ERROR] Maximum number of planes reached (" << MAX_PLANES << ").\n";
        return -1;
    }

    int newPlaneIndex = activePlaneCount;
    initializePlane(newPlaneIndex);
    activePlaneCount++;

    cout << "[INFO] Created new Plane #" << (newPlaneIndex + 1) << "\n";
    return newPlaneIndex;
}

int findPlaneWithAvailableSeat()
{
    // Find first plane with available space
    for (int i = 0; i < activePlaneCount; i++)
    {
        if (planes[i].isActive && planes[i].activePassengerCount < SEATS_PER_PLANE)
            return i;
    }

    // No plane has space, create new one
    return createNewPlane();
}

int getTotalPassengers()
{
    int total = 0;
    for (int i = 0; i < activePlaneCount; i++)
        if (planes[i].isActive)
            total += planes[i].activePassengerCount;
    return total;
}

int getTotalAvailableSeats()
{
    int total = 0;
    for (int i = 0; i < activePlaneCount; i++)
        if (planes[i].isActive)
            total += (SEATS_PER_PLANE - planes[i].activePassengerCount);
    return total;
}

// ============================================================================
// PASSENGER SEARCH FUNCTIONS
// ============================================================================

bool findPassengerByID(const string &passengerId, int &planeIndex, int &passengerIndex)
{
    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        for (int i = 0; i < planes[p].activePassengerCount; i++)
        {
            if (planes[p].passengers[i].isActive &&
                planes[p].passengers[i].passengerId == passengerId)
            {
                planeIndex = p;
                passengerIndex = i;
                return true;
            }
        }
    }
    return false;
}

bool isClassSectionFullOnPlane(int planeIndex, const string &className)
{
    int startRow, endRow;

    if (className == "First")
    {
        startRow = 0;
        endRow = 2;
    }
    else if (className == "Business")
    {
        startRow = 3;
        endRow = 9;
    }
    else if (className == "Economy")
    {
        startRow = 10;
        endRow = 29;
    }
    else
    {
        return true; // Unknown class, consider full
    }

    // Check if any seat is available in this class section
    for (int row = startRow; row <= endRow; row++)
    {
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        {
            if (planes[planeIndex].seatingGrid[row][col] == AVAILABLE_SEAT)
            {
                return false; // Found available seat, section NOT full
            }
        }
    }

    return true; // All seats occupied, section is FULL
}

string getClassFromSeatRow(int seatRow)
{
    if (seatRow >= 0 && seatRow <= 2)
        return "First";
    else if (seatRow >= 3 && seatRow <= 9)
        return "Business";
    else if (seatRow >= 10 && seatRow <= 29)
        return "Economy";
    return "Economy"; // Default
}

// Helper function to determine class based on seat row
// Helper function to count available seats in a specific class on a plane
int countAvailableSeatsInClass(int planeIndex, const string &className)
{
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
        return 0;

    int count = 0;
    int startRow, endRow;

    // Determine row range for the class
    if (className == "First")
    {
        startRow = 0;
        endRow = 2;
    }
    else if (className == "Business")
    {
        startRow = 3;
        endRow = 9;
    }
    else // Economy
    {
        startRow = 10;
        endRow = 29;
    }

    // Count available seats in this class
    for (int row = startRow; row <= endRow; row++)
    {
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        {
            if (planes[planeIndex].seatingGrid[row][col] == AVAILABLE_SEAT)
            {
                count++;
            }
        }
    }

    return count;
}

bool findPassengerByNameOnPlane(const string &passengerName, int planeIndex, int &passengerIndex)
{
    string searchNameUpper = toUpperCase(passengerName);

    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
        return false;

    for (int i = 0; i < planes[planeIndex].activePassengerCount; i++)
    {
        if (planes[planeIndex].passengers[i].isActive)
        {
            string storedNameUpper = toUpperCase(planes[planeIndex].passengers[i].passengerName);
            if (storedNameUpper == searchNameUpper)
            {
                passengerIndex = i;
                return true;
            }
        }
    }
    return false;
}

bool findPassengerByName(const string &passengerName, int &planeIndex, int &passengerIndex)
{
    string searchNameUpper = toUpperCase(passengerName);

    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        for (int i = 0; i < planes[p].activePassengerCount; i++)
        {
            if (planes[p].passengers[i].isActive)
            {
                string storedNameUpper = toUpperCase(planes[p].passengers[i].passengerName);
                if (storedNameUpper == searchNameUpper)
                {
                    planeIndex = p;
                    passengerIndex = i;
                    return true;
                }
            }
        }
    }
    return false;
}

string generateUniquePassengerID()
{
    int maxID = 99999;

    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        for (int i = 0; i < planes[p].activePassengerCount; i++)
        {
            if (planes[p].passengers[i].isActive && !planes[p].passengers[i].passengerId.empty())
            {
                int currentID = stoi(planes[p].passengers[i].passengerId);
                if (currentID > maxID)
                    maxID = currentID;
            }
        }
    }

    return to_string(maxID + 1);
}

// ============================================================================
// SEAT MANAGEMENT FUNCTIONS
// ============================================================================

bool isSeatAvailable(int planeIndex, int seatRow, int seatColumn)
{
    return planes[planeIndex].seatingGrid[seatRow][seatColumn] == AVAILABLE_SEAT;
}

void allocateSeat(int planeIndex, int seatRow, int seatColumn)
{
    planes[planeIndex].seatingGrid[seatRow][seatColumn] = OCCUPIED_SEAT;
}

void deallocateSeat(int planeIndex, int seatRow, int seatColumn)
{
    planes[planeIndex].seatingGrid[seatRow][seatColumn] = AVAILABLE_SEAT;
}

int findAvailableSeat(int planeIndex, int &seatRow, int &seatColumn)
{
    // Find first available seat in the plane
    for (int row = 0; row < ROWS_PER_PLANE; row++)
    {
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        {
            if (isSeatAvailable(planeIndex, row, col))
            {
                seatRow = row;
                seatColumn = col;
                return planeIndex;
            }
        }
    }
    return -1; // No seat available
}

// ============================================================================
// RESERVATION FUNCTIONS
// ============================================================================

bool insertReservation(const string &passengerId, const string &passengerName,
                       const string &passengerClass, int planeIndex, int seatRow, int seatColumn)
{
    // Validate plane index
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
    {
        cout << "\n[ERROR] Invalid plane number.\n";
        return false;
    }

    // Check for duplicate ID across ALL planes (IDs must be globally unique)
    int dummyPlane, dummyPass;
    if (findPassengerByID(passengerId, dummyPlane, dummyPass))
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' already exists.\n";
        return false;
    }

    // Check for duplicate name ONLY on this specific plane (names can duplicate across planes)
    int dummyIndex;
    if (findPassengerByNameOnPlane(passengerName, planeIndex, dummyIndex))
    {
        cout << "\n[ERROR] Passenger '" << passengerName << "' already exists on Plane #"
             << (planeIndex + 1) << ".\n";
        cout << "Same name is allowed on different planes, but not on the same plane.\n";
        return false;
    }

    // Check if seat is available
    if (!isSeatAvailable(planeIndex, seatRow, seatColumn))
    {
        cout << "\n[ERROR] Seat " << (seatRow + 1) << columnIndexToLetter(seatColumn)
             << " is already occupied.\n";
        return false;
    }

    // Check if plane is full
    if (planes[planeIndex].activePassengerCount >= SEATS_PER_PLANE)
    {
        cout << "\n[ERROR] Plane #" << (planeIndex + 1) << " is full.\n";
        return false;
    }

    // Add passenger to plane
    int passengerIndex = planes[planeIndex].activePassengerCount;

    planes[planeIndex].passengers[passengerIndex].passengerId = passengerId;
    planes[planeIndex].passengers[passengerIndex].passengerName = passengerName;
    planes[planeIndex].passengers[passengerIndex].planeNumber = planeIndex;
    planes[planeIndex].passengers[passengerIndex].seatRow = seatRow;
    planes[planeIndex].passengers[passengerIndex].seatColumn = seatColumn;
    planes[planeIndex].passengers[passengerIndex].passengerClass = passengerClass;
    planes[planeIndex].passengers[passengerIndex].isActive = true;

    allocateSeat(planeIndex, seatRow, seatColumn);
    planes[planeIndex].activePassengerCount++;

    cout << "\n[SUCCESS] Reservation added successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Plane: #" << (planeIndex + 1) << "\n";
    cout << "Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn)
         << " (" << passengerClass << " Class)\n";

    return true;
}

bool cancelReservation(const string &passengerId)
{
    int planeIndex, passengerIndex;

    if (!findPassengerByID(passengerId, planeIndex, passengerIndex))
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' not found!\n";
        return false;
    }

    Passenger &passenger = planes[planeIndex].passengers[passengerIndex];

    int seatRow = passenger.seatRow;
    int seatColumn = passenger.seatColumn;
    string passengerName = passenger.passengerName;

    // Free the seat
    deallocateSeat(planeIndex, seatRow, seatColumn);

    // Shift passengers left in the array
    for (int i = passengerIndex; i < planes[planeIndex].activePassengerCount - 1; i++)
    {
        planes[planeIndex].passengers[i] = planes[planeIndex].passengers[i + 1];
    }

    // Clear last passenger slot
    planes[planeIndex].passengers[planes[planeIndex].activePassengerCount - 1] = Passenger();
    planes[planeIndex].activePassengerCount--;

    cout << "\n[SUCCESS] Reservation cancelled successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Plane: #" << (planeIndex + 1) << "\n";
    cout << "Freed Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn) << "\n";

    return true;
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

// ============================================================================
// CSV FILE I/O
// ============================================================================

bool loadPassengerDataFromCSV()
{
    ifstream inputFile(CSV_FILE_PATH);

    if (!inputFile.is_open())
    {
        cout << "[WARNING] Could not open '" << CSV_FILE_PATH << "'.\n";
        cout << "Starting with empty system.\n";
        return false;
    }

    string line;
    int recordsLoaded = 0;
    int duplicateIDsSkipped = 0;

    getline(inputFile, line); // Skip header

    while (getline(inputFile, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        string passengerId, passengerName, seatRowStr, seatColumnStr, passengerClass;

        getline(ss, passengerId, ',');
        getline(ss, passengerName, ',');
        getline(ss, seatRowStr, ',');
        getline(ss, seatColumnStr, ',');
        getline(ss, passengerClass, ',');

        // Convert seat row from CSV (1-30) to 0-indexed (0-29)
        int csvSeatRow = stoi(seatRowStr) - 1;

        // AUTO-DETERMINE CLASS BASED ON SEAT ROW (ignore class column from CSV)
        string autoClass = getClassFromSeatRow(csvSeatRow);

        // Check for duplicate ID across ALL planes (must be globally unique)
        int dummyP, dummyI;
        if (findPassengerByID(passengerId, dummyP, dummyI))
        {
            duplicateIDsSkipped++;
            continue;
        }

        // Find a plane where this name doesn't exist AND has space
        int planeIndex = -1;
        int nameIdx;

        // First, try existing planes
        for (int i = 0; i < activePlaneCount; i++)
        {
            // Check if name doesn't exist on this plane AND plane has space
            if (!findPassengerByNameOnPlane(passengerName, i, nameIdx) &&
                planes[i].activePassengerCount < SEATS_PER_PLANE)
            {
                planeIndex = i;
                break;
            }
        }

        // If no suitable plane found, create a new one
        if (planeIndex == -1)
        {
            planeIndex = createNewPlane();
            if (planeIndex == -1)
            {
                cout << "[ERROR] Cannot create more planes. Stopping load.\n";
                break;
            }
        }

        // Find available seat on the selected plane
        int seatRow, seatColumn;
        if (findAvailableSeat(planeIndex, seatRow, seatColumn) == -1)
        {
            cout << "[ERROR] No available seat on plane " << (planeIndex + 1) << ". Skipping passenger.\n";
            continue;
        }

        // Insert passenger with AUTO-DETERMINED CLASS
        if (insertReservation(passengerId, passengerName, autoClass, planeIndex, seatRow, seatColumn))
        {
            recordsLoaded++;
        }
    }

    inputFile.close();

    cout << "\n[SUCCESS] CSV data loaded.\n";
    cout << "Records Loaded: " << recordsLoaded << "\n";
    if (duplicateIDsSkipped > 0)
        cout << "Duplicate IDs Skipped: " << duplicateIDsSkipped << "\n";
    cout << "Total Planes Created: " << activePlaneCount << "\n";
    cout << "Total Passengers: " << getTotalPassengers() << "\n";

    return true;
}

bool savePassengerDataToCSV()
{
    ofstream outputFile(CSV_FILE_PATH);

    if (!outputFile.is_open())
    {
        cout << "[ERROR] Could not open '" << CSV_FILE_PATH << "' for writing.\n";
        return false;
    }

    // Write header
    outputFile << "PassengerID,Name,SeatRow,SeatColumn,Class\n";

    int recordsSaved = 0;

    // Loop through all planes and all passengers
    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        for (int i = 0; i < SEATS_PER_PLANE; i++)
        {
            if (planes[p].passengers[i].isActive)
            {
                Passenger &pass = planes[p].passengers[i];

                // Write passenger data
                outputFile << pass.passengerId << ","
                           << pass.passengerName << ","
                           << (pass.seatRow + 1) << "," // Convert back to 1-indexed
                           << columnIndexToLetter(pass.seatColumn) << ","
                           << pass.passengerClass << "\n";

                recordsSaved++;
            }
        }
    }

    outputFile.close();

    cout << "\n[SUCCESS] " << recordsSaved << " passengers saved to CSV file.\n";
    return true;
}

// ============================================================================
// USER INTERACTION
// ============================================================================

void handleReservation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "          NEW RESERVATION\n";
    cout << "========================================\n\n";

    // Get passenger name
    string passengerName;
    while (true)
    {
        cout << "Enter Passenger Name: ";
        getline(cin, passengerName);
        passengerName = trimWhitespace(passengerName);

        if (passengerName.empty())
        {
            cout << "[ERROR] Name cannot be empty. Try again.\n";
            continue;
        }

        break; // Valid name (duplicates allowed)
    }

    // Get passenger class
    string passengerClass;
    while (true)
    {
        cout << "Enter Class (First/Business/Economy): ";
        getline(cin, passengerClass);
        passengerClass = trimWhitespace(passengerClass);

        if (!isValidClass(passengerClass))
        {
            cout << "[ERROR] Invalid class. Must be First, Business, or Economy. Try again.\n";
            continue;
        }

        passengerClass = toUpperCase(passengerClass);
        if (passengerClass == "FIRST")
            passengerClass = "First";
        else if (passengerClass == "BUSINESS")
            passengerClass = "Business";
        else if (passengerClass == "ECONOMY")
            passengerClass = "Economy";

        break; // Valid class
    }

    // Show available planes with class availability
    cout << "\n========================================\n";
    cout << "         AVAILABLE PLANES\n";
    cout << "========================================\n\n";

    bool hasAvailablePlane = false;
    for (int i = 0; i < activePlaneCount; i++)
    {
        if (planes[i].isActive && planes[i].activePassengerCount < SEATS_PER_PLANE)
        {
            int availableInClass = countAvailableSeatsInClass(i, passengerClass);
            int totalAvailable = SEATS_PER_PLANE - planes[i].activePassengerCount;

            cout << "Plane #" << (i + 1) << " - Total: " << totalAvailable << " seats available";

            if (availableInClass > 0)
            {
                cout << " (" << passengerClass << ": " << availableInClass << " seats) ✓";
            }
            else
            {
                cout << " (" << passengerClass << ": FULL) ✗";
            }
            cout << "\n";

            if (availableInClass > 0)
            {
                hasAvailablePlane = true;
            }
        }
    }

    if (!hasAvailablePlane && activePlaneCount < MAX_PLANES)
    {
        cout << "\nNo planes with available " << passengerClass << " class seats.\n";
        cout << "A new plane will be created.\n";
        int newPlane = createNewPlane();
        if (newPlane == -1)
        {
            cout << "[ERROR] Cannot create more planes.\n";
            pauseForUserInput();
            return;
        }
    }
    else if (!hasAvailablePlane)
    {
        cout << "[ERROR] No " << passengerClass << " class seats available and cannot create more planes.\n";
        pauseForUserInput();
        return;
    }

    // Select plane
    int selectedPlane;
    while (true)
    {
        cout << "\nEnter Plane Number (1-" << activePlaneCount << "): ";
        if (!(cin >> selectedPlane))
        {
            clearInputBuffer();
            cout << "[ERROR] Invalid input. Enter a number.\n";
            continue;
        }
        clearInputBuffer();

        selectedPlane--; // Convert to 0-indexed

        if (selectedPlane < 0 || selectedPlane >= activePlaneCount)
        {
            cout << "[ERROR] Invalid plane number. Try again.\n";
            continue;
        }

        if (planes[selectedPlane].activePassengerCount >= SEATS_PER_PLANE)
        {
            cout << "[ERROR] Plane #" << (selectedPlane + 1) << " is full. Choose another.\n";
            continue;
        }

        // Check if the specific CLASS has available seats on this plane
        int availableSeatsInClass = countAvailableSeatsInClass(selectedPlane, passengerClass);
        if (availableSeatsInClass == 0)
        {
            cout << "[ERROR] " << passengerClass << " class is FULL on Plane #"
                 << (selectedPlane + 1) << ".\n";
            cout << "Available seats in other classes:\n";
            cout << "  - First: " << countAvailableSeatsInClass(selectedPlane, "First") << " seats\n";
            cout << "  - Business: " << countAvailableSeatsInClass(selectedPlane, "Business") << " seats\n";
            cout << "  - Economy: " << countAvailableSeatsInClass(selectedPlane, "Economy") << " seats\n";
            cout << "Please choose a different plane or different class.\n";
            continue;
        }

        // Check for duplicate name on THIS plane only
        int dummyIdx;
        if (findPassengerByNameOnPlane(passengerName, selectedPlane, dummyIdx))
        {
            cout << "[ERROR] Passenger '" << passengerName << "' already exists on Plane #"
                 << (selectedPlane + 1) << ".\n";
            cout << "Please choose a different plane or use a different name.\n";
            continue;
        }

        break; // Valid plane
    }

    // Display seating grid for selected plane
    cout << "\n[INFO] Plane #" << (selectedPlane + 1) << " - "
         << (SEATS_PER_PLANE - planes[selectedPlane].activePassengerCount)
         << " seats available\n";

    // Determine valid row range for the selected class
    int minRow, maxRow;
    if (passengerClass == "First")
    {
        minRow = 1;
        maxRow = 3;
        cout << "\nYour class: First (Rows 1-3)\n";
    }
    else if (passengerClass == "Business")
    {
        minRow = 4;
        maxRow = 10;
        cout << "\nYour class: Business (Rows 4-10)\n";
    }
    else if (passengerClass == "Economy")
    {
        minRow = 11;
        maxRow = 30;
        cout << "\nYour class: Economy (Rows 11-30)\n";
    }

    // Select seat row and column with option to go back
    int seatRow;
    char seatColumnLetter;
    int seatColumn;
    bool seatSelected = false;

    while (!seatSelected)
    {
        // Select seat row
        while (true)
        {
            cout << "\nEnter Seat Row (" << minRow << "-" << maxRow << ") or 0 to choose different plane: ";
            if (!(cin >> seatRow))
            {
                clearInputBuffer();
                cout << "[ERROR] Invalid input. Enter a number.\n";
                continue;
            }
            clearInputBuffer();

            // Option to go back
            if (seatRow == 0)
            {
                cout << "\n[INFO] Going back to plane selection...\n";
                handleReservation(); // Restart the reservation process
                return;
            }

            if (seatRow < minRow || seatRow > maxRow)
            {
                cout << "[ERROR] Row must be between " << minRow << " and " << maxRow
                     << " for " << passengerClass << " class. Try again.\n";
                continue;
            }

            seatRow--; // Convert to 0-indexed
            break;     // Valid row
        }

        // Select seat column
        while (true)
        {
            cout << "Enter Seat Column (A-F) or 0 to go back: ";
            cin >> seatColumnLetter;
            clearInputBuffer();

            // Option to go back to row selection
            if (seatColumnLetter == '0')
            {
                seatRow++; // Restore for display
                break;     // Go back to row selection
            }

            seatColumn = columnLetterToIndex(seatColumnLetter);
            if (seatColumn == -1)
            {
                cout << "[ERROR] Column must be between A and F. Try again.\n";
                continue;
            }

            // Check if seat is available
            if (!isSeatAvailable(selectedPlane, seatRow, seatColumn))
            {
                cout << "[ERROR] Seat " << (seatRow + 1) << columnIndexToLetter(seatColumn)
                     << " is occupied. Choose another seat.\n";

                // Ask what they want to do
                cout << "\nOptions:\n";
                cout << "1. View seating grid again\n";
                cout << "2. Choose different plane\n";
                cout << "3. Try another seat\n";
                cout << "Enter choice (1-3): ";

                int choice;
                if (!(cin >> choice))
                {
                    clearInputBuffer();
                    continue;
                }
                clearInputBuffer();

                if (choice == 1)
                {
                    cout << "\n[INFO] Please try a different seat.\n";
                    seatRow++; // Restore for display
                    break;     // Go back to row selection
                }
                else if (choice == 2)
                {
                    cout << "\n[INFO] Going back to plane selection...\n";
                    handleReservation(); // Restart the reservation process
                    return;
                }
                else
                {
                    continue; // Try another column
                }
            }

            // Valid and available seat!
            seatSelected = true;
            break;
        }
    }

    // Generate ID and insert reservation
    // AUTO-DETERMINE CLASS based on the actual seat row selected
    string actualClass = getClassFromSeatRow(seatRow);
    string passengerId = generateUniquePassengerID();
    insertReservation(passengerId, passengerName, actualClass, selectedPlane, seatRow, seatColumn);

    // Auto-save to CSV after new reservation
    savePassengerDataToCSV();

    pauseForUserInput();
}

void handleCancellation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "       CANCEL RESERVATION\n";
    cout << "========================================\n\n";

    string passengerId;
    cout << "Enter Passenger ID to cancel: ";
    getline(cin, passengerId);
    passengerId = trimWhitespace(passengerId);

    if (passengerId.empty())
    {
        cout << "[ERROR] ID cannot be empty.\n";
        pauseForUserInput();
        return;
    }

    if (cancelReservation(passengerId))
    {
        // Auto-save to CSV after cancellation
        savePassengerDataToCSV();
    }

    pauseForUserInput();
}

// ============================================================================
// MAIN MENU
// ============================================================================

void displayMainMenu()
{
    cout << "\n========================================\n";
    cout << "  PLANE RESERVATION SYSTEM\n";
    cout << "========================================\n";
    cout << "  Students ID: TP079279 and TP083605\n";
    cout << "  Total Planes: " << activePlaneCount << "\n";
    cout << "  Total Passengers: " << getTotalPassengers() << "\n";
    cout << "========================================\n\n";
    cout << "1. Make a Reservation\n";
    cout << "2. Cancel a Reservation\n";
    cout << "3. Exit\n\n";
    cout << "Enter your choice (1-3): ";
}

int main()
{
    cout << "========================================\n";
    cout << "  PLANE RESERVATION SYSTEM\n";
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
            cout << "\n[ERROR] Invalid input.\n";
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
            cout << "\nSaving data to CSV...\n";
            savePassengerDataToCSV();
            cout << "Goodbye!\n";
            running = false;
            break;
        default:
            cout << "\n[ERROR] Invalid choice.\n";
            pauseForUserInput();
        }
    }

    return 0;
}