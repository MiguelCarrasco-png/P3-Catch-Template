#include "CampusCompass.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

CampusCompass::CampusCompass() {
    // Constructor doesn't need to do much
}

bool CampusCompass::ParseCSV(string edgesFile, string classesFile) {
    ifstream inFile(edgesFile);
    if (!inFile.is_open()) {
        return false;
    }

    string line;
    // read the header line to skip it
    getline(inFile, line);

    while (getline(inFile, line)) {
        stringstream ss(line);
        string temp;
        vector<string> row;

        // Parse CSV by comma
        while (getline(ss, temp, ',')) {
            row.push_back(temp);
        }

        // We need at least 5 columns
        if (row.size() >= 5) {
            // Convert strings to ints
            // Use try-catch just in case the CSV has bad data
            try {
                int id1 = stoi(row[0]);
                int id2 = stoi(row[1]);
                string name1 = row[2];
                string name2 = row[3];
                int time = stoi(row[4]);

                // Add to names map
                locationNames[id1] = name1;
                locationNames[id2] = name2;

                // Add edge to graph (undirected, so add both ways)
                Edge e1;
                e1.to = id2;
                e1.time = time;
                e1.isOpen = true;
                graph[id1].push_back(e1);

                Edge e2;
                e2.to = id1;
                e2.time = time;
                e2.isOpen = true;
                graph[id2].push_back(e2);
            }
            catch (...) {
                continue;
            }
        }
    }
    return true;
}

