#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

static const string BOOKS_FILE = "books.txt";
static const string STUDENTS_FILE = "students.txt";
static const string TRANSACTIONS_FILE = "transactions.txt";

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

vector<string> split(const string& s, char delim) {
    vector<string> parts;
    string item;
    stringstream ss(s);
    while (getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

string currentDate() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", local);
    return string(buffer);
}

time_t parseDate(const string& dateStr) {
    tm t{};
    istringstream iss(dateStr);
    iss >> get_time(&t, "%Y-%m-%d");
    t.tm_isdst = -1;
    return mktime(&t);
}

int daysBetween(const string& d1, const string& d2) {
    time_t t1 = parseDate(d1);
    time_t t2 = parseDate(d2);
    double diff = difftime(t2, t1);
    return static_cast<int>(diff / (60 * 60 * 24));
}

bool containsCaseInsensitive(string text, string pattern) {
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
    return text.find(pattern) != string::npos;
}

class Book {
public:
    int isbn;
    string title;
    string author;
    bool available;

    Book(int i = 0, string t = "", string a = "", bool av = true)
        : isbn(i), title(std::move(t)), author(std::move(a)), available(av) {}
};

class Student {
public:
    int roll;
    string name;
    double balance;
    bool active;
    vector<pair<int, string>> issuedBooks; // {isbn, issueDate}

    Student(int r = 0, string n = "", double b = 0.0, bool act = true)
        : roll(r), name(std::move(n)), balance(b), active(act) {}
};

class Transaction {
public:
    string date;
    string type;   // ISSUE / RETURN / FINE
    int roll;
    int isbn;
    double fine;

    Transaction(string d = "", string t = "", int r = 0, int i = 0, double f = 0.0)
        : date(std::move(d)), type(std::move(t)), roll(r), isbn(i), fine(f) {}
};

class Library {
private:
    vector<Book> books;
    vector<Student> students;
    vector<Transaction> transactions;

    int findStudentIndex(int roll) {
        for (int i = 0; i < (int)students.size(); i++) {
            if (students[i].roll == roll) return i;
        }
        return -1;
    }

    int findBookIndex(int isbn) {
        for (int i = 0; i < (int)books.size(); i++) {
            if (books[i].isbn == isbn) return i;
        }
        return -1;
    }

    void logTransaction(const string& type, int roll, int isbn, double fine = 0.0) {
        transactions.push_back(Transaction(currentDate(), type, roll, isbn, fine));
    }

    void ensureDefaultBooks() {
        if (!books.empty()) return;

        books.push_back(Book(1001, "Clean Code", "Robert C. Martin", true));
        books.push_back(Book(1002, "Introduction to Algorithms", "Cormen", true));
        books.push_back(Book(1003, "C++ Primer", "Lippman", true));
        books.push_back(Book(1004, "The Pragmatic Programmer", "Andrew Hunt", true));
        books.push_back(Book(1005, "Design Patterns", "Erich Gamma", true));
        books.push_back(Book(1006, "Operating System Concepts", "Silberschatz", true));
        books.push_back(Book(1007, "Computer Networking", "Kurose", true));
        books.push_back(Book(1008, "Database System Concepts", "Silberschatz", true));
        books.push_back(Book(1009, "Artificial Intelligence", "Russell", true));
        books.push_back(Book(1010, "Discrete Mathematics", "Rosen", true));
        books.push_back(Book(1011, "The Mythical Man-Month", "Fred Brooks", true));
        books.push_back(Book(1012, "Effective Modern C++", "Scott Meyers", true));
        books.push_back(Book(1013, "Head First Design Patterns", "Freeman", true));
        books.push_back(Book(1014, "Data Structures and Algorithms", "S. Lipschutz", true));
        books.push_back(Book(1015, "Algorithm Design", "Kleinberg", true));
    }

    void saveBooks() {
        ofstream out(BOOKS_FILE);
        for (const auto& b : books) {
            out << b.isbn << "|" << b.title << "|" << b.author << "|" << b.available << "\n";
        }
    }

    void saveStudents() {
        ofstream out(STUDENTS_FILE);
        for (const auto& s : students) {
            out << s.roll << "|" << s.name << "|" << s.balance << "|" << s.active << "|";
            for (size_t i = 0; i < s.issuedBooks.size(); i++) {
                out << s.issuedBooks[i].first << "@" << s.issuedBooks[i].second;
                if (i + 1 < s.issuedBooks.size()) out << ",";
            }
            out << "\n";
        }
    }

    void saveTransactions() {
        ofstream out(TRANSACTIONS_FILE);
        for (const auto& t : transactions) {
            out << t.date << "|" << t.type << "|" << t.roll << "|" << t.isbn << "|" << t.fine << "\n";
        }
    }

    void loadBooks() {
        ifstream in(BOOKS_FILE);
        string line;
        while (getline(in, line)) {
            line = trim(line);
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            if (p.size() < 4) continue;
            books.push_back(Book(stoi(p[0]), p[1], p[2], stoi(p[3]) != 0));
        }
    }

    void loadStudents() {
        ifstream in(STUDENTS_FILE);
        string line;
        while (getline(in, line)) {
            line = trim(line);
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            if (p.size() < 5) continue;

            Student s(stoi(p[0]), p[1], stod(p[2]), stoi(p[3]) != 0);

            string issued = p[4];
            if (!issued.empty()) {
                vector<string> items = split(issued, ',');
                for (const string& item : items) {
                    size_t pos = item.find('@');
                    if (pos != string::npos) {
                        int isbn = stoi(item.substr(0, pos));
                        string issueDate = item.substr(pos + 1);
                        s.issuedBooks.push_back({isbn, issueDate});
                    }
                }
            }
            students.push_back(s);
        }
    }

    void loadTransactions() {
        ifstream in(TRANSACTIONS_FILE);
        string line;
        while (getline(in, line)) {
            line = trim(line);
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            if (p.size() < 5) continue;
            transactions.push_back(Transaction(p[0], p[1], stoi(p[2]), stoi(p[3]), stod(p[4])));
        }
    }

public:
    Library() {
        loadBooks();
        loadStudents();
        loadTransactions();
        ensureDefaultBooks();
    }

    void saveAll() {
        saveBooks();
        saveStudents();
        saveTransactions();
    }

    void showSummary() {
        int availableCount = 0;
        for (const auto& b : books) if (b.available) availableCount++;

        cout << "\n--- Library Summary ---\n";
        cout << "Total books: " << books.size() << "\n";
        cout << "Available books: " << availableCount << "\n";
        cout << "Issued books: " << books.size() - availableCount << "\n";
        cout << "Total students: " << students.size() << "\n";
        cout << "Total transactions: " << transactions.size() << "\n";
    }

    void addBook() {
        int isbn;
        string title, author;

        cout << "Enter ISBN: ";
        cin >> isbn;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (findBookIndex(isbn) != -1) {
            cout << "A book with this ISBN already exists.\n";
            return;
        }

        cout << "Enter title: ";
        getline(cin, title);
        cout << "Enter author: ";
        getline(cin, author);

        books.push_back(Book(isbn, title, author, true));
        cout << "Book added successfully.\n";
    }

    void editBook() {
        int isbn;
        cout << "Enter ISBN of book to edit: ";
        cin >> isbn;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int idx = findBookIndex(isbn);
        if (idx == -1) {
            cout << "Book not found.\n";
            return;
        }

        cout << "Current title: " << books[idx].title << "\n";
        cout << "Enter new title: ";
        getline(cin, books[idx].title);

        cout << "Current author: " << books[idx].author << "\n";
        cout << "Enter new author: ";
        getline(cin, books[idx].author);

        cout << "Book details updated.\n";
    }

    void viewBooks() {
        if (books.empty()) {
            cout << "No books available.\n";
            return;
        }

        cout << "\n--- Book List ---\n";
        for (const auto& b : books) {
            cout << "ISBN: " << b.isbn
                 << " | Title: " << b.title
                 << " | Author: " << b.author
                 << " | Status: " << (b.available ? "Available" : "Issued")
                 << "\n";
        }
    }

    void viewStudents() {
        if (students.empty()) {
            cout << "No students registered.\n";
            return;
        }

        cout << "\n--- Student List ---\n";
        for (const auto& s : students) {
            cout << "Roll: " << s.roll
                 << " | Name: " << s.name
                 << " | Balance: $" << fixed << setprecision(2) << s.balance
                 << " | Status: " << (s.active ? "Active" : "Suspended")
                 << " | Issued Books: " << s.issuedBooks.size()
                 << "\n";
        }
    }

    void createAccount() {
        if ((int)students.size() >= 20) {
            cout << "Student limit reached.\n";
            return;
        }

        int roll;
        string name;
        double deposit;

        cout << "Enter roll number: ";
        cin >> roll;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (findStudentIndex(roll) != -1) {
            cout << "Account already exists for this roll number.\n";
            return;
        }

        cout << "Enter student name: ";
        getline(cin, name);

        cout << "Enter initial deposit (minimum $50): ";
        cin >> deposit;

        if (deposit < 50) {
            cout << "Initial deposit must be at least $50.\n";
            return;
        }

        double balance = deposit - 50.0; // 20 account opening + 30 security deposit
        students.push_back(Student(roll, name, balance, true));
        cout << "Account created successfully. Current balance: $" << fixed << setprecision(2) << balance << "\n";
    }

    void displayStudent(int roll) {
        int idx = findStudentIndex(roll);
        if (idx == -1) {
            cout << "Student not found.\n";
            return;
        }

        const Student& s = students[idx];
        cout << "\n--- Student Details ---\n";
        cout << "Roll No: " << s.roll << "\n";
        cout << "Name: " << s.name << "\n";
        cout << "Balance: $" << fixed << setprecision(2) << s.balance << "\n";
        cout << "Status: " << (s.active ? "Active" : "Suspended") << "\n";
        cout << "Issued Books:\n";
        if (s.issuedBooks.empty()) {
            cout << "  None\n";
        } else {
            for (const auto& p : s.issuedBooks) {
                int bidx = findBookIndex(p.first);
                cout << "  ISBN: " << p.first;
                if (bidx != -1) cout << " | Title: " << books[bidx].title;
                cout << " | Issued on: " << p.second << "\n";
            }
        }
    }

    void depositAmount() {
        int roll;
        double amount;

        cout << "Enter roll number: ";
        cin >> roll;

        int idx = findStudentIndex(roll);
        if (idx == -1) {
            cout << "Student not found.\n";
            return;
        }

        cout << "Enter amount to deposit: ";
        cin >> amount;

        if (amount <= 0) {
            cout << "Invalid amount.\n";
            return;
        }

        students[idx].balance += amount;
        cout << "Deposit successful. New balance: $" << fixed << setprecision(2) << students[idx].balance << "\n";
    }

    void searchBook() {
        if (books.empty()) {
            cout << "No books available.\n";
            return;
        }

        int choice;
        cout << "\nSearch by:\n1. ISBN\n2. Title\n3. Author\nChoose: ";
        cin >> choice;

        if (choice == 1) {
            int isbn;
            cout << "Enter ISBN: ";
            cin >> isbn;
            int idx = findBookIndex(isbn);
            if (idx == -1) cout << "Book not found.\n";
            else printBook(idx);
        } else if (choice == 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string key;
            cout << "Enter title keyword: ";
            getline(cin, key);
            bool found = false;
            for (int i = 0; i < (int)books.size(); i++) {
                if (containsCaseInsensitive(books[i].title, key)) {
                    printBook(i);
                    found = true;
                }
            }
            if (!found) cout << "No books matched your search.\n";
        } else if (choice == 3) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string key;
            cout << "Enter author keyword: ";
            getline(cin, key);
            bool found = false;
            for (int i = 0; i < (int)books.size(); i++) {
                if (containsCaseInsensitive(books[i].author, key)) {
                    printBook(i);
                    found = true;
                }
            }
            if (!found) cout << "No books matched your search.\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void sortStudentsByRoll() {
        sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
            return a.roll < b.roll;
        });
        cout << "Students sorted by roll number.\n";
    }

    void sortBooksByISBN() {
        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.isbn < b.isbn;
        });
        cout << "Books sorted by ISBN.\n";
    }

    void issueBook() {
        int roll, isbn;
        cout << "Enter roll number: ";
        cin >> roll;
        int sidx = findStudentIndex(roll);
        if (sidx == -1) {
            cout << "Student not found.\n";
            return;
        }

        if (!students[sidx].active) {
            cout << "Student account is suspended.\n";
            return;
        }

        cout << "Enter ISBN of book to issue: ";
        cin >> isbn;

        int bidx = findBookIndex(isbn);
        if (bidx == -1) {
            cout << "Book not found.\n";
            return;
        }

        if (!books[bidx].available) {
            cout << "Book is currently unavailable.\n";
            return;
        }

        if (students[sidx].balance < 2.0) {
            cout << "Insufficient balance. Minimum $2 required to issue a book.\n";
            return;
        }

        for (const auto& p : students[sidx].issuedBooks) {
            if (p.first == isbn) {
                cout << "This student already has this book issued.\n";
                return;
            }
        }

        books[bidx].available = false;
        students[sidx].balance -= 2.0;
        students[sidx].issuedBooks.push_back({isbn, currentDate()});
        logTransaction("ISSUE", roll, isbn, 0.0);

        cout << "Book issued successfully.\n";
        cout << "Issue date: " << currentDate() << "\n";
        cout << "Remaining balance: $" << fixed << setprecision(2) << students[sidx].balance << "\n";
    }

    void returnBook() {
        int roll, isbn;
        cout << "Enter roll number: ";
        cin >> roll;
        int sidx = findStudentIndex(roll);
        if (sidx == -1) {
            cout << "Student not found.\n";
            return;
        }

        cout << "Enter ISBN of book to return: ";
        cin >> isbn;

        int bidx = findBookIndex(isbn);
        if (bidx == -1) {
            cout << "Book not found.\n";
            return;
        }

        auto& issued = students[sidx].issuedBooks;
        auto it = find_if(issued.begin(), issued.end(), [isbn](const pair<int, string>& p) {
            return p.first == isbn;
        });

        if (it == issued.end()) {
            cout << "This book is not issued to this student.\n";
            return;
        }

        string issueDate = it->second;
        string today = currentDate();
        int days = daysBetween(issueDate, today);

        double fine = 0.0;
        if (days <= 10) {
            fine = 0.0;
        } else if (days <= 30) {
            fine = 5.0;
        } else {
            fine = 0.0;
            students[sidx].active = false;
            cout << "Membership cancelled due to late return beyond 30 days.\n";
        }

        if (students[sidx].balance < fine) {
            cout << "Insufficient balance to pay fine of $" << fixed << setprecision(2) << fine << "\n";
            cout << "Please deposit money first.\n";
            return;
        }

        students[sidx].balance -= fine;
        books[bidx].available = true;
        issued.erase(it);

        logTransaction("RETURN", roll, isbn, fine);
        if (fine > 0) logTransaction("FINE", roll, isbn, fine);

        cout << "Book returned successfully.\n";
        cout << "Days since issue: " << days << "\n";
        cout << "Fine paid: $" << fixed << setprecision(2) << fine << "\n";
        cout << "Remaining balance: $" << fixed << setprecision(2) << students[sidx].balance << "\n";
    }

    void viewTransactions() {
        if (transactions.empty()) {
            cout << "No transactions recorded.\n";
            return;
        }

        cout << "\n--- Transaction History ---\n";
        for (const auto& t : transactions) {
            cout << t.date << " | " << t.type
                 << " | Roll: " << t.roll
                 << " | ISBN: " << t.isbn
                 << " | Fine: $" << fixed << setprecision(2) << t.fine
                 << "\n";
        }
    }

    void printBook(int idx) {
        const Book& b = books[idx];
        cout << "ISBN: " << b.isbn
             << " | Title: " << b.title
             << " | Author: " << b.author
             << " | Status: " << (b.available ? "Available" : "Issued")
             << "\n";
    }

    void adminMenu() {
        int choice;
        do {
            cout << "\n--- Admin Menu ---\n";
            cout << "1. Add Book\n";
            cout << "2. Edit Book\n";
            cout << "3. View Books\n";
            cout << "4. View Students\n";
            cout << "5. Search Book\n";
            cout << "6. Sort Students by Roll\n";
            cout << "7. Sort Books by ISBN\n";
            cout << "8. View Transactions\n";
            cout << "9. Summary Dashboard\n";
            cout << "0. Logout\n";
            cout << "Choose: ";
            cin >> choice;

            switch (choice) {
                case 1: addBook(); break;
                case 2: editBook(); break;
                case 3: viewBooks(); break;
                case 4: viewStudents(); break;
                case 5: searchBook(); break;
                case 6: sortStudentsByRoll(); break;
                case 7: sortBooksByISBN(); break;
                case 8: viewTransactions(); break;
                case 9: showSummary(); break;
                case 0: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    void studentMenu(int roll) {
        int idx = findStudentIndex(roll);
        if (idx == -1) {
            cout << "Student not found.\n";
            return;
        }

        int choice;
        do {
            cout << "\n--- Student Menu ---\n";
            cout << "1. View My Details\n";
            cout << "2. Deposit Amount\n";
            cout << "3. Search Book\n";
            cout << "4. Issue Book\n";
            cout << "5. Return Book\n";
            cout << "6. View Available Books\n";
            cout << "0. Logout\n";
            cout << "Choose: ";
            cin >> choice;

            switch (choice) {
                case 1: displayStudent(roll); break;
                case 2: depositAmount(); break;
                case 3: searchBook(); break;
                case 4: issueBook(); break;
                case 5: returnBook(); break;
                case 6: viewBooks(); break;
                case 0: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    bool adminLogin() {
        string user, pass;
        cout << "Admin username: ";
        cin >> user;
        cout << "Admin password: ";
        cin >> pass;

        return (user == "admin" && pass == "admin123");
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Library library;
    int choice;

    while (true) {
        cout << "\n===============================\n";
        cout << " Library Management System\n";
        cout << "===============================\n";
        cout << "1. Admin Login\n";
        cout << "2. Student Login / Register\n";
        cout << "0. Exit\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 0) {
            library.saveAll();
            cout << "Data saved. Goodbye.\n";
            break;
        } else if (choice == 1) {
            if (library.adminLogin()) {
                cout << "Admin login successful.\n";
                library.adminMenu();
            } else {
                cout << "Invalid admin credentials.\n";
            }
        } else if (choice == 2) {
            int roll;
            cout << "Enter roll number: ";
            cin >> roll;

            cout << "1. Login as existing student\n";
            cout << "2. Create new account\n";
            cout << "Choose: ";
            int subChoice;
            cin >> subChoice;

            if (subChoice == 2) {
                library.createAccount();
            }

            cout << "Re-enter your roll number to continue: ";
            cin >> roll;
            library.studentMenu(roll);
        } else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
