#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include "User.h"
#include "Car.h"
#include "Bike.h"
#include "RentalAgreement.h"

using namespace std;
namespace fs = filesystem;

class DataStore {
    vector<User>            users;
    vector<VehicleBase*>    vehicles;    // raw pointers — we own these, freed in destructor
    
    vector<RentalAgreement> agreements;

    
    static inline const string DIR = "data/";

    vector<string> split(const string& line, char delim = '|') {
        vector<string> tokens;

        stringstream ss(line);
        string t;
        while (getline(ss, t, delim)) tokens.push_back(t);
        return tokens;
    }

    string nextId(const string& prefix) {
        int maxNum = 0;
        if      (prefix == "U") for (auto& u : users)      { try { maxNum = max(maxNum, stoi(u.getId().substr(1)));    } catch(...){} }
        
        else if (prefix == "V") for (auto  v : vehicles)   { try { maxNum = max(maxNum, stoi(v->getId().substr(1)));   } catch(...){} }
        else if (prefix == "A") for (auto& a : agreements) { try { maxNum = max(maxNum, stoi(a.getId().substr(1)));    } catch(...){} }
        
        return prefix + to_string(maxNum + 1);
    }

    // Factory — allocates the correct derived type on the heap and returns a raw pointer
    static VehicleBase* createVehicle(const string& type) {
        if (type == "2-Seater")   return new TwoSeaterCar();
        if (type == "6-Seater")   return new SixSeaterCar();
        if (type == "Motorcycle") return new Bike();
        
        return new FourSeaterCar();
    }

    void loadUsers() {
        ifstream f(DIR + "users.txt");
        string line;
        while (getline(f, line)) {
            auto t = split(line);
            if (t.size() < 7) continue;
            User u;
            u.setId(t[0]); u.setUsername(t[1]); u.setpassword(t[2]); u.setName(t[3]);
            try { u.setBalance(stod(t[4])); u.setRating(stof(t[5])); u.setRatingCount(stoi(t[6])); } catch(...) {}

            if (t.size() > 7) u.setLicenseNumber(t[7]);
            if (t.size() > 8) u.setIdType(t[8]);
            if (t.size() > 9) u.setIdNumber(t[9]);

            users.push_back(u);
        }
    }

    void loadTransactions() {
        ifstream f(DIR + "transactions.txt");
        string line;
        while (getline(f, line)) {

            auto t = split(line);
            if (t.size() < 7) continue;
            User* u = findUserById(t[0]);
            if (!u) continue;
            try {
                Date d(stoi(t[1]), stoi(t[2]), stoi(t[3]));
                u->addTransaction(Transaction(d, t[4], t[5], stod(t[6])));
            } catch(...) {}
        }
    }

    void loadVehicles() {
        ifstream f(DIR + "vehicles.txt");
        string line;
        while (getline(f, line)) {
            auto t = split(line);
            if (t.size() < 9) continue;
            string type = (t.size() > 9) ? t[9] : "4-Seater";

            VehicleBase* v = createVehicle(type);
            v->setId(t[0]); v->setOwnerId(t[1]); v->setMake(t[2]); v->setModel(t[3]);

            try { v->setYear(stoi(t[4])); } catch(...) { v->setYear(2024); }
            v->setLocation(t[5]);
            try { v->setDailyRate(stod(t[6])); } catch(...) {}
            v->setListed(t[7] == "1");
            v->setRegistrationNum(t[8]);

            vehicles.push_back(v);
        }
    }

