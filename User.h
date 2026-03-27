#pragma once
#include "Person.h"
#include "Date.h"
#include <string>
#include <vector>

// A single financial event tied to a user account.
struct Transaction {
    Date        date;
    std::string type;
    std::string details;
    double      amount;

    Transaction(
        Date d,
        const std::string& type,
        const std::string& details,
        double amount
    );
};

// Registered member who can both list and rent vehicles.
class User : public Person {
private:
    std::string id;
    std::string username;
    std::string password;
    double balance;
    float rating;
    int ratingCount;
    std::vector<Transaction> transactions;

public:
    User();
    User(
        const std::string& id,
        const std::string& username,
        const std::string& name,
        const std::string& password,
        const std::string& license  = "",
        const std::string& idType = "NID",
        const std::string& idNumber = ""
    );

    std::string getRole() const override;

    bool authenticate(const std::string& pass) const;
    bool deductBalance(double amount);
    void creditBalance(double amount);
    void addTransaction(const Transaction& t);
    void addRating(int stars);
    std::string getRatingDisplay() const;

    const std::string& getId() const;
    const std::string& getUsername()const;
    const std::string& getPassword() const;
    double getBalance() const;
    float getRating() const;
    int getRatingCount() const;
    const std::vector<Transaction>& getTransactions() const;

    void setId(const std::string& v);
    void setUsername(const std::string& v);
    void setPassword(const std::string& v);
    void setBalance(double v);
    void setRating(float v);
    void setRatingCount(int v);
};
