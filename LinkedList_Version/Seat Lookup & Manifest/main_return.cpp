#include <string>
#include <iostream>
#include "..\..\Shared_Resources\passengerNode.cpp"
#include "..\..\Shared_Resources\passengerRepo.cpp"

using namespace std;

int main() {

    PassengerLinkedList passengerLinkedList;
    passengerLinkedList = readPassengerCSV();

    cout << "Size of the linked list: " << passengerLinkedList.getSize() << endl;;
    return 0;
}
