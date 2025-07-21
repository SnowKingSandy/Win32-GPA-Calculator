# Win32 GPA Calculator Suite in C

This repository contains a collection of GPA calculator applications built from the ground up using C and the native Windows (Win32) API. The project showcases an evolution from a basic, single-student calculator to a more advanced, multi-student management system, demonstrating different approaches to UI design, state management, and application logic within the Win32 framework.

These applications are excellent examples of procedural programming and direct interaction with a low-level GUI toolkit, avoiding modern wrappers and frameworks. They serve as a practical study in C-based desktop application development for the Windows platform.

-----

## Application 1: Simple GPA Calculator (`gpa_calculator.c`)

This program is a straightforward, single-instance GPA calculator. It is designed for quickly calculating the Grade Point Average for one student at a time based on a set of numerical grades. The user first specifies the number of subjects, and the application's UI dynamically shows the corresponding number of input fields for the grades.

### Key Features 

  * **Dynamic Grade Fields**: The user interface adapts by showing only the required number of grade input fields (from 1 to 10) based on the user's input.
  * **Simple Average Calculation**: The GPA is calculated as a direct average of the numerical grades entered, making it suitable for simple grading systems.
  * **Dynamic Memory Allocation**: The program uses `realloc` to dynamically allocate memory for student records, demonstrating efficient memory management as more students are added.
  * **Student Record List**: A running list of calculated student records is maintained in a listbox, displaying the student's name and their final GPA in a clean, tabular format.
  * **Form Controls**: Includes "Calculate GPA," "Reset Form," and "New Student" buttons for a complete and intuitive user workflow. The "Reset" and "New Student" buttons call a `clearForm()` function to prepare the interface for fresh input.

-----

## Application 2: Advanced Multi-Student GPA Calculator (`gpa_calculator_adv.c` and `Dev C++ Compatible.c`)

This is a far more robust and feature-rich application designed to manage the academic records of multiple students simultaneously. It employs a more realistic GPA calculation method that takes into account both **letter grades** (e.g., A+, B-, C) and the **credit hours** for each course, resulting in a weighted GPA.

The UI is more complex, featuring distinct panels for student management and course entry. Users can add multiple students to a list and then switch between their records to add courses or calculate their GPA individually.

### Key Features 

  * **Multi-Student Management**: The core feature allows a user to add up to 10 students by name. A listbox displays all students, and the user can select any student to view or edit their course data.
  * **Realistic Weighted GPA Calculation**: The GPA is calculated using a weighted average. Each letter grade is converted to grade points (e.g., 'A' = 4.0, 'B' = 3.0), adjusted for '+' or '-' modifiers, and then weighted by the course's credit hours.
  * **Course-Based Entry System**: Instead of entering all grades at once, the user adds courses one by one for the currently selected student. Each course includes a name, credit hours, and a letter grade selected from a dropdown menu.
  * **Stateful Interface**: The application maintains the state for each student. Switching between students will clear the form and display the specific course list and calculated GPA for the newly selected student.
  * **Comprehensive Controls**: Includes buttons to "Add Student," "Switch Student," "Add Course," "Calculate GPA," and "Clear Form," providing full control over the data entry process.

### Note on `Dev C++ Compatible.c`

The `Dev C++ Compatible.c` file is functionally identical to `gpa_calculator_adv.c`. The only modifications are minor syntactic changes, such as declaring loop-counter variables at the beginning of a function block. This was done to ensure compatibility with older C standards (like C89/ANSI C) that are sometimes the default in legacy compilers like Dev-C++. For all modern compilers (like GCC or Clang), both versions will behave identically.

-----

### How to Compile and Run

To compile these applications, you will need a C compiler configured for Windows development, such as **MinGW-w64** (which provides GCC) or the compiler included with **Visual Studio**.

1.  **Open a command prompt** or terminal with access to your C compiler.

2.  **Navigate** to the directory where you saved the source files.

3.  **Compile** the desired program using the following command, linking against the necessary Windows libraries:

    **For the Simple Calculator:**

    ```bash
    gcc gpa_calculator.c -o gpa_simple.exe -luser32 -lgdi32
    ```

    **For the Advanced Calculator:**

    ```bash
    gcc gpa_calculator_adv.c -o gpa_advanced.exe -luser32 -lgdi32
    ```

4.  **Run** the generated executable file:

    ```bash
    ./gpa_simple.exe
    ```

    or

    ```bash
    ./gpa_advanced.exe
    ```
