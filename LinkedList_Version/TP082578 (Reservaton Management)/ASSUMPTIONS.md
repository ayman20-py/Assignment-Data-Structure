# Project Assumptions - Reservation & Cancellation System (Linked List)

**Student ID:** TP082578
**Assignment:** Data Structures Assignment Task 1
**Component:** Reservation & Cancellation (Linked List Version)
**Date:** January 2026

---

## Executive Summary

This document outlines all assumptions made during the implementation of the Reservation and Cancellation system using a linked list data structure. These assumptions were necessary to address ambiguities in the requirements and ensure a functional, user-friendly system.

---

## 1. Data Management Assumptions

### 1.1 CSV File Handling

**Assumption:** The CSV file (`flight_passenger_data.csv`) contains **historical booking data** with 10,000+ records, but the airplane has only **180 physical seats** (30 rows × 6 columns).

**Rationale:**
- The CSV contains 10,000 records but there are only 180 seats available
- Multiple bookings exist for the same seat throughout the file
- This represents a booking history over time, not the current flight state

**Implementation Decision:**
- Load all 10,000 records into a temporary buffer
- For each seat position, keep only the **most recent booking** (last occurrence in CSV)
- This results in at most 180 active passengers in the linked list
- The final state represents the "current flight manifest"

**Alternative Considered:** Loading all 10,000 records into the linked list was rejected due to:
- Memory inefficiency
- Display impracticality (cannot show 10,000 passengers meaningfully)
- Logical inconsistency (plane only has 180 seats)

---

### 1.2 Passenger ID Generation

**Assumption:** Users should **not** manually enter passenger IDs; the system should auto-generate unique IDs.

**Rationale:**
- Manual ID entry allows duplicate IDs, breaking the cancellation system
- Users cannot reasonably know which IDs are already in use
- Professional booking systems auto-generate identifiers

**Implementation Decision:**
- Scan all existing passenger IDs in the linked list
- Find the highest numeric ID (e.g., 110234)
- Auto-generate next sequential ID (e.g., 110235)
- Display the generated ID to the user after successful booking

**Benefits:**
- Guaranteed uniqueness
- No user burden to track IDs
- Seamless continuation from CSV data (starts from highest existing ID + 1)

---

### 1.3 Save Behavior

**Assumption:** When saving, the system should **overwrite the entire CSV** with the current flight state, not append.

**Rationale:**
- Need to reflect cancellations in the saved data
- Appending would not remove cancelled reservations
- The CSV should always represent the current state after the session

**Implementation Decision:**
- On "Save and Exit", overwrite `flight_passenger_data.csv`
- Write header row
- Write all current passengers in the linked list
- Result: CSV contains exact current state (e.g., 175 passengers if 5 were cancelled)

**Alternative Considered:** Appending only new reservations was rejected because:
- Cancellations would not be saved
- CSV would grow unbounded with duplicate seat assignments
- No way to represent the "current state" accurately

---

## 2. Seating Structure Assumptions

### 2.1 Seat Layout

**Assumption:** The airplane has a **three-class configuration** with fixed sections:

| Class    | Rows  | Seats | Total Capacity |
|----------|-------|-------|----------------|
| First    | 1-3   | 6     | 18 seats       |
| Business | 4-10  | 6     | 42 seats       |
| Economy  | 11-30 | 6     | 120 seats      |
| **TOTAL**|**30** |**6**  |**180 seats**   |

**Rationale:**
- Analyzed CSV data to determine seat class distribution
- All "First" class passengers occupy rows 1-3
- All "Business" class passengers occupy rows 4-10
- All "Economy" class passengers occupy rows 11-30
- Standard commercial airline configuration

**Implementation Decision:**
- Display seating grid with three clearly labeled sections
- Validate passenger class selection (First/Business/Economy only)
- No restriction on which class a user can book (user can book any available seat in any class)

---

### 2.2 Seat Notation

**Assumption:** Seats should be identified using **airline-style notation** (e.g., "12A", "5F") rather than array indices.

**Rationale:**
- Industry standard for seat identification
- User-friendly and intuitive
- Matches real-world flight booking systems

**Implementation Decision:**
- Rows: displayed as 1-30 (user-facing)
- Columns: displayed as A-F (user-facing)
- Internal storage: zero-based indices (0-29 rows, 0-5 columns)
- Conversion functions handle translation between user notation and internal representation

---

## 3. Functional Assumptions

### 3.1 Cancellation Policy

**Assumption:** Users can cancel **any existing reservation**, not just those created in the current session.

