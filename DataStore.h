#pragma once
#include <vector>
#include <string>
#include "User.h"
#include "Car.h"
#include "Bike.h"
#include "RentalAgreement.h"

class DataStore {
private:
    std::vector<User>            users;
    std::vector<VehicleBase*>    vehicles;  
    std::vector<RentalAgreement> agreements;

    static const std::string DIR;

    std::vector<std::string> split(
        const std::string& line,
        char delim = '|'
    ) const;
    std::string nextId(const std::string& prefix) const;

    static VehicleBase* createVehicle(const std::string& type);

    void loadUsers();
    void loadTransactions();
    void loadVehicles();
    void loadAgreements();

    void saveUsers() const;
    void saveTransactions() const;
    void saveVehicles() const;
    void saveAgreements() const;

public:
    DataStore() = default;
    ~DataStore();

    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;

    void load();
    void save() const;

    User* findUserById(const std::string& id);
    User* findUserByUsername(const std::string& username);
    VehicleBase* findVehicleById(const std::string& id);
    RentalAgreement* findAgreementById(const std::string& id);

    // Returns true if the vehicle has no conflicting active/pending agreement.
    bool isVehicleAvailable(
        const std::string& vehicleId,
        const Date& start,
        const Date& end
    ) const;

    bool isVehicleCurrentlyRented(const std::string& vehicleId) const;

    // Returns listed vehicles not owned by excludeUserId,
    // optionally filtered by make/model/type substring.
    std::vector<VehicleBase*> getAvailableVehicles(
        const std::string& excludeUserId,
        const std::string& filter = ""
    ) const;

    std::vector<VehicleBase*> getVehiclesForOwner( const std::string& userId) const;
    std::vector<RentalAgreement*> getAgreementsAsRenter( const std::string& userId);
    std::vector<RentalAgreement*> getAgreementsAsOwner( const std::string& userId);

    // Returns the new user's ID, "" if username taken, "ERR_ID" if ID
    // number already registered.
    std::string registerUser(
        const std::string& username,
        const std::string& fullName,
        const std::string& password,
        const std::string& license,
        const std::string& idType,
        const std::string& idNumber
    );

    std::string addVehicle(
        const std::string& ownerId,
        const std::string& type,
        const std::string& make,
        const std::string& model,
        int year,
        const std::string& location,
        double rate,
        const std::string& regNum
    );

    std::string addAgreement(
        const std::string& renterId,
        const std::string& ownerId,
        const std::string& vehicleId,
        const Date& start,
        const Date& end,
        double rate
    );
};
