#include "DataStore.h"
#include <fstream>
#include <sstream>
#include <cctype>

using namespace std;

string DataStore::trim(const string& s) {
    int i = 0;
    int j = (int)s.size() - 1;
    while (i <= j && isspace((unsigned char)s[i])) i++;
    while (j >= i && isspace((unsigned char)s[j])) j--;
    if (i > j) return "";
    return s.substr(i, j - i + 1);
}

vector<User> DataStore::loadUsers(const string& file) {
    vector<User> users;
    ifstream fin(file);
    if (!fin.is_open()) return users;

    string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string id, name, bal;

        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, bal,  '|');

        User u;
        u.id = trim(id);
        u.name = trim(name);

        try { u.balance = stod(trim(bal)); }
        catch (...) { u.balance = 0.0; }

        if (!u.id.empty() && !u.name.empty())
            users.push_back(u);
    }
    return users;
}

bool DataStore::saveUsers(const vector<User>& users, const string& file) {
    ofstream fout(file, ios::trunc);
    if (!fout.is_open()) return false;

    for (auto& u : users) {
        fout << u.id << "|" << u.name << "|" << u.balance << "\n";
    }
    return true;
}

// vehicles.txt:
// V001|U001|Toyota|Corolla|2022|Downtown|45.00|AVAIL
vector<VehicleRecord> DataStore::loadVehicles(const string& file) {
    vector<VehicleRecord> vehicles;
    ifstream fin(file);
    if (!fin.is_open()) return vehicles;

    string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string vid, owner, make, model, year_s, loc, rate_s, status;

        getline(ss, vid,    '|');
        getline(ss, owner,  '|');
        getline(ss, make,   '|');
        getline(ss, model,  '|');
        getline(ss, year_s, '|');
        getline(ss, loc,    '|');
        getline(ss, rate_s, '|');
        getline(ss, status, '|');

        VehicleRecord v;
        v.vehicle_id = trim(vid);
        v.owner_id   = trim(owner);
        v.make       = trim(make);
        v.model      = trim(model);
        v.location   = trim(loc);
        v.status     = trim(status);

        try { v.year = stoi(trim(year_s)); }
        catch (...) { v.year = 0; }

        try { v.rate_per_day = stod(trim(rate_s)); }
        catch (...) { v.rate_per_day = 0.0; }

        if (!v.vehicle_id.empty() && !v.owner_id.empty())
            vehicles.push_back(v);
    }
    return vehicles;
}

bool DataStore::saveVehicles(const vector<VehicleRecord>& vehicles, const string& file) {
    ofstream fout(file, ios::trunc);
    if (!fout.is_open()) return false;

    for (auto& v : vehicles) {
        fout << v.vehicle_id << "|"
             << v.owner_id << "|"
             << v.make << "|"
             << v.model << "|"
             << v.year << "|"
             << v.location << "|"
             << v.rate_per_day << "|"
             << v.status << "\n";
    }
    return true;
}

// agreements.txt format:
// A001|V001|U001|U002|2026-01-10|2026-01-12|180.00|REQUESTED

vector<AgreementRecord> DataStore::loadAgreements(const string& file) {
    vector<AgreementRecord> agreements;
    ifstream fin(file);
    if (!fin.is_open()) return agreements;

    string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string aid, vid, oid, rid, sd, ed, cost_s, status;

        getline(ss, aid, '|');
        getline(ss, vid, '|');
        getline(ss, oid, '|');
        getline(ss, rid, '|');
        getline(ss, sd,  '|');
        getline(ss, ed,  '|');
        getline(ss, cost_s, '|');
        getline(ss, status, '|');

        AgreementRecord a;
        a.agreement_id = trim(aid);
        a.vehicle_id   = trim(vid);
        a.owner_id     = trim(oid);
        a.renter_id    = trim(rid);
        a.start_date   = trim(sd);
        a.end_date     = trim(ed);
        a.status       = trim(status);

        try { a.total_cost = stod(trim(cost_s)); }
        catch (...) { a.total_cost = 0.0; }

        if (!a.agreement_id.empty())
            agreements.push_back(a);
    }

    return agreements;
}

bool DataStore::saveAgreements(const vector<AgreementRecord>& agreements, const string& file) {
    ofstream fout(file, ios::trunc);
    if (!fout.is_open()) return false;

    for (auto& a : agreements) {
        fout << a.agreement_id << "|"
             << a.vehicle_id   << "|"
             << a.owner_id     << "|"
             << a.renter_id    << "|"
             << a.start_date   << "|"
             << a.end_date     << "|"
             << a.total_cost   << "|"
             << a.status       << "\n";
    }
    return true;
}
