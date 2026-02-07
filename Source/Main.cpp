#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <sstream>

using namespace std;

#define main ArrayMainEntry
#include "Array/ArrayMain.cpp"
#undef main

#include "LinkedList/LinkedListMain.cpp"

struct UILines {
    string lines[100];
    int count = 0;
    void add(const string& line) {
        if (count < 100) lines[count++] = line;
    }
};

struct PassengerManifest {
    PassengerNode passengers[180]; // SEATS_PER_PLANE
    int count = 0;
};

struct SeatGrid {
    char grid[30][6]; // totalRows x totalColumns
};

struct LoadStats {
    double arrayLoadMs;
    double linkedListLoadMs;
    bool arrayLoaded;
    bool linkedListLoaded;
};

string formatBytes(size_t bytes) {
    const double kb = 1024.0;
    const double mb = kb * 1024.0;
    const double gb = mb * 1024.0;

    ostringstream out;
    out << fixed << setprecision(2);

    if (bytes >= static_cast<size_t>(gb)) {
        out << (bytes / gb) << " GB";
        return out.str();
    }
    if (bytes >= static_cast<size_t>(mb)) {
        out << (bytes / mb) << " MB";
        return out.str();
    }
    if (bytes >= static_cast<size_t>(kb)) {
        out << (bytes / kb) << " KB";
        return out.str();
    }
    out << bytes << " B";
    return out.str();
}

string formatMs(double ms) {
    ostringstream out;
    out << fixed << setprecision(4) << ms << " ms";
    return out.str();
}

size_t estimateLinkedListMemory(PassengerLinkedList& list) {
    return static_cast<size_t>(list.getSize()) * sizeof(PassengerNode);
}

size_t estimateArrayReservedMemory() {
    return sizeof(planes);
}

size_t estimateArrayActiveMemory() {
    return static_cast<size_t>(activePlaneCount) * sizeof(Plane);
}

void printLine(char ch, int width) {
    for (int i = 0; i < width; i++) {
        cout << ch;
    }
    cout << "\n";
}

void printBox(const string& title, const UILines& lines, int width) {
    cout << "+" << string(width - 2, '-') << "+\n";
    cout << "|" << setw(width - 2) << left << title << "|\n";
    cout << "+" << string(width - 2, '-') << "+\n";

    for (int i = 0; i < lines.count; i++) {
        cout << "|" << setw(width - 2) << left << lines.lines[i] << "|\n";
    }

    cout << "+" << string(width - 2, '-') << "+\n";
}

void resetArrayData() {
    activePlaneCount = 0;
    for (int i = 0; i < MAX_PLANES; i++) {
        planes[i] = Plane();
    }
}

bool loadArrayDataSilently(double& loadMs) {
    ostringstream sink;
    streambuf* original = cout.rdbuf();
    cout.rdbuf(sink.rdbuf());

    auto start = chrono::high_resolution_clock::now();
    bool result = loadPassengerDataFromCSV();
    auto end = chrono::high_resolution_clock::now();

    cout.rdbuf(original);
    loadMs = chrono::duration<double, milli>(end - start).count();
    return result;
}

bool loadLinkedListData(PassengerLinkedList& list, double& loadMs) {
    auto start = chrono::high_resolution_clock::now();
    list = readPassengerCSV();
    auto end = chrono::high_resolution_clock::now();

    loadMs = chrono::duration<double, milli>(end - start).count();
    return list.getSize() > 0;
}

void printPerformanceDashboard(const LoadStats& stats, PassengerLinkedList& list) {
    const int boxWidth = 68;

    cout << "\n";
    printLine('=', boxWidth);
    cout << "FLIGHT MANAGEMENT PERFORMANCE DASHBOARD\n";
    printLine('=', boxWidth);

    UILines linkedListLines;
    linkedListLines.add("Version        : Linked List");
    linkedListLines.add("Load Time      : " + formatMs(stats.linkedListLoadMs));
    linkedListLines.add("Passengers     : " + to_string(list.getSize()));
    linkedListLines.add("Planes         : " + to_string(list.getTotalPlanes()));
    linkedListLines.add("Est. Memory    : " + formatBytes(estimateLinkedListMemory(list)) + " (nodes only)");

    UILines arrayLines;
    arrayLines.add("Version        : Array (1D + 2D)");
    arrayLines.add("Load Time      : " + formatMs(stats.arrayLoadMs));
    arrayLines.add("Passengers     : " + to_string(getTotalPassengers()));
    arrayLines.add("Active Planes  : " + to_string(activePlaneCount));
    arrayLines.add("Est. Reserved  : " + formatBytes(estimateArrayReservedMemory()));
    arrayLines.add("Est. Active    : " + formatBytes(estimateArrayActiveMemory()));

    printBox("Linked List Performance", linkedListLines, boxWidth);
    cout << "\n";
    printBox("Array Performance", arrayLines, boxWidth);
    cout << "\n";
}

