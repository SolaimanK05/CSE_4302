#include "DataStore.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

using namespace std;
namespace fs = filesystem;

const string DataStore::DIR = "data/";

// ─── Private helpers ──────────────────────────────────────────────────────────

vector<string> DataStore::split(const string& line, char delim) const {
    vector<string> tokens;
    stringstream ss(line);
    string t;
    while (getline(ss, t, delim)) tokens.push_back(t);
    return tokens;
}

string DataStore::nextId(const string& prefix) const {
    int maxNum = 0;
    if (prefix == "U") {
        for (auto& u : users) {
            try { maxNum = max(maxNum, stoi(u.getId().substr(1))); }
            catch (const invalid_argument&) {} 
        }
    } else if (prefix == "V") {
        for (auto v : vehicles) {
            try { maxNum = max(maxNum, stoi(v->getId().substr(1))); }
            catch (const invalid_argument&) {}
        }
    } else if (prefix == "A") {
        for (auto& a : agreements) {
            try { maxNum = max(maxNum, stoi(a.getId().substr(1))); }
            catch (const invalid_argument&) {}
        }
    }
    return prefix + to_string(maxNum + 1);
}

VehicleBase* DataStore::createVehicle(const string& type) {
    if (type == "2-Seater") return new TwoSeaterCar();
    if (type == "6-Seater") return new SixSeaterCar();
    if (type == "Motorcycle") return new Bike();
    return new FourSeaterCar();
}

// ─── Load ─────────────────────────────────────────────────────────────────────

void DataStore::loadUsers() {
    ifstream f(DIR + "users.txt");
    if (!f.is_open()) return; 
    string line;
    while (getline(f, line)) {
        auto t = split(line);
        if (t.size() < 7) {
            cerr << "[DataStore] Skipping malformed user line: " << line << "\n";
            continue;
        }
        User u;
        u.setId(t[0]); u.setUsername(t[1]); u.setPassword(t[2]); u.setName(t[3]);
        try {
            u.setBalance(stod(t[4]));
            u.setRating(stof(t[5]));
            u.setRatingCount(stoi(t[6]));
        } catch (const invalid_argument& e) {
            cerr << "[DataStore] Bad numeric field in user " << t[0]
                 << ": " << e.what() << "\n";
        }
        if (t.size() > 7) u.setLicenseNumber(t[7]);
        if (t.size() > 8) u.setIdType(t[8]);
        if (t.size() > 9) u.setIdNumber(t[9]);
        users.push_back(u);
    }
}

void DataStore::loadTransactions() {
    ifstream f(DIR + "transactions.txt");
    if (!f.is_open()) return;
    string line;
    while (getline(f, line)) {
        auto t = split(line);
        if (t.size() < 7) {
            cerr << "[DataStore] Skipping malformed transaction line: "
                 << line << "\n";
            continue;
        }
        User* u = findUserById(t[0]);
        if (!u) continue;
        try {
            Date d(stoi(t[1]), stoi(t[2]), stoi(t[3]));
            u->addTransaction(Transaction(d, t[4], t[5], stod(t[6])));
        } catch (const invalid_argument& e) {
            cerr << "[DataStore] Bad transaction for user " << t[0]
                 << ": " << e.what() << "\n";
        }
    }
}

void DataStore::loadVehicles() {
    ifstream f(DIR + "vehicles.txt");
    if (!f.is_open()) return;
    string line;
    while (getline(f, line)) {
        auto t = split(line);
        if (t.size() < 9) {
            cerr << "[DataStore] Skipping malformed vehicle line: "
                 << line << "\n";
            continue;
        }
        string type = (t.size() > 9) ? t[9] : "4-Seater";
        VehicleBase* v = createVehicle(type);
        v->setId(t[0]); v->setOwnerId(t[1]);
        v->setMake(t[2]); v->setModel(t[3]);
        try { v->setYear(stoi(t[4])); }
        catch (const invalid_argument& e) {
            cerr << "[DataStore] Bad year for vehicle " << t[0]
                 << ": " << e.what() << " — defaulting to 2024\n";
            v->setYear(2024);
        }
        v->setLocation(t[5]);
        try { v->setDailyRate(stod(t[6])); }
        catch (const invalid_argument& e) {
            cerr << "[DataStore] Bad rate for vehicle " << t[0]
                 << ": " << e.what() << "\n";
        }
        v->setListed(t[7] == "1");
        v->setRegistrationNum(t[8]);
        vehicles.push_back(v);
    }
}

