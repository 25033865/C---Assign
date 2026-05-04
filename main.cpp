#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
using namespace std;

// ===================== STRUCTURES =====================
struct Date {
    int day;
    int month;
    int year;
};

struct Borrower {
    int borrowerId;
    string fullName;
    string phone;
};

struct LoanInfo {
    Borrower borrower;
    Date issueDate;
    Date dueDate;
    bool returned;
};

struct Book {
    int bookId;
    string title;
    string author;
    int yearPublished;
    bool available;
    LoanInfo* loan;
};

// ===================== CONSTANTS =====================
const int LOAN_PERIOD_DAYS = 14;

// ===================== FUNCTION PROTOTYPES =====================
void showMainMenu();
void processChoice(int choice, Book*& books, int& bookCount, Borrower*& borrowers, int& borrowerCount);

void addBook(Book*& books, int& bookCount);
void displayBooks(Book* books, int bookCount);
int findBookIndexById(Book* books, int bookCount, int id);
void searchBookByTitle(Book* books, int bookCount);

void addBorrower(Borrower*& borrowers, int& borrowerCount);
void displayBorrowers(Borrower* borrowers, int borrowerCount);
int findBorrowerIndexById(Borrower* borrowers, int borrowerCount, int id);

void borrowBook(Book* books, int bookCount, Borrower* borrowers, int borrowerCount);
Date createDueDate(Date issueDate, int daysAllowed);
Date getCurrentDate();

void returnBook(Book* books, int bookCount);
bool isOverdue(Date dueDate, Date currentDate);

void resizeBooksArray(Book*& books, int oldSize, int newSize);
void resizeBorrowersArray(Borrower*& borrowers, int oldSize, int newSize);
void releaseMemory(Book*& books, int& bookCount, Borrower*& borrowers, int& borrowerCount);

void saveBooksToFile(Book* books, int bookCount);
void loadBooksFromFile(Book*& books, int& bookCount);
void saveBorrowersToFile(Borrower* borrowers, int borrowerCount);
void loadBorrowersFromFile(Borrower*& borrowers, int& borrowerCount);

void displayActiveLoans(Book* books, int bookCount);
void showSummary(Book* books, int bookCount, Borrower* borrowers, int borrowerCount);

// ===================== TEMPLATE MODULE =====================
template <typename T>
void swapValues(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp; // fixed: original starter code wrongly assigned a = temp
}

template <typename T>
int linearSearchId(T* arr, int size, int targetId) {
    return -1; // generic version is not used directly
}

template <>
int linearSearchId<Book>(Book* arr, int size, int targetId) {
    for (int i = 0; i < size; i++) {
        if (arr[i].bookId == targetId) {
            return i;
        }
    }
    return -1;
}

template <>
int linearSearchId<Borrower>(Borrower* arr, int size, int targetId) {
    for (int i = 0; i < size; i++) {
        if (arr[i].borrowerId == targetId) {
            return i;
        }
    }
    return -1;
}

void runTests();

// ===================== HELPER FUNCTIONS =====================
void printDate(Date date) {
    cout << date.day << "/" << date.month << "/" << date.year;
}

string getField(stringstream& ss) {
    string field;
    getline(ss, field, '|');
    return field;
}

// ===================== MAIN =====================
int main() {
    Book* books = nullptr;
    Borrower* borrowers = nullptr;
    int bookCount = 0;
    int borrowerCount = 0;

    loadBooksFromFile(books, bookCount);
    loadBorrowersFromFile(borrowers, borrowerCount);

    int choice;
    do {
        showMainMenu();
        cin >> choice;
        processChoice(choice, books, bookCount, borrowers, borrowerCount);
    } while (choice != 0);

    saveBooksToFile(books, bookCount);
    saveBorrowersToFile(borrowers, borrowerCount);
    releaseMemory(books, bookCount, borrowers, borrowerCount);

    return 0;
}

// ===================== MENU =====================
void showMainMenu() {
    cout << "\n===== COMMUNITY LIBRARY SYSTEM =====\n";
    cout << "1. Add Book\n";
    cout << "2. Add Borrower\n";
    cout << "3. Borrow Book\n";
    cout << "4. Return Book\n";
    cout << "5. Search Book by Title\n";
    cout << "6. Display All Books\n";
    cout << "7. Display All Borrowers\n";
    cout << "8. Display Active Loans\n";
    cout << "9. Show Summary\n";
    cout << "10. Run Tests\n";
    cout << "0. Exit\n";
    cout << "Enter choice: ";
}