bool readYesNo(const string& prompt) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        if (input.empty()) {
            continue;
        }
        char value = static_cast<char>(toupper(input[0]));
        if (value == 'Y') {
            return true;
        }
        if (value == 'N') {
            return false;
        }
    }
}

bool readSeatPreference(int& rowIndex, int& columnIndex) {
    while (true) {
        cout << "Enter Seat Row (1-30): ";
        int rowInput;
        if (!(cin >> rowInput)) {
            clearInputBuffer();
            cout << "[ERROR] Invalid row input.\n";
            continue;
        }
        clearInputBuffer();

        cout << "Enter Seat Column (A-F): ";
        string columnInput;
        getline(cin, columnInput);
        if (columnInput.empty()) {
            cout << "[ERROR] Column cannot be empty.\n";
            continue;
        }
        char columnChar = static_cast<char>(toupper(columnInput[0]));

        int row = rowInput - 1;
        int col = convertColumnCharToIndex(columnChar);
        if (row < 0 || row >= totalRows || col < 0 || col >= totalColumns) {
            cout << "[ERROR] Seat is out of range.\n";
            continue;
        }

        rowIndex = row;
        columnIndex = col;
        return true;
    }
}

class OutputSilencer {
public:
    OutputSilencer() : original(cout.rdbuf()), sink() {
        cout.rdbuf(sink.rdbuf());
    }

    ~OutputSilencer() {
        cout.rdbuf(original);
    }

private:
    streambuf* original;
    ostringstream sink;
};

struct ReservationResultView {
    bool success;
    string message;
    string passengerId;
    string passengerName;
    string passengerClass;
    int planeNumber;
    int seatRowIndex;
    int seatColumnIndex;
    double elapsedMs;
};

struct LookupResultView {
    bool found;
    string passengerId;
    string passengerName;
    string passengerClass;
    int planeNumber;
    int seatRowIndex;
    int seatColumnIndex;
    double elapsedMs;
};

void getClassRowRange(const string& passengerClass, int& startRowIndex, int& endRowIndex) {
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

bool isSeatInClassRange(const string& passengerClass, int seatRowIndex) {
    int startRow = 0;
    int endRow = 29;
    getClassRowRange(passengerClass, startRow, endRow);
    return seatRowIndex >= startRow && seatRowIndex <= endRow;
}

void renderSeatGrid(const SeatGrid& grid, const string& title) {
    cout << "\n" << title << "\n";
    cout << "   A   B   C   D   E   F\n";
    cout << "---------- First Class (Rows 1-3) ----------\n";
    for (int row = 0; row < 3; row++) {
        cout << (row + 1) << "   ";
        for (int col = 0; col < totalColumns; col++) {
            cout << grid.grid[row][col] << "   ";
        }
        cout << "\n";
    }
    cout << "---------- Business Class (Rows 4-10) ----------\n";
    for (int row = 3; row < 10; row++) {
        if ((row + 1) == 10) {
            cout << (row + 1) << "  ";
        } else {
            cout << (row + 1) << "   ";
        }
        for (int col = 0; col < totalColumns; col++) {
            cout << grid.grid[row][col] << "   ";
        }
        cout << "\n";
    }
    cout << "---------- Economy Class (Rows 11-30) ----------\n";
    for (int row = 10; row < 30; row++) {
        cout << (row + 1) << "  ";
        for (int col = 0; col < totalColumns; col++) {
            cout << grid.grid[row][col] << "   ";
        }
        cout << "\n";
    }
    cout << "-------Legend--------\n";
    cout << "Occupied Seat: X\n";
    cout << "Available Seat: O\n";
}

void renderManifest(const PassengerManifest& manifest, const string& title) {
    cout << "\n" << title << "\n";
    cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(8) << "Seat" << setw(12)
         << "Class" << "\n";
    cout << string(55, '-') << "\n";

    for (int i = 0; i < manifest.count; i++) {
        const PassengerNode& passenger = manifest.passengers[i];
        string seat = to_string(passenger.seatRow + 1) + PassengerconvertColumnIndexToChar(passenger.seatColumn);
        cout << left << setw(10) << passenger.passengerId
             << setw(25) << passenger.passengerName
             << setw(8) << seat
             << setw(12) << passenger.passengerClass << "\n";
    }
}

PassengerManifest collectArrayManifest(int planeIndex) {
    PassengerManifest manifest;
    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive) {
        return manifest;
    }

    for (int i = 0; i < planes[planeIndex].activePassengerCount; i++) {
        Passenger& arrayPassenger = planes[planeIndex].passengers[i];
        if (!arrayPassenger.isActive) {
            continue;
        }

        PassengerNode& node = manifest.passengers[manifest.count++];
        node.passengerId = arrayPassenger.passengerId;
        node.passengerName = arrayPassenger.passengerName;
        node.seatRow = arrayPassenger.seatRow;
        node.seatColumn = arrayPassenger.seatColumn;
        node.passengerClass = arrayPassenger.passengerClass;
        node.planeNum = arrayPassenger.planeNumber + 1;
    }

    return manifest;
}