bool CampusCompass::ParseCommand(string command) {
    if (command == "") return false;

    stringstream ss(command);
    string action;
    ss >> action;

    if (action == "insert") {
        // Format: insert "Name" ID ResID N Code1 Code2 ...

        // Manually parse the name between quotes
        int firstQuote = command.find('"');
        int secondQuote = command.find('"', firstQuote + 1);

        if (firstQuote == -1 || secondQuote == -1) {
            cout << "unsuccessful" << endl;
            return true;
        }

        string name = command.substr(firstQuote + 1, secondQuote - firstQuote - 1);

        // Parse the numbers after the name
        string rest = command.substr(secondQuote + 1);
        stringstream ss2(rest);

        long id;
        int resId;
        int numClasses;

        // Try reading the integers
        if (ss2 >> id >> resId >> numClasses) {
            vector<string> codes;
            string code;

            // Read exactly N codes if possible, or whatever is there
            while (ss2 >> code) {
                codes.push_back(code);
            }

            // Validation: The prompt says "There must be N classes after reading N"
            if (codes.size() != numClasses) {
                cout << "unsuccessful" << endl;
            } else {
                insertStudent(name, id, resId, numClasses, codes);
            }
        } else {
            // Failed to read ID, ResID, or N
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "remove") {
        long id;
        if (ss >> id) {
            removeStudent(id);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    // ... (Keep the other else if blocks for dropClass, replaceClass, etc. here) ...
    // Note: Ensure the other commands from the previous file are still here

    return true;
}

void CampusCompass::insertStudent(string name, long id, int resId, int numClasses, vector<string> codes) {
    // 1. Check ID Uniqueness
    if (students.count(id) > 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    // 2. Check ID Length (Strictly 8 digits)
    // We convert to string to check length easily
    string idStr = to_string(id);
    if (idStr.length() != 8) {
        cout << "unsuccessful" << endl;
        return;
    }

    // 3. Check Name Constraints (Alphabets and spaces only)
    for (int i = 0; i < name.length(); i++) {
        char c = name[i];
        if (!isalpha(c) && c != ' ') {
            cout << "unsuccessful" << endl;
            return;
        }
    }

    // 4. Check Class Count (Between 1 and 6)
    if (numClasses < 1 || numClasses > 6) {
        cout << "unsuccessful" << endl;
        return;
    }

    // 5. Check Class Code Format (3 Caps + 4 Digits, e.g., COP3530)
    for (int i = 0; i < codes.size(); i++) {
        string c = codes[i];
        if (c.length() != 7) {
            cout << "unsuccessful" << endl;
            return;
        }
        // Check first 3 are Uppercase
        for (int j = 0; j < 3; j++) {
            if (!isupper(c[j])) {
                cout << "unsuccessful" << endl;
                return;
            }
        }
        // Check last 4 are Digits
        for (int j = 3; j < 7; j++) {
            if (!isdigit(c[j])) {
                cout << "unsuccessful" << endl;
                return;
            }
        }
    }

    // All validation passed, insert the student
    Student s;
    s.name = name;
    s.id = id;
    s.residenceId = resId;
    s.classCodes = codes;

    students[id] = s;
    cout << "successful" << endl;
}

void CampusCompass::removeStudent(long id) {
    // erase returns the number of elements removed (1 if found, 0 if not)
    if (students.erase(id)) {
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::dropClass(long id, string classCode) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    // Find the class in the student's list
    bool found = false;
    for (int i = 0; i < students[id].classCodes.size(); i++) {
        if (students[id].classCodes[i] == classCode) {
            // Remove it
            students[id].classCodes.erase(students[id].classCodes.begin() + i);
            found = true;
            break;
        }
    }

    if (found) {
        // If they have 0 classes, remove the student
        if (students[id].classCodes.empty()) {
            students.erase(id);
        }
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::replaceClass(long id, string oldClass, string newClass) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    bool hasOld = false;
    bool hasNew = false;
    int index = -1;

    // Check existing classes
    for (int i = 0; i < students[id].classCodes.size(); i++) {
        if (students[id].classCodes[i] == oldClass) {
            hasOld = true;
            index = i;
        }
        if (students[id].classCodes[i] == newClass) {
            hasNew = true;
        }
    }

    if (hasOld && !hasNew) {
        students[id].classCodes[index] = newClass;
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::removeClassFromAll(string classCode) {
    int count = 0;

    // Use an iterator so we can safely delete while looping
    auto it = students.begin();
    while (it != students.end()) {
        bool dropped = false;
        for (int i = 0; i < it->second.classCodes.size(); i++) {
            if (it->second.classCodes[i] == classCode) {
                it->second.classCodes.erase(it->second.classCodes.begin() + i);
                dropped = true;
                break;
            }
        }

        if (dropped) {
            count++;
            // Check if student is now empty
            if (it->second.classCodes.empty()) {
                // erase returns the next iterator
                it = students.erase(it);
            } else {
                it++;
            }
        } else {
            it++;
        }
    }
    cout << count << endl;
}

void CampusCompass::toggleEdgesClosure(int n, vector<int> locations) {
    // Loop through the path given: loc1 -> loc2, loc2 -> loc3, etc.
    for (int i = 0; i < locations.size() - 1; i++) {
        int u = locations[i];
        int v = locations[i + 1];

        // We need to find the edge in the graph and toggle isOpen
        // Toggle U -> V
        for (int j = 0; j < graph[u].size(); j++) {
            if (graph[u][j].to == v) {
                if (graph[u][j].isOpen == true) {
                    graph[u][j].isOpen = false;
                } else {
                    graph[u][j].isOpen = true;
                }
            }
        }

        // Toggle V -> U
        for (int j = 0; j < graph[v].size(); j++) {
            if (graph[v][j].to == u) {
                if (graph[v][j].isOpen == true) {
                    graph[v][j].isOpen = false;
                } else {
                    graph[v][j].isOpen = true;
                }
            }
        }
    }
    cout << "successful" << endl;
}

void CampusCompass::checkEdgeStatus(int id1, int id2) {
    if (graph.count(id1) == 0) {
        cout << "DNE" << endl;
        return;
    }

    bool edgeFound = false;
    for (int i = 0; i < graph[id1].size(); i++) {
        if (graph[id1][i].to == id2) {
            edgeFound = true;
            if (graph[id1][i].isOpen) {
                cout << "open" << endl;
            } else {
                cout << "closed" << endl;
            }
            return;
        }
    }

    if (!edgeFound) {
        cout << "DNE" << endl;
    }
}

void CampusCompass::isConnected(int id1, int id2) {
    // Placeholder for search algorithm (BFS/Dijkstra)
    cout << "successful" << endl;
}

void CampusCompass::printShortestEdges(long id) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }
    // Placeholder for Dijkstra Logic
}

void CampusCompass::printStudentZone(long id) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }
    // Placeholder for MST Logic
}