#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define MAX_STUDENTS 10
#define MAX_COURSES 20
#define NAME_LENGTH 100

// Course structure
typedef struct {
    char name[NAME_LENGTH];
    char letterGrade[3];  // e.g., A+, B-, etc.
    int creditHours;
    float gradePoints;    // Calculated from letter grade
} Course;

// Student structure
typedef struct {
    char name[NAME_LENGTH];
    Course courses[MAX_COURSES];
    int courseCount;
    float gpa;
} Student;

// Globals
Student students[MAX_STUDENTS];
int studentCount = 0;
int currentStudent = 0;

// UI handles
HWND hMainWindow;
HWND hStudentNameEdit, hStudentList;
HWND hCourseNameEdit, hCreditEdit, hGradeCombo;
HWND hCoursesListBox, hOutputEdit;
HWND hAddCourseBtn, hCalcGPABtn, hNewStudentBtn, hClearBtn, hSwitchStudentBtn;

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
float letterGradeToPoints(const char *grade);
void calculateGPA(int studentIndex);
void addCourse();
void clearCurrentForm();
void displayStudentData(int index);
void switchStudent();

// Convert letter grade to grade points
float letterGradeToPoints(const char *grade) {
    char letter = grade[0];
    char modifier = grade[1];
    float points = 0.0f;

    switch (letter) {
        case 'A': points = 4.0f; break;
        case 'B': points = 3.0f; break;
        case 'C': points = 2.0f; break;
        case 'D': points = 1.0f; break;
        case 'F': points = 0.0f; break;
        default: return 0.0f;
    }

    // Adjust for + or - (except A+ remains 4.0)
    if (modifier == '+' && letter != 'A') {
        points += 0.3f;
    } else if (modifier == '-') {
        points -= 0.3f;
    }

    return points;
}

// Calculate GPA for a student
void calculateGPA(int studentIndex) {
    if (studentIndex < 0 || studentIndex >= studentCount) return;

    Student *student = &students[studentIndex];
    float totalPoints = 0.0f;
    int totalCredits = 0;
    int i;

    for (i = 0; i < student->courseCount; i++) {
        Course *course = &student->courses[i];
        course->gradePoints = letterGradeToPoints(course->letterGrade);
        totalPoints += course->gradePoints * course->creditHours;
        totalCredits += course->creditHours;
    }

    if (totalCredits > 0) {
        student->gpa = totalPoints / totalCredits;
    } else {
        student->gpa = 0.0f;
    }

    // Display calculated GPA
    char result[256];
    sprintf(result, "Student: %s\r\nTotal Credits: %d\r\nGPA: %.2f",
            student->name, totalCredits, student->gpa);
    SetWindowText(hOutputEdit, result);

    // Update the student list
    SendMessage(hStudentList, LB_DELETESTRING, studentIndex, 0);
    sprintf(result, "%s (GPA: %.2f)", student->name, student->gpa);
    SendMessage(hStudentList, LB_INSERTSTRING, studentIndex, (LPARAM)result);
    SendMessage(hStudentList, LB_SETCURSEL, studentIndex, 0);
}

