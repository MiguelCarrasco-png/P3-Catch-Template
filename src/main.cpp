#include <iostream>
#include "CampusCompass.h"

using namespace std;

int main() {
    CampusCompass compass;

    // Load data
    compass.ParseCSV("../data/edges.csv", "../data/classes.csv");

    int numLines;
    cin >> numLines;

    // We need to clear the newline from the buffer after reading the int
    string dummy;
    getline(cin, dummy);

    for (int i = 0; i < numLines; i++) {
        string command;
        getline(cin, command);
        compass.ParseCommand(command);
    }

    return 0;
}