**Rationale:**
- Real-world booking systems allow cancellation of any confirmed booking
- Restricting cancellation to session-only bookings is impractical
- Users may need to cancel pre-existing reservations loaded from CSV

**Implementation Decision:**
- Allow cancellation of any passenger by ID
- Free the seat immediately upon cancellation
- Reflect cancellations in the saved CSV file

**Initial Implementation:** Originally restricted cancellation to "new" reservations only, but this was changed based on usability requirements.

---

### 3.2 Fully Booked Flight Handling

**Assumption:** If all 180 seats are occupied, the system should **prevent reservation attempts** immediately.

**Rationale:**
- Avoids wasting user time entering details for an impossible booking
- Provides clear feedback about flight status
- Suggests alternative action (cancel an existing reservation)

**Implementation Decision:**
- Check seat availability at the start of `handleReservation()`
- If no available seats exist, display error message and return to menu
- Do not prompt for passenger details if flight is full

---

## 4. Linked List Design Assumptions

### 4.1 Node Structure

**Assumption:** The `PassengerNode` structure should contain all passenger information plus list navigation.

**Structure:**
```cpp
struct PassengerNode {
    string passengerId;           // Unique identifier
    string passengerName;         // Full name
    int seatRow;                  // Row index (0-based)
    int seatColumn;               // Column index (0-based)
    string passengerClass;        // First/Business/Economy
    bool isNewReservation;        // Session tracking flag
    PassengerNode* nextNode;      // Pointer to next node
};
```

**Rationale:**
- Stores all essential passenger information
- Includes seat assignment for grid synchronization
- `isNewReservation` flag tracks which bookings were created in current session (for future extensibility)
- Single `nextNode` pointer for singly-linked list implementation

---

### 4.2 Insertion Method

**Assumption:** New reservations should be inserted **at the end** of the linked list (append operation).

**Rationale:**
- Maintains chronological order of bookings
- Simpler logic than sorted insertion
- Order doesn't affect functionality (searches traverse entire list regardless)

**Implementation:**
- Handles three cases: empty list, single node, multiple nodes
- Traverse to tail and append new node
- O(n) time complexity for insertion

---

### 4.3 Deletion Method

**Assumption:** Deletion should handle **all three cases**: head, middle, and tail node deletion.

**Rationale:**
- Comprehensive implementation demonstrates understanding of linked list operations
- Meets assignment requirement to "handle head, middle, and tail deletion"
- Ensures robust cancellation functionality

**Implementation:**
- Case 1: Head node deletion (update head pointer)
- Case 2: Middle node deletion (bypass node with pointer adjustment)
- Case 3: Tail node deletion (handle when nextNode is nullptr)
- Free memory with `delete` after unlinking node
- Deallocate seat in grid after deletion

---

## 5. User Interface Assumptions

### 5.1 Input Validation

**Assumption:** All user inputs require **comprehensive validation** to prevent crashes and logical errors.

**Validation Rules Implemented:**

| Input Type       | Validation                                      | Error Handling              |
|------------------|------------------------------------------------|------------------------------|
| Passenger Name   | Non-empty, not whitespace-only                 | Reject and show error        |
| Passenger Class  | Must be "First", "Business", or "Economy"      | Case-insensitive acceptance  |
| Seat Row         | Integer between 1-30                           | Type checking, range checking|
| Seat Column      | Character between A-F                          | Case-insensitive acceptance  |
| Menu Choice      | Integer between 1-4                            | Type checking, range checking|
| Passenger ID     | Non-empty string                               | Check existence in list      |

**Rationale:**
- Prevents program crashes from invalid input
- Provides clear error messages to guide users
- Ensures data integrity in linked list and CSV
- Professional user experience

---

### 5.2 Display Format

**Assumption:** Displaying 180+ passengers requires a **compact table format**, not detailed multi-line entries.

**Rationale:**
- Multi-line format would require 1,080+ lines to display all passengers
- Impossible to view without excessive scrolling
- Table format shows all key information in one line per passenger

**Implementation:**
- Table columns: ID, Name, Seat, Class
- One line per passenger
- Truncate long names (>22 characters) with ellipsis
- Summary statistics at top (total active reservations)

---

### 5.3 Screen Management

**Assumption:** The terminal should be **cleared between operations** for a clean user experience.

**Rationale:**
- Prevents cluttered terminal output
- Focuses user attention on current operation
- Professional appearance
- Easier to read and navigate

**Implementation:**
- `clearScreen()` function calls `system("cls")` (Windows)
- Clear before showing main menu
- Clear before each operation (reserve, cancel, display)
- `pauseForUserInput()` allows user to read output before clearing

---

## 6. Data Structure Synchronization Assumptions