void processChoice(int choice, Book*& books, int& bookCount, Borrower*& borrowers, int& borrowerCount) {
    switch (choice) {
        case 1: addBook(books, bookCount); break;
        case 2: addBorrower(borrowers, borrowerCount); break;
        case 3: borrowBook(books, bookCount, borrowers, borrowerCount); break;
        case 4: returnBook(books, bookCount); break;
        case 5: searchBookByTitle(books, bookCount); break;
        case 6: displayBooks(books, bookCount); break;
        case 7: displayBorrowers(borrowers, borrowerCount); break;
        case 8: displayActiveLoans(books, bookCount); break;
        case 9: showSummary(books, bookCount, borrowers, borrowerCount); break;
        case 10: runTests(); break;
        case 0: cout << "Exiting... Data will be saved.\n"; break;
        default: cout << "Invalid choice. Please try again.\n";
    }
}

// ===================== BOOK MODULE =====================
void addBook(Book*& books, int& bookCount) {
    resizeBooksArray(books, bookCount, bookCount + 1);

    cout << "\nEnter book ID: ";
    cin >> books[bookCount].bookId;

    if (findBookIndexById(books, bookCount, books[bookCount].bookId) != -1) {
        cout << "A book with this ID already exists. Book not added.\n";
        resizeBooksArray(books, bookCount + 1, bookCount);
        return;
    }

    cin.ignore(1000, '\n');
    cout << "Enter book title: ";
    getline(cin, books[bookCount].title);
    cout << "Enter author: ";
    getline(cin, books[bookCount].author);
    cout << "Enter year published: ";
    cin >> books[bookCount].yearPublished;

    books[bookCount].available = true;
    books[bookCount].loan = nullptr;
    bookCount++;

    cout << "Book added successfully.\n";
}

void displayBooks(Book* books, int bookCount) {
    if (bookCount == 0) {
        cout << "\nNo books available in the system.\n";
        return;
    }

    cout << "\n===== ALL BOOKS =====\n";
    for (int i = 0; i < bookCount; i++) {
        cout << "Book ID: " << books[i].bookId << "\n";
        cout << "Title: " << books[i].title << "\n";
        cout << "Author: " << books[i].author << "\n";
        cout << "Year Published: " << books[i].yearPublished << "\n";
        cout << "Status: " << (books[i].available ? "Available" : "Borrowed") << "\n";
        if (books[i].loan != nullptr && !books[i].loan->returned) {
            cout << "Borrowed by: " << books[i].loan->borrower.fullName << "\n";
            cout << "Due date: ";
            printDate(books[i].loan->dueDate);
            cout << "\n";
        }
        cout << "-----------------------------\n";
    }
}

int findBookIndexById(Book* books, int bookCount, int id) {
    return linearSearchId<Book>(books, bookCount, id);
}

