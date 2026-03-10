#include "VehicleBase.h"

using namespace std;

VehicleBase::VehicleBase()
    : year(2024), dailyRate(0.0), listed(true) {}

double VehicleBase::getDailyRate() const {
    return dailyRate > 0.0 ? dailyRate : getBaseRate();
}

string VehicleBase::getDisplayName() const {
    return make + " " + model + " (" + to_string(year) + ")";
}

const string& VehicleBase::getId() const { 
    return id;
}
const string& VehicleBase::getOwnerId() const { 
    return ownerId;
}
const string& VehicleBase::getRegistrationNum() const { 
    return registrationNumber; 
}
const string& VehicleBase::getMake() const { 
    return make;
}
const string& VehicleBase::getModel() const { 
    return model;
}
const string& VehicleBase::getLocation() const { 
    return location;
}
int VehicleBase::getYear() const { 
    return year;
}
bool VehicleBase::isListed() const { 
    return listed;
}

void VehicleBase::setId(const string& v) { 
    id = v; 
}
void VehicleBase::setOwnerId(const string& v) { 
    ownerId = v; 
}
void VehicleBase::setRegistrationNum(const string& v) { 
    registrationNumber = v; 
}
void VehicleBase::setMake(const string& v) { 
    make = v; 
}
void VehicleBase::setModel(const string& v) { 
    model = v; 
}
void VehicleBase::setLocation(const string& v) { 
    location = v; 
}
void VehicleBase::setYear(int v) { 
    year = v; 
}
void VehicleBase::setDailyRate(double v) { 
    dailyRate = v; 
}
void VehicleBase::setListed(bool v) { 
    listed = v; 
}
