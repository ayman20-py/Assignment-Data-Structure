#include <iostream>
using namespace std;

// Forward declarations for external functions
// These functions will be defined in the respective .cpp files
void runArrayVersion();
// void runLinkedListVersion();

void displayMainMenu()
{
    cout << "\n========================================\n";
    cout << "  FLIGHT RESERVATION SYSTEM\n";
    cout << "========================================\n";
    cout << "  Data Structure Assignment\n";
    cout << "========================================\n\n";
    cout << "1. Array Version\n";
    cout << "2. Linked List Version\n";
    cout << "3. Exit\n\n";
    cout << "Enter your choice (1-3): ";
}

void clearInputBuffer()
{
    cin.clear();
    cin.ignore(10000, '\n');
}

int main()
{
    cout << "This is the assignment repo for data structure" << endl;

    int choice;
    bool running = true;

    while (running)
    {
        displayMainMenu();

        if (!(cin >> choice))
        {
            clearInputBuffer();
            cout << "\n[ERROR] Invalid input. Please enter a number.\n";
            cout << "\nPress Enter to continue...";
            cin.get();
            continue;
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            cout << "\n========================================\n";
            cout << "  Loading Array Version...\n";
            cout << "========================================\n\n";
            runArrayVersion();
            break;

        case 2:
            cout << "\n========================================\n";
            cout << "  Loading Linked List Version...\n";
            cout << "========================================\n\n";
            // runLinkedListVersion();
            break;

        case 3:
            cout << "\n========================================\n";
            cout << "  Thank you for using the system!\n";
            cout << "  Goodbye!\n";
            cout << "========================================\n\n";
            running = false;
            break;

        default:
            cout << "\n[ERROR] Invalid choice. Please select 1, 2, or 3.\n";
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }

    return 0;
}