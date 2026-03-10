#pragma once
#include "VehicleBase.h"

class Car : public VehicleBase {
public:
    std::string getCategory() const override { return "Car"; }
};

class TwoSeaterCar : public Car {
public:
    double getBaseRate() const override { 
        return 25.0; 
    }
    std::string getVehicleType() const override { 
        return "2-Seater"; 
    }
    int getSeats() const override { 
        return 2;
    }
};

class FourSeaterCar : public Car {
public:
    double getBaseRate() const override { 
        return 45.0;
    }
    std::string getVehicleType() const override { 
        return "4-Seater"; 
    }
    int getSeats() const override { 
        return 4;
    }
};

class SixSeaterCar : public Car {
public:
    double getBaseRate() const override { 
        return 75.0;
    }
    std::string getVehicleType() const override { 
        return "6-Seater"; 
    }
    int getSeats() const override { 
        return 6;
    }
};
