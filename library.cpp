// https://github.com/Vs79singh/Library-managment/blob/main/README.md
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;

// Constants for fine calculation
const double DAILY_FINE = 0.50;

// Book class definition
class Book {
private:
    string title;
    string author;
    int totalCopies;
    int availableCopies;

public:
    // Constructor
    Book(string t, string a, int copies) : title(t), author(a), totalCopies(copies), availableCopies(copies) {}

    // Function to get the title of the book
    string getTitle() {
        return title;
    }

    // Function to check availability
    bool checkAvailability() {
        return availableCopies > 0;
    }

    // Function to borrow the book
    void borrow() {
        if (availableCopies > 0) {
            availableCopies--;
        }
    }

    // Function to return the book
    void returned() {
        if (availableCopies < totalCopies) {
            availableCopies++;
        }
    }

    // Function to get the author of the book
    string getAuthor() {
        return author;
    }

    // Function to display book details
    void displayDetails() {
        cout << "Title: " << title << ", Author: " << author 
             << ", Available Copies: " << availableCopies << '/' << totalCopies << endl;
    }

    // Function to save book data to a file
    void saveToFile(ofstream &file) {
        file << title << ";" << author << ";" << totalCopies << ";" << availableCopies << endl;
    }

    // Function to load book data from a file
    static Book loadFromFile(const string &line) {
        size_t pos1 = line.find(';');
        size_t pos2 = line.find(';', pos1 + 1);
        size_t pos3 = line.find(';', pos2 + 1);
        
        string title = line.substr(0, pos1);
        string author = line.substr(pos1 + 1, pos2 - pos1 - 1);
        int totalCopies = stoi(line.substr(pos2 + 1, pos3 - pos2 - 1));
        int availableCopies = stoi(line.substr(pos3 + 1));
        
        return Book(title, author, totalCopies);
    }
};

// Member class definition
class Member {
private:
    string name;
    string password;

public:
    // Constructor
    Member(string n, string p) : name(n), password(p) {}

    // Function to get the member's name
    string getName() {
        return name;
    }

    // Function to validate password
    bool validatePassword(string p) {
        return password == p;
    }
};

// Structure to hold borrowed book information
struct BorrowedBook {
    Book* book;
    time_t dueDate;
    string memberName;
};

// Library class definition
class Library {
private:
    vector<Book> books;
    vector<Member> members;
    vector<BorrowedBook> borrowedBooks;

public:
    // Function to add a book to the library
    void addBook(Book b) {
        books.push_back(b);
    }

    // Function to add a member to the library
    void addMember(Member m) {
        members.push_back(m);
    }

    // Function to borrow a book
    void borrowBook(string title, string memberName) {
        for (auto &book : books) {
            if (book.getTitle() == title && book.checkAvailability()) {
                book.borrow();
                BorrowedBook borrowed = {&book, time(0) + (7 * 24 * 60 * 60), memberName}; // Due date is 7 days from now
                borrowedBooks.push_back(borrowed);
                cout << memberName << " borrowed " << title << ". Due date: " 
                     << put_time(localtime(&borrowed.dueDate), "%Y-%m-%d") << endl;
                return;
            }
        }
        cout << "Book not available or does not exist." << endl;
    }

    // Function to return a book and calculate fine if overdue
    void returnBook(string title) {
        for (auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
            if (it->book->getTitle() == title) {
                time_t currentTime = time(0);
                double fine = 0.0;
                if (it->dueDate < currentTime) {
                    double daysOverdue = difftime(currentTime, it->dueDate) / (60 * 60 * 24);
                    fine = daysOverdue * DAILY_FINE;
                    cout << "Book is overdue by " << daysOverdue << " days. Fine: $" << fixed << setprecision(2) << fine << endl;
                }
                else {
                    cout << "Book returned on time. No fine." << endl;
                }
                it->book->returned(); // Increase available copies
                borrowedBooks.erase(it); // Remove from borrowed books list
                return;
            }
        }
        cout << "Book not found in borrowed list." << endl;
    }

    // Function to display all books in the library
    void displayBooks() {
        for (auto &book : books) {
            book.displayDetails();
        }
    }

    // Function to display all borrowed books
    void displayBorrowedBooks() {
        for (auto &borrowed : borrowedBooks) {
            cout << "Book: " << borrowed.book->getTitle() 
                 << ", Borrowed by: " << borrowed.memberName 
                 << ", Due date: " << put_time(localtime(&borrowed.dueDate), "%Y-%m-%d") << endl;
        }
    }

    // Function to save library data to a file
    void saveLibraryData(const string &filename) {
        ofstream file(filename);
        if (file.is_open()) {
            for (auto &book : books) {
                book.saveToFile(file);
            }
            file.close();
            cout << "Library data saved to " << filename << endl;
        } else {
            cout << "Error saving library data." << endl;
        }
    }

    // Function to load library data from a file
    void loadLibraryData(const string &filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                books.push_back(Book::loadFromFile(line));
            }
            file.close();
            cout << "Library data loaded from " << filename << endl;
        } else {
            cout << "Error loading library data." << endl;
        }
    }
};

// Main function
int main() {
    Library lib;

    // Add sample data to library
    lib.addBook(Book("The Great Gatsby", "F. Scott Fitzgerald", 5));
    lib.addBook(Book("1984", "George Orwell", 3));
    lib.addBook(Book("To Kill a Mockingbird", "Harper Lee", 4));

    lib.addMember(Member("John Doe", "password123"));
    lib.addMember(Member("Jane Smith", "password456"));

    int choice;
    string title, memberName;

    while (true) {
        cout << "\nLibrary Management System\n";
        cout << "1. Display all books\n";
        cout << "2. Borrow a book\n";
        cout << "3. Return a book\n";
        cout << "4. Display borrowed books\n";
        cout << "5. Save library data\n";
        cout << "6. Load library data\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                lib.displayBooks();
                break;
            case 2:
                cout << "Enter book title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter member name: ";
                getline(cin, memberName);
                lib.borrowBook(title, memberName);
                break;
            case 3:
                cout << "Enter book title to return: ";
                cin.ignore();
                getline(cin, title);
                lib.returnBook(title);
                break;
            case 4:
                lib.displayBorrowedBooks();
                break;
            case 5:
                lib.saveLibraryData("library_data.txt");
                break;
            case 6:
                lib.loadLibraryData("library_data.txt");
                break;
            case 7:
                cout << "Exiting system...\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
