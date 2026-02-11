#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <string>
#include <vector>

using namespace std;

struct User {
    string id;
    string name;
    double balance;
};

struct VehicleRecord {
    string vehicle_id;
    string owner_id;
    string make;
    string model;
    int year;
    string location;
    double rate_per_day;
    string status;
};

struct AgreementRecord {
    string agreement_id; // A001
    string vehicle_id;   // V001
    string owner_id;     // U001
    string renter_id;    // U002
    string start_date;   // 2026-01-10
    string end_date;     // 2026-01-12
    double total_cost;
    string status;       // REQUESTED / ACTIVE / COMPLETED
};

class DataStore {
public:
    vector<User> loadUsers(const string& file = "users.txt");
    bool saveUsers(const vector<User>& users, const string& file = "users.txt");

    vector<VehicleRecord> loadVehicles(const string& file = "vehicles.txt");
    bool saveVehicles(const vector<VehicleRecord>& vehicles, const string& file = "vehicles.txt");

    // NEW: Agreements
    vector<AgreementRecord> loadAgreements(const string& file = "agreements.txt");
    bool saveAgreements(const vector<AgreementRecord>& agreements, const string& file = "agreements.txt");

    static string trim(const string& s);
};

#endif
