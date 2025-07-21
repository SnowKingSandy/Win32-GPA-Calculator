#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_STUDENTS 100
#define MAX_SUBJECTS 10

typedef struct {
    char name[100];
    float grades[MAX_SUBJECTS];
    int subject_count;
} Student;

Student *students = NULL;
int student_count = 0;

HWND hNameEdit, hSubjectEdit, hGradeEdits[MAX_SUBJECTS], hOutputBox, hListBox;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// New function to clear all form inputs
void clearForm() {
    // Clear student name
    SetWindowText(hNameEdit, "");
    
    // Clear subject count
    SetWindowText(hSubjectEdit, "");
    
    // Clear all grade inputs
    for (int i = 0; i < MAX_SUBJECTS; i++) {
        SetWindowText(hGradeEdits[i], "");
    }
    
    // Clear output box
    SetWindowText(hOutputBox, "");
}

void calculateGPA(HWND hwnd) {
    char name[100];
    char buf[32];
    GetWindowText(hNameEdit, name, 100);
    GetWindowText(hSubjectEdit, buf, 32);
    int subject_count = atoi(buf);

    if (subject_count <= 0 || subject_count > MAX_SUBJECTS) {
        MessageBox(hwnd, "Invalid number of subjects (1-10).", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    students = realloc(students, sizeof(Student) * (student_count + 1));
    Student *new_student = &students[student_count];
    student_count++;

    strncpy(new_student->name, name, sizeof(new_student->name));
    new_student->subject_count = subject_count;

    float sum = 0;
    for (int i = 0; i < subject_count; i++) {
        GetWindowText(hGradeEdits[i], buf, 32);
        float grade = atof(buf);
        new_student->grades[i] = grade;
        sum += grade;
    }

    float gpa = sum / subject_count;

    char result[512];
    snprintf(result, sizeof(result), "Student: %s\r\nGPA: %.2f\r\n", name, gpa);
    SetWindowText(hOutputBox, result);

    // Add student to ListBox in tabular format
    char list_entry[512];
    snprintf(list_entry, sizeof(list_entry), "%s\t%.2f", name, gpa);
    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)list_entry);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "GPAWinClass";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "GPA Calculator",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    free(students);
    return (int) msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hCalcButton, hClearButton, hNewStudentButton;
    switch (msg) {
        case WM_CREATE: {
            CreateWindow("STATIC", "Student Name:", WS_VISIBLE | WS_CHILD,
                         20, 20, 100, 20, hwnd, NULL, NULL, NULL);
            hNameEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                     130, 20, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Subjects:", WS_VISIBLE | WS_CHILD,
                         20, 50, 100, 20, hwnd, NULL, NULL, NULL);
            hSubjectEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                        130, 50, 50, 20, hwnd, NULL, NULL, NULL);

            for (int i = 0; i < MAX_SUBJECTS; i++) {
                char label[32];
                sprintf(label, "Grade %d:", i+1);
                CreateWindow("STATIC", label, WS_VISIBLE | WS_CHILD,
                             20, 80 + i*25, 100, 20, hwnd, NULL, NULL, NULL);
                hGradeEdits[i] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                              130, 80 + i*25, 50, 20, hwnd, NULL, NULL, NULL);
            }

            hCalcButton = CreateWindow("BUTTON", "Calculate GPA", WS_VISIBLE | WS_CHILD,
                                       130, 340, 120, 30, hwnd, (HMENU)1, NULL, NULL);

            hClearButton = CreateWindow("BUTTON", "Reset Form", WS_VISIBLE | WS_CHILD,
                                        260, 340, 120, 30, hwnd, (HMENU)2, NULL, NULL);

            hNewStudentButton = CreateWindow("BUTTON", "New Student", WS_VISIBLE | WS_CHILD,
                                             20, 340, 120, 30, hwnd, (HMENU)3, NULL, NULL);

            hOutputBox = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
                                      20, 380, 340, 60, hwnd, NULL, NULL, NULL);

            // Create header for student list display
            CreateWindow("STATIC", "Student Records", WS_VISIBLE | WS_CHILD,
                         370, 20, 200, 20, hwnd, NULL, NULL, NULL);
                         
            // Create column headers for tabular display
            CreateWindow("STATIC", "Name\t\tGPA", WS_VISIBLE | WS_CHILD,
                         370, 50, 200, 20, hwnd, NULL, NULL, NULL);
                         
            // Create the listbox for tabular student records display
            hListBox = CreateWindow("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
                                    370, 70, 200, 370, hwnd, NULL, NULL, NULL);
            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) {
                calculateGPA(hwnd);
            }
            else if (LOWORD(wParam) == 2) {
                // Reset Form button functionality
                clearForm();
            }
            else if (LOWORD(wParam) == 3) {
                // New Student button functionality
                clearForm();
                SetFocus(hNameEdit);  // Set focus to name field for convenience
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
