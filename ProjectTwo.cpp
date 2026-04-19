// ProjectTwo.cpp
// ABCU Advising Assistance Program
// Author: Emmett Perreault
// CS 300 - Project Two


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;


struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course c) : course(c), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root;

    Node* insert(Node* node, Course course) {
        if (node == nullptr) {
            return new Node(course);
        }
        if (course.courseNumber < node->course.courseNumber) {
            node->left = insert(node->left, course);
        } else if (course.courseNumber > node->course.courseNumber) {
            node->right = insert(node->right, course);
        }
        return node;
    }

    void inOrder(Node* node) const {
        if (node == nullptr) {
            return;
        }
        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << "\n";
        inOrder(node->right);
    }

    Node* search(Node* node, const string& courseNumber) const {
        if (node == nullptr) {
            return nullptr;
        }
        if (courseNumber == node->course.courseNumber) {
            return node;
        }
        if (courseNumber < node->course.courseNumber) {
            return search(node->left, courseNumber);
        }
        return search(node->right, courseNumber);
    }

    void destroyTree(Node* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    BST() : root(nullptr) {}

    ~BST() {
        destroyTree(root);
    }

    void insert(Course course) {
        root = insert(root, course);
    }

    void printSortedList() const {
        inOrder(root);
    }

    Course* findCourse(const string& courseNumber) const {
        Node* result = search(root, courseNumber);
        if (result == nullptr) return nullptr;
        return &result->course;
    }

    bool isEmpty() const {
        return root == nullptr;
    }
};


string toUpperCase(const string& str) {
    string upper = str;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

void loadDataStructure(const string& filename, BST& bst) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open file \"" << filename << "\". "
             << "Please check the filename and try again.\n";
        return;
    }

    set<string> validCourseNumbers;
    string line;

    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        istringstream ss(line);
        string token;
        if (getline(ss, token, ',')) {
            string courseNum = toUpperCase(token);
            courseNum.erase(0, courseNum.find_first_not_of(" \t"));
            courseNum.erase(courseNum.find_last_not_of(" \t") + 1);
            if (!courseNum.empty()) {
                validCourseNumbers.insert(courseNum);
            }
        }
    }

    file.clear();
    file.seekg(0);  

    int loadedCount = 0;
    int errorCount  = 0;

    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        istringstream ss(line);
        vector<string> tokens;
        string token;

        while (getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            tokens.push_back(token);
        }

        if (tokens.size() < 2) {
            cout << "Warning: Skipping malformed line (fewer than 2 fields): \""
                 << line << "\"\n";
            errorCount++;
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle  = tokens[1];

        bool prereqError = false;
        for (size_t i = 2; i < tokens.size(); i++) {
            string prereq = toUpperCase(tokens[i]);
            if (prereq.empty()) continue;

            if (validCourseNumbers.find(prereq) == validCourseNumbers.end()) {
                cout << "Warning: Prerequisite \"" << prereq
                     << "\" for course \"" << course.courseNumber
                     << "\" is not a valid course and will be skipped.\n";
                prereqError = true;
            } else {
                course.prerequisites.push_back(prereq);
            }
        }

        bst.insert(course);
        loadedCount++;

        if (prereqError) errorCount++;
    }

    file.close();

    cout << "Data loaded successfully. "
         << loadedCount << " course(s) added";
    if (errorCount > 0) {
        cout << " (" << errorCount << " warning(s) encountered)";
    }
    cout << ".\n";
}

void printCourseList(const BST& bst) {
    if (bst.isEmpty()) {
        cout << "No data loaded. Please use Option 1 to load a file first.\n";
        return;
    }
    cout << "Here is a sample schedule:\n\n";
    bst.printSortedList();
}

void printCourseInfo(const BST& bst) {
    if (bst.isEmpty()) {
        cout << "No data loaded. Please use Option 1 to load a file first.\n";
        return;
    }

    cout << "What course do you want to know about? ";
    string input;
    cin >> input;
    string courseNumber = toUpperCase(input);

    Course* course = bst.findCourse(courseNumber);

    if (course == nullptr) {
        cout << "Course \"" << courseNumber << "\" not found.\n";
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << "\n";

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None\n";
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); i++) {
            if (i > 0) cout << ", ";
            cout << course->prerequisites[i];
        }
        cout << "\n";
    }
}



void displayMenu() {
    cout << "\n";
    cout << "  1. Load Data Structure.\n";
    cout << "  2. Print Course List.\n";
    cout << "  3. Print Course.\n";
    cout << "  9. Exit\n";
    cout << "\n";
    cout << "What would you like to do? ";
}


int main() {
    BST courseTree;
    int choice = 0;

    cout << "Welcome to the course planner.\n";

    while (true) {
        displayMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                cout << "Enter the filename to load: ";
                string filename;
                cin >> filename;
                loadDataStructure(filename, courseTree);
                break;
            }
            case 2:
                printCourseList(courseTree);
                break;

            case 3:
                printCourseInfo(courseTree);
                break;

            case 9:
                cout << "Thank you for using the course planner!\n";
                return 0;

            default:
                cout << choice << " is not a valid option.\n";
                break;
        }
    }

    return 0;
}