### 6.1 Dual Data Structure Approach

**Assumption:** The system requires **both a linked list and a 2D seating grid** for efficient operations.

**Rationale:**
- Linked list: stores passenger details, enables insertion/deletion operations
- Seating grid: provides O(1) seat availability checking
- Each serves a distinct purpose; neither alone is sufficient

**Implementation:**
```cpp
PassengerNode* headPassengerNode;           // Linked list head
char seatingGrid[totalRows][totalColumns];  // 2D availability grid
```

**Synchronization Rules:**
- When inserting: add to linked list AND mark seat as occupied in grid
- When deleting: remove from linked list AND mark seat as available in grid
- Grid is used for display and availability checks
- Linked list is used for passenger data storage and iteration

---

### 6.2 Grid Markers

**Assumption:** Seating grid should use **simple character markers** for availability.

**Implementation:**
```cpp
const char availableSeatMarker = 'O';   // Open seat
const char occupiedSeatMarker = 'X';    // Taken seat
```

**Rationale:**
- Visual clarity (O = open, X = taken)
- Single-character representation for compact display
- Easy to check with simple equality comparison

---

## 7. Error Handling Assumptions

### 7.1 Input Buffer Management

**Assumption:** Invalid input can cause **cin error states** that require explicit clearing.

**Rationale:**
- `cin >> variable` fails when type mismatch occurs (e.g., entering "abc" for an integer)
- Failed state persists and causes infinite loops if not cleared
- Buffer contains invalid data that must be discarded

**Implementation:**
```cpp
void clearInputBuffer() {
    cin.clear();                  // Clear error flags
    cin.ignore(10000, '\n');      // Discard remaining input
}
```

**Applied After:**
- Invalid integer input (menu choice, seat row)
- Any cin failure detected
- Before critical operations to ensure clean state

---

### 7.2 File Operation Failures

**Assumption:** CSV file operations can **fail** and should be handled gracefully.

**Handled Cases:**
- File not found (on load): show error, continue with empty list
- File cannot be opened for writing (on save): show error, warn user
- Corrupted CSV data: skip invalid lines (though not explicitly validated in current implementation)

**Implementation:**
- Check `is_open()` after opening file
- Return boolean success/failure status
- Display clear error messages to user
- Continue program execution when possible

---

## 8. Standards Compliance Assumptions

### 8.1 Coding Standards

**Assumption:** The implementation must follow the coding philosophy defined in `Standards.md`.

**Key Standards Applied:**

1. **Naming Conventions:**
   - camelCase: variables, functions, parameters
   - PascalCase: classes, structs, types
   - No abbreviations (e.g., `passengerName` not `pName`)

2. **Comments:**
   - Section banners for major code sections
   - Full-sentence comments with punctuation
   - Purpose-focused explanations (why, not what)

3. **Code Organization:**
   - Logical grouping: includes → constants → structures → functions → main
   - No magic numbers (named constants for all values)
   - Clear function responsibilities

4. **Documentation:**
   - Function descriptions explain purpose and parameters
   - Edge cases documented (head/middle/tail deletion)
   - Assumptions noted in code comments

---

## 9. Assignment Interpretation Assumptions

### 9.1 Scope of Implementation

**Assumption:** The "Reservation & Cancellation (Linked List Version)" component is **one part of a larger multi-person assignment**.

**Interpretation:**
- Focus on linked list implementation for reservations and cancellations
- Integrate with provided CSV dataset
- Ensure clean interfaces for potential integration with other components
- Complete standalone functionality for demonstration purposes

---

### 9.2 Demonstration Requirements

**Assumption:** The implementation must demonstrate understanding of:

1. **Linked List Operations:**
   - Node structure design ✓
   - Insertion (append to end) ✓
   - Deletion (head, middle, tail cases) ✓
   - Traversal for display and search ✓

2. **Memory Management:**
   - Dynamic allocation with `new` ✓
   - Proper deallocation with `delete` ✓
   - No memory leaks ✓

3. **Pointer Logic:**
   - Node linking ✓
   - Pointer reassignment for deletion ✓
   - NULL pointer handling ✓

4. **Integration with Supporting Structures:**
   - Seating grid synchronization ✓
   - CSV file I/O ✓
   - User interface integration ✓

---

## 10. Non-Functional Assumptions

### 10.1 Performance

**Assumption:** With a maximum of 180 passengers, **linear search complexity** is acceptable.

**Rationale:**
- O(n) operations (insertion, deletion, search) are fast for n ≤ 180
- No need for complex optimizations (binary search, hash tables, etc.)
- Simplicity and clarity prioritized over micro-optimization

