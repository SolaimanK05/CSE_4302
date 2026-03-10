#include "RentalAgreement.h"
#include <algorithm>

using namespace std;

RentalAgreement::RentalAgreement()
    : totalCost(0), platformFee(0),
      status(AgreementStatus::PENDING),
      renterHasReviewed(false), ownerHasReviewed(false),
      ratingForOwner(0), ratingForRenter(0) {}

RentalAgreement::RentalAgreement(
    const string& id,
    const string& renterId,
    const string& ownerId,
    const string& vehicleId,
    const Date& start,
    const Date& end,
    double dailyRate
) : id(id), renterId(renterId), ownerId(ownerId), vehicleId(vehicleId),
    startDate(start), endDate(end),
    status(AgreementStatus::PENDING),
    renterHasReviewed(false), ownerHasReviewed(false),
    ratingForOwner(0), ratingForRenter(0)

{
    int days    = max(1, start.daysBetween(end));
    totalCost   = days * dailyRate;
    platformFee = totalCost * PLATFORM_FEE_RATE;
}

int    RentalAgreement::getDurationDays() const {
    return max(1, startDate.daysBetween(endDate));
}
double RentalAgreement::getOwnerPayout() const { 
    return totalCost - platformFee; 
}

string RentalAgreement::statusToString() const {
    switch (status) {
        case AgreementStatus::PENDING:   return "PENDING";
        case AgreementStatus::ACTIVE:    return "ACTIVE";
        case AgreementStatus::COMPLETED: return "COMPLETED";
        case AgreementStatus::REJECTED:  return "REJECTED";
        case AgreementStatus::CANCELLED: return "CANCELLED";
    }
    return "UNKNOWN";
}

AgreementStatus RentalAgreement::statusFromStr(const string& s) {
    if (s == "ACTIVE") return AgreementStatus::ACTIVE;
    if (s == "COMPLETED") return AgreementStatus::COMPLETED;
    if (s == "REJECTED") return AgreementStatus::REJECTED;
    if (s == "CANCELLED") return AgreementStatus::CANCELLED;
    return AgreementStatus::PENDING;
}

const string& RentalAgreement::getId() const { 
    return id;
}
const string& RentalAgreement::getRenterId() const { 
    return renterId;
}
const string& RentalAgreement::getOwnerId() const {
    return ownerId;
}
const string& RentalAgreement::getVehicleId() const { 
    return vehicleId;
}
const Date& RentalAgreement::getStartDate() const { 
    return startDate;
}
const Date& RentalAgreement::getEndDate() const {
    return endDate;         
 }
double RentalAgreement::getTotalCost() const { 
    return totalCost;       
}
double RentalAgreement::getPlatformFee() const { 
    return platformFee;      
}
AgreementStatus RentalAgreement::getStatus() const { 
    return status;           
}
bool RentalAgreement::renterReviewed() const { 
    return renterHasReviewed;
}
bool RentalAgreement::ownerReviewed() const { 
    return ownerHasReviewed; 
}
int RentalAgreement::getRatingForOwner() const { 
    return ratingForOwner;  
}
int RentalAgreement::getRatingForRenter() const { 
    return ratingForRenter;  
}


void RentalAgreement::setId(const string& v) { 
    id = v; 
}
void RentalAgreement::setRenterId(const string& v) { 
    renterId = v; 
}
void RentalAgreement::setOwnerId(const string& v) { 
    ownerId = v; 
}
void RentalAgreement::setVehicleId(const string& v){ 
    vehicleId = v; 
}
void RentalAgreement::setStartDate(const Date& d) { 
    startDate = d; 
}
void RentalAgreement::setEndDate(const Date& d) { 
    endDate = d; 
}
void RentalAgreement::setTotalCost(double v) { 
    totalCost  = v;
}
void RentalAgreement::setPlatformFee(double v) { 
    platformFee = v; 
}
void RentalAgreement::setStatus(AgreementStatus s) { 
    status = s; 
}
void RentalAgreement::setRenterReviewed(bool v) { 
    renterHasReviewed = v; 
}
void RentalAgreement::setOwnerReviewed(bool v) { 
    ownerHasReviewed  = v; 
}
void RentalAgreement::setRatingForOwner(int v) { 
    ratingForOwner    = v; 
}
void RentalAgreement::setRatingForRenter(int v) { 
    ratingForRenter   = v; 
}
