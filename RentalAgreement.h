#pragma once
#include <string>
#include "Date.h"

enum class AgreementStatus {
    PENDING, ACTIVE, COMPLETED, REJECTED, CANCELLED
};

class RentalAgreement {
private:
    std::string     id;
    std::string     renterId;
    std::string     ownerId;
    std::string     vehicleId;
    Date            startDate;
    Date            endDate;
    double          totalCost;
    double          platformFee;
    AgreementStatus status;

    // Review
    bool renterHasReviewed;
    bool ownerHasReviewed;
    int  ratingForOwner;  
    int  ratingForRenter; 

    static constexpr double PLATFORM_FEE_RATE = 0.10; // fees

public:
    RentalAgreement();
    RentalAgreement(
        const std::string& id,
        const std::string& renterId,
        const std::string& ownerId,
        const std::string& vehicleId,
        const Date& start,
        const Date& end,
        double dailyRate
    );

    int getDurationDays()  const;
    double getOwnerPayout() const;

    std::string statusToString() const;
    static AgreementStatus statusFromStr(const std::string& s);

    const std::string& getId() const;
    const std::string& getRenterId() const;
    const std::string& getOwnerId() const;
    const std::string& getVehicleId() const;
    const Date& getStartDate() const;
    const Date& getEndDate() const;
    double getTotalCost() const;
    double getPlatformFee() const;
    AgreementStatus getStatus()  const;
    bool renterReviewed() const;
    bool ownerReviewed() const;
    int getRatingForOwner() const;
    int getRatingForRenter() const;

    void setId(const std::string& v);
    void setRenterId(const std::string& v);
    void setOwnerId(const std::string& v);
    void setVehicleId(const std::string& v);
    void setStartDate(const Date& d);
    void setEndDate(const Date& d);
    void setTotalCost(double v);
    void setPlatformFee(double v);
    void setStatus(AgreementStatus s);
    void setRenterReviewed(bool v);
    void setOwnerReviewed(bool v);
    void setRatingForOwner(int v);
    void setRatingForRenter(int v);
};