SeatGrid collectArrayGrid(int planeIndex) {
    SeatGrid grid;
    for (int r = 0; r < 30; r++) for (int c = 0; c < 6; c++) grid.grid[r][c] = 'O';

    if (planeIndex < 0 || planeIndex >= activePlaneCount || !planes[planeIndex].isActive) {
        return grid;
    }

    for (int row = 0; row < totalRows; row++) {
        for (int col = 0; col < totalColumns; col++) {
            grid.grid[row][col] = planes[planeIndex].seatingGrid[row][col];
        }
    }
    return grid;
}

PassengerManifest collectLinkedListManifest(PassengerLinkedList& list, int planeNumber) {
    PassengerNode passengerList[30][6];
    list.getPassengersFromPlane(passengerList, planeNumber);

    PassengerManifest manifest;
    for (int row = 0; row < totalRows; row++) {
        for (int col = 0; col < totalColumns; col++) {
            if (passengerList[row][col].passengerName != "") {
                manifest.passengers[manifest.count++] = passengerList[row][col];
            }
        }
    }
    return manifest;
}

SeatGrid collectLinkedListGrid(PassengerLinkedList& list, int planeNumber) {
    PassengerNode passengerList[30][6];
    list.getPassengersFromPlane(passengerList, planeNumber);

    SeatGrid grid;
    for (int row = 0; row < totalRows; row++) {
        for (int col = 0; col < totalColumns; col++) {
            grid.grid[row][col] = passengerList[row][col].passengerName.empty() ? 'O' : 'X';
        }
    }
    return grid;
}

ReservationResultView runLinkedListReservation(
    PassengerLinkedList& list,
    const string& passengerId,
    const string& passengerName,
    const string& passengerClass,
    bool hasPreferredSeat,
    int seatRow,
    int seatColumn,
    int targetPlaneNumber = -1
) {
    ReservationResultView view{};
    auto start = chrono::high_resolution_clock::now();
    ReservationInsertionResult result = insertPassengerReservation(
        list,
        passengerId,
        passengerName,
        passengerClass,
        hasPreferredSeat,
        seatRow,
        seatColumn,
        targetPlaneNumber
    );
    auto end = chrono::high_resolution_clock::now();

    view.success = result.isSuccessful;
    view.message = result.errorMessage;
    view.passengerId = passengerId;
    view.passengerName = passengerName;
    view.passengerClass = passengerClass;
    view.planeNumber = result.planeNumber;
    view.seatRowIndex = result.seatRowIndex;
    view.seatColumnIndex = result.seatColumnIndex;
    view.elapsedMs = chrono::duration<double, milli>(end - start).count();
    return view;
}

ReservationResultView runArrayReservation(
    const string& passengerId,
    const string& passengerName,
    const string& passengerClass,
    bool hasPreferredSeat,
    int seatRow,
    int seatColumn,
    int targetPlaneIndex = -1
) {
    ReservationResultView view{};
    view.passengerId = passengerId;
    view.passengerName = passengerName;
    view.passengerClass = passengerClass;
    view.seatRowIndex = -1;
    view.seatColumnIndex = -1;
    view.planeNumber = -1;

    if (!isSeatInClassRange(passengerClass, seatRow) && hasPreferredSeat) {
        view.success = false;
        view.message = "Selected seat does not match the requested class.";
        return view;
    }

    int startRow = 0;
    int endRow = 29;
    getClassRowRange(passengerClass, startRow, endRow);

    int selectedPlane = -1;
    int selectedRow = seatRow;
    int selectedColumn = seatColumn;

    if (targetPlaneIndex >= 0) {
        if (targetPlaneIndex < activePlaneCount && planes[targetPlaneIndex].isActive) {
            if (isSeatAvailable(targetPlaneIndex, seatRow, seatColumn)) {
                selectedPlane = targetPlaneIndex;
            } else {
                view.success = false;
                view.message = "Seat is already occupied on Plane #" + to_string(targetPlaneIndex + 1);
                return view;
            }
        } else {
            view.success = false;
            view.message = "Target plane is not active or out of range.";
            return view;
        }
    } else if (hasPreferredSeat) {
        for (int i = 0; i < activePlaneCount; i++) {
            if (!planes[i].isActive) {
                continue;
            }
            if (isSeatAvailable(i, seatRow, seatColumn)) {
                selectedPlane = i;
                break;
            }
        }

        if (selectedPlane == -1) {
            OutputSilencer silencer;
            selectedPlane = createNewPlane();
        }
    } else {
        for (int i = 0; i < activePlaneCount; i++) {
            if (!planes[i].isActive) {
                continue;
            }
            for (int row = startRow; row <= endRow; row++) {
                for (int col = 0; col < totalColumns; col++) {
                    if (isSeatAvailable(i, row, col)) {
                        selectedPlane = i;
                        selectedRow = row;
                        selectedColumn = col;
                        break;
                    }
                }
                if (selectedPlane != -1) {
                    break;
                }
            }
            if (selectedPlane != -1) {
                break;
            }
        }

        if (selectedPlane == -1) {
            OutputSilencer silencer;
            selectedPlane = createNewPlane();
            selectedRow = startRow;
            selectedColumn = 0;
        }
    }

    if (selectedPlane < 0) {
        view.success = false;
        view.message = "Unable to allocate a plane for the reservation.";
        return view;
    }

    auto start = chrono::high_resolution_clock::now();
    bool result;
    {
        OutputSilencer silencer;
        result = insertReservation(
            passengerId,
            passengerName,
            passengerClass,
            selectedPlane,
            selectedRow,
            selectedColumn
        );
    }
    auto end = chrono::high_resolution_clock::now();

    view.success = result;
    view.message = result ? "" : "Reservation could not be completed.";
    view.planeNumber = selectedPlane + 1;
    view.seatRowIndex = selectedRow;
    view.seatColumnIndex = selectedColumn;
    view.elapsedMs = chrono::duration<double, milli>(end - start).count();
    return view;
}

