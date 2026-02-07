#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include "Shared_Resources/passengerRepo.cpp"
#include "Array_Version/main_array.cpp"

using namespace std;

// Utility functions for main
void mainClearScreen() {
    clearScreen();
}

void mainPause() {
    clearInputBuffer();
    pauseForUserInput();
}

int main() {
    // Initializing both data structures from the CSV file
    PassengerLinkedList linkedList = readPassengerCSV();
    
    loadPassengerDataFromCSV();
    
    int choice;
    bool running = true;

    while (running) {
        mainClearScreen();
        cout << "========================================" << endl;
        cout << "   FLIGHT MANAGEMENT SYSTEM (DUAL MODE)" << endl;
        cout << "========================================" << endl;
        cout << "1. Reservation (Array Only)" << endl;
        cout << "2. Cancellation (Array Only)" << endl;
        cout << "3. Seat Lookup (Comparative)" << endl;
        cout << "4. Manifest & Seat Report (Comparative)" << endl;
        cout << "5. Exit" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n'); // Clear buffer for string inputs in cases

        switch (choice) {
            case 1:
                handleReservation();
                break;
            case 2:
                handleCancellation();
                break;
            case 3: {
                string id;
                cout << "\nEnter Passenger ID: ";
                cin >> id;
                
                cout << "\n--- SEARCH RESULTS (LINKED LIST) ---" << endl;
                auto startLL = chrono::high_resolution_clock::now();
                PassengerNode* p = linkedList.searchPassenger(id);
                auto endLL = chrono::high_resolution_clock::now();
                
                if (p) {
                    cout << "\n========================================\n";
                    cout << "          PASSENGER FOUND\n";
                    cout << "========================================\n";
                    cout << "Passenger ID   : " << p->passengerId << "\n";
                    cout << "Name           : " << p->passengerName << "\n";
                    cout << "Plane          : #" << p->planeNum << "\n";
                    cout << "Seat           : " << (p->seatRow + 1) << PassengerconvertColumnIndexToChar(p->seatColumn) << "\n";
                    cout << "Class          : " << p->passengerClass << "\n";
                    cout << "========================================\n";
                } else {
                    cout << "[ERROR] Not found in Linked List." << endl;
                }
                chrono::duration<double, milli> elapsedLL = endLL - startLL;

                cout << "\n--- SEARCH RESULTS (ARRAY VERSION) ---" << endl;
                int planeIdx, passIdx;
                auto startArr = chrono::high_resolution_clock::now();
                bool foundArr = findPassengerByID(id, planeIdx, passIdx);
                auto endArr = chrono::high_resolution_clock::now();
                
                if (foundArr) {
                    displayPassengerDetails(planeIdx, passIdx);
                } else {
                    cout << "[ERROR] Not found in Array." << endl;
                }
                chrono::duration<double, milli> elapsedArr = endArr - startArr;

                cout << "\n----------------------------------------" << endl;
                cout << "Search Time (Linked List) : " << fixed << setprecision(4) << elapsedLL.count() << " ms" << endl;
                cout << "Search Time (Array Version): " << fixed << setprecision(4) << elapsedArr.count() << " ms" << endl;
                mainPause();
                break;
            }
            case 4: {
                int planeNum;
                int totalPlanes = linkedList.getTotalPlanes();
                
                while (true) {
                    cout << "\nEnter Plane Number (1-" << totalPlanes << "): ";
                    if (cin >> planeNum && planeNum >= 1 && planeNum <= totalPlanes) {
                        break;
                    }
                    cout << "[ERROR] Invalid plane number." << endl;
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                
                cout << "\n--- MANIFEST (LINKED LIST) ---" << endl;
                auto startLL = chrono::high_resolution_clock::now();
                linkedList.displayPlaneManifest(planeNum);
                auto endLL = chrono::high_resolution_clock::now();
                chrono::duration<double, milli> elapsedLL = endLL - startLL;
                
                cout << "\n--- MANIFEST (ARRAY VERSION) ---" << endl;
                auto startArr = chrono::high_resolution_clock::now();
                displaySeatingChartForPlane(planeNum - 1);
                auto endArr = chrono::high_resolution_clock::now();
                chrono::duration<double, milli> elapsedArr = endArr - startArr;
                
                cout << "\n----------------------------------------" << endl;
                cout << "Manifest Time (Linked List) : " << fixed << setprecision(4) << elapsedLL.count() << " ms" << endl;
                cout << "Manifest Time (Array Version): " << fixed << setprecision(4) << elapsedArr.count() << " ms" << endl;
                mainPause();
                break;
            }
            case 5:
                running = false;
                break;
            default:
                cout << "\n[ERROR] Invalid choice." << endl;
                mainPause();
                break;
        }
    }

    return 0;
}
