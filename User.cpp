#include "User.h"
#include <sstream>
#include <iomanip>

using namespace std;

// ─── Transaction ─────────────────────────────────────────────────────────────

Transaction::Transaction(Date d, const string& type, const string& details, double amount) 
    : date(d), type(type), details(details), amount(amount) {}

// ─── User ─────────────────────────────────────────────────────────────────────

User::User()
    : Person(), balance(0.0), rating(0.0f), ratingCount(0) {}

User::User(
    const string& id,
    const string& username,
    const string& name,
    const string& password,
    const string& license,
    const string& idType,
    const string& idNumber
) : Person(name, license, idType, idNumber),
    id(id), username(username), password(password),
    balance(0.0), rating(0.0f), ratingCount(0) {}

string User::getRole() const { return "Member"; }

bool User::authenticate(const string& pass) const {
    return password == pass;
}

bool User::deductBalance(double amount) {
    if (balance < amount) return false;
    balance -= amount;
    return true;
}

void User::creditBalance(double amount) {
    balance += amount;
}

void User::addTransaction(const Transaction& t) {
    transactions.push_back(t);
}

void User::addRating(int stars) {
    rating = (rating * ratingCount + stars) / float(ratingCount + 1);
    ratingCount++;
}

string User::getRatingDisplay() const {
    if (ratingCount == 0) return "New";
    ostringstream ss;
    ss << fixed << setprecision(1) << rating << "/5";
    return ss.str();
}

const string& User::getId() const { 
    return id;
}
const string& User::getUsername() const { 
    return username;
}
const string& User::getPassword() const { 
    return password;
}
double User::getBalance()  const { 
    return balance;
}
float User::getRating() const { 
    return rating;
}
int User::getRatingCount() const { 
    return ratingCount;
}
const vector<Transaction>& User::getTransactions() const {
    return transactions;
}

void User::setId(const string& v) { 
    id = v; 
}
void User::setUsername(const string& v) { 
    username = v; 
}
void User::setPassword(const string& v) { 
    password = v; 
}
void User::setBalance(double v){ 
    balance = v;
}
void User::setRating(float v) { 
    rating  = v; 
}
void User::setRatingCount(int v) { 
    ratingCount = v; 
}