---

### 10.2 Platform

**Assumption:** The program runs on **Windows** with a command-line interface.

**Platform-Specific Code:**
- `system("cls")` for screen clearing (Windows-specific)
- File paths use backslash separators

**Portability Note:** For cross-platform support, would need:
- Conditional compilation for screen clearing
- Path separator abstraction

---

### 10.3 Data Persistence

**Assumption:** Data persists **only when explicitly saved** on exit.

**Behavior:**
- Changes are in-memory during program execution
- "Save and Exit" writes current state to CSV
- No auto-save or crash recovery
- Closing program without saving = lost changes

**Rationale:**
- Explicit save gives user control
- Prevents partial/corrupted saves from crashes
- Clear mental model for users

---

## 11. Design Decisions Summary

| Aspect                  | Assumption                                | Justification                          |
|-------------------------|------------------------------------------|----------------------------------------|
| CSV Record Handling     | Keep most recent booking per seat        | Logical current state representation   |
| Passenger ID            | Auto-generate sequential IDs             | Prevent duplicates, improve UX         |
| Cancellation Policy     | Allow cancelling any reservation         | Matches real-world systems             |
| Insertion Method        | Append to end of list                    | Simple, maintains chronological order  |
| Deletion Cases          | Handle head/middle/tail explicitly       | Comprehensive linked list demo         |
| Seating Grid            | 30 rows × 6 columns, 3 classes          | Based on CSV data analysis             |
| Display Format          | Compact table, one line per passenger    | Scalability for 180 records            |
| Input Validation        | Comprehensive validation all inputs      | Robustness and user experience         |
| Save Behavior           | Overwrite CSV with current state         | Reflect all changes including cancels  |
| Screen Management       | Clear between operations                 | Clean, professional interface          |

---

## 12. Limitations and Future Considerations

### Known Limitations

1. **No Transaction Logging:** Cannot track individual booking/cancellation events over time
2. **No Undo Functionality:** Cancelled reservations are permanently removed
3. **No Concurrent Access:** Single-user system, no multi-user support
4. **Platform-Specific:** Screen clearing tied to Windows
5. **No Backup:** No automatic backup before overwriting CSV

### Potential Enhancements

1. **Search Functionality:** Search passengers by name, seat, or class
2. **Sorting Options:** Sort display by name, ID, seat, or class
3. **Filter Views:** Show only First/Business/Economy passengers
4. **Booking History:** Log all transactions with timestamps
5. **Waitlist Management:** Queue for fully booked flights
6. **Seat Selection UI:** Visual seat map for selection

---

## 13. Validation Against Requirements

### Requirements Met

| Requirement                              | Implementation Status          | Evidence                              |
|------------------------------------------|-------------------------------|---------------------------------------|
| Design linked list node structure        | ✅ Complete                   | `PassengerNode` struct (line 39-58)  |
| Implement Reservation (Insertion)        | ✅ Complete                   | `insertReservation()` (line 260-278) |
| Insert passenger node into linked list   | ✅ Complete                   | Append to end logic                  |
| Allocate seat using seating grid         | ✅ Complete                   | `allocateSeat()` (line 232-234)      |
| Implement Cancellation (Deletion)        | ✅ Complete                   | `cancelReservation()` (line 280-334) |
| Remove passenger node by PassengerID     | ✅ Complete                   | Search by ID, remove match           |
| Handle head deletion                     | ✅ Complete                   | Case handled (line 288-303)          |
| Handle middle deletion                   | ✅ Complete                   | Case handled (line 305-334)          |
| Handle tail deletion                     | ✅ Complete                   | Included in middle case logic        |
| Free allocated seat                      | ✅ Complete                   | `deallocateSeat()` called on cancel  |

### Additional Features Implemented

- Auto-generated passenger IDs
- Comprehensive input validation
- CSV data persistence
- Full flight handling
- Compact display for 180 passengers
- Three-class seating structure display
- Clean terminal interface

---

## 14. Conclusion

These assumptions were made to create a **functional, robust, and user-friendly** reservation and cancellation system while adhering to the assignment requirements for linked list implementation. All decisions prioritize:

1. **Correctness:** Proper linked list operations and memory management
2. **Usability:** Clear interfaces, validation, and error messages
3. **Maintainability:** Clean code following Standards.md
4. **Practicality:** Real-world applicability and logical behavior

The implementation successfully demonstrates understanding of linked list data structures while providing a complete, professional booking system suitable for demonstration and evaluation.

---

**Document Version:** 1.0
**Last Updated:** January 12, 2026
**Author:** TP082578
