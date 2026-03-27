#pragma once
#include <string>

// Abstract base class for all people in the system.
// Stores identity and driving-licence information.
class Person {
private:
    std::string name;
    std::string licenseNumber;
    std::string idType;
    std::string idNumber;

public:
    Person(
        const std::string& name = "",
        const std::string& license = "",
        const std::string& idType = "",
        const std::string& idNumber = ""
    );

    virtual ~Person() {}
    virtual std::string getRole() const = 0;

    const std::string& getName() const;
    const std::string& getLicenseNumber() const;
    const std::string& getIdType() const;
    const std::string& getIdNumber() const;

    void setName(const std::string& v);
    void setLicenseNumber(const std::string& v);
    void setIdType(const std::string& v);
    void setIdNumber(const std::string& v);
};