ReservationResultView runLinkedListCancellation(PassengerLinkedList& list, const string& passengerId) {
    ReservationResultView view{};
    auto start = chrono::high_resolution_clock::now();
    ReservationDeletionResult result = deletePassengerReservation(list, passengerId);
    auto end = chrono::high_resolution_clock::now();

    view.success = result.isSuccessful;
    view.message = result.errorMessage;
    view.passengerId = result.passengerId;
    view.passengerName = result.passengerName;
    view.passengerClass = result.passengerClass;
    view.planeNumber = result.planeNumber;
    view.seatRowIndex = result.seatRowIndex;
    view.seatColumnIndex = result.seatColumnIndex;
    view.elapsedMs = chrono::duration<double, milli>(end - start).count();
    return view;
}

ReservationResultView runArrayCancellation(const string& passengerId) {
    ReservationResultView view{};
    view.passengerId = passengerId;

    int planeIndex = -1;
    int passengerIndex = -1;
    if (!findPassengerByID(passengerId, planeIndex, passengerIndex)) {
        view.success = false;
        view.message = "Passenger ID not found.";
        return view;
    }

    Passenger passenger = planes[planeIndex].passengers[passengerIndex];

    auto start = chrono::high_resolution_clock::now();
    bool result;
    {
        OutputSilencer silencer;
        result = cancelReservation(passengerId);
    }
    auto end = chrono::high_resolution_clock::now();

    view.success = result;
    view.message = result ? "" : "Cancellation failed.";
    view.passengerName = passenger.passengerName;
    view.passengerClass = passenger.passengerClass;
    view.planeNumber = passenger.planeNumber + 1;
    view.seatRowIndex = passenger.seatRow;
    view.seatColumnIndex = passenger.seatColumn;
    view.elapsedMs = chrono::duration<double, milli>(end - start).count();
    return view;
}

LookupResultView runLinkedListLookup(PassengerLinkedList& list, const string& passengerId) {
    LookupResultView view{};
    auto start = chrono::high_resolution_clock::now();
    PassengerNode* passenger = list.searchPassenger(passengerId);
    auto end = chrono::high_resolution_clock::now();

    view.found = passenger != nullptr;
    view.passengerId = passengerId;
    view.elapsedMs = chrono::duration<double, milli>(end - start).count();

    if (passenger) {
        view.passengerName = passenger->passengerName;
        view.passengerClass = passenger->passengerClass;
        view.planeNumber = passenger->planeNum;
        view.seatRowIndex = passenger->seatRow;
        view.seatColumnIndex = passenger->seatColumn;
    }

    return view;
}

LookupResultView runArrayLookup(const string& passengerId) {
    LookupResultView view{};
    int planeIndex = -1;
    int passengerIndex = -1;

    auto start = chrono::high_resolution_clock::now();
    bool found = findPassengerByID(passengerId, planeIndex, passengerIndex);
    auto end = chrono::high_resolution_clock::now();

    view.found = found;
    view.passengerId = passengerId;
    view.elapsedMs = chrono::duration<double, milli>(end - start).count();

    if (found) {
        Passenger& passenger = planes[planeIndex].passengers[passengerIndex];
        view.passengerName = passenger.passengerName;
        view.passengerClass = passenger.passengerClass;
        view.planeNumber = passenger.planeNumber + 1;
        view.seatRowIndex = passenger.seatRow;
        view.seatColumnIndex = passenger.seatColumn;
    }

    return view;
}

