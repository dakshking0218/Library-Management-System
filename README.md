# 📚 Library Management System

A **file-persistent, multi-user library management system** built in C++ with separate Admin and Student portals. Handles book inventory, student accounts, book issuing/returns, fine calculation, and transaction logging — all persisted to disk across sessions.

---

## 📌 Overview

This system models a real-world library with two distinct user roles. Admins manage the catalogue and monitor activity; students search, borrow, and return books. All data is read from and written to flat files, so nothing is lost between runs.

Built to demonstrate **OOP design**, **file I/O**, **STL algorithms**, and **real-world business logic** in C++.

---

## 🧠 Technical Highlights

### Object-Oriented Design
Three core domain classes — each with a clear, single responsibility:

```
Book          → ISBN, title, author, availability status
Student       → Roll no., name, balance, issued book list
Transaction   → Date, type (ISSUE/RETURN/FINE), roll, ISBN, fine amount
```

The `Library` class owns all three collections and exposes the full business logic layer above them.

### File Persistence — No Database Needed
All data survives program restarts via three flat files:

| File                  | Contains                          |
|-----------------------|-----------------------------------|
| `books.txt`           | Book catalogue with availability  |
| `students.txt`        | Student accounts + issued books   |
| `transactions.txt`    | Full audit log of every action    |

Records are pipe-delimited (`|`) and parsed with a custom `split()` + `trim()` utility — robust against whitespace and empty lines.

### Fine Calculation Logic
Return timing is enforced with a real date-difference engine using `<ctime>`:

| Days Since Issue | Fine      | Action                          |
|------------------|-----------|---------------------------------|
| ≤ 10 days        | $0.00     | Free return                     |
| 11 – 30 days     | $5.00     | Fine deducted from balance      |
| > 30 days        | Membership cancelled + suspended |

### STL Usage
- `std::sort` with lambda comparators for sorting by roll number or ISBN
- `std::find_if` for targeted issued-book lookup during returns
- `std::transform` for case-insensitive search across title and author fields
- `std::vector` for all dynamic collections

---

## 🗂️ Features

### 🔐 Admin Portal
| Feature              | Description                                      |
|----------------------|--------------------------------------------------|
| Add Book             | Register new books with ISBN, title, author      |
| Edit Book            | Update title or author of an existing book       |
| View All Books       | Full catalogue with availability status          |
| View All Students    | All accounts with balance and issued book count  |
| Search Books         | By ISBN, title keyword, or author keyword        |
| Sort Students        | By roll number (ascending)                       |
| Sort Books           | By ISBN (ascending)                              |
| Transaction History  | Full audit log — every issue, return, and fine   |
| Summary Dashboard    | Total books, available, issued, students, transactions |

### 🎓 Student Portal
| Feature              | Description                                      |
|----------------------|--------------------------------------------------|
| View My Details      | Balance, status, currently issued books          |
| Deposit Amount       | Top up wallet balance                            |
| Search Books         | Find available books before issuing              |
| Issue Book           | Borrow a book ($2 fee deducted from balance)     |
| Return Book          | Return with automatic fine calculation           |
| View Available Books | Browse the full catalogue                        |

---

## 🖥️ Usage Walkthrough

### Login Screen
```
===============================
 Library Management System
===============================
1. Admin Login
2. Student Login / Register
0. Exit
```

### Admin Credentials (default)
```
Username: admin
Password: admin123
```

### Admin Menu
```
--- Admin Menu ---
1. Add Book
2. Edit Book
3. View Books
4. View Students
5. Search Book
6. Sort Students by Roll
7. Sort Books by ISBN
8. View Transactions
9. Summary Dashboard
0. Logout
```

### Student — Issue a Book
```
Enter roll number: 101
Enter ISBN of book to issue: 1002

Book issued successfully.
Issue date: 2026-06-26
Remaining balance: $18.00
```

### Student — Return with Fine
```
Enter roll number: 101
Enter ISBN of book to return: 1002

Book returned successfully.
Days since issue: 18
Fine paid: $5.00
Remaining balance: $13.00
```

---

## 📁 Project Structure

```
library-management-system/
│
├── main.cpp            # Full source — single-file project
└── README.md
```

---

## 💡 Key Design Decisions

**Why flat files instead of SQLite?**
The goal was to demonstrate manual serialization and parsing — a fundamental skill. Implementing `save` / `load` logic by hand is more instructive (and interview-relevant) than calling a database API.

**Why pipe `|` as delimiter?**
Commas appear naturally in book titles and author names. Pipes are rare in that context, making the format reliable without needing escape logic.

**Why deduct $2 on issue and not on return?**
Prevents students with zero balance from issuing books they can't afford to return. The balance acts as a lightweight credit check before every transaction.

**Why suspend on 30+ day late returns vs. just a larger fine?**
Models a realistic library policy where repeat offenders lose borrowing privileges — a richer behaviour than a flat penalty.

---

## 📋 Default Book Catalogue (15 books pre-loaded)

| ISBN | Title                          | Author             |
|------|--------------------------------|--------------------|
| 1001 | Clean Code                     | Robert C. Martin   |
| 1002 | Introduction to Algorithms     | Cormen             |
| 1003 | C++ Primer                     | Lippman            |
| 1004 | The Pragmatic Programmer       | Andrew Hunt        |
| 1005 | Design Patterns                | Erich Gamma        |
| 1006 | Operating System Concepts      | Silberschatz       |
| 1007 | Computer Networking            | Kurose             |
| 1008 | Database System Concepts       | Silberschatz       |
| 1009 | Artificial Intelligence        | Russell            |
| 1010 | Discrete Mathematics           | Rosen              |
| 1011 | The Mythical Man-Month         | Fred Brooks        |
| 1012 | Effective Modern C++           | Scott Meyers       |
| 1013 | Head First Design Patterns     | Freeman            |
| 1014 | Data Structures and Algorithms | S. Lipschutz       |
| 1015 | Algorithm Design               | Kleinberg          |

---

## 🛠️ Possible Extensions

- [ ] Password protection for student accounts
- [ ] Book reservation / waitlist system
- [ ] Admin ability to suspend/reinstate students manually
- [ ] Export transaction history to CSV
- [ ] Multiple copies per book (copy count instead of single boolean)
- [ ] Email/SMS fine reminders (via external API)

---