    void loadAgreements() {
        ifstream f(DIR + "agreements.txt");
        string line;
        while (getline(f, line)) {
            auto t = split(line);
            if (t.size() < 17) continue;
            try {
                RentalAgreement a;
               
                a.setId(t[0]); a.setRenterId(t[1]); a.setOwnerId(t[2]); a.setVehicleId(t[3]);
                a.setStartDate(Date(stoi(t[4]), stoi(t[5]),  stoi(t[6])));
                a.setEndDate  (Date(stoi(t[7]), stoi(t[8]),  stoi(t[9])));
                a.setTotalCost(stod(t[10])); a.setPlatformFee(stod(t[11]));
                a.setStatus(RentalAgreement::statusFromStr(t[12]));
                
                a.setRenterReviewed(t[13]=="1"); a.setOwnerReviewed(t[14]=="1");
                a.setRatingForOwner(stoi(t[15])); a.setRatingForRenter(stoi(t[16]));

                agreements.push_back(a);
            } catch(...) {}
        }
    }

    void saveUsers() {
        ofstream f(DIR + "users.txt");
        f << fixed << setprecision(2);
        for (auto& u : users)
            f << u.getId()            << "|" << u.getUsername()    << "|" << u.getpassword() << "|"
              << u.getName()          << "|" << u.getBalance()     << "|"
              << u.getRating()        << "|" << u.getRatingCount() << "|"
              << u.getLicenseNumber() << "|" << u.getIdType()      << "|" << u.getIdNumber() << "\n";
    }

    void saveTransactions() {
        ofstream f(DIR + "transactions.txt");
        f << fixed << setprecision(2);
        for (auto& u : users)

            for (auto& tx : u.getTransactions())

                f << u.getId() << "|" << tx.date.getDay() << "|" << tx.date.getMonth() << "|"
                  << tx.date.getYear() << "|" << tx.type << "|" << tx.details << "|" << tx.amount << "\n";
    }

    void saveVehicles() {
        ofstream f(DIR + "vehicles.txt");

        f << fixed << setprecision(2);
        for (auto v : vehicles)
            f << v->getId()          << "|" << v->getOwnerId()        << "|" << v->getMake()    << "|"
              << v->getModel()       << "|" << v->getYear()           << "|" << v->getLocation()<< "|"
              << v->getDailyRate()   << "|" << (v->isListed()?"1":"0")<< "|"
              << v->getRegistrationNum() << "|" << v->getVehicleType() << "\n";
    }

    void saveAgreements() {
        ofstream f(DIR + "agreements.txt");
        f << fixed << setprecision(2);
        for (auto& a : agreements)
            f << a.getId()      << "|" << a.getRenterId()  << "|" << a.getOwnerId()  << "|" << a.getVehicleId() << "|"
              << a.getStartDate().getDay()   << "|" << a.getStartDate().getMonth() << "|" << a.getStartDate().getYear() << "|"
              << a.getEndDate().getDay()     << "|" << a.getEndDate().getMonth()   << "|" << a.getEndDate().getYear()   << "|"
              << a.getTotalCost() << "|" << a.getPlatformFee() << "|" << a.statusToString()        << "|"
              << (a.renterReviewed()?"1":"0") << "|" << (a.ownerReviewed()?"1":"0") << "|"
              << a.getRatingForOwner() << "|" << a.getRatingForRenter() << "\n";
    }

public:
    DataStore() = default;

    // Destructor frees every heap-allocated vehicle
    ~DataStore() {
        for (VehicleBase* v : vehicles) delete v;
    }

    // Prevent accidental copying — copying raw pointers without deep copy would cause double-free
    DataStore(const DataStore&)            = delete;
    DataStore& operator=(const DataStore&) = delete;

    void load() {
        fs::create_directory(DIR);
        loadUsers(); loadTransactions(); loadVehicles(); loadAgreements();
    }

    void save() {
        saveUsers(); saveTransactions(); saveVehicles(); saveAgreements();
    }

    User* findUserById(const string& id) {
        for (auto& u : users) if (u.getId() == id) return &u;
        return nullptr;
    }

    
    User* findUserByUsername(const string& uname) {
        for (auto& u : users) if (u.getUsername() == uname) return &u;
        return nullptr;
    }

   
    VehicleBase* findVehicleById(const string& id) {
        for (auto v : vehicles) if (v->getId() == id) return v;
        return nullptr;
    }

