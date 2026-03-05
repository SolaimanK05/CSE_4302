#pragma once
#include <string>
using namespace std;

class Person {
protected:
    string name;
    string licenseNumber;
    string idType; 
    string idNumber;

public:
    Person(const string& name = "", const string& license="", const string& idType="", const string& idNumber="")
    : name(name), licenseNumber(license), idType(idType), idNumber(idNumber) {}

    virtual ~Person(){}
    virtual string getRole() const = 0;

    const string& getName() const { 
        return name;          
    }
    const string& getLicenseNumber() const { 
        return licenseNumber; 
    }
    const string& getIdType() const { 
        return idType;        
    }
    const string& getIdNumber() const { 
        return idNumber;      
    }

    void setName(const string& v) { 
        name = v;          
    }
    void setLicenseNumber(const string& v) { 
        licenseNumber = v; 
    }
    void setIdType(const string& v) { 
        idType = v;        
    }
    void setIdNumber(const string& v) { 
        idNumber = v;
    }
};