// Add a course to the current student
void addCourse() {
    if (currentStudent < 0 || currentStudent >= studentCount) return;

    Student *student = &students[currentStudent];

    if (student->courseCount >= MAX_COURSES) {
        MessageBox(hMainWindow, "Maximum number of courses reached.", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    Course *course = &student->courses[student->courseCount];

    // Get course name
    GetWindowText(hCourseNameEdit, course->name, NAME_LENGTH);
    if (strlen(course->name) == 0) {
        MessageBox(hMainWindow, "Please enter a course name.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Get credit hours
    char creditStr[10];
    GetWindowText(hCreditEdit, creditStr, 10);
    course->creditHours = atoi(creditStr);
    if (course->creditHours <= 0) {
        MessageBox(hMainWindow, "Please enter valid credit hours.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Get letter grade
    int selectedGrade = SendMessage(hGradeCombo, CB_GETCURSEL, 0, 0);
    if (selectedGrade == CB_ERR) {
        MessageBox(hMainWindow, "Please select a grade.", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    SendMessage(hGradeCombo, CB_GETLBTEXT, selectedGrade, (LPARAM)course->letterGrade);

    // Add course to list
    char listEntry[256];
    sprintf(listEntry, "%s - %d credits - %s", course->name, course->creditHours, course->letterGrade);
    SendMessage(hCoursesListBox, LB_ADDSTRING, 0, (LPARAM)listEntry);

    student->courseCount++;

    // Clear input fields for next course
    SetWindowText(hCourseNameEdit, "");
    SetWindowText(hCreditEdit, "");
    SendMessage(hGradeCombo, CB_SETCURSEL, 0, 0);
}

// Clear the current form
void clearCurrentForm() {
    SetWindowText(hCourseNameEdit, "");
    SetWindowText(hCreditEdit, "");
    SendMessage(hGradeCombo, CB_SETCURSEL, 0, 0);
    SendMessage(hCoursesListBox, LB_RESETCONTENT, 0, 0);
    SetWindowText(hOutputEdit, "");

    if (currentStudent >= 0 && currentStudent < studentCount) {
        students[currentStudent].courseCount = 0;
    }
}

// Display student data in the form
void displayStudentData(int index) {
    if (index < 0 || index >= studentCount) return;

    currentStudent = index;
    Student *student = &students[index];

    // Clear current display
    SendMessage(hCoursesListBox, LB_RESETCONTENT, 0, 0);

    int i;
    // Display student courses
    for (i = 0; i < student->courseCount; i++) {
        Course *course = &student->courses[i];
        char listEntry[256];
        sprintf(listEntry, "%s - %d credits - %s", course->name, course->creditHours, course->letterGrade);
        SendMessage(hCoursesListBox, LB_ADDSTRING, 0, (LPARAM)listEntry);
    }

    // Display GPA if calculated
    if (student->gpa > 0) {
        char result[256];
        int totalCredits = 0;
        for (i = 0; i < student->courseCount; i++) {
            totalCredits += student->courses[i].creditHours;
        }
        sprintf(result, "Student: %s\r\nTotal Credits: %d\r\nGPA: %.2f",
                student->name, totalCredits, student->gpa);
        SetWindowText(hOutputEdit, result);
    } else {
        SetWindowText(hOutputEdit, "");
    }
}

// Switch to selected student
void switchStudent() {
    int selectedIndex = SendMessage(hStudentList, LB_GETCURSEL, 0, 0);
    if (selectedIndex != LB_ERR) {
        displayStudentData(selectedIndex);
    }
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Student section
            CreateWindow("STATIC", "Student Name:", WS_VISIBLE | WS_CHILD,
                         20, 20, 100, 20, hwnd, NULL, NULL, NULL);
            hStudentNameEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                            130, 20, 150, 20, hwnd, NULL, NULL, NULL);
            hNewStudentBtn = CreateWindow("BUTTON", "Add Student", WS_VISIBLE | WS_CHILD,
                                          290, 20, 100, 20, hwnd, (HMENU)1, NULL, NULL);

            // Student list
            CreateWindow("STATIC", "Students:", WS_VISIBLE | WS_CHILD,
                         400, 20, 100, 20, hwnd, NULL, NULL, NULL);
            hStudentList = CreateWindow("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                                        400, 40, 180, 100, hwnd, NULL, NULL, NULL);
            hSwitchStudentBtn = CreateWindow("BUTTON", "Switch Student", WS_VISIBLE | WS_CHILD,
                                             400, 150, 180, 20, hwnd, (HMENU)5, NULL, NULL);

            // Course section
            CreateWindow("STATIC", "Course Name:", WS_VISIBLE | WS_CHILD,
                         20, 60, 100, 20, hwnd, NULL, NULL, NULL);
            hCourseNameEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           130, 60, 150, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Credit Hours:", WS_VISIBLE | WS_CHILD,
                         20, 90, 100, 20, hwnd, NULL, NULL, NULL);
            hCreditEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                       130, 90, 50, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Grade:", WS_VISIBLE | WS_CHILD,
                         20, 120, 100, 20, hwnd, NULL, NULL, NULL);
            hGradeCombo = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                       130, 120, 80, 200, hwnd, NULL, NULL, NULL);

            // Add grades to combo box
            const char *grades[] = {"A+", "A", "A-", "B+", "B", "B-", "C+", "C", "C-", "D+", "D", "D-", "F"};
            int i;
            for (i = 0; i < 13; i++) {
                SendMessage(hGradeCombo, CB_ADDSTRING, 0, (LPARAM)grades[i]);
            }
            SendMessage(hGradeCombo, CB_SETCURSEL, 0, 0);

            // Course list
            CreateWindow("STATIC", "Courses:", WS_VISIBLE | WS_CHILD,
                         20, 150, 100, 20, hwnd, NULL, NULL, NULL);
            hCoursesListBox = CreateWindow("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                                           20, 170, 360, 150, hwnd, NULL, NULL, NULL);

            // Buttons
            hAddCourseBtn = CreateWindow("BUTTON", "Add Course", WS_VISIBLE | WS_CHILD,
                                         290, 60, 90, 20, hwnd, (HMENU)2, NULL, NULL);
            hCalcGPABtn = CreateWindow("BUTTON", "Calculate GPA", WS_VISIBLE | WS_CHILD,
                                       20, 330, 120, 30, hwnd, (HMENU)3, NULL, NULL);
            hClearBtn = CreateWindow("BUTTON", "Clear Form", WS_VISIBLE | WS_CHILD,
                                     150, 330, 120, 30, hwnd, (HMENU)4, NULL, NULL);

            // Output area
            hOutputEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
                                       20, 370, 560, 80, hwnd, NULL, NULL, NULL);
            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 1: { // Add Student
                    if (studentCount >= MAX_STUDENTS) {
                        MessageBox(hwnd, "Maximum number of students reached.", "Warning", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    char studentName[NAME_LENGTH];
                    GetWindowText(hStudentNameEdit, studentName, NAME_LENGTH);

                    if (strlen(studentName) == 0) {
                        MessageBox(hwnd, "Please enter a student name.", "Error", MB_OK | MB_ICONERROR);
                        break;
                    }

                    // Create new student
                    strcpy(students[studentCount].name, studentName);
                    students[studentCount].courseCount = 0;
                    students[studentCount].gpa = 0.0f;

                    // Add to list and select
                    SendMessage(hStudentList, LB_ADDSTRING, 0, (LPARAM)studentName);
                    SendMessage(hStudentList, LB_SETCURSEL, studentCount, 0);

                    currentStudent = studentCount;
                    studentCount++;

                    // Clear form for new student
                    clearCurrentForm();
                    SetWindowText(hStudentNameEdit, "");
                    break;
                }

                case 2: // Add Course
                    addCourse();
                    break;

                case 3: // Calculate GPA
                    calculateGPA(currentStudent);
                    break;

                case 4: // Clear Form
                    clearCurrentForm();
                    break;

                case 5: // Switch Student
                    switchStudent();
                    break;
            }

            // Handle student list selection
            if (HIWORD(wParam) == LBN_SELCHANGE && (HWND)lParam == hStudentList) {
                switchStudent();
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    const char CLASS_NAME[] = "GPACalculatorClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Create window
    hMainWindow = CreateWindow(
        CLASS_NAME,
        "GPA Calculator",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 480,
        NULL, NULL, hInstance, NULL
    );
    if (hMainWindow == NULL) {
        return 0;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}