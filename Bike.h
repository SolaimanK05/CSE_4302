#pragma once
#include "VehicleBase.h"

class Bike : public VehicleBase {
public:
    double getBaseRate() const override {
        return 12.0; 
    }
    std::string getVehicleType() const override { 
        return "Motorcycle";
    }
    int getSeats() const override { 
        return 1;
    }
    std::string getCategory() const override { 
        return "Bike";
    }
};
