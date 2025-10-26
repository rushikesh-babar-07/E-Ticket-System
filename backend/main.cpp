#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <cmath>
#include <cctype>
#include <chrono>
#include <random>
#include <fstream>
#include <filesystem>
using namespace std;

// -------------------------------------
// Helper structures
// -------------------------------------
struct City {
    double lat;
    double lon;
};

// -------------------------------------
// Global city dictionary
// -------------------------------------
unordered_map<string, City> cityMap = {
    {"mumbai", {19.0760, 72.8777}},
    {"pune", {18.5204, 73.8567}},
    {"nashik", {19.9975, 73.7898}},
    {"nagpur", {21.1458, 79.0882}},
    {"kolhapur", {16.7050, 74.2433}},
    {"satara", {17.6805, 74.0183}},
    {"sangli", {16.8524, 74.5815}},
    {"aurangabad", {19.8762, 75.3433}},
    {"solapur", {17.6599, 75.9064}},
    {"thane", {19.2183, 72.9781}},
    {"navi mumbai", {19.0330, 73.0297}},
    {"ahmednagar", {19.0948, 74.7480}},
    {"latur", {18.4088, 76.5604}},
    {"karad", {17.2890, 74.1811}},
    {"amravati", {20.9374, 77.7796}},
    {"jalgaon", {21.0077, 75.5626}},
    {"nanded", {19.1383, 77.3210}},
    {"miraj", {16.8226, 74.6473}}
};

// -------------------------------------
// Helper functions
// -------------------------------------
string toLowerTrim(string s) {
    // trim
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    if (start == string::npos) return "";
    s = s.substr(start, end - start + 1);
    // lower
    for (auto &c : s) c = tolower(c);
    return s;
}

bool isValidPhone(const string &phone) {
    if (phone.size() < 10 || phone.size() > 15) return false;
    for (char c : phone) if (!isdigit(c)) return false;
    return true;
}

bool findCityByName(const string &name, City &out) {
    string key = toLowerTrim(name);
    auto it = cityMap.find(key);
    if (it != cityMap.end()) {
        out = it->second;
        return true;
    }
    return false;
}

double deg2rad(double deg) { return deg * M_PI / 180.0; }

double haversineKm(const City &c1, const City &c2) {
    double R = 6371.0;
    double dLat = deg2rad(c2.lat - c1.lat);
    double dLon = deg2rad(c2.lon - c1.lon);
    double a = pow(sin(dLat / 2), 2) +
               cos(deg2rad(c1.lat)) * cos(deg2rad(c2.lat)) * pow(sin(dLon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return round(R * c * 100.0) / 100.0;
}

double computeFare(double distance) {
    double fare = 0;
    if (distance <= 10)
        fare = distance * 10;
    else if (distance <= 50)
        fare = (10 * 10) + (distance - 10) * 8;
    else
        fare = (10 * 10) + (40 * 8) + (distance - 50) * 5;
    return round(fare * 100.0) / 100.0;
}

string generateTicketId() {
    const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string id;
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 rng(seed);
    uniform_int_distribution<int> dist(0, chars.size() - 1);
    for (int i = 0; i < 10; ++i)
        id += chars[dist(rng)];
    return id;
}

// -------------------------------------
// Main flow functions
// -------------------------------------
void inputCustomer(string &name, int &age, string &contact) {
    do {
        cout << "Enter full name: ";
        getline(cin, name);
        name = toLowerTrim(name);
        if (name.empty()) cout << "Name cannot be empty.\n";
    } while (name.empty());

    do {
        cout << "Enter age (5-120): ";
        cin >> age;
        if (cin.fail() || age < 5 || age > 120) {
            cout << "Invalid age. Try again.\n";
            cin.clear();
            cin.ignore(1000, '\n');
        } else break;
    } while (true);

    cin.ignore();
    do {
        cout << "Enter contact number (10-15 digits): ";
        getline(cin, contact);
        if (!isValidPhone(contact)) cout << "Invalid contact number.\n";
    } while (!isValidPhone(contact));
}

void inputLocations(string &startCity, string &destCity, double &distance) {
    City c1, c2;
    while (true) {
        cout << "Enter starting city: ";
        getline(cin, startCity);
        if (!findCityByName(startCity, c1)) {
            cout << "Unknown city. Try again.\n";
            continue;
        }
        cout << "Enter destination city: ";
        getline(cin, destCity);
        if (!findCityByName(destCity, c2)) {
            cout << "Unknown city. Try again.\n";
            continue;
        }
        distance = haversineKm(c1, c2);
        break;
    }
}

bool simulatePayment(string &method) {
    string proceed;
    cout << fixed << setprecision(2);
    cout << "Proceed to payment? (yes/no): ";
    getline(cin, proceed);
    if (toLowerTrim(proceed) != "yes") {
        cout << "Payment canceled. Ticket not generated.\n";
        return false;
    }

    cout << "Select payment method:\n1) UPI\n2) Card\n3) NetBanking\nChoice: ";
    int choice;
    cin >> choice;
    cin.ignore();
    switch (choice) {
        case 1: method = "UPI"; break;
        case 2: method = "Card"; break;
        case 3: method = "NetBanking"; break;
        default: method = "Unknown";
    }

    cout << "Processing payment...\n";
    cout << "Payment successful!\n";
    return true;
}

void printTicket(const string &id, const string &name, int age, const string &contact,
                 const string &startCity, const string &destCity, double distance,
                 double fare, const string &method) {
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "\n================= BUS E-TICKET =================\n";
    cout << "Ticket ID: " << id << "\n";
    cout << "Passenger: " << name << " | Age: " << age << "\n";
    cout << "Contact: " << contact << "\n";
    cout << "From: " << startCity << " -> To: " << destCity << "\n";
    cout << "Distance: " << fixed << setprecision(2) << distance << " km\n";
    cout << "Fare: ₹" << fixed << setprecision(2) << fare << "\n";
    cout << "Payment Method: " << method << " | Status: PAID\n";
    cout << "Timestamp: " << ctime(&now);
    cout << "================================================\n";

    // Save to file
    filesystem::create_directories("tickets");
    ofstream fout("tickets/ticket_" + id + ".txt");
    fout << "TICKET ID: " << id << "\nPassenger: " << name << "\nAge: " << age
         << "\nContact: " << contact << "\nFrom: " << startCity
         << "\nTo: " << destCity << "\nDistance: " << distance
         << " km\nFare: ₹" << fare << "\nPayment: " << method
         << "\nStatus: PAID\nTime: " << ctime(&now);
    fout.close();
    cout << "(Ticket saved in tickets/ folder)\n";
}

int main() {
    string name, contact, startCity, destCity, method;
    int age;
    double distance;

    cout << "==== Online Bus E-Ticket System ====\n\n";
    inputCustomer(name, age, contact);
    inputLocations(startCity, destCity, distance);

    double fare = computeFare(distance);
    cout << "Calculated Distance: " << distance << " km\n";
    cout << "Total Fare: ₹" << fare << "\n";

    if (simulatePayment(method)) {
        string id = generateTicketId();
        printTicket(id, name, age, contact, startCity, destCity, distance, fare, method);
    }

    cout << "\nThank you for using the E-Ticket system!\n";
    return 0;
}
