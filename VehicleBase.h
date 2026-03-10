#pragma once
#include <string>

class VehicleBase {
protected:
    std::string id;
    std::string ownerId;
    std::string registrationNumber;
    std::string make;
    std::string model;
    std::string location;
    int         year;
    double      dailyRate;
    bool        listed;

public:
    VehicleBase();
    virtual ~VehicleBase() = default;

    // Pure-virtual type metadata — each derived class supplies these.
    virtual double getBaseRate() const = 0;
    virtual std::string getVehicleType() const = 0;
    virtual int         getSeats() const = 0;
    virtual std::string getCategory() const = 0;

    double getDailyRate() const;
    std::string getDisplayName() const;

    const std::string& getId() const;
    const std::string& getOwnerId() const;
    const std::string& getRegistrationNum() const;
    const std::string& getMake() const;
    const std::string& getModel() const;
    const std::string& getLocation() const;
    int getYear() const;
    bool isListed() const;

    void setId(const std::string& v);
    void setOwnerId(const std::string& v);
    void setRegistrationNum(const std::string& v);
    void setMake(const std::string& v);
    void setModel(const std::string& v);
    void setLocation(const std::string& v);
    void setYear(int v);
    void setDailyRate(double v);
    void setListed(bool v);
};