void searchBookByTitle(Book* books, int bookCount) {
    if (bookCount == 0) {
        cout << "No books available to search.\n";
        return;
    }

    cin.ignore(1000, '\n');
    string searchTitle;
    cout << "Enter title or part of title to search: ";
    getline(cin, searchTitle);

    bool found = false;
    cout << "\n===== SEARCH RESULTS =====\n";
    for (int i = 0; i < bookCount; i++) {
        if (books[i].title.find(searchTitle) != string::npos) {
            cout << "Book ID: " << books[i].bookId << "\n";
            cout << "Title: " << books[i].title << "\n";
            cout << "Author: " << books[i].author << "\n";
            cout << "Status: " << (books[i].available ? "Available" : "Borrowed") << "\n";
            cout << "-----------------------------\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No matching book found.\n";
    }
}

// ===================== BORROWER MODULE =====================
void addBorrower(Borrower*& borrowers, int& borrowerCount) {
    resizeBorrowersArray(borrowers, borrowerCount, borrowerCount + 1);

    cout << "\nEnter borrower ID: ";
    cin >> borrowers[borrowerCount].borrowerId;

    if (findBorrowerIndexById(borrowers, borrowerCount, borrowers[borrowerCount].borrowerId) != -1) {
        cout << "A borrower with this ID already exists. Borrower not added.\n";
        resizeBorrowersArray(borrowers, borrowerCount + 1, borrowerCount);
        return;
    }

    cin.ignore(1000, '\n');
    cout << "Enter full name: ";
    getline(cin, borrowers[borrowerCount].fullName);
    cout << "Enter phone number: ";
    getline(cin, borrowers[borrowerCount].phone);

    borrowerCount++;
    cout << "Borrower added successfully.\n";
}

void displayBorrowers(Borrower* borrowers, int borrowerCount) {
    if (borrowerCount == 0) {
        cout << "\nNo borrowers available in the system.\n";
        return;
    }

    cout << "\n===== ALL BORROWERS =====\n";
    for (int i = 0; i < borrowerCount; i++) {
        cout << "Borrower ID: " << borrowers[i].borrowerId << "\n";
        cout << "Full Name: " << borrowers[i].fullName << "\n";
        cout << "Phone: " << borrowers[i].phone << "\n";
        cout << "-----------------------------\n";
    }
}

int findBorrowerIndexById(Borrower* borrowers, int borrowerCount, int id) {
    return linearSearchId<Borrower>(borrowers, borrowerCount, id);
}

// ===================== LOAN MODULE =====================
Date createDueDate(Date issueDate, int daysAllowed) {
    Date due = issueDate;
    due.day = issueDate.day + daysAllowed;
    return due;
}

Date getCurrentDate() {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    Date today;
    today.day = localTime->tm_mday;
    today.month = localTime->tm_mon + 1;
    today.year = localTime->tm_year + 1900;
    return today;
}

void borrowBook(Book* books, int bookCount, Borrower* borrowers, int borrowerCount) {
    if (bookCount == 0 || borrowerCount == 0) {
        cout << "Books and borrowers must exist before borrowing can happen.\n";
        return;
    }

    int bookId, borrowerId;
    cout << "Enter book ID to borrow: ";
    cin >> bookId;
    cout << "Enter borrower ID: ";
    cin >> borrowerId;

    int bookIndex = findBookIndexById(books, bookCount, bookId);
    int borrowerIndex = findBorrowerIndexById(borrowers, borrowerCount, borrowerId);

    if (bookIndex == -1) {
        cout << "Book not found.\n";
        return;
    }

    if (borrowerIndex == -1) {
        cout << "Borrower not found.\n";
        return;
    }

    if (!books[bookIndex].available || books[bookIndex].loan != nullptr) {
        cout << "This book is already borrowed.\n";
        return;
    }

    books[bookIndex].loan = new LoanInfo;
    books[bookIndex].loan->borrower = borrowers[borrowerIndex];
    books[bookIndex].loan->issueDate = getCurrentDate();
    books[bookIndex].loan->dueDate = createDueDate(books[bookIndex].loan->issueDate, LOAN_PERIOD_DAYS);
    books[bookIndex].loan->returned = false;
    books[bookIndex].available = false;

    cout << "Book borrowed successfully.\n";
    cout << "Issue date: ";
    printDate(books[bookIndex].loan->issueDate);
    cout << "\nDue date: ";
    printDate(books[bookIndex].loan->dueDate);
    cout << "\n";
}

// ===================== RETURN MODULE =====================
void returnBook(Book* books, int bookCount) {
    if (bookCount == 0) {
        cout << "No books available.\n";
        return;
    }

    int bookId;
    cout << "Enter book ID to return: ";
    cin >> bookId;

    int bookIndex = findBookIndexById(books, bookCount, bookId);
    if (bookIndex == -1) {
        cout << "Book not found.\n";
        return;
    }

    if (books[bookIndex].loan == nullptr || books[bookIndex].available) {
        cout << "This book is not currently borrowed.\n";
        return;
    }

    Date today = getCurrentDate();
    if (isOverdue(books[bookIndex].loan->dueDate, today)) {
        cout << "Book is overdue. Due date was ";
        printDate(books[bookIndex].loan->dueDate);
        cout << ".\n";
    } else {
        cout << "Book returned on time.\n";
    }

    books[bookIndex].loan->returned = true;
    delete books[bookIndex].loan;
    books[bookIndex].loan = nullptr;
    books[bookIndex].available = true;

    cout << "Return completed successfully.\n";
}

bool isOverdue(Date dueDate, Date currentDate) {
    if (currentDate.year > dueDate.year) return true;
    if (currentDate.year < dueDate.year) return false;
    if (currentDate.month > dueDate.month) return true;
    if (currentDate.month < dueDate.month) return false;
    return currentDate.day > dueDate.day;
}

// ===================== DYNAMIC MEMORY MODULE =====================
void resizeBooksArray(Book*& books, int oldSize, int newSize) {
    Book* newBooks = nullptr;

    if (newSize > 0) {
        newBooks = new Book[newSize];

        int limit = oldSize < newSize ? oldSize : newSize;
        for (int i = 0; i < limit; i++) {
            newBooks[i] = books[i];
        }

        for (int i = oldSize; i < newSize; i++) {
            newBooks[i].bookId = 0;
            newBooks[i].title = "";
            newBooks[i].author = "";
            newBooks[i].yearPublished = 0;
            newBooks[i].available = true;
            newBooks[i].loan = nullptr;
        }
    }

    delete[] books;
    books = newBooks;
}

void resizeBorrowersArray(Borrower*& borrowers, int oldSize, int newSize) {
    Borrower* newBorrowers = nullptr;

    if (newSize > 0) {
        newBorrowers = new Borrower[newSize];

        int limit = oldSize < newSize ? oldSize : newSize;
        for (int i = 0; i < limit; i++) {
            newBorrowers[i] = borrowers[i];
        }

        for (int i = oldSize; i < newSize; i++) {
            newBorrowers[i].borrowerId = 0;
            newBorrowers[i].fullName = "";
            newBorrowers[i].phone = "";
        }
    }

    delete[] borrowers;
    borrowers = newBorrowers;
}

void releaseMemory(Book*& books, int& bookCount, Borrower*& borrowers, int& borrowerCount) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].loan != nullptr) {
            delete books[i].loan;
            books[i].loan = nullptr;
        }
    }

    delete[] books;
    books = nullptr;

    delete[] borrowers;
    borrowers = nullptr;

    bookCount = 0;
    borrowerCount = 0;
}

