#pragma once
#include <string>
#include <algorithm>
#include "Date.h"

using namespace std;

enum class AgreementStatus { PENDING, ACTIVE, COMPLETED, REJECTED, CANCELLED };

class RentalAgreement {
private:
    string id, renterId, ownerId, vehicleId;
    Date   startDate, endDate;
    double totalCost, platformFee;
    AgreementStatus status;

    //dual rating system - each party rates the other after completion
    bool renterHasReviewed, ownerHasReviewed;
    int  ratingForOwner;   // stars the renter gives the owner
    int  ratingForRenter;  // stars the owner gives the renter

    static const double PLATFORM_FEE_RATE = 0.10; // 10% platform commission

public:
    RentalAgreement()
        : totalCost(0), platformFee(0), status(AgreementStatus::PENDING),
          renterHasReviewed(false), ownerHasReviewed(false),
          ratingForOwner(0), ratingForRenter(0) {}

    RentalAgreement(const string& id, const string& renterId, const string& ownerId,
                    const string& vehicleId, const Date& start, const Date& end, double dailyRate)
        : id(id), renterId(renterId), ownerId(ownerId), vehicleId(vehicleId),
          startDate(start), endDate(end), status(AgreementStatus::PENDING),
          renterHasReviewed(false), ownerHasReviewed(false),
          ratingForOwner(0), ratingForRenter(0) {
        int days = max(1, start.daysBetween(end));
        totalCost   = days * dailyRate;
        platformFee = totalCost * PLATFORM_FEE_RATE;
    }

    int getDurationDays()  const { 
        return max(1, startDate.daysBetween(endDate)); 
    }
    double getOwnerPayout() const { 
        return totalCost - platformFee; 
    }

    string statusToString() const {
        switch (status) {
            case AgreementStatus::PENDING:   return "PENDING";
            case AgreementStatus::ACTIVE:    return "ACTIVE";
            case AgreementStatus::COMPLETED: return "COMPLETED";
            case AgreementStatus::REJECTED:  return "REJECTED";
            case AgreementStatus::CANCELLED: return "CANCELLED";
        }
        return "UNKNOWN";
    }

    static AgreementStatus statusFromStr(const string& s) {
        if (s == "ACTIVE")    return AgreementStatus::ACTIVE;
        if (s == "COMPLETED") return AgreementStatus::COMPLETED;
        if (s == "REJECTED")  return AgreementStatus::REJECTED;
        if (s == "CANCELLED") return AgreementStatus::CANCELLED;
        return AgreementStatus::PENDING;
    }

    const string&   getId() const { 
        return id;               
    }
    const string&   getRenterId() const { 
        return renterId;         
    }
    const string&   getOwnerId() const { 
        return ownerId;          
    }
    const string&   getVehicleId() const { 
        return vehicleId;        
    }
    const Date&     getStartDate() const { 
        return startDate;        
    }
    const Date&     getEndDate() const { 
        return endDate;          
    }
    double          getTotalCost() const { 
        return totalCost;        
    }
    double          getPlatformFee() const { 
        return platformFee;      
    }
    AgreementStatus getStatus() const { 
        return status;           
    }
    bool renterReviewed() const { 
        return renterHasReviewed;
    }
    bool ownerReviewed() const { 
        return ownerHasReviewed; 
    }
    int getRatingForOwner()  const { 
        return ratingForOwner;   
    }
    int getRatingForRenter() const { 
        return ratingForRenter;  
    }
    void setId(const string& v) { 
        id = v;         
     }
    void setRenterId(const string& v) { 
        renterId = v;    
    }
    void setOwnerId(const string& v) { 
        ownerId = v;     
    }
    void setVehicleId(const string& v) { 
        vehicleId = v;   
    }
    void setStartDate(const Date& d) { 
        startDate = d;   
    }
    void setEndDate(const Date& d) { 
        endDate = d;     
    }
    void setTotalCost(double v) { 
        totalCost = v;   
    }
    void setPlatformFee(double v) { 
        platformFee = v; 
    }
    void setStatus(AgreementStatus s) { 
        status = s;      
    }
    void setRenterReviewed(bool v) { 
        renterHasReviewed = v; 
    }
    void setOwnerReviewed(bool v) { 
        ownerHasReviewed = v;  
    }
    void setRatingForOwner(int v) { 
        ratingForOwner = v;    
    }
    void setRatingForRenter(int v) { 
        ratingForRenter = v;   
    }
};