void printOperationBox(const string& title, const UILines& lines) {
    const int boxWidth = 68;
    printBox(title, lines, boxWidth);
}

int parseNumericId(const string& value, int fallback) {
    if (value.empty()) {
        return fallback;
    }
    for (char c : value) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            return fallback;
        }
    }
    return stoi(value);
}

string generateJointPassengerId(PassengerLinkedList& list) {
    string linkedListNext = generateNextPassengerId(list);
    string arrayNext = generateUniquePassengerID();

    int linkedListValue = parseNumericId(linkedListNext, 0);
    int arrayValue = parseNumericId(arrayNext, 0);
    int nextId = max(linkedListValue, arrayValue);
    if (nextId <= 0) {
        nextId = 1;
    }
    return to_string(nextId);
}

void handleJointReservation(PassengerLinkedList& list) {
    while (true) {
        clearScreen();
        cout << "\n========================================\n";
    cout << "     RESERVATION (ARRAY + LINKED LIST)\n";
        cout << "========================================\n\n";

        size_t linkedListMemBefore = estimateLinkedListMemory(list);
        size_t arrayReservedBefore = estimateArrayReservedMemory();
        size_t arrayActiveBefore = estimateArrayActiveMemory();

        // 1. Get passenger name
        string passengerName;
        while (true) {
            cout << "Enter Passenger Name: ";
            getline(cin, passengerName);
            passengerName = trimWhitespace(passengerName);
            if (!passengerName.empty()) break;
            cout << "[ERROR] Name cannot be empty. Try again.\n";
        }

        // 2. Get passenger class
        string passengerClass;
        while (true) {
            cout << "Enter Class (First/Business/Economy): ";
            getline(cin, passengerClass);
            string normalized;
            if (normalizePassengerClass(passengerClass, normalized)) {
                passengerClass = normalized;
                break;
            }
            cout << "[ERROR] Invalid class. Must be First, Business, or Economy.\n";
        }

        // 3. Show available planes and select
        int selectedPlaneIndex = -1;
        while (true) {
            cout << "\n========================================\n";
            cout << "         AVAILABLE PLANES\n";
            cout << "========================================\n\n";

            bool hasAvailablePlane = false;
            for (int i = 0; i < activePlaneCount; i++) {
                if (planes[i].isActive) {
                    int availableInClass = countAvailableSeatsInClass(i, passengerClass);
                    if (availableInClass > 0) {
                        int totalAvailable = SEATS_PER_PLANE - planes[i].activePassengerCount;
                        cout << "Plane #" << (i + 1) << " - Total: " << totalAvailable << " seats available";
                        cout << " (" << passengerClass << ": " << availableInClass << " seats) [V]\n";
                        hasAvailablePlane = true;
                    }
                }
            }

            if (!hasAvailablePlane) {
                cout << "\n[INFO] No planes with available " << passengerClass << " class seats.\n";
                cout << "A new plane will be created.\n";
                selectedPlaneIndex = createNewPlane();
                if (selectedPlaneIndex == -1) {
                    cout << "[ERROR] Cannot create more planes.\n";
                    pauseForUserInput();
                    return;
                }
                break;
            }

            cout << "\nEnter Plane Number (1-" << activePlaneCount << ") or 0 to go back: ";
            int choice;
            if (!(cin >> choice)) {
                clearInputBuffer();
                cout << "[ERROR] Invalid input. Enter a number.\n";
                continue;
            }
            clearInputBuffer();

            if (choice == 0) return;
            selectedPlaneIndex = choice - 1;

            if (selectedPlaneIndex < 0 || selectedPlaneIndex >= activePlaneCount || !planes[selectedPlaneIndex].isActive) {
                cout << "[ERROR] Invalid plane number. Try again.\n";
                continue;
            }

            if (countAvailableSeatsInClass(selectedPlaneIndex, passengerClass) == 0) {
                cout << "[ERROR] " << passengerClass << " class is FULL on Plane #" << choice << ".\n";
                continue;
            }
            break;
        }

        // 4. Select seat
        int seatRow = -1;
        int seatColumn = -1;
        bool seatSelected = false;

        int minRow, maxRow;
        getClassRowRange(passengerClass, minRow, maxRow);

        while (!seatSelected) {
            clearScreen();
            cout << "\n========================================\n";
            cout << "    PLANE #" << (selectedPlaneIndex + 1) << " SEATING GRID\n";
            cout << "========================================\n\n";
            
            SeatGrid grid = collectArrayGrid(selectedPlaneIndex);
            renderSeatGrid(grid, "Array System View (Plane #" + to_string(selectedPlaneIndex+1) + ")");

            cout << "\nYour class: " << passengerClass << " (Rows " << (minRow + 1) << "-" << (maxRow + 1) << ")\n";

            while (true) {
                cout << "\nEnter Seat Row (" << (minRow + 1) << "-" << (maxRow + 1) << ") or 0 to choose different plane: ";
                int rowInput;
                if (!(cin >> rowInput)) {
                    clearInputBuffer();
                    cout << "[ERROR] Invalid input. Enter a number.\n";
                    continue;
                }
                clearInputBuffer();

                if (rowInput == 0) break; // Inner loop break to different plane choice
                if (rowInput < (minRow + 1) || rowInput > (maxRow + 1)) {
                    cout << "[ERROR] Row out of range for " << passengerClass << " class.\n";
                    continue;
                }
                seatRow = rowInput - 1;
                break;
            }

            if (seatRow == -1) break; // Back to plane selection

            while (true) {
                cout << "Enter Seat Column (A-F) or 0 to go back to row selection: ";
                char colChar;
                cin >> colChar;
                clearInputBuffer();

                if (colChar == '0') {
                    seatRow = -1;
                    break;
                }
                seatColumn = columnLetterToIndex(colChar);
                if (seatColumn == -1) {
                    cout << "[ERROR] Column must be between A and F.\n";
                    continue;
                }

                if (!isSeatAvailable(selectedPlaneIndex, seatRow, seatColumn)) {
                    cout << "[ERROR] Seat " << (seatRow + 1) << colChar << " is occupied.\n";
                    continue;
                }
                seatSelected = true;
                break;
            }
        }

        if (!seatSelected) continue; // Back to top of outer while loop (plane selection)

        // 5. Execution
        string passengerId = generateJointPassengerId(list);

        ReservationResultView linkedListResult = runLinkedListReservation(
            list, passengerId, passengerName, passengerClass, true, seatRow, seatColumn, selectedPlaneIndex + 1
        );

        ReservationResultView arrayResult = runArrayReservation(
            passengerId, passengerName, passengerClass, true, seatRow, seatColumn, selectedPlaneIndex
        );

        size_t linkedListMemAfter = estimateLinkedListMemory(list);
        size_t arrayReservedAfter = estimateArrayReservedMemory();
        size_t arrayActiveAfter = estimateArrayActiveMemory();

        if (linkedListResult.success) savePassengerCSV(list);
        if (arrayResult.success) savePassengerDataToCSV();

        // 6. Stats and Report
        UILines llLines;
        llLines.add("Status       : " + string(linkedListResult.success ? "SUCCESS" : "FAILED"));
        if (!linkedListResult.success) llLines.add("Message      : " + linkedListResult.message);
        llLines.add("Passenger ID : " + passengerId);
        llLines.add("Name         : " + passengerName);
        llLines.add("Plane        : " + to_string(linkedListResult.planeNumber));
        llLines.add("Seat         : " + to_string(linkedListResult.seatRowIndex + 1) + convertColumnIndexToChar(linkedListResult.seatColumnIndex));
        llLines.add("Est. Memory  : " + formatBytes(linkedListMemAfter) + " (" + formatBytes(linkedListMemBefore) + " before)");
        llLines.add("Time         : " + formatMs(linkedListResult.elapsedMs));

        UILines arrLines;
        arrLines.add("Status       : " + string(arrayResult.success ? "SUCCESS" : "FAILED"));
        if (!arrayResult.success) arrLines.add("Message      : " + arrayResult.message);
        arrLines.add("Passenger ID : " + passengerId);
        arrLines.add("Name         : " + passengerName);
        arrLines.add("Plane        : " + to_string(arrayResult.planeNumber));
        arrLines.add("Seat         : " + to_string(arrayResult.seatRowIndex + 1) + convertColumnIndexToChar(arrayResult.seatColumnIndex));
        arrLines.add("Est. Reserved: " + formatBytes(arrayReservedAfter) + " (" + formatBytes(arrayReservedBefore) + " before)");
        arrLines.add("Est. Active  : " + formatBytes(arrayActiveAfter) + " (" + formatBytes(arrayActiveBefore) + " before)");
        arrLines.add("Time         : " + formatMs(arrayResult.elapsedMs));

        cout << "\n";
        printOperationBox("Linked List Result", llLines);
        cout << "\n";
        printOperationBox("Array Result", arrLines);
        cout << "\n";
        
        pauseForUserInput();
        break;
    }
}

