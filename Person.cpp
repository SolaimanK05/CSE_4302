#include "Person.h"

using namespace std;

Person::Person(
    const string& name,
    const string& license,
    const string& idType,
    const string& idNumber
) : name(name), licenseNumber(license), idType(idType), idNumber(idNumber) {}

const string& Person::getName() const { 
    return name;         
}
const string& Person::getLicenseNumber() const { 
    return licenseNumber; 
}
const string& Person::getIdType() const { 
    return idType;        
}
const string& Person::getIdNumber() const { 
    return idNumber; 
}

void Person::setName(const string& v)  { 
    name = v; 
}
void Person::setLicenseNumber(const string& v) { 
    licenseNumber = v; 
}
void Person::setIdType(const string& v) { 
    idType = v; 
}
void Person::setIdNumber(const string& v) { 
    idNumber = v; 
}
