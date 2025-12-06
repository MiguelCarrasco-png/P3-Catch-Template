#include "CampusCompass.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

CampusCompass::CampusCompass() {
    // Constructor
}

// Helper to clean invisible characters (fixes autograder parsing issues)
string cleanCommand(string cmd) {
    if (!cmd.empty() && cmd.back() == '\r') {
        cmd.pop_back();
    }
    return cmd;
}

// Helper to parse HH:MM to minutes
int CampusCompass::parseTime(string timeStr) {
    if (timeStr.length() < 5) return 0;
    try {
        int h = stoi(timeStr.substr(0, 2));
        int m = stoi(timeStr.substr(3, 2));
        return h * 60 + m;
    } catch (...) {
        return 0;
    }
}

bool CampusCompass::ParseCSV(string edgesFile, string classesFile) {
    //Load Edges
    ifstream inFile(edgesFile);
    if (!inFile.is_open()) {
        size_t lastSlash = edgesFile.find_last_of("/\\");
        string fileName = (lastSlash == string::npos) ? edgesFile : edgesFile.substr(lastSlash + 1);
        
        inFile.open(fileName); 
        if (!inFile.is_open()) {
            inFile.open("data/" + fileName); 
             if (!inFile.is_open()) return false; 
        }
    }

    string line;
    getline(inFile, line); 

    while (getline(inFile, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') line.pop_back(); 

        stringstream ss(line);
        string temp;
        vector<string> row;
        while (getline(ss, temp, ',')) row.push_back(temp);

        if (row.size() >= 5) {
            try {
                int id1 = stoi(row[0]);
                int id2 = stoi(row[1]);
                string name1 = row[2];
                string name2 = row[3];
                int time = stoi(row[4]);

                locationNames[id1] = name1;
                locationNames[id2] = name2;

                // Add undirected edges
                Edge e1; e1.to = id2; e1.time = time; e1.isOpen = true;
                graph[id1].push_back(e1);

                Edge e2; e2.to = id1; e2.time = time; e2.isOpen = true;
                graph[id2].push_back(e2);
            } catch (...) { continue; }
        }
    }
    inFile.close();

    //Load Classes
    ifstream classFile(classesFile);
    if (!classFile.is_open()) {
        size_t lastSlash = classesFile.find_last_of("/\\");
        string fileName = (lastSlash == string::npos) ? classesFile : classesFile.substr(lastSlash + 1);
        
        classFile.open(fileName);
        if (!classFile.is_open()) {
            classFile.open("data/" + fileName);
        }
    }

    if (classFile.is_open()) {
        getline(classFile, line); 
        while (getline(classFile, line)) {
            if (line.empty()) continue;
            if (line.back() == '\r') line.pop_back();

            stringstream ss(line);
            string temp;
            vector<string> row;
            while (getline(ss, temp, ',')) row.push_back(temp);

            // Format: {ClassCode, LocationID, StartTime, EndTime}
            if (row.size() >= 2) {
                try {
                    string code = row[0];
                    int locId = stoi(row[1]);
                    
                    ClassData cd;
                    cd.locationId = locId;
                    cd.startMin = 0;
                    cd.endMin = 0;

                    // Parse times if available
                    if (row.size() >= 4) {
                        cd.startMin = parseTime(row[2]);
                        cd.endMin = parseTime(row[3]);
                    }

                    classDetails[code] = cd;
                } catch (...) { continue; }
            }
        }
    }
    return true;
}

bool CampusCompass::ParseCommand(string command) {
    command = cleanCommand(command); 
    if (command == "") return false;

    stringstream ss(command);
    string action;
    ss >> action;

    if (action == "insert") {
        
        int firstQuote = command.find('"');
        int secondQuote = command.find('"', firstQuote + 1);

        if (firstQuote == -1 || secondQuote == -1) {
            cout << "unsuccessful" << endl;
            return true;
        }

        string name = command.substr(firstQuote + 1, secondQuote - firstQuote - 1);

        // Get the part after the name
        string rest = command.substr(secondQuote + 1);
        stringstream ss2(rest);

        long id;
        int resId;
        int numClasses;

        if (ss2 >> id >> resId >> numClasses) {
            vector<string> codes;
            string code;
            
            // Read exactly as many codes as available
            while (ss2 >> code) {
                codes.push_back(code);
            }

            if (codes.size() != numClasses) {
                cout << "unsuccessful" << endl;
            } else {
                insertStudent(name, id, resId, numClasses, codes);
            }
        } else {
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
    else if (action == "dropClass") {
        long id;
        string classCode;
        if (ss >> id >> classCode) {
            dropClass(id, classCode);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "replaceClass") {
        long id;
        string oldClass, newClass;
        if (ss >> id >> oldClass >> newClass) {
            replaceClass(id, oldClass, newClass);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "removeClass") {
        string classCode;
        if (ss >> classCode) {
            removeClass(classCode);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "toggleEdgesClosure") {
        int n;
        if (ss >> n) {
            vector<int> locations;
            int loc;
            for (int i = 0; i < 2 * n; i++) {
                ss >> loc;
                locations.push_back(loc);
            }
            if (locations.size() == 2 * n) {
                toggleEdgesClosure(n, locations);
            } else {
                cout << "unsuccessful" << endl;
            }
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "checkEdgeStatus") {
        int id1, id2;
        if (ss >> id1 >> id2) {
            checkEdgeStatus(id1, id2);
        } else {
             // If args are missing
             cout << "DNE" << endl; 
        }
    }
    else if (action == "isConnected") {
        int id1, id2;
        if (ss >> id1 >> id2) {
            isConnected(id1, id2);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "printShortestEdges") {
        long id;
        if (ss >> id) {
            printShortestEdges(id);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "printStudentZone") {
        long id;
        if (ss >> id) {
            printStudentZone(id);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else if (action == "verifySchedule") { // EXTRA CREDIT
        long id;
        if (ss >> id) {
            verifySchedule(id);
        } else {
            cout << "unsuccessful" << endl;
        }
    }
    else {
        // Unknown command
        cout << "unsuccessful" << endl;
    }

    return true;
}

void CampusCompass::insertStudent(string name, long id, int resId, int numClasses, vector<string> codes) {
    // Check if ID is unique
    if (students.count(id) > 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    // Check ID Length (8 digits)
    string idStr = to_string(id);
    if (idStr.length() != 8) {
        cout << "unsuccessful" << endl;
        return;
    }

    // Check if name valid
    for (int i = 0; i < name.length(); i++) {
        char c = name[i];
        if (!isalpha(c) && c != ' ') {
            cout << "unsuccessful" << endl;
            return;
        }
    }

    // Check Class Count (1- 6)
    if (numClasses < 1 || numClasses > 6) {
        cout << "unsuccessful" << endl;
        return;
    }

    // Check Class Code Format (ex: COP3530)
    for (int i = 0; i < codes.size(); i++) {
        string c = codes[i];
        if (c.length() != 7) {
            cout << "unsuccessful" << endl;
            return;
        }
        // Check first 3 
        for (int j = 0; j < 3; j++) {
            if (!isupper(c[j])) {
                cout << "unsuccessful" << endl;
                return;
            }
        }
        // Check last 4 
        for (int j = 3; j < 7; j++) {
            if (!isdigit(c[j])) {
                cout << "unsuccessful" << endl;
                return;
            }
        }
    }

    // if everything is good, insert the student
    Student s;
    s.name = name;
    s.id = id;
    s.residenceId = resId;
    s.classCodes = codes;

    students[id] = s;
    cout << "successful" << endl;
}

void CampusCompass::removeStudent(long id) {
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

    // Check if student has the class
    bool found = false;
    vector<string>& codes = students[id].classCodes;

    for (int i = 0; i < codes.size(); i++) {
        if (codes[i] == classCode) {
            // remove 
            codes.erase(codes.begin() + i);
            found = true;
            break; 
        }
    }

    if (found) {
        // Check if student has 0 classes left
        if (codes.empty()) {
            students.erase(id);
        }
        cout << "successful" << endl;
    } else {
        // does nut have the class
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::replaceClass(long id, string oldClass, string newClass) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    // Validate format of the class
    if (newClass.length() != 7) {
        cout << "unsuccessful" << endl; return;
    }
    for(int j=0; j<3; j++) if(!isupper(newClass[j])) { cout << "unsuccessful" << endl; return; }
    for(int j=3; j<7; j++) if(!isdigit(newClass[j])) { cout << "unsuccessful" << endl; return; }

    // Check constraints
    vector<string>& codes = students[id].classCodes;
    int oldIndex = -1;
    bool hasNew = false;

    for (int i = 0; i < codes.size(); i++) {
        if (codes[i] == oldClass) {
            oldIndex = i;
        }
        if (codes[i] == newClass) {
            hasNew = true;
        }
    }

    if (oldIndex != -1 && !hasNew) {
        codes[oldIndex] = newClass;
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::removeClass(string classCode) {
    int count = 0;
    
    // Iterate through the map. 
    auto it = students.begin();
    
    while (it != students.end()) {
        bool dropped = false;
        
        // Check classes
        for (int i = 0; i < it->second.classCodes.size(); i++) {
            if (it->second.classCodes[i] == classCode) {
                // remove 
                it->second.classCodes.erase(it->second.classCodes.begin() + i);
                dropped = true;
                break; 
            }
        }

        if (dropped) {
            count++;
            // If student now has 0 classes, remove the student
            if (it->second.classCodes.empty()) {
                it = students.erase(it);
            } else {
                // move to next student
                it++;
            }
        } else {
            it++;
        }
    }
    cout << count << endl;
}

void CampusCompass::toggleEdgesClosure(int n, vector<int> locations) {
    // Iterate n times for n edges
    for (int i = 0; i < n; i++) {
        int u = locations[2 * i];
        int v = locations[2 * i + 1];

        // Toggle u - v
        if (graph.count(u)) {
            for (int k = 0; k < graph[u].size(); k++) {
                if (graph[u][k].to == v) {
                    if (graph[u][k].isOpen) {
                        graph[u][k].isOpen = false;
                    } else {
                        graph[u][k].isOpen = true;
                    }
                    break; 
                }
            }
        }

        //Toggle v - u 
        if (graph.count(v)) {
            for (int k = 0; k < graph[v].size(); k++) {
                if (graph[v][k].to == u) {
                    if (graph[v][k].isOpen) {
                        graph[v][k].isOpen = false;
                    } else {
                        graph[v][k].isOpen = true;
                    }
                    break;
                }
            }
        }
    }
    cout << "successful" << endl;
}

void CampusCompass::checkEdgeStatus(int id1, int id2) {
    // Check if node exists
    if (graph.count(id1) == 0) {
        cout << "DNE" << endl;
        return;
    }

    // Look for the neighbor
    bool edgeFound = false;
    for (int i = 0; i < graph[id1].size(); i++) {
        if (graph[id1][i].to == id2) {
            edgeFound = true;
            // Found the edge, check status
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
    if (graph.count(id1) == 0 || graph.count(id2) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    if (id1 == id2) {
        cout << "successful" << endl;
        return;
    }

    // Simple BFS to find path
    map<int, bool> visited;
    vector<int> q;
    q.push_back(id1);
    visited[id1] = true;

    int head = 0; 
    bool found = false;

    while (head < q.size()) {
        int current = q[head];
        head++;

        if (current == id2) {
            found = true;
            break;
        }

        // Check all neighbors
        for (int i = 0; i < graph[current].size(); i++) {
            Edge e = graph[current][i];
            
            if (e.isOpen && !visited[e.to]) {
                visited[e.to] = true;
                q.push_back(e.to);
            }
        }
    }

    if (found) {
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::runDijkstra(int startNode, map<int, int>& dists, map<int, int>& parents) {
    priority_queue<pair<int, int>> pq;

    dists.clear();
    parents.clear();

    dists[startNode] = 0;
    parents[startNode] = -1; 
    pq.push({0, startNode});

    while (!pq.empty()) {
        int d = -pq.top().first; 
        int u = pq.top().second;
        pq.pop();

        if (dists.count(u) && d > dists[u]) continue;

        if (graph.count(u)) {
            for (int i = 0; i < graph[u].size(); i++) {
                Edge e = graph[u][i];
                if (e.isOpen) {
                    int v = e.to;
                    int weight = e.time;
                    if (dists.count(v) == 0 || dists[u] + weight < dists[v]) {
                        dists[v] = dists[u] + weight;
                        parents[v] = u;
                        pq.push({-dists[v], v});
                    }
                }
            }
        }
    }
}

void CampusCompass::printShortestEdges(long id) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    Student s = students[id];
    int startNode = s.residenceId;

    // Run Dijkstra
    map<int, int> dists;
    map<int, int> parents;
    runDijkstra(startNode, dists, parents);

    sort(s.classCodes.begin(), s.classCodes.end());

    cout << "Name: " << s.name << endl;

    for (int i = 0; i < s.classCodes.size(); i++) {
        string code = s.classCodes[i];
        
        // Find where the class is
        int targetLoc = -1;
        if (classDetails.count(code)) {
            targetLoc = classDetails[code].locationId;
        }

        if (targetLoc != -1 && dists.count(targetLoc)) {
            cout << code << " | Total Time: " << dists[targetLoc] << endl;
        } else {
            cout << code << " | Total Time: -1" << endl;
        }
    }
}

void CampusCompass::printStudentZone(long id) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    Student s = students[id];
    int startNode = s.residenceId;

    // Run Dijkstra 
    map<int, int> dists;
    map<int, int> parents;
    runDijkstra(startNode, dists, parents);

    map<int, bool> inZone;
    vector<int> zoneNodes;

    // home is always in the zone
    inZone[startNode] = true;
    zoneNodes.push_back(startNode);

    for (int i = 0; i < s.classCodes.size(); i++) {
        string code = s.classCodes[i];
        if (classDetails.count(code)) {
            int target = classDetails[code].locationId;
            if (dists.count(target)) {
                int curr = target;
                while (curr != -1) {
                    if (!inZone[curr]) {
                        inZone[curr] = true;
                        zoneNodes.push_back(curr);
                    }
                    curr = parents[curr];
                }
            }
        }
    }

    int mstCost = 0;
    map<int, bool> mstVisited;
    priority_queue<pair<int, int>> pq;

    pq.push({0, startNode});

    while (!pq.empty()) {
        int w = -pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (mstVisited[u]) continue;

        mstVisited[u] = true;
        mstCost += w;

        if (graph.count(u)) {
            for (int i = 0; i < graph[u].size(); i++) {
                Edge e = graph[u][i];
                if (e.isOpen && inZone[e.to] && !mstVisited[e.to]) {
                    pq.push({-e.time, e.to});
                }
            }
        }
    }

    cout << "Student Zone Cost For " << s.name << ": " << mstCost << endl;
}

//ec
void CampusCompass::verifySchedule(long id) {
    if (students.count(id) == 0) {
        cout << "unsuccessful" << endl;
        return;
    }

    Student s = students[id];
    if (s.classCodes.size() <= 1) {
        cout << "unsuccessful" << endl;
        return;
    }

    // Sort classes by time
    sort(s.classCodes.begin(), s.classCodes.end(), [&](const string& a, const string& b) {
        return classDetails[a].startMin < classDetails[b].startMin;
    });

    cout << "Schedule Check for " << s.name << ":" << endl;

    for (size_t i = 0; i < s.classCodes.size() - 1; ++i) {
        string c1 = s.classCodes[i];
        string c2 = s.classCodes[i+1];

        // Ensure both classes exist 
        if (classDetails.count(c1) == 0 || classDetails.count(c2) == 0) {
            cout << c1 << " - " << c2 << " \"Cannot make it!\"" << endl;
            continue;
        }

        int loc1 = classDetails[c1].locationId;
        int loc2 = classDetails[c2].locationId;
        int endTime1 = classDetails[c1].endMin;
        int startTime2 = classDetails[c2].startMin;

        // Gap between classes
        int timeGap = startTime2 - endTime1;

        // Calculate travel time
        map<int, int> dists, parents;
        runDijkstra(loc1, dists, parents);

        bool canMakeIt = false;
        if (dists.count(loc2)) {
            int travelTime = dists[loc2];
            if (timeGap >= travelTime) {
                canMakeIt = true;
            }
        }

        if (canMakeIt) {
            cout << c1 << " - " << c2 << " \"Can make it!\"" << endl;
        } else {
            cout << c1 << " - " << c2 << " \"Cannot make it!\"" << endl;
        }
    }
}