#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include "Shared_Resources/passengerRepo.cpp"

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}

int main() {
    PassengerLinkedList linkedList = readPassengerCSV();
    int choice;
    bool running = true;

    while (running) {
        clearScreen();
        cout << "========================================" << endl;
        cout << "   FLIGHT MANAGEMENT SYSTEM (LINKED LIST)" << endl;
        cout << "========================================" << endl;
        cout << "1. Reservation (Insertion) [N/A]" << endl;
        cout << "2. Cancellation (Deletion) [N/A]" << endl;
        cout << "3. Seat Lookup (Search)" << endl;
        cout << "4. Manifest & Seat Report" << endl;
        cout << "5. Exit" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                cout << "\n[INFO] Reservation is not in your current scope." << endl;
                pause();
                break;
            case 2:
                cout << "\n[INFO] Cancellation is not in your current scope." << endl;
                pause();
                break;
            case 3: {
                string id;
                cout << "\nEnter Passenger ID: ";
                cin >> id;
                
                auto start = chrono::high_resolution_clock::now();
                PassengerNode* p = linkedList.searchPassenger(id);
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double, milli> elapsed = end - start;

                if (p) {
                    cout << "\nPassenger Found:" << endl;
                    cout << "ID       : " << p->passengerId << endl;
                    cout << "Name     : " << p->passengerName << endl;
                    cout << "Seat     : " << (p->seatRow + 1) << PassengerconvertColumnIndexToChar(p->seatColumn) << endl;
                    cout << "Class    : " << p->passengerClass << endl;
                    cout << "Plane    : " << p->planeNum << endl;
                } else {
                    cout << "\n[ERROR] Passenger with ID " << id << " not found." << endl;
                }
                cout << "\nSearch Time (Linked List): " << fixed << setprecision(4) << elapsed.count() << " ms" << endl;
                pause();
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
                    cout << "[ERROR] Invalid plane number. Please enter a value between 1 and " << totalPlanes << "." << endl;
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                
                auto start = chrono::high_resolution_clock::now();
                linkedList.displayPlaneManifest(planeNum);
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double, milli> elapsed = end - start;
                
                cout << "\nManifest Display Time (Linked List): " << fixed << setprecision(4) << elapsed.count() << " ms" << endl;
                pause();
                break;
            }
            case 5:
                running = false;
                break;
            default:
                cout << "\n[ERROR] Invalid choice. Try again." << endl;
                pause();
                break;
        }
    }

    return 0;
}
