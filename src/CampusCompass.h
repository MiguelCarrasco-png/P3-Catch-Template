#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// Simple struct for the graph edges
struct Edge {
    int to;
    int time;
    bool isOpen;
};

// Simple struct for the student
struct Student {
    string name;
    long id;
    int residenceId;
    vector<string> classCodes;
};

class CampusCompass {
private:
    // Using a map for the graph: Location ID -> List of Edges
    map<int, vector<Edge>> graph;

    // Map for storing location names: ID -> Name
    map<int, string> locationNames;

    // Map for students: ID -> Student object
    map<long, Student> students;

public:
    CampusCompass();

    // Basic required methods
    bool ParseCSV(string edgesFile, string classesFile);
    bool ParseCommand(string command);

    // Command functions
    void insertStudent(string name, long id, int resId, int numClasses, vector<string> codes);
    void removeStudent(long id);
    void dropClass(long id, string classCode);
    void replaceClass(long id, string oldClass, string newClass);
    void removeClassFromAll(string classCode);

    // Graph functions
    void toggleEdgesClosure(int n, vector<int> locations);
    void checkEdgeStatus(int id1, int id2);
    void isConnected(int id1, int id2);
    void printShortestEdges(long id);
    void printStudentZone(long id);
};