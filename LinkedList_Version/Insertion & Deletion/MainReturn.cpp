/* ===========================================================
                      INCLUDES AND DEPENDENCIES
   =========================================================== */

#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include "../../Shared_Resources/PassengerRepository.cpp"

using namespace std;

/* ===========================================================
                         DATA STRUCTURES
   =========================================================== */

struct ReservationInsertionResult {
    bool isSuccessful;
    string errorMessage;
    int planeNumber;
    int seatRowIndex;
    int seatColumnIndex;
};

/* ===========================================================
                         HELPER FUNCTIONS
   =========================================================== */

bool normalizePassengerClass(const string& passengerClassInput, string& normalizedClass) {
    string uppercaseClass = passengerClassInput;
    transform(uppercaseClass.begin(), uppercaseClass.end(), uppercaseClass.begin(), ::toupper);

    if (uppercaseClass == "FIRST") {
        normalizedClass = "First";
        return true;
    }
    if (uppercaseClass == "BUSINESS") {
        normalizedClass = "Business";
        return true;
    }
    if (uppercaseClass == "ECONOMY") {
        normalizedClass = "Economy";
        return true;
    }

    normalizedClass.clear();
    return false;
}

bool hasNonWhitespaceContent(const string& value) {
    for (char c : value) {
        if (!isspace(static_cast<unsigned char>(c))) {
            return true;
        }
    }
    return false;
}

void getSeatClassRowRange(const string& passengerClass, int& startRowIndex, int& endRowIndex) {
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

bool isSeatIndexValid(int seatRowIndex, int seatColumnIndex) {
    if (seatRowIndex < 0 || seatRowIndex >= totalRows) {
        return false;
    }
    if (seatColumnIndex < 0 || seatColumnIndex >= totalColumns) {
        return false;
    }
    return true;
}

string generateNextPassengerId(PassengerLinkedList& linkedList) {
    return linkedList.getNextPassengerId();
}

/* ===========================================================
                      RESERVATION INSERTION
   =========================================================== */

ReservationInsertionResult insertPassengerReservation(
    PassengerLinkedList& linkedList,
    const string& passengerId,
    const string& passengerName,
    const string& passengerClassInput,
    bool hasPreferredSeat,
    int preferredSeatRowIndex,
    int preferredSeatColumnIndex
) {
    ReservationInsertionResult result{};
    result.isSuccessful = false;
    result.errorMessage = "";
    result.planeNumber = -1;
    result.seatRowIndex = -1;
    result.seatColumnIndex = -1;

    // This ensures the passenger ID is provided and unique.
    if (!hasNonWhitespaceContent(passengerId)) {
        result.errorMessage = "Passenger ID cannot be empty.";
        return result;
    }
    if (linkedList.doesPassengerExists(passengerId)) {
        result.errorMessage = "Passenger ID already exists.";
        return result;
    }

    // This ensures the passenger name is provided.
    if (!hasNonWhitespaceContent(passengerName)) {
        result.errorMessage = "Passenger name cannot be empty.";
        return result;
    }

    // This validates and normalizes the class.
    string normalizedClass;
    if (!normalizePassengerClass(passengerClassInput, normalizedClass)) {
        result.errorMessage = "Passenger class must be First, Business, or Economy.";
        return result;
    }

    // This determines the allowed row range for the class.
    int startRowIndex = 0;
    int endRowIndex = 29;
    getSeatClassRowRange(normalizedClass, startRowIndex, endRowIndex);

    // This makes sure there is at least one plane to consider.
    int totalPlanes = linkedList.getTotalPlanes();
    if (totalPlanes < 1) {
        totalPlanes = 1;
    }

    // This handles the case where a specific seat is requested.
    if (hasPreferredSeat) {
        if (!isSeatIndexValid(preferredSeatRowIndex, preferredSeatColumnIndex)) {
            result.errorMessage = "Seat row or column is out of range.";
            return result;
        }
        if (preferredSeatRowIndex < startRowIndex || preferredSeatRowIndex > endRowIndex) {
            result.errorMessage = "Selected seat does not match the requested class.";
            return result;
        }

        for (int planeNumber = 1; planeNumber <= totalPlanes; planeNumber++) {
            if (!linkedList.isSeatOccupied(preferredSeatRowIndex, preferredSeatColumnIndex, planeNumber)) {
                linkedList.init(passengerId, passengerName, preferredSeatRowIndex, preferredSeatColumnIndex, planeNumber, normalizedClass);
                result.isSuccessful = true;
                result.planeNumber = planeNumber;
                result.seatRowIndex = preferredSeatRowIndex;
                result.seatColumnIndex = preferredSeatColumnIndex;
                return result;
            }
        }

        int newPlaneNumber = totalPlanes + 1;
        linkedList.init(passengerId, passengerName, preferredSeatRowIndex, preferredSeatColumnIndex, newPlaneNumber, normalizedClass);
        linkedList.setTotalPlanes(newPlaneNumber);
        result.isSuccessful = true;
        result.planeNumber = newPlaneNumber;
        result.seatRowIndex = preferredSeatRowIndex;
        result.seatColumnIndex = preferredSeatColumnIndex;
        return result;
    }

    // This finds the first open seat in the correct class across planes.
    for (int planeNumber = 1; planeNumber <= totalPlanes; planeNumber++) {
        for (int rowIndex = startRowIndex; rowIndex <= endRowIndex; rowIndex++) {
            for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++) {
                if (!linkedList.isSeatOccupied(rowIndex, columnIndex, planeNumber)) {
                    linkedList.init(passengerId, passengerName, rowIndex, columnIndex, planeNumber, normalizedClass);
                    result.isSuccessful = true;
                    result.planeNumber = planeNumber;
                    result.seatRowIndex = rowIndex;
                    result.seatColumnIndex = columnIndex;
                    return result;
                }
            }
        }
    }

    // This creates a new plane when all existing planes are full for the class.
    int newPlaneNumber = totalPlanes + 1;
    int defaultRowIndex = startRowIndex;
    int defaultColumnIndex = 0;
    linkedList.init(passengerId, passengerName, defaultRowIndex, defaultColumnIndex, newPlaneNumber, normalizedClass);
    linkedList.setTotalPlanes(newPlaneNumber);
    result.isSuccessful = true;
    result.planeNumber = newPlaneNumber;
    result.seatRowIndex = defaultRowIndex;
    result.seatColumnIndex = defaultColumnIndex;
    return result;
}