// ===================== FILE MODULE =====================
void saveBooksToFile(Book* books, int bookCount) {
    ofstream outFile("books.txt", ios::out);

    if (!outFile) {
        cout << "Error opening books.txt for saving.\n";
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        outFile << books[i].bookId << "|"
                << books[i].title << "|"
                << books[i].author << "|"
                << books[i].yearPublished << "|"
                << books[i].available << "\n";

        if (books[i].loan != nullptr) {
            outFile << "LOAN|"
                    << books[i].loan->borrower.borrowerId << "|"
                    << books[i].loan->borrower.fullName << "|"
                    << books[i].loan->borrower.phone << "|"
                    << books[i].loan->issueDate.day << "|"
                    << books[i].loan->issueDate.month << "|"
                    << books[i].loan->issueDate.year << "|"
                    << books[i].loan->dueDate.day << "|"
                    << books[i].loan->dueDate.month << "|"
                    << books[i].loan->dueDate.year << "|"
                    << books[i].loan->returned << "\n";
        } else {
            outFile << "NOLOAN\n";
        }
    }

    outFile.close();
}

void loadBooksFromFile(Book*& books, int& bookCount) {
    ifstream inFile("books.txt");

    if (!inFile) {
        return;
    }

    string bookLine;
    while (getline(inFile, bookLine)) {
        if (bookLine == "") continue;

        resizeBooksArray(books, bookCount, bookCount + 1);

        stringstream bookStream(bookLine);
        books[bookCount].bookId = stoi(getField(bookStream));
        books[bookCount].title = getField(bookStream);
        books[bookCount].author = getField(bookStream);
        books[bookCount].yearPublished = stoi(getField(bookStream));
        books[bookCount].available = stoi(getField(bookStream));
        books[bookCount].loan = nullptr;

        string loanLine;
        if (getline(inFile, loanLine)) {
            if (loanLine.find("LOAN|") == 0) {
                stringstream loanStream(loanLine);
                getField(loanStream); // skip LOAN label

                books[bookCount].loan = new LoanInfo;
                books[bookCount].loan->borrower.borrowerId = stoi(getField(loanStream));
                books[bookCount].loan->borrower.fullName = getField(loanStream);
                books[bookCount].loan->borrower.phone = getField(loanStream);
                books[bookCount].loan->issueDate.day = stoi(getField(loanStream));
                books[bookCount].loan->issueDate.month = stoi(getField(loanStream));
                books[bookCount].loan->issueDate.year = stoi(getField(loanStream));
                books[bookCount].loan->dueDate.day = stoi(getField(loanStream));
                books[bookCount].loan->dueDate.month = stoi(getField(loanStream));
                books[bookCount].loan->dueDate.year = stoi(getField(loanStream));
                books[bookCount].loan->returned = stoi(getField(loanStream));
                books[bookCount].available = false;
            }
        }

        bookCount++;
    }

    inFile.close();
}

