/*
===============================================================================
PLANE FLIGHT RESERVATION SYSTEM - ARRAY VERSION
===============================================================================
Student ID: TP079279 (Menu 1 & 2: Reservation & Cancellation)
Student ID: TP083605 (Menu 3, 4 & 5: Seat Lookup, Manifest, Seat Report)
Assignment: Data Structures Assignment Task 1
Component: Plane Reservation System (Array Version)
Date: Feb 2026

CODE ORGANIZATION:
- Section 1: Shared/Common Functions (Both team members use)
- Section 2: TP079279 - Reservation (Insertion) Functions
- Section 3: TP079279 - Reservation Menu Handler (Menu 1)
- Section 4: TP079279 - Cancellation Functions (Menu 2)
- Section 5: TP083605 - Seat Lookup Functions (Menu 3)
- Section 6: TP083605 - Manifest Report Functions (Menu 4)
- Section 7: TP083605 - Seat Report Functions (Menu 5)
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
// CONSTANTS & CONFIGURATION (SHARED BY BOTH TEAM MEMBERS)
// ============================================================================

const int SEATS_PER_PLANE = 180;
const int MAX_PLANES = 1000;
const int ROWS_PER_PLANE = 30;
const int COLUMNS_PER_PLANE = 6;

const char AVAILABLE_SEAT = 'O';
const char OCCUPIED_SEAT = 'X';

const string COLUMN_LABELS = "ABCDEF";
const string CSV_FILE_PATH = "Dataset/flight_passenger_data.csv.csv";

// ============================================================================
// DATA STRUCTURES (SHARED BY BOTH TEAM MEMBERS)
// ============================================================================

struct Passenger
{
    string passengerId;
    string passengerName;
    int planeNumber;
    int seatRow;
    int seatColumn;
    string passengerClass;
    bool isActive;

    Passenger() : passengerId(""), passengerName(""), planeNumber(-1),
                  seatRow(-1), seatColumn(-1), passengerClass(""), isActive(false) {}
};

struct Plane
{
    int planeNumber;
    Passenger passengers[SEATS_PER_PLANE];               // 1D Array
    char seatingGrid[ROWS_PER_PLANE][COLUMNS_PER_PLANE]; // 2D Array
    int activePassengerCount;
    bool isActive;

    Plane() : planeNumber(-1), activePassengerCount(0), isActive(false) {}
};

// Global Arrays
Plane planes[MAX_PLANES]; // 1D Array of planes
int activePlaneCount = 0;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

// TP079279 - Reservation & Cancellation
void handleReservation();
void handleCancellation();
bool insertReservation(const string &passengerId, const string &passengerName,
                       const string &passengerClass, int planeIndex, int seatRow, int seatColumn);
bool cancelReservation(const string &passengerId);

// TP083605 - Seat Lookup, Manifest, Seat Report
void handleSeatLookup();
void handleManifestMenu();
void handleSeatReport();
void displayPassengerDetails(int planeIndex, int passengerIndex);
void displayManifestForPlane(int planeIndex);
void displayManifestForPlane_FilterByClass(int planeIndex, const string &className);
void displayPassengersInRow(int planeIndex, int rowNumber);
void displaySeatingChartForPlane(int planeIndex);

// Shared functions
void displayAllPassengersForCancellation();
bool loadPassengerDataFromCSV();
bool savePassengerDataToCSV();

// ============================================================================
// SECTION 1: SHARED/COMMON FUNCTIONS (USED BY BOTH TEAM MEMBERS)
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// 1.1 Utility Functions
// ────────────────────────────────────────────────────────────────────────────

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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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

// ────────────────────────────────────────────────────────────────────────────
// 1.2 Plane Management Functions (1D Array Operations)
// ────────────────────────────────────────────────────────────────────────────

void initializePlane(int planeIndex)
{
    planes[planeIndex].planeNumber = planeIndex;
    planes[planeIndex].activePassengerCount = 0;
    planes[planeIndex].isActive = true;

    // Initialize 2D seating grid
    for (int row = 0; row < ROWS_PER_PLANE; row++)
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
            planes[planeIndex].seatingGrid[row][col] = AVAILABLE_SEAT;

    // Initialize 1D passengers array
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
    // Linear search through 1D array of planes
    for (int i = 0; i < activePlaneCount; i++)
    {
        if (planes[i].isActive && planes[i].activePassengerCount < SEATS_PER_PLANE)
            return i;
    }

    return createNewPlane();
}

int getTotalPassengers()
{
    int total = 0;
    // Traverse 1D array of planes
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

// ────────────────────────────────────────────────────────────────────────────
// 1.3 Search Functions (1D Array Search)
// ────────────────────────────────────────────────────────────────────────────

bool findPassengerByID(const string &passengerId, int &planeIndex, int &passengerIndex)
{
    // Linear search through 1D array of planes
    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        // Linear search through 1D array of passengers
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

bool findPassengerByNameOnPlane(const string &passengerName, int planeIndex, int &passengerIndex)
{
    string searchNameUpper = toUpperCase(passengerName);

    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
        return false;

    // Linear search through 1D array of passengers on specific plane
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

string generateUniquePassengerID()
{
    int maxID = 99999;

    // Traverse all planes (1D array)
    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        // Traverse all passengers (1D array)
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

// ────────────────────────────────────────────────────────────────────────────
// 1.4 Seat Management Functions (2D Array Operations)
// ────────────────────────────────────────────────────────────────────────────

bool isSeatAvailable(int planeIndex, int seatRow, int seatColumn)
{
    // Direct access to 2D array
    return planes[planeIndex].seatingGrid[seatRow][seatColumn] == AVAILABLE_SEAT;
}

void allocateSeat(int planeIndex, int seatRow, int seatColumn)
{
    // Update 2D array
    planes[planeIndex].seatingGrid[seatRow][seatColumn] = OCCUPIED_SEAT;
}

void deallocateSeat(int planeIndex, int seatRow, int seatColumn)
{
    // Update 2D array
    planes[planeIndex].seatingGrid[seatRow][seatColumn] = AVAILABLE_SEAT;
}

int findAvailableSeat(int planeIndex, int &seatRow, int &seatColumn)
{
    // Sequential scan of 2D grid
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
    return -1;
}

// ────────────────────────────────────────────────────────────────────────────
// 1.5 Class Helper Functions
// ────────────────────────────────────────────────────────────────────────────

string getClassFromSeatRow(int seatRow)
{
    if (seatRow >= 0 && seatRow <= 2)
        return "First";
    else if (seatRow >= 3 && seatRow <= 9)
        return "Business";
    else if (seatRow >= 10 && seatRow <= 29)
        return "Economy";
    return "Economy";
}

int countAvailableSeatsInClass(int planeIndex, const string &className)
{
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
        return 0;

    int count = 0;
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
    else
    {
        startRow = 10;
        endRow = 29;
    }

    // Scan 2D grid in class range
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

// ────────────────────────────────────────────────────────────────────────────
// 1.6 CSV File I/O Functions
// ────────────────────────────────────────────────────────────────────────────

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

        // Convert seat from CSV (1-30, A-F) to 0-indexed
        int seatRow = stoi(seatRowStr) - 1; // CSV: 1-30 → Code: 0-29
        char seatColLetter = seatColumnStr[0];
        int seatColumn = columnLetterToIndex(seatColLetter); // A-F → 0-5

        // Auto-determine class from row
        string autoClass = getClassFromSeatRow(seatRow);

        // Check for duplicate ID globally
        int dummyP, dummyI;
        if (findPassengerByID(passengerId, dummyP, dummyI))
        {
            duplicateIDsSkipped++;
            continue;
        }

        // ═══════════════════════════════════════════════════════════════════
        // NEW LOGIC: Use EXACT seat from CSV
        // Find a plane where this EXACT seat is available
        // ═══════════════════════════════════════════════════════════════════
        int planeIndex = -1;

        // Search through existing planes for this exact seat
        for (int i = 0; i < activePlaneCount; i++)
        {
            if (isSeatAvailable(i, seatRow, seatColumn))
            {
                planeIndex = i;
                break; // Found plane with this seat available
            }
        }

        // If seat taken on all existing planes, create new plane
        if (planeIndex == -1)
        {
            planeIndex = createNewPlane();
            if (planeIndex == -1)
            {
                cout << "[ERROR] Cannot create more planes. Stopping load.\n";
                break;
            }
        }

        // Insert passenger with their EXACT seat from CSV
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
    cout << "Average passengers per plane: " << (getTotalPassengers() / activePlaneCount) << "\n";

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

    outputFile << "PassengerID,Name,SeatRow,SeatColumn,Class\n";

    int recordsSaved = 0;

    // Traverse all planes (1D array)
    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive)
            continue;

        // Traverse all passengers (1D array)
        for (int i = 0; i < SEATS_PER_PLANE; i++)
        {
            if (planes[p].passengers[i].isActive)
            {
                Passenger &pass = planes[p].passengers[i];

                outputFile << pass.passengerId << ","
                           << pass.passengerName << ","
                           << (pass.seatRow + 1) << ","
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

// ────────────────────────────────────────────────────────────────────────────
// 1.7 Display All Passengers (Helper for Cancellation)
// ────────────────────────────────────────────────────────────────────────────

void displayAllPassengersForCancellation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "    ALL PASSENGERS (ALL PLANES)\n";
    cout << "========================================\n\n";

    int totalCount = 0;
    int displayCount = 0;

    cout << left
         << setw(5) << "No"
         << setw(12) << "ID"
         << setw(22) << "Name"
         << setw(8) << "Plane"
         << setw(8) << "Seat"
         << setw(10) << "Class"
         << "\n";
    cout << string(65, '-') << "\n";

    // Traverse 1D array of planes
    for (int p = 0; p < activePlaneCount; p++)
    {
        if (!planes[p].isActive || planes[p].activePassengerCount == 0)
            continue;

        // Traverse 1D array of passengers
        for (int i = 0; i < planes[p].activePassengerCount; i++)
        {
            if (planes[p].passengers[i].isActive)
            {
                Passenger &pass = planes[p].passengers[i];
                string seat = to_string(pass.seatRow + 1) + columnIndexToLetter(pass.seatColumn);

                cout << left
                     << setw(5) << (++displayCount)
                     << setw(12) << pass.passengerId
                     << setw(22) << pass.passengerName.substr(0, 20)
                     << setw(8) << ("#" + to_string(p + 1))
                     << setw(8) << seat
                     << setw(10) << pass.passengerClass
                     << "\n";

                totalCount++;

                if (displayCount % 20 == 0)
                {
                    cout << "\n[Press Enter to continue or 'q' to stop viewing...]";
                    string input;
                    getline(cin, input);
                    if (!input.empty() && (input[0] == 'q' || input[0] == 'Q'))
                    {
                        cout << "\n"
                             << string(65, '=') << "\n";
                        cout << "Viewing stopped. Total passengers displayed: " << displayCount << "\n";
                        return;
                    }
                }
            }
        }
    }

    cout << "\n"
         << string(65, '=') << "\n";
    cout << "Total Passengers: " << totalCount << "\n";
    cout << "Total Active Planes: " << activePlaneCount << "\n";
    cout << string(65, '=') << "\n";
}

// ============================================================================
// SECTION 2: TP079279 - RESERVATION (INSERTION) FUNCTIONS
// ============================================================================
// This section handles inserting passengers into the 1D and 2D arrays
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

    // Check for duplicate ID globally (across all planes)
    int dummyPlane, dummyPass;
    if (findPassengerByID(passengerId, dummyPlane, dummyPass))
    {
        // Skip this error message during CSV load (silent)
        return false;
    }

    // NOTE: Duplicate name check removed to allow all passengers to load
    // Manual reservations still check for duplicates in handleReservation()

    // Check if seat is available in 2D grid
    if (!isSeatAvailable(planeIndex, seatRow, seatColumn))
    {
        return false;
    }

    // Check if plane is full
    if (planes[planeIndex].activePassengerCount >= SEATS_PER_PLANE)
    {
        return false;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // INSERTION INTO 1D ARRAY OF PASSENGERS
    // ═══════════════════════════════════════════════════════════════════════
    int passengerIndex = planes[planeIndex].activePassengerCount;

    planes[planeIndex].passengers[passengerIndex].passengerId = passengerId;
    planes[planeIndex].passengers[passengerIndex].passengerName = passengerName;
    planes[planeIndex].passengers[passengerIndex].planeNumber = planeIndex;
    planes[planeIndex].passengers[passengerIndex].seatRow = seatRow;
    planes[planeIndex].passengers[passengerIndex].seatColumn = seatColumn;
    planes[planeIndex].passengers[passengerIndex].passengerClass = passengerClass;
    planes[planeIndex].passengers[passengerIndex].isActive = true;

    // ═══════════════════════════════════════════════════════════════════════
    // UPDATE 2D SEATING GRID
    // ═══════════════════════════════════════════════════════════════════════
    allocateSeat(planeIndex, seatRow, seatColumn);

    planes[planeIndex].activePassengerCount++;

    // Only show success message if called from manual reservation (not CSV load)
    // We detect this by checking if we're in the middle of bulk loading
    static bool bulkLoading = false;
    if (bulkLoading)
    {
        cout << "\n[SUCCESS] Reservation added successfully!\n";
        cout << "Passenger ID: " << passengerId << "\n";
        cout << "Name: " << passengerName << "\n";
        cout << "Plane: #" << (planeIndex + 1) << "\n";
        cout << "Seat: " << (seatRow + 1)
             << columnIndexToLetter(seatColumn)
             << " (" << passengerClass << " Class)\n";
    }

    return true;
}

// ============================================================================
// SECTION 3: TP079279 - RESERVATION MENU HANDLER (MENU 1)
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

        break;
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

        break;
    }

    // Show available planes
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

        selectedPlane--;

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

        int dummyIdx;
        if (findPassengerByNameOnPlane(passengerName, selectedPlane, dummyIdx))
        {
            cout << "[ERROR] Passenger '" << passengerName << "' already exists on Plane #"
                 << (selectedPlane + 1) << ".\n";
            cout << "Please choose a different plane or use a different name.\n";
            continue;
        }

        break;
    }

    // Display seating grid (2D array visualization)
    clearScreen();
    cout << "\n========================================\n";
    cout << "    PLANE #" << (selectedPlane + 1) << " SEATING GRID\n";
    cout << "========================================\n\n";

    cout << "Your Class: " << passengerClass << "\n";
    cout << "Available Seats: " << (SEATS_PER_PLANE - planes[selectedPlane].activePassengerCount) << "\n\n";

    cout << "     ";
    for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        cout << " " << columnIndexToLetter(col) << "  ";
    cout << "\n\n";

    cout << "======== FIRST CLASS (Rows 1-3) ========\n";
    for (int row = 0; row <= 2; row++)
    {
        cout << setw(2) << (row + 1) << "   ";
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
            cout << (planes[selectedPlane].seatingGrid[row][col] == AVAILABLE_SEAT ? " O  " : " X  ");
        cout << "\n";
    }

    cout << "\n";

    cout << "====== BUSINESS CLASS (Rows 4-10) ======\n";
    for (int row = 3; row <= 9; row++)
    {
        cout << setw(2) << (row + 1) << "   ";
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
            cout << (planes[selectedPlane].seatingGrid[row][col] == AVAILABLE_SEAT ? " O  " : " X  ");
        cout << "\n";
    }

    cout << "\n";

    cout << "====== ECONOMY CLASS (Rows 11-30) ======\n";
    for (int row = 10; row <= 29; row++)
    {
        cout << setw(2) << (row + 1) << "   ";
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
            cout << (planes[selectedPlane].seatingGrid[row][col] == AVAILABLE_SEAT ? " O  " : " X  ");
        cout << "\n";
    }

    cout << "\n";
    cout << "Legend: O = Available, X = Occupied\n";

    // Determine valid row range for class
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

    // Select seat
    int seatRow;
    char seatColumnLetter;
    int seatColumn;
    bool seatSelected = false;

    while (!seatSelected)
    {
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

            if (seatRow == 0)
            {
                cout << "\n[INFO] Going back to plane selection...\n";
                handleReservation();
                return;
            }

            if (seatRow < minRow || seatRow > maxRow)
            {
                cout << "[ERROR] Row must be between " << minRow << " and " << maxRow
                     << " for " << passengerClass << " class. Try again.\n";
                continue;
            }

            seatRow--;
            break;
        }

        while (true)
        {
            cout << "Enter Seat Column (A-F) or 0 to go back: ";
            cin >> seatColumnLetter;
            clearInputBuffer();

            if (seatColumnLetter == '0')
            {
                seatRow++;
                break;
            }

            seatColumn = columnLetterToIndex(seatColumnLetter);
            if (seatColumn == -1)
            {
                cout << "[ERROR] Column must be between A and F. Try again.\n";
                continue;
            }

            if (!isSeatAvailable(selectedPlane, seatRow, seatColumn))
            {
                cout << "[ERROR] Seat " << (seatRow + 1) << columnIndexToLetter(seatColumn)
                     << " is occupied. Choose another seat.\n";

                cout << "\nOptions:\n";
                cout << "1. Try another seat\n";
                cout << "2. Choose different plane\n";
                cout << "Enter choice (1-2): ";

                int choice;
                if (!(cin >> choice))
                {
                    clearInputBuffer();
                    continue;
                }
                clearInputBuffer();

                if (choice == 2)
                {
                    cout << "\n[INFO] Going back to plane selection...\n";
                    handleReservation();
                    return;
                }
                else
                {
                    continue;
                }
            }

            seatSelected = true;
            break;
        }
    }

    // Generate ID and insert
    string actualClass = getClassFromSeatRow(seatRow);
    string passengerId = generateUniquePassengerID();
    insertReservation(passengerId, passengerName, actualClass, selectedPlane, seatRow, seatColumn);

    // Save to CSV
    savePassengerDataToCSV();

    pauseForUserInput();
}

// ============================================================================
// SECTION 4: TP079279 - CANCELLATION FUNCTIONS (MENU 2)
// ============================================================================

bool cancelReservation(const string &passengerId)
{
    int planeIndex, passengerIndex;

    // Search for passenger using linear search (1D arrays)
    if (!findPassengerByID(passengerId, planeIndex, passengerIndex))
    {
        cout << "\n[ERROR] Passenger ID '" << passengerId << "' not found!\n";
        return false;
    }

    Passenger &passenger = planes[planeIndex].passengers[passengerIndex];

    int seatRow = passenger.seatRow;
    int seatColumn = passenger.seatColumn;
    string passengerName = passenger.passengerName;

    // ═══════════════════════════════════════════════════════════════════════
    // FREE SEAT IN 2D GRID
    // ═══════════════════════════════════════════════════════════════════════
    deallocateSeat(planeIndex, seatRow, seatColumn);

    // ═══════════════════════════════════════════════════════════════════════
    // DELETE FROM 1D ARRAY (Shift elements left)
    // ═══════════════════════════════════════════════════════════════════════
    for (int i = passengerIndex; i < planes[planeIndex].activePassengerCount - 1; i++)
    {
        planes[planeIndex].passengers[i] = planes[planeIndex].passengers[i + 1];
    }

    // Clear last slot
    planes[planeIndex].passengers[planes[planeIndex].activePassengerCount - 1] = Passenger();
    planes[planeIndex].activePassengerCount--;

    cout << "\n[SUCCESS] Reservation cancelled successfully!\n";
    cout << "Passenger ID: " << passengerId << "\n";
    cout << "Name: " << passengerName << "\n";
    cout << "Plane: #" << (planeIndex + 1) << "\n";
    cout << "Freed Seat: " << (seatRow + 1) << columnIndexToLetter(seatColumn) << "\n";

    return true;
}

void handleCancellation()
{
    clearScreen();
    cout << "\n========================================\n";
    cout << "       CANCEL RESERVATION\n";
    cout << "========================================\n\n";

    // Option to view all passengers first
    cout << "Do you want to view all passengers before cancelling? (y/n): ";
    char viewChoice;
    cin >> viewChoice;
    clearInputBuffer();

    if (viewChoice == 'y' || viewChoice == 'Y')
    {
        displayAllPassengersForCancellation();
        cout << "\nPress Enter to continue to cancellation...";
        cin.get();
    }

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
        savePassengerDataToCSV();
    }

    pauseForUserInput();
}

// ============================================================================
// SECTION 5: TP083605 - SEAT LOOKUP FUNCTIONS (MENU 3)
// ============================================================================

void displayPassengerDetails(int planeIndex, int passengerIndex)
{
    // Create a reference to the passenger object so we don’t copy it
    Passenger &p = planes[planeIndex].passengers[passengerIndex];

    // Print a header for clarity
    cout << "\n========================================\n";
    cout << "          PASSENGER FOUND\n";
    cout << "========================================\n";

    // Display passenger information
    cout << "Passenger ID   : " << p.passengerId << "\n";
    cout << "Name           : " << p.passengerName << "\n";

    // Plane index is stored as 0-based, so add 1 for display
    cout << "Plane          : #" << (planeIndex + 1) << "\n";

    // Seat row is stored as 0-based, so add 1
    // columnIndexToLetter converts column index (0–5) to A–F
    cout << "Seat           : " << (p.seatRow + 1)
         << columnIndexToLetter(p.seatColumn) << "\n";

    // Display passenger class (First, Business, or Economy)
    cout << "Class          : " << p.passengerClass << "\n";

    // Footer line
    cout << "========================================\n";
}

void handleSeatLookup()
{
    // Clear the screen before showing the search menu
    clearScreen();

    // Print the seat lookup title
    cout << "\n========================================\n";
    cout << "            SEAT LOOKUP\n";
    cout << "========================================\n\n";

    // Ask the user to enter the Passenger ID
    string passengerId;
    cout << "Enter Passenger ID to search: ";
    getline(cin, passengerId);

    // Remove extra spaces from the input
    passengerId = trimWhitespace(passengerId);

    // Validate: Passenger ID must not be empty
    if (passengerId.empty())
    {
        cout << "\n[ERROR] Passenger ID cannot be empty.\n";
        pauseForUserInput();
        return; // Stop function if input is invalid
    }

    // Variables to store where the passenger is found
    int planeIndex = -1;
    int passengerIndex = -1;

    // Linear search through 1D arrays of planes and passengers
    if (findPassengerByID(passengerId, planeIndex, passengerIndex))
    {
        // If found, display passenger details
        displayPassengerDetails(planeIndex, passengerIndex);
    }
    else
    {
        // If not found, inform the user
        cout << "\n[INFO] Passenger ID '" << passengerId << "' not found.\n";
    }

    // Pause so the user can read the result
    pauseForUserInput();
}


// ============================================================================
// SECTION 6: TP083605 - MANIFEST REPORT FUNCTIONS (MENU 4)
// ============================================================================

void displayManifestForPlane(int planeIndex)
{
    // Clear the screen before displaying the manifest
    clearScreen();

    // Validate plane index and make sure the plane is active
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
    {
        cout << "\n[ERROR] Invalid plane number.\n";
        pauseForUserInput();
        return; // Stop if plane is invalid
    }

    // Check if the plane has no passengers
    if (planes[planeIndex].activePassengerCount == 0)
    {
        cout << "\n========================================\n";
        cout << "         PLANE #" << (planeIndex + 1) << " MANIFEST\n";
        cout << "========================================\n";
        cout << "\n[INFO] This plane has no passengers.\n";
        pauseForUserInput();
        return; // Nothing to display
    }

    // Display manifest header
    cout << "\n========================================\n";
    cout << "         PLANE #" << (planeIndex + 1) << " MANIFEST\n";
    cout << "========================================\n\n";

    // Display table column headings
    cout << left
         << setw(5) << "No"
         << setw(12) << "ID"
         << setw(22) << "Name"
         << setw(8) << "Seat"
         << setw(10) << "Class"
         << "\n";

    cout << "----------------------------------------\n";

    // Traverse the 1D array of passengers for this plane
    for (int i = 0; i < planes[planeIndex].activePassengerCount; i++)
    {
        // Skip inactive passengers
        if (!planes[planeIndex].passengers[i].isActive)
            continue;

        // Reference the current passenger
        Passenger &ps = planes[planeIndex].passengers[i];

        // Convert seat row and column into readable format (e.g. 12A)
        string seat = to_string(ps.seatRow + 1);
        seat += columnIndexToLetter(ps.seatColumn);

        // Display passenger details in table format
        cout << left
             << setw(5) << (i + 1)
             << setw(12) << ps.passengerId
             << setw(22) << ps.passengerName.substr(0, 20)
             << setw(8) << seat
             << setw(10) << ps.passengerClass
             << "\n";
    }

    // Display total passengers on this plane
    cout << "\n----------------------------------------\n";
    cout << "Passengers on this plane: " << planes[planeIndex].activePassengerCount << "\n";

    // Pause so the user can read the manifest
    pauseForUserInput();
}

void displayManifestForPlane_FilterByClass(int planeIndex, const string &className)
{
    // Clear the screen before showing results
    clearScreen();

    // Validate plane index + make sure plane is active
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
    {
        cout << "\n[ERROR] Invalid plane.\n";
        pauseForUserInput();
        return; // Stop if plane is invalid
    }

    // Print header showing which plane + which class is being filtered
    cout << "\n========================================\n";
    cout << "  PLANE #" << (planeIndex + 1) << " MANIFEST (" << className << " ONLY)\n";
    cout << "========================================\n\n";

    // Print table headings
    cout << left
         << setw(5) << "No"
         << setw(12) << "ID"
         << setw(22) << "Name"
         << setw(8) << "Seat"
         << setw(10) << "Class"
         << "\n";
    cout << "----------------------------------------\n";

    int no = 1; // Numbering ONLY for passengers that match the filter

    // Traverse the 1D passenger array, but ONLY display passengers in the chosen class
    for (int i = 0; i < planes[planeIndex].activePassengerCount; i++)
    {
        Passenger &ps = planes[planeIndex].passengers[i];

        // Skip inactive slots
        if (!ps.isActive)
            continue;

        // Skip passengers not in the requested class
        if (ps.passengerClass != className)
            continue;

        // Convert seat position into readable format (e.g. 12A)
        string seat = to_string(ps.seatRow + 1);
        seat += columnIndexToLetter(ps.seatColumn);

        // Print passenger info
        cout << left
             << setw(5) << no++
             << setw(12) << ps.passengerId
             << setw(22) << ps.passengerName.substr(0, 20)
             << setw(8) << seat
             << setw(10) << ps.passengerClass
             << "\n";
    }

    // If no passengers matched, no will still be 1
    if (no == 1)
        cout << "\n[INFO] No passengers found in this class.\n";

    pauseForUserInput(); // Pause so user can read the output
}

void displayPassengersInRow(int planeIndex, int rowNumber)
{
    // Clear the screen before showing the row passengers
    clearScreen();

    // Print header showing which plane + which row is being displayed
    cout << "\n========================================\n";
    cout << "  PLANE #" << (planeIndex + 1) << " - ROW " << (rowNumber + 1) << " PASSENGERS\n";
    cout << "========================================\n\n";

    // Print table headings
    cout << left
         << setw(5) << "No"
         << setw(8) << "Seat"
         << setw(12) << "ID"
         << setw(22) << "Name"
         << setw(10) << "Class"
         << "\n";
    cout << "----------------------------------------\n";

    int count = 0; // Counts how many passengers are found in that row

    // Check the 2D seating grid for occupied seats in THIS row only
    for (int col = 0; col < COLUMNS_PER_PLANE; col++)
    {
        // If the seat is occupied in the grid, then a passenger must exist there
        if (planes[planeIndex].seatingGrid[rowNumber][col] == OCCUPIED_SEAT)
        {
            // Now search the 1D passenger array to find the passenger who owns that seat
            for (int i = 0; i < SEATS_PER_PLANE; i++)
            {
                Passenger &ps = planes[planeIndex].passengers[i];

                // Match by seatRow + seatColumn
                if (ps.isActive && ps.seatRow == rowNumber && ps.seatColumn == col)
                {
                    // Convert seat to readable format (e.g. 5C)
                    string seat = to_string(ps.seatRow + 1);
                    seat += columnIndexToLetter(ps.seatColumn);

                    // Print passenger
                    cout << left
                         << setw(5) << (++count)
                         << setw(8) << seat
                         << setw(12) << ps.passengerId
                         << setw(22) << ps.passengerName.substr(0, 20)
                         << setw(10) << ps.passengerClass
                         << "\n";

                    break; // Stop searching once the passenger is found
                }
            }
        }
    }

    // Show message if the row has no passengers
    if (count == 0)
        cout << "\n[INFO] No passengers in this row.\n";
    else
        cout << "\n----------------------------------------\n"
             << "Total passengers in row " << (rowNumber + 1) << ": " << count << "\n";

    pauseForUserInput(); // Pause so user can read the output
}

void handleManifestMenu()
{
    // Clear screen and display menu options
    clearScreen();
    cout << "\n========================================\n";
    cout << "          MANIFEST OPTIONS\n";
    cout << "========================================\n";
    cout << "1. Normal (no sorting)\n";
    cout << "2. Display Passengers by Row\n";
    cout << "3. Filter: First Class ONLY\n";
    cout << "4. Filter: Business Class ONLY\n";
    cout << "5. Filter: Economy Class ONLY\n";
    cout << "Enter choice (1-5): ";

    int choice;

    // Validate menu choice input
    if (!(cin >> choice))
    {
        clearInputBuffer();
        cout << "\n[ERROR] Invalid input.\n";
        pauseForUserInput();
        return;
    }
    clearInputBuffer();

    int planeChoice;

    // Ask which plane the user wants to view
    cout << "\nEnter Plane Number (1-" << activePlaneCount << "): ";
    if (!(cin >> planeChoice))
    {
        clearInputBuffer();
        cout << "\n[ERROR] Invalid input.\n";
        pauseForUserInput();
        return;
    }
    clearInputBuffer();

    // Validate plane number range
    if (planeChoice < 1 || planeChoice > activePlaneCount)
    {
        cout << "\n[ERROR] Plane number must be between 1 and " << activePlaneCount << ".\n";
        pauseForUserInput();
        return;
    }

    planeChoice--; // Convert to 0-based index for array

    // Safety check: plane must exist and be active
    if (planeChoice < 0 || planeChoice >= activePlaneCount || !planes[planeChoice].isActive)
    {
        cout << "\n[ERROR] Invalid plane.\n";
        pauseForUserInput();
        return;
    }

    // Option 2: Show passengers in a specific row
    if (choice == 2)
    {
        int rowChoice;

        // Ask which row to display
        cout << "Enter Row Number (1-" << ROWS_PER_PLANE << "): ";
        if (!(cin >> rowChoice))
        {
            clearInputBuffer();
            cout << "\n[ERROR] Invalid input.\n";
            pauseForUserInput();
            return;
        }
        clearInputBuffer();

        // Validate row range
        if (rowChoice < 1 || rowChoice > ROWS_PER_PLANE)
        {
            cout << "\n[ERROR] Row number must be between 1 and " << ROWS_PER_PLANE << ".\n";
            pauseForUserInput();
            return;
        }

        // Convert to 0-based row index, then display passengers in that row
        displayPassengersInRow(planeChoice, rowChoice - 1);
        return;
    }

    // Option 3/4/5: Filter manifest by class
    if (choice == 3)
    {
        displayManifestForPlane_FilterByClass(planeChoice, "First");
        return;
    }
    else if (choice == 4)
    {
        displayManifestForPlane_FilterByClass(planeChoice, "Business");
        return;
    }
    else if (choice == 5)
    {
        displayManifestForPlane_FilterByClass(planeChoice, "Economy");
        return;
    }

    // Default option: show full manifest for the plane
    displayManifestForPlane(planeChoice);
}

// ============================================================================
// SECTION 7: TP083605 - SEAT REPORT FUNCTIONS (MENU 5)
// ============================================================================

void displaySeatingChartForPlane(int planeIndex)
{
    // Clear the screen before showing the seating chart
    clearScreen();

    // Validate the plane index:
    // - must be within range
    // - plane must be active
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive)
    {
        cout << "[ERROR] Invalid plane number.\n";
        pauseForUserInput(); // Wait so the user can read the error
        return; // Stop the function if plane is invalid
    }

    // Print the seating chart title
    cout << "=============================================\n";
    cout << "       PLANE #" << (planeIndex + 1) << " SEATING GRID\n";
    cout << "=============================================\n\n";

    // Print column headers (seat letters A–F)
    cout << "       A   B   C   D   E   F\n";

    // ---------------- FIRST CLASS ----------------
    // Rows 1 to 3 (stored as 0 to 2 in the array)
    cout << "\n======== FIRST CLASS (Rows 1-3) ========\n";
    for (int row = 0; row < 3; row++)
    {
        // Display the row number (converted back to 1-based)
        cout << setw(2) << (row + 1) << "    ";

        // Loop through all columns (A–F)
        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        {
            // Print seat status from the 2D array
            cout << " " << planes[planeIndex].seatingGrid[row][col] << "  ";
        }
        cout << "\n";
    }

    // ---------------- BUSINESS CLASS ----------------
    // Rows 4 to 10 (stored as 3 to 9)
    cout << "\n====== BUSINESS CLASS (Rows 4-10) ======\n";
    for (int row = 3; row < 10; row++)
    {
        cout << setw(2) << (row + 1) << " ";

        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        {
            cout << "   " << planes[planeIndex].seatingGrid[row][col];
        }
        cout << "\n";
    }

    // ---------------- ECONOMY CLASS ----------------
    // Rows 11 to 30 (stored as 10 to 29)
    cout << "\n====== ECONOMY CLASS (Rows 11-30) ======\n";
    for (int row = 10; row < ROWS_PER_PLANE; row++)
    {
        cout << setw(2) << (row + 1) << " ";

        for (int col = 0; col < COLUMNS_PER_PLANE; col++)
        {
            cout << "   " << planes[planeIndex].seatingGrid[row][col];
        }
        cout << "\n";
    }

    // Explain what the seat symbols mean
    cout << "\nLegend: " << AVAILABLE_SEAT
         << " = Available, " << OCCUPIED_SEAT << " = Occupied\n";

    // Pause so the user can view the seating chart
    pauseForUserInput();
}

void handleSeatReport()
{
    // Clear the screen so the seat report looks clean
    clearScreen();

    // Print the title of the seat report
    cout << "\n========================================\n";
    cout << "            SEAT REPORT\n";
    cout << "========================================\n\n";

    // If there are no planes in the system, stop here
    if (activePlaneCount == 0)
    {
        cout << "[INFO] No planes available.\n";
        pauseForUserInput(); // Wait for user before going back
        return;
    }

    int planeChoice;

    // Ask the user which plane they want to see
    cout << "Enter Plane Number (1-" << activePlaneCount << "): ";

    // Check if the user entered a valid number
    if (!(cin >> planeChoice))
    {
        clearInputBuffer(); // Clear invalid input
        cout << "[ERROR] Invalid input.\n";
        pauseForUserInput();
        return;
    }

    // Remove leftover newline from input buffer
    clearInputBuffer();

    // Check if the plane number is within the valid range
    if (planeChoice < 1 || planeChoice > activePlaneCount)
    {
        cout << "[ERROR] Plane number must be between 1 and " << activePlaneCount << ".\n";
        pauseForUserInput();
        return;
    }

    // Convert plane number from 1-based to 0-based index
    planeChoice--;

    // Display the full seating chart for the selected plane
    displaySeatingChartForPlane(planeChoice);
}

// ============================================================================
// SECTION 8: MAIN MENU & PROGRAM ENTRY
// ============================================================================

void displayMainMenu()
{
    cout << "\n========================================\n";
    cout << "  MULTI-PLANE RESERVATION SYSTEM\n";
    cout << "========================================\n";
    cout << "  TP079279: Menu 1 & 2 (Reservation & Cancellation)\n";
    cout << "  TP083605: Menu 3, 4 & 5 (Lookup, Manifest, Seat Report)\n";
    cout << "========================================\n";
    cout << "  Total Planes: " << activePlaneCount << "\n";
    cout << "  Total Passengers: " << getTotalPassengers() << "\n";
    cout << "========================================\n\n";
    cout << "1. Make a Reservation (TP079279)\n";
    cout << "2. Cancel a Reservation (TP079279)\n";
    cout << "3. Seat Lookup - Search by ID (TP083605)\n";
    cout << "4. Manifest Report - Passenger List (TP083605)\n";
    cout << "5. Seat Report - Seating Chart (TP083605)\n";
    cout << "6. Exit\n\n";
    cout << "Enter your choice (1-6): ";
}

/*
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
            handleReservation(); // TP079279
            break;
        case 2:
            handleCancellation(); // TP079279
            break;
        case 3:
            handleSeatLookup(); // TP083605
            break;
        case 4:
            handleManifestMenu(); // TP083605
            break;
        case 5:
            handleSeatReport(); // TP083605
            break;
        case 6:
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
*/