void handleJointCancellation(PassengerLinkedList& list) {
    clearScreen();
    cout << "\n========================================\n";
    cout << "     CANCELLATION (ARRAY + LINKED LIST)\n";
    cout << "========================================\n\n";

    size_t linkedListMemBefore = estimateLinkedListMemory(list);
    size_t arrayReservedBefore = estimateArrayReservedMemory();
    size_t arrayActiveBefore = estimateArrayActiveMemory();

    string passengerId;
    cout << "Enter Passenger ID to cancel: ";
    getline(cin, passengerId);

    ReservationResultView linkedListResult = runLinkedListCancellation(list, passengerId);
    ReservationResultView arrayResult = runArrayCancellation(passengerId);

    size_t linkedListMemAfter = estimateLinkedListMemory(list);
    size_t arrayReservedAfter = estimateArrayReservedMemory();
    size_t arrayActiveAfter = estimateArrayActiveMemory();

    if (linkedListResult.success) {
        savePassengerCSV(list);
    }
    if (arrayResult.success) {
        savePassengerDataToCSV();
    }

    UILines linkedListLines;
    linkedListLines.add("Status       : " + string(linkedListResult.success ? "SUCCESS" : "FAILED"));
    if (!linkedListResult.success) {
        linkedListLines.add("Message      : " + linkedListResult.message);
    }
    linkedListLines.add("Passenger ID : " + passengerId);
    if (linkedListResult.success) {
        linkedListLines.add("Name         : " + linkedListResult.passengerName);
        linkedListLines.add("Class        : " + linkedListResult.passengerClass);
        linkedListLines.add("Plane        : " + to_string(linkedListResult.planeNumber));
        linkedListLines.add("Seat         : " + to_string(linkedListResult.seatRowIndex + 1) +
                                  convertColumnIndexToChar(linkedListResult.seatColumnIndex));
    }
    linkedListLines.add("Est. Memory  : " + formatBytes(linkedListMemAfter) +
                              " (" + formatBytes(linkedListMemBefore) + " before)");
    linkedListLines.add("Time         : " + formatMs(linkedListResult.elapsedMs));

    UILines arrayLines;
    arrayLines.add("Status       : " + string(arrayResult.success ? "SUCCESS" : "FAILED"));
    if (!arrayResult.success) {
        arrayLines.add("Message      : " + arrayResult.message);
    }
    arrayLines.add("Passenger ID : " + passengerId);
    if (arrayResult.success) {
        arrayLines.add("Name         : " + arrayResult.passengerName);
        arrayLines.add("Class        : " + arrayResult.passengerClass);
        arrayLines.add("Plane        : " + to_string(arrayResult.planeNumber));
        arrayLines.add("Seat         : " + to_string(arrayResult.seatRowIndex + 1) +
                              convertColumnIndexToChar(arrayResult.seatColumnIndex));
    }
    arrayLines.add("Est. Reserved: " + formatBytes(arrayReservedAfter) +
                         " (" + formatBytes(arrayReservedBefore) + " before)");
    arrayLines.add("Est. Active  : " + formatBytes(arrayActiveAfter) +
                         " (" + formatBytes(arrayActiveBefore) + " before)");
    arrayLines.add("Time         : " + formatMs(arrayResult.elapsedMs));

    cout << "\n";
    printOperationBox("Linked List Result", linkedListLines);
    cout << "\n";
    printOperationBox("Array Result", arrayLines);
    cout << "\n";
    pauseForUserInput();
}

void handleJointLookup(PassengerLinkedList& list) {
    clearScreen();
    cout << "\n========================================\n";
    cout << "       SEAT LOOKUP (ARRAY + LINKED LIST)\n";
    cout << "========================================\n\n";

    string passengerId;
    cout << "Enter Passenger ID to search: ";
    getline(cin, passengerId);

    LookupResultView linkedListResult = runLinkedListLookup(list, passengerId);
    LookupResultView arrayResult = runArrayLookup(passengerId);

    UILines linkedListLines;
    linkedListLines.add("Status       : " + string(linkedListResult.found ? "FOUND" : "NOT FOUND"));
    linkedListLines.add("Passenger ID : " + passengerId);
    if (linkedListResult.found) {
        linkedListLines.add("Name         : " + linkedListResult.passengerName);
        linkedListLines.add("Class        : " + linkedListResult.passengerClass);
        linkedListLines.add("Plane        : " + to_string(linkedListResult.planeNumber));
        linkedListLines.add("Seat         : " + to_string(linkedListResult.seatRowIndex + 1) +
                                  PassengerconvertColumnIndexToChar(linkedListResult.seatColumnIndex));
    }
    linkedListLines.add("Time         : " + formatMs(linkedListResult.elapsedMs));

    UILines arrayLines;
    arrayLines.add("Status       : " + string(arrayResult.found ? "FOUND" : "NOT FOUND"));
    arrayLines.add("Passenger ID : " + passengerId);
    if (arrayResult.found) {
        arrayLines.add("Name         : " + arrayResult.passengerName);
        arrayLines.add("Class        : " + arrayResult.passengerClass);
        arrayLines.add("Plane        : " + to_string(arrayResult.planeNumber));
        arrayLines.add("Seat         : " + to_string(arrayResult.seatRowIndex + 1) +
                               convertColumnIndexToChar(arrayResult.seatColumnIndex));
    }
    arrayLines.add("Time         : " + formatMs(arrayResult.elapsedMs));

    cout << "\n";
    printOperationBox("Linked List Result", linkedListLines);
    cout << "\n";
    printOperationBox("Array Result", arrayLines);
    cout << "\n";
    pauseForUserInput();
}

