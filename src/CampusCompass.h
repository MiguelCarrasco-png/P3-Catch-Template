#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <queue> 
#include <climits> 

using namespace std;

struct Edge {
    int to;
    int time;
    bool isOpen;
};

struct Student {
    string name;
    long id;
    int residenceId;
    vector<string> classCodes;
};

//hold loca and time
struct ClassData {
    int locationId;
    int startMin; 
    int endMin;
};

class CampusCompass {
private:
    map<int, vector<Edge>> graph;
    map<int, string> locationNames;
    map<long, Student> students;
    
    //Class data
    map<string, ClassData> classDetails;

    // Helper to run Dijkstra's algo
    void runDijkstra(int startNode, map<int, int>& dists, map<int, int>& parents);

    //time converter
    int parseTime(string timeStr);

public:
    CampusCompass();

    bool ParseCSV(string edgesFile, string classesFile);
    bool ParseCommand(string command);

    void insertStudent(string name, long id, int resId, int numClasses, vector<string> codes);
    void removeStudent(long id);
    void dropClass(long id, string classCode);
    void replaceClass(long id, string oldClass, string newClass);
    void removeClass(string classCode);
    
    void toggleEdgesClosure(int n, vector<int> locations);
    void checkEdgeStatus(int id1, int id2);
    void isConnected(int id1, int id2);
    
    void printShortestEdges(long id);
    void printStudentZone(long id);

    // EXTRA CREDIT METHOD
    void verifySchedule(long id);
};