void saveBorrowersToFile(Borrower* borrowers, int borrowerCount) {
    ofstream outFile("borrowers.txt", ios::out);

    if (!outFile) {
        cout << "Error opening borrowers.txt for saving.\n";
        return;
    }

    for (int i = 0; i < borrowerCount; i++) {
        outFile << borrowers[i].borrowerId << "|"
                << borrowers[i].fullName << "|"
                << borrowers[i].phone << "\n";
    }

    outFile.close();
}

void loadBorrowersFromFile(Borrower*& borrowers, int& borrowerCount) {
    ifstream inFile("borrowers.txt");

    if (!inFile) {
        return;
    }

    string line;
    while (getline(inFile, line)) {
        if (line == "") continue;

        resizeBorrowersArray(borrowers, borrowerCount, borrowerCount + 1);

        stringstream ss(line);
        borrowers[borrowerCount].borrowerId = stoi(getField(ss));
        borrowers[borrowerCount].fullName = getField(ss);
        borrowers[borrowerCount].phone = getField(ss);

        borrowerCount++;
    }

    inFile.close();
}

// ===================== REPORT MODULE =====================
void displayActiveLoans(Book* books, int bookCount) {
    bool found = false;

    cout << "\n===== ACTIVE LOANS =====\n";
    for (int i = 0; i < bookCount; i++) {
        if (books[i].loan != nullptr && !books[i].loan->returned) {
            cout << "Book ID: " << books[i].bookId << "\n";
            cout << "Title: " << books[i].title << "\n";
            cout << "Borrower: " << books[i].loan->borrower.fullName << "\n";
            cout << "Issue date: ";
            printDate(books[i].loan->issueDate);
            cout << "\nDue date: ";
            printDate(books[i].loan->dueDate);
            cout << "\n-----------------------------\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No active loans found.\n";
    }
}

void showSummary(Book* books, int bookCount, Borrower* borrowers, int borrowerCount) {
    int borrowedBooks = 0;
    int availableBooks = 0;

    for (int i = 0; i < bookCount; i++) {
        if (books[i].available) {
            availableBooks++;
        } else {
            borrowedBooks++;
        }
    }

    cout << "\n===== SUMMARY STATISTICS =====\n";
    cout << "Total books: " << bookCount << "\n";
    cout << "Total borrowers: " << borrowerCount << "\n";
    cout << "Available books: " << availableBooks << "\n";
    cout << "Borrowed books: " << borrowedBooks << "\n";
}

// ===================== TESTING MODULE =====================
void runTests() {
    cout << "\n===== RUNNING SIMPLE TESTS =====\n";

    int x = 5, y = 10;
    cout << "Before swap: x=" << x << " y=" << y << "\n";
    swapValues(x, y);
    cout << "After swap: x=" << x << " y=" << y << "\n";
    cout << "Expected result: x=10 y=5\n";

    Date due;
    due.day = 10;
    due.month = 5;
    due.year = 2026;

    Date current;
    current.day = 12;
    current.month = 5;
    current.year = 2026;

    cout << "Overdue test result: " << (isOverdue(due, current) ? "Overdue" : "Not overdue") << "\n";
    cout << "Expected result: Overdue\n";
}