    RentalAgreement* findAgreementById(const string& id) {
        for (auto& a : agreements) if (a.getId() == id) return &a;
        return nullptr;
    }

    
    bool isVehicleAvailable(const string& vehicleId, const Date& start, const Date& end) {
        for (auto& a : agreements) {
            if (a.getVehicleId() != vehicleId) continue;
            auto s = a.getStatus();
            if (s==AgreementStatus::REJECTED || s==AgreementStatus::CANCELLED ||
                s==AgreementStatus::COMPLETED) continue;
            
            if (!(end <= a.getStartDate() || start >= a.getEndDate())) return false;
        }
        return true;
    }

    
    bool isVehicleCurrentlyRented(const string& vehicleId) {
        Date today = Date::today();
        for (auto& a : agreements) {
            if (a.getVehicleId() != vehicleId) continue;
            if (a.getStatus() != AgreementStatus::ACTIVE) continue;
            
            if (a.getStartDate() <= today && today <= a.getEndDate()) return true;
        }
        return false;
    }

    
    vector<VehicleBase*> getAvailableVehicles(const string& excludeUserId, const string& filter = "") {
        vector<VehicleBase*> result;
        string lf = filter;
        transform(lf.begin(), lf.end(), lf.begin(), ::tolower);
        for (auto v : vehicles) {
            if (!v->isListed() || v->getOwnerId() == excludeUserId) continue;
            if (!lf.empty()) {
                string lm = v->getMake(), lmo = v->getModel(), lt = v->getVehicleType();
                transform(lm.begin(),  lm.end(),  lm.begin(),  ::tolower);
                transform(lmo.begin(), lmo.end(), lmo.begin(), ::tolower);
                transform(lt.begin(),  lt.end(),  lt.begin(),  ::tolower);
                
                if (lm.find(lf)==string::npos && lmo.find(lf)==string::npos && lt.find(lf)==string::npos) continue;
            }
            result.push_back(v);
        }
        return result;
    }

    
    vector<VehicleBase*> getVehiclesForOwner(const string& uid) {
        vector<VehicleBase*> r;
        for (auto v : vehicles) if (v->getOwnerId() == uid) r.push_back(v);
        return r;
    }

    vector<RentalAgreement*> getAgreementsAsRenter(const string& uid) {
        vector<RentalAgreement*> r;
        for (auto& a : agreements) if (a.getRenterId() == uid) r.push_back(&a);
        return r;
    }

    
    vector<RentalAgreement*> getAgreementsAsOwner(const string& uid) {
        vector<RentalAgreement*> r;
        for (auto& a : agreements) if (a.getOwnerId() == uid) r.push_back(&a);
        return r;
    }

    string registerUser(const string& uname, const string& fullName, const string& pass,
                        const string& license, const string& idType, const string& idNum) {
        if (findUserByUsername(uname)) return "";
        string id = nextId("U");
        users.emplace_back(id, uname, fullName, pass, license, idType, idNum);
        save();
        return id;
    }

     string addVehicle(const string& ownerId, const string& type, const string& make,
                      const string& model, int year, const string& location,
                      double rate, const string& regNum) {
        string       id = nextId("V");
        VehicleBase*  v = createVehicle(type);
        v->setId(id); v->setOwnerId(ownerId); v->setMake(make); v->setModel(model);
        v->setYear(year); v->setLocation(location); v->setDailyRate(rate);
        v->setRegistrationNum(regNum);
        vehicles.push_back(v);
        save();
        return id;
    }

    
    string addAgreement(const string& renterId, const string& ownerId,
                        const string& vehicleId, const Date& start, const Date& end, double rate) {
        string id = nextId("A");
        agreements.emplace_back(id, renterId, ownerId, vehicleId, start, end, rate);
        save();
        return id;
    }


};
