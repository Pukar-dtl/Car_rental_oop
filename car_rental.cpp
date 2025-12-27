#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Date {
    int day;
    int month;
    int year;
    
    Date() : day(1), month(1), year(2000) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    
    bool isValid() const {
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        
        // Check days in month
        int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        
        // Handle leap year for February
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (day < 1 || day > (isLeap ? 29 : 28)) return false;
        } else {
            if (day < 1 || day > daysInMonth[month-1]) return false;
        }
        
        return true;
    }
    
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    bool operator<=(const Date& other) const {
        return (*this < other) || (year == other.year && month == other.month && day == other.day);
    }
    
    bool operator>(const Date& other) const {
        return !(*this <= other);
    }
    
    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
    
    int differenceInDays(const Date& other) const {
        // Simple calculation - for exact calculation we'd need more complex date math
        int days1 = year * 365 + month * 30 + day;
        int days2 = other.year * 365 + other.month * 30 + other.day;
        return days2 - days1;
    }
    
    string toString() const {
        return to_string(day) + "/" + to_string(month) + "/" + to_string(year);
    }
};

Date getToday() {
    time_t now = time(0);
    tm* localtm = localtime(&now);
    return Date(localtm->tm_mday, localtm->tm_mon + 1, localtm->tm_year + 1900);
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

struct Car {
    int id;
    string company;
    string model;
    int dailyRent;
    bool isAvailable;
    
    Car(int carId, const string& comp, const string& mod, int rent)
        : id(carId), company(comp), model(mod), dailyRent(rent), isAvailable(true) {}
    
    string getFullName() const {
        return company + " " + model;
    }
    
    void display() const {
        cout << left << setw(5) << id 
             << setw(15) << company 
             << setw(15) << model 
             << setw(10) << dailyRent
             << setw(12) << (isAvailable ? "Available" : "Rented") << endl;
    }
};

struct Rental {
    int id;
    int carId;
    string customerName;
    Date rentDate;
    Date returnDate;
    int totalAmount;
    bool isActive;
    
    Rental(int rentId, int cId, const string& cust, const Date& rDate, const Date& retDate, int amount)
        : id(rentId), carId(cId), customerName(cust), rentDate(rDate), 
          returnDate(retDate), totalAmount(amount), isActive(true) {}
    
    void display() const {
        cout << left << setw(10) << id
             << setw(25) << customerName
             << setw(15) << rentDate.toString()
             << setw(15) << returnDate.toString()
             << setw(10) << totalAmount
             << setw(10) << (isActive ? "Active" : "Returned") << endl;
    }
    
    int calculateLateFee(int dailyRate, const Date& actualReturn) const {
        if (actualReturn <= returnDate) return 0;
        
        int daysLate = actualReturn.differenceInDays(returnDate);
        return daysLate * dailyRate * 1.5; // 50% late fee
    }
};

class CarRentalSystem {
private:
    vector<Car> cars;
    vector<Rental> rentals;
    int nextCarId;
    int nextRentalId;
    
    void displayHeader(const string& title) {
        cout << "\n" << string(60, '=') << endl;
        cout << " " << title << endl;
        cout << string(60, '=') << endl;
    }
    
    void displayTableHeader(const vector<string>& headers, const vector<int>& widths) {
        for (size_t i = 0; i < headers.size(); i++) {
            cout << left << setw(widths[i]) << headers[i];
        }
        cout << endl;
        
        // Calculate total width for the separator line
        int totalWidth = 0;
        for (int w : widths) {
            totalWidth += w;
        }
        cout << string(totalWidth, '-') << endl;
    }
    
    Car* findCarById(int carId) {
        for (auto& car : cars) {
            if (car.id == carId) {
                return &car;
            }
        }
        return nullptr;
    }
    
    Rental* findRentalById(int rentalId) {
        for (auto& rental : rentals) {
            if (rental.id == rentalId) {
                return &rental;
            }
        }
        return nullptr;
    }
    
    void updateCarAvailability() {
        Date today = getToday();
        for (auto& rental : rentals) {
            if (rental.isActive && today > rental.returnDate) {
                rental.isActive = false;
                Car* car = findCarById(rental.carId);
                if (car) {
                    car->isAvailable = true;
                }
            }
        }
    }

public:
    CarRentalSystem() : nextCarId(1), nextRentalId(1) {
        // No sample cars - system starts empty
        // User must add cars through the menu
    }

    void addCar() {
        displayHeader("ADD NEW CAR");
        
        string company, model;
        int dailyRent;
        
        cout << "Enter car company: ";
        getline(cin, company);
        
        cout << "Enter car model: ";
        getline(cin, model);
        
        while (true) {
            cout << "Enter daily rental rate: ";
            cin >> dailyRent;
            
            if (cin.fail() || dailyRent <= 0) {
                cout << "Invalid amount. Please enter a positive number." << endl;
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }
        
        cars.push_back(Car(nextCarId++, company, model, dailyRent));
        cout << "\nCar added successfully with ID: " << (nextCarId - 1) << endl;
    }
    
    void showAvailableCars() {
        updateCarAvailability();
        displayHeader("AVAILABLE CARS");
        
        if (cars.empty()) {
            cout << "No cars in the system. Please add cars first." << endl;
            return;
        }
        
        vector<string> headers = {"ID", "Company", "Model", "Rate/Day", "Status"};
        vector<int> widths = {5, 15, 15, 10, 12};
        displayTableHeader(headers, widths);
        
        bool foundAvailable = false;
        for (const auto& car : cars) {
            if (car.isAvailable) {
                car.display();
                foundAvailable = true;
            }
        }
        
        if (!foundAvailable) {
            cout << "No cars available for rent at the moment." << endl;
        }
    }
    
    void rentCar() {
        updateCarAvailability();
        displayHeader("RENT A CAR");
        
        // Show available cars first
        showAvailableCars();
        
        if (cars.empty()) {
            cout << "Please add cars first." << endl;
            return;
        }
        
        // Check if any car is available
        bool anyAvailable = false;
        for (const auto& car : cars) {
            if (car.isAvailable) {
                anyAvailable = true;
                break;
            }
        }
        
        if (!anyAvailable) {
            cout << "No cars available for rent." << endl;
            return;
        }
        
        int carId;
        cout << "\nEnter Car ID to rent: ";
        cin >> carId;
        clearInputBuffer();
        
        Car* car = findCarById(carId);
        if (!car) {
            cout << "Car ID not found!" << endl;
            return;
        }
        
        if (!car->isAvailable) {
            cout << "Car is already rented!" << endl;
            return;
        }
        
        // Get customer details
        string customerName;
        cout << "Enter customer name: ";
        getline(cin, customerName);
        
        // Get return date
        Date returnDate;
        while (true) {
            cout << "Enter return date (dd mm yyyy): ";
            int d, m, y;
            if (cin >> d >> m >> y) {
                returnDate = Date(d, m, y);
                if (!returnDate.isValid()) {
                    cout << "Invalid date! Please try again." << endl;
                    continue;
                }
                
                Date today = getToday();
                if (returnDate <= today) {
                    cout << "Return date must be in the future!" << endl;
                    continue;
                }
                
                int rentalDays = returnDate.differenceInDays(today);
                if (rentalDays > 365) {
                    cout << "Maximum rental period is 1 year!" << endl;
                    continue;
                }
                
                clearInputBuffer(); // Clear the newline
                break;
            } else {
                cout << "Invalid input format! Please use dd mm yyyy." << endl;
                clearInputBuffer();
            }
        }
        
        // Calculate total amount
        Date today = getToday();
        int rentalDays = returnDate.differenceInDays(today);
        int totalAmount = rentalDays * car->dailyRent;
        
        // Show summary
        cout << "\n" << string(50, '-') << endl;
        cout << "RENTAL SUMMARY" << endl;
        cout << string(50, '-') << endl;
        cout << "Car: " << car->getFullName() << endl;
        cout << "Customer: " << customerName << endl;
        cout << "Rental Date: " << today.toString() << endl;
        cout << "Return Date: " << returnDate.toString() << endl;
        cout << "Daily Rate: " << car->dailyRent << endl;
        cout << "Rental Days: " << rentalDays << endl;
        cout << "Total Amount: " << totalAmount << endl;
        cout << string(50, '-') << endl;
        
        // Confirm
        char confirm;
        cout << "\nConfirm rental? (y/n): ";
        cin >> confirm;
        clearInputBuffer();
        
        if (tolower(confirm) == 'y') {
            // Create rental record
            Rental newRental(nextRentalId++, carId, customerName, today, returnDate, totalAmount);
            rentals.push_back(newRental);
            
            // Update car availability
            car->isAvailable = false;
            
            cout << "\nCar rented successfully!" << endl;
            cout << "Rental ID: " << (nextRentalId - 1) << endl;
            cout << "Keep this ID for returning the car." << endl;
        } else {
            cout << "Rental cancelled." << endl;
        }
    }
    
    void showRentedCars() {
        updateCarAvailability();
        displayHeader("CURRENTLY RENTED CARS");
        
        if (rentals.empty()) {
            cout << "No rental records found." << endl;
            return;
        }
        
        vector<string> headers = {"Rental ID", "Customer", "Rent Date", "Return Date", "Amount", "Status"};
        vector<int> widths = {10, 25, 15, 15, 10, 10};
        displayTableHeader(headers, widths);
        
        bool foundActive = false;
        for (const auto& rental : rentals) {
            if (rental.isActive) {
                Car* car = findCarById(rental.carId);
                if (car) {
                    cout << left << setw(10) << rental.id
                         << setw(25) << rental.customerName
                         << setw(15) << rental.rentDate.toString()
                         << setw(15) << rental.returnDate.toString()
                         << setw(10) << rental.totalAmount
                         << setw(10) << "Active"
                         << " [Car: " << car->getFullName() << "]" << endl;
                }
                foundActive = true;
            }
        }
        
        if (!foundActive) {
            cout << "No cars are currently rented." << endl;
        }
    }
    
    void showRentalHistory() {
        updateCarAvailability();
        displayHeader("RENTAL HISTORY");
        
        if (rentals.empty()) {
            cout << "No rental history available." << endl;
            return;
        }
        
        vector<string> headers = {"Rental ID", "Customer", "Rent Date", "Return Date", "Amount", "Status"};
        vector<int> widths = {10, 25, 15, 15, 10, 10};
        displayTableHeader(headers, widths);
        
        for (const auto& rental : rentals) {
            Car* car = findCarById(rental.carId);
            if (car) {
                cout << left << setw(10) << rental.id
                     << setw(25) << rental.customerName
                     << setw(15) << rental.rentDate.toString()
                     << setw(15) << rental.returnDate.toString()
                     << setw(10) << rental.totalAmount
                     << setw(10) << (rental.isActive ? "Active" : "Returned")
                     << " [Car: " << car->getFullName() << "]" << endl;
            }
        }
    }
    
    void returnCar() {
        updateCarAvailability();
        displayHeader("RETURN A CAR");
        
        // Show active rentals
        bool hasActiveRentals = false;
        for (const auto& rental : rentals) {
            if (rental.isActive) {
                if (!hasActiveRentals) {
                    cout << "Active Rentals:" << endl;
                    cout << string(60, '-') << endl;
                    cout << left << setw(10) << "ID" 
                         << setw(25) << "Customer" 
                         << setw(20) << "Car" 
                         << setw(15) << "Return Date" << endl;
                    cout << string(60, '-') << endl;
                    hasActiveRentals = true;
                }
                
                Car* car = findCarById(rental.carId);
                if (car) {
                    cout << left << setw(10) << rental.id
                         << setw(25) << rental.customerName
                         << setw(20) << car->getFullName()
                         << setw(15) << rental.returnDate.toString() << endl;
                }
            }
        }
        
        if (!hasActiveRentals) {
            cout << "No active rentals to return." << endl;
            return;
        }
        
        int rentalId;
        cout << "\nEnter Rental ID to return: ";
        cin >> rentalId;
        clearInputBuffer();
        
        Rental* rental = findRentalById(rentalId);
        if (!rental) {
            cout << "Rental ID not found!" << endl;
            return;
        }
        
        if (!rental->isActive) {
            cout << "This car has already been returned." << endl;
            return;
        }
        
        Car* car = findCarById(rental->carId);
        if (!car) {
            cout << "Error: Car not found!" << endl;
            return;
        }
        
        // Check for late return
        Date actualReturn = getToday();
        int lateFee = 0;
        
        if (actualReturn > rental->returnDate) {
            int daysLate = actualReturn.differenceInDays(rental->returnDate);
            lateFee = daysLate * car->dailyRent * 1.5;
            
            cout << "\n" << string(50, '!') << endl;
            cout << "LATE RETURN DETECTED!" << endl;
            cout << string(50, '!') << endl;
            cout << "Scheduled Return: " << rental->returnDate.toString() << endl;
            cout << "Actual Return: " << actualReturn.toString() << endl;
            cout << "Days Late: " << daysLate << endl;
            cout << "Daily Rate: " << car->dailyRent << endl;
            cout << "Late Fee (150%): " << lateFee << endl;
            cout << "Original Amount: " << rental->totalAmount << endl;
            cout << "New Total: " << (rental->totalAmount + lateFee) << endl;
            cout << string(50, '!') << endl;
            
            char confirm;
            cout << "\nAccept late return charges? (y/n): ";
            cin >> confirm;
            clearInputBuffer();
            
            if (tolower(confirm) != 'y') {
                cout << "Return cancelled." << endl;
                return;
            }
        }
        
        // Process return
        if (lateFee > 0) {
            rental->totalAmount += lateFee;
        }
        
        rental->isActive = false;
        car->isAvailable = true;
        
        cout << "\nCar returned successfully!" << endl;
        cout << "Final amount: " << rental->totalAmount << endl;
        
        if (actualReturn < rental->returnDate) {
            cout << "Note: Early return. No refund for unused days." << endl;
        }
    }
    
    void exitSystem() {
        displayHeader("THANK YOU");
        cout << "Goodbye! Have a great day!" << endl <<"Contributors: Pukar Dhital & Pankaj Dahal"<< endl;
    }
};

void displayMainMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "        CAR RENTAL SYSTEM" << endl;
    cout << string(50, '=') << endl;
    cout << "1. Add New Car" << endl;
    cout << "2. View Available Cars" << endl;
    cout << "3. Rent a Car" << endl;
    cout << "4. View Rented Cars" << endl;
    cout << "5. View Rental History" << endl;
    cout << "6. Return a Car" << endl;
    cout << "7. Exit" << endl;
    cout << string(50, '-') << endl;
    cout << "Enter your choice (1-7): ";
}

int main() {
    CarRentalSystem system;
    int choice = 0;
    
    cout << "\n" << string(60, '*') << endl;
    cout << "      WELCOME TO CAR RENTAL MANAGEMENT SYSTEM" << endl;
    cout << string(60, '*') << endl;
    
    do {
        displayMainMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cout << "Invalid input! Please enter a number." << endl;
            clearInputBuffer();
            continue;
        }
        
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                system.addCar();
                break;
            case 2:
                system.showAvailableCars();
                break;
            case 3:
                system.rentCar();
                break;
            case 4:
                system.showRentedCars();
                break;
            case 5:
                system.showRentalHistory();
                break;
            case 6:
                system.returnCar();
                break;
            case 7:
                system.exitSystem();
                break;
            default:
                if(choice == 0){
                    continue;
                }else{
                    cout << "Invalid choice! Please enter 1-7." << endl;
                }

                
        }
        
    } while (choice != 7);
    
    return 0;
}