void handleJointManifest(PassengerLinkedList& list) {
    clearScreen();
    cout << "\n========================================\n";
    cout << "   MANIFEST & SEAT REPORT (JOINT VIEW)\n";
    cout << "========================================\n\n";

    int linkedListPlanes = list.getTotalPlanes();
    int arrayPlanes = activePlaneCount;
    int maxPlane = max(linkedListPlanes, arrayPlanes);

    if (maxPlane < 1) {
        cout << "[INFO] No planes loaded.\n";
        pauseForUserInput();
        return;
    }

    int planeNumber;
    while (true) {
        cout << "Enter Plane Number (1-" << maxPlane << "): ";
        if (!(cin >> planeNumber)) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        if (planeNumber >= 1 && planeNumber <= maxPlane) {
            break;
        }
        cout << "[ERROR] Invalid plane number.\n";
    }

    int arrayPlaneIndex = planeNumber - 1;
    bool arrayValid = arrayPlaneIndex >= 0 && arrayPlaneIndex < activePlaneCount && planes[arrayPlaneIndex].isActive;
    bool linkedListValid = planeNumber >= 1 && planeNumber <= linkedListPlanes;

    if (!linkedListValid && !arrayValid) {
        cout << "\n[INFO] Plane " << planeNumber << " is not available in either version.\n";
        cout << "\n";
        pauseForUserInput();
        return;
    }

    double linkedListMs = -1.0;
    double arrayMs = -1.0;

    SeatGrid linkedListGrid;
    PassengerManifest linkedListManifest;
    SeatGrid arrayGrid;
    PassengerManifest arrayManifest;

    if (linkedListValid) {
        auto start = chrono::high_resolution_clock::now();
        linkedListGrid = collectLinkedListGrid(list, planeNumber);
        linkedListManifest = collectLinkedListManifest(list, planeNumber);
        auto end = chrono::high_resolution_clock::now();
        linkedListMs = chrono::duration<double, milli>(end - start).count();
    }

    if (arrayValid) {
        auto start = chrono::high_resolution_clock::now();
        arrayGrid = collectArrayGrid(arrayPlaneIndex);
        arrayManifest = collectArrayManifest(arrayPlaneIndex);
        auto end = chrono::high_resolution_clock::now();
        arrayMs = chrono::duration<double, milli>(end - start).count();
    }

    if (linkedListValid) {
        renderSeatGrid(linkedListGrid, "\nSEAT GRID (Plane " + to_string(planeNumber) + ")");
        renderManifest(linkedListManifest, "\nPASSENGER MANIFEST");
    } else {
        renderSeatGrid(arrayGrid, "\nSEAT GRID (Plane " + to_string(planeNumber) + ")");
        renderManifest(arrayManifest, "\nPASSENGER MANIFEST");
    }

    cout << "\nPerformance Metrics\n";
    cout << "Linked List Time: ";
    if (linkedListMs >= 0.0) {
        cout << formatMs(linkedListMs) << "\n";
    } else {
        cout << "N/A\n";
    }
    cout << "Array Time     : ";
    if (arrayMs >= 0.0) {
        cout << formatMs(arrayMs) << "\n";
    } else {
        cout << "N/A\n";
    }
    cout << "\n";
    pauseForUserInput();
}

LoadStats loadAllData(PassengerLinkedList& list) {
    LoadStats stats{};
    resetArrayData();
    stats.arrayLoaded = loadArrayDataSilently(stats.arrayLoadMs);
    stats.linkedListLoaded = loadLinkedListData(list, stats.linkedListLoadMs);
    return stats;
}

void printMainMenu() {
    cout << "========================================\n";
    cout << "     JOINT FLIGHT MANAGEMENT MENU\n";
    cout << "========================================\n";
    cout << "1. Reservation (Insertion)\n";
    cout << "2. Cancellation (Deletion)\n";
    cout << "3. Seat Lookup (Search)\n";
    cout << "4. Manifest & Seat Report\n";
    cout << "5. Refresh Performance Stats\n";
    cout << "6. Exit\n";
    cout << "----------------------------------------\n";
    cout << "Enter choice: ";
}

int main() {
    LoadStats stats = loadAllData(passengerLinkedList);
    bool running = true;

    while (running) {
        clearScreen();
        printPerformanceDashboard(stats, passengerLinkedList);
        printMainMenu();

        int choice;
        if (!(cin >> choice)) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                handleJointReservation(passengerLinkedList);
                break;
            case 2:
                handleJointCancellation(passengerLinkedList);
                break;
            case 3:
                handleJointLookup(passengerLinkedList);
                break;
            case 4:
                handleJointManifest(passengerLinkedList);
                break;
            case 5:
                stats = loadAllData(passengerLinkedList);
                break;
            case 6:
                running = false;
                break;
            default:
                break;
        }
    }

    return 0;
}
