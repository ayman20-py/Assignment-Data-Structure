#include <iostream>
#include <fstream>

using namespace std;

//  A function cannot directly return a 2D array
// Need to create a struct to hold it and then return it as a struct
struct StringMatrix {
    std::string data[10000][5];
};

StringMatrix getPassengersArray() {

    string filepath = "C:\\Users\\User\\Dev\\C++\\Assignment-Data-Structure\\Dataset\\flight_passenger_data.csv.csv";
    ifstream iFile;

    iFile.open(filepath);
    string line = "";

    getline(iFile, line); // Bypass Header line

    int index = 0;

    string id, name, seatRow, seatColumn, seatClass;
    int sepLocator;

    StringMatrix passengerArray;
    

    int currentRow = 0;
    int currentColumn = 0;

    while (getline(iFile, line))
    {
        id = getNextItem(line)[0];
        line = getNextItem(line)[1];

        name = getNextItem(line)[0];
        line = getNextItem(line)[1];

        seatRow = getNextItem(line)[0];
        line = getNextItem(line)[1];

        seatColumn = getNextItem(line)[0];
        line = getNextItem(line)[1];

        seatClass = getNextItem(line)[0];
        line = getNextItem(line)[1];

        // setting the values in the array
        passengerArray.data[currentRow][0] = id;
        passengerArray.data[currentRow][1] = name;
        passengerArray.data[currentRow][2] = seatRow;
        passengerArray.data[currentRow][3] = seatColumn;
        passengerArray.data[currentRow][4] = seatClass;
        currentRow++;

    }
    iFile.close();

    return passengerArray;
}

string* getNextItem(string line) {
    static string returnData[2];

    int sepLocator = line.find(",");
    string element = line.substr(0, sepLocator);
    string newline = line.substr(sepLocator+1, line.length());

    returnData[0] = element;
    returnData[1] = newline;

    return returnData;
} 


int main() {
    StringMatrix x = getPassengersArray();
    // cout << x.data[4000][1] << endl;  checking if the array has been populated correctly
}