void DataStore::loadAgreements() {
    ifstream f(DIR + "agreements.txt");
    if (!f.is_open()) return;
    string line;
    while (getline(f, line)) {
        auto t = split(line);
        if (t.size() < 17) {
            cerr << "[DataStore] Skipping malformed agreement line: "
                 << line << "\n";
            continue;
        }
        try {
            RentalAgreement a;
            a.setId(t[0]); a.setRenterId(t[1]);
            a.setOwnerId(t[2]); a.setVehicleId(t[3]);
            a.setStartDate(Date(stoi(t[4]),  stoi(t[5]),  stoi(t[6])));
            a.setEndDate  (Date(stoi(t[7]),  stoi(t[8]),  stoi(t[9])));
            a.setTotalCost(stod(t[10]));
            a.setPlatformFee(stod(t[11]));
            a.setStatus(RentalAgreement::statusFromStr(t[12]));
            a.setRenterReviewed(t[13] == "1");
            a.setOwnerReviewed (t[14] == "1");
            a.setRatingForOwner(stoi(t[15]));
            a.setRatingForRenter(stoi(t[16]));
            agreements.push_back(a);
        } catch (const invalid_argument& e) {
            cerr << "[DataStore] Bad agreement data: " << e.what() << "\n";
        }
    }
}

// ─── Save ─────────────────────────────────────────────────────────────────────

void DataStore::saveUsers() const {
    ofstream f(DIR + "users.txt");
    f << fixed << setprecision(2);
    for (auto& u : users) {
        f << u.getId()            << "|" << u.getUsername()    << "|"
          << u.getPassword()      << "|" << u.getName()        << "|"
          << u.getBalance()       << "|" << u.getRating()      << "|"
          << u.getRatingCount()   << "|" << u.getLicenseNumber()<< "|"
          << u.getIdType()        << "|" << u.getIdNumber()    << "\n";
    }
}

void DataStore::saveTransactions() const {
    ofstream f(DIR + "transactions.txt");
    f << fixed << setprecision(2);
    for (auto& u : users) {
        for (auto& tx : u.getTransactions()) {
            f << u.getId()             << "|"
              << tx.date.getDay()      << "|"
              << tx.date.getMonth()    << "|"
              << tx.date.getYear()     << "|"
              << tx.type              << "|"
              << tx.details           << "|"
              << tx.amount            << "\n";
        }
    }
}

void DataStore::saveVehicles() const {
    ofstream f(DIR + "vehicles.txt");
    f << fixed << setprecision(2);
    for (auto v : vehicles) {
        f << v->getId()              << "|" << v->getOwnerId()    << "|"
          << v->getMake()            << "|" << v->getModel()      << "|"
          << v->getYear()            << "|" << v->getLocation()   << "|"
          << v->getDailyRate()       << "|"
          << (v->isListed() ? "1" : "0") << "|"
          << v->getRegistrationNum() << "|" << v->getVehicleType()<< "\n";
    }
}

void DataStore::saveAgreements() const {
    ofstream f(DIR + "agreements.txt");
    f << fixed << setprecision(2);
    for (auto& a : agreements) {
        f << a.getId()      << "|" << a.getRenterId()  << "|"
          << a.getOwnerId() << "|" << a.getVehicleId() << "|"
          << a.getStartDate().getDay()   << "|"
          << a.getStartDate().getMonth() << "|"
          << a.getStartDate().getYear()  << "|"
          << a.getEndDate().getDay()     << "|"
          << a.getEndDate().getMonth()   << "|"
          << a.getEndDate().getYear()    << "|"
          << a.getTotalCost()   << "|" << a.getPlatformFee()  << "|"
          << a.statusToString() << "|"
          << (a.renterReviewed() ? "1" : "0") << "|"
          << (a.ownerReviewed()  ? "1" : "0") << "|"
          << a.getRatingForOwner() << "|" << a.getRatingForRenter() << "\n";
    }
}

// ─── Public interface ─────────────────────────────────────────────────────────

DataStore::~DataStore() {
    for (VehicleBase* v : vehicles) delete v;
}

void DataStore::load() {
    fs::create_directory(DIR);
    loadUsers();
    loadTransactions();
    loadVehicles();
    loadAgreements();
}

void DataStore::save() const {
    saveUsers();
    saveTransactions();
    saveVehicles();
    saveAgreements();
}

User* DataStore::findUserById(const string& id) {
    for (auto& u : users) if (u.getId() == id) return &u;
    return nullptr;
}

User* DataStore::findUserByUsername(const string& username) {
    for (auto& u : users) if (u.getUsername() == username) return &u;
    return nullptr;
}

VehicleBase* DataStore::findVehicleById(const string& id) {
    for (auto v : vehicles) if (v->getId() == id) return v;
    return nullptr;
}

RentalAgreement* DataStore::findAgreementById(const string& id) {
    for (auto& a : agreements) if (a.getId() == id) return &a;
    return nullptr;
}

bool DataStore::isVehicleAvailable(
    const string& vehicleId,
    const Date& start,
    const Date& end
) const {
    for (auto& a : agreements) {
        if (a.getVehicleId() != vehicleId) continue;
        auto s = a.getStatus();
        if (s == AgreementStatus::REJECTED  ||
            s == AgreementStatus::CANCELLED ||
            s == AgreementStatus::COMPLETED) continue;
        if (!(end <= a.getStartDate() || start >= a.getEndDate()))
            return false;
    }
    return true;
}

bool DataStore::isVehicleCurrentlyRented(const string& vehicleId) const {
    Date today = Date::today();
    for (auto& a : agreements) {
        if (a.getVehicleId() != vehicleId) continue;
        if (a.getStatus() != AgreementStatus::ACTIVE) continue;
        if (a.getStartDate() <= today && today <= a.getEndDate())
            return true;
    }
    return false;
}

vector<VehicleBase*> DataStore::getAvailableVehicles(
    const string& excludeUserId,
    const string& filter
) const {
    vector<VehicleBase*> result;
    string lf = filter;
    transform(lf.begin(), lf.end(), lf.begin(), ::tolower);

    for (auto v : vehicles) {
        if (!v->isListed() || v->getOwnerId() == excludeUserId) continue;
        if (!lf.empty()) {
            string lm  = v->getMake();
            string lmo = v->getModel();
            string lt  = v->getVehicleType();
            transform(lm.begin(),  lm.end(),  lm.begin(),  ::tolower);
            transform(lmo.begin(), lmo.end(), lmo.begin(), ::tolower);
            transform(lt.begin(),  lt.end(),  lt.begin(),  ::tolower);
            if (lm.find(lf)  == string::npos &&
                lmo.find(lf) == string::npos &&
                lt.find(lf)  == string::npos) continue;
        }
        result.push_back(v);
    }
    return result;
}

vector<VehicleBase*> DataStore::getVehiclesForOwner(
    const string& userId
) const {
    vector<VehicleBase*> r;
    for (auto v : vehicles) if (v->getOwnerId() == userId) r.push_back(v);
    return r;
}

vector<RentalAgreement*> DataStore::getAgreementsAsRenter(
    const string& userId
) {
    vector<RentalAgreement*> r;
    for (auto& a : agreements) if (a.getRenterId() == userId) r.push_back(&a);
    return r;
}

vector<RentalAgreement*> DataStore::getAgreementsAsOwner(
    const string& userId
) {
    vector<RentalAgreement*> r;
    for (auto& a : agreements) if (a.getOwnerId() == userId) r.push_back(&a);
    return r;
}

string DataStore::registerUser(
    const string& username,
    const string& fullName,
    const string& password,
    const string& license,
    const string& idType,
    const string& idNumber
) {
    if (findUserByUsername(username)) return "";
    if (!idNumber.empty()) {
        for (auto& u : users) {
            if (!u.getIdNumber().empty() && u.getIdNumber() == idNumber)
                return "ERR_ID";
        }
    }
    string id = nextId("U");
    users.emplace_back(id, username, fullName, password,
                       license, idType, idNumber);
    save();
    return id;
}

string DataStore::addVehicle(
    const string& ownerId,
    const string& type,
    const string& make,
    const string& model,
    int year,
    const string& location,
    double rate,
    const string& regNum
) {
    string       id = nextId("V");
    VehicleBase*  v = createVehicle(type);
    v->setId(id); v->setOwnerId(ownerId);
    v->setMake(make); v->setModel(model);
    v->setYear(year); v->setLocation(location);
    v->setDailyRate(rate); v->setRegistrationNum(regNum);
    vehicles.push_back(v);
    save();
    return id;
}

string DataStore::addAgreement(
    const string& renterId,
    const string& ownerId,
    const string& vehicleId,
    const Date& start,
    const Date& end,
    double rate
) {
    string id = nextId("A");
    agreements.emplace_back(
        id, renterId, ownerId, vehicleId, start, end, rate
    );
    save();
    return id;
}
