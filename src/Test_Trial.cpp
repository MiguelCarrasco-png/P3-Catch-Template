#include <iostream>
#include <fstream>
#include "CampusCompass.h"

using namespace std;

void createFiles() {
    // 1. Create the edges.csv provided in the prompt
    ofstream edges("edges.csv");
    edges << "LocationID_1,LocationID_2,Name_1,Name_2,Time" << endl;
    edges << "1,2,Hume Hall,Graham Hall,3" << endl;
    edges << "1,4,Hume Hall,Rawlings Hall,2" << endl;
    edges << "1,50,Hume Hall,Jennings Hall,4" << endl;
    edges << "2,4,Graham Hall,Rawlings Hall,3" << endl;
    edges << "14,17,Turlington Hall,Little Hall,7" << endl;
    edges << "14,23,Turlington Hall,Carleton Auditorium,2" << endl;
    edges << "23,24,Carleton Auditorium,Grinter Hall,2" << endl;
    edges << "43,7,Malachowsky Hall,Computer Science & Engineering,2" << endl;
    // ... (Adding a few specific ones for the test case)
    edges.close();

    // 2. Create the classes.csv provided in the prompt
    ofstream classes("classes.csv");
    classes << "ClassCode,LocationID,Start Time (HH:MM),End Time (HH:MM)" << endl;
    classes << "COP3530,14,10:40,11:30" << endl; // Turlington (14)
    classes << "CNT4007,43,11:45,12:35" << endl; // Malachowsky (43)
    classes << "COP3502,23,09:35,10:25" << endl; // Carleton (23)
    classes.close();
}

int main() {
    createFiles();

    CampusCompass app;
    app.ParseCSV("edges.csv", "classes.csv");

    cout << "--- Test 1: Insert Student ---" << endl;
    // Student lives at Hume Hall (1). Takes COP3530 (at 14) and CNT4007 (at 43).
    // Path 1 -> 4 -> ... -> 14 is likely needed.
    // Note: Since I didn't paste the WHOLE 50 line CSV in createFiles(), 
    // some paths might be unreachable in this specific mini-test unless you use the full files.
    // But logic stands.
    app.ParseCommand("insert \"Test Student\" 11112222 14 2 COP3530 COP3502");
    
    cout << "\n--- Test 2: Print Shortest Edges ---" << endl;
    // Student lives at 14. Class COP3530 is at 14 (Time 0). Class COP3502 is at 23.
    // Edge 14->23 is cost 2.
    app.ParseCommand("printShortestEdges 11112222");

    cout << "\n--- Test 3: Student Zone ---" << endl;
    // Zone includes 14 and 23. MST cost should be 2.
    app.ParseCommand("printStudentZone 11112222");

    cout << "\n--- Test 4: Check Edge ---" << endl;
    app.ParseCommand("checkEdgeStatus 14 23");

    cout << "\n--- Test 5: Toggle Edge ---" << endl;
    app.ParseCommand("toggleEdgesClosure 1 14 23");
    app.ParseCommand("checkEdgeStatus 14 23");

    return 0;
}