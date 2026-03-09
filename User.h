#pragma once
#include "Person.h"
#include "Date.h"
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

struct Transaction {
    Date   date;
    string type;
    string details;
    double amount;
    Transaction(Date d, const string& t, const string& det, double a) : date(d), type(t), details(det), amount(a) {}
};

class User : public Person {
    string id, username, password;
    double balance;
    float  rating;
    int    ratingCount;
    vector<Transaction> transactions;


public:
    User() : Person(), balance(0.0), rating(0.0f), ratingCount(0) {}

    User(const string& id, const string& uname, const string& name, const string& pass,
         const string& license = "", const string& idType = "NID", const string& idNum = "")
        : Person(name, license, idType, idNum),
          id(id), username(uname), password(pass),
          balance(0.0), rating(0.0f), ratingCount(0) {}

    string getRole() const override { 
        return "Member"; 
    }

    bool authenticate(const string& pass) const { 
        return password == pass; 
    }

    bool deductBalance(double amount) {
        if (balance < amount) return false;
        balance -= amount;
        return true;
    }
    void creditBalance(double amount) { 
        balance += amount; 
    }

    void addTransaction(const Transaction& t) { 
        transactions.push_back(t); 
    }

    void addRating(int stars) {
        rating = (rating * ratingCount + stars) / float(ratingCount + 1);
        ratingCount++;
    }

    string getRatingDisplay() const {
        if (ratingCount == 0) return "New";
        ostringstream ss;
        ss << fixed << setprecision(1) << rating << "/5";
        return ss.str();
    }

    const string& getId() const { 
        return id;           
    }
    const string& getUsername()const { 
        return username;    
     }
    const string& getpassword() const { 
        return password; 
    }
    double getBalance() const { 
        return balance;      
    }
    float getRating() const { 
        return rating;       
    }
    int getRatingCount()const { 
        return ratingCount;  
    }
    const vector<Transaction>& getTransactions() const { 
        return transactions; 
    }

    void setId(const string& v) { 
        id = v;           
    }
    void setUsername(const string& v) { 
        username = v;     
    }
    void setpassword(const string& v) { 
        password = v; 
    }
    void setBalance(double v) { 
        balance = v;      
    }
    void setRating(float v) { 
        rating = v;       
    }
    void setRatingCount(int v) { 
        ratingCount = v;  
    }
};
