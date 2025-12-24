P2P Car Rental Marketplace
Peer to peer (P2P) asset sharing is a very economically efficient model in which idle assets are utilized to generate revenue. With the increase in costs for purchasing and maintaining a vehicle—including insurance and taxes—it is becoming increasingly burdensome on middle to lower-class families to use private vehicles. Most private vehicles remain unused for the majority of the day. This creates an idle asset that incurs losses for the owner.

Our program acts as a Peer-to-Peer Car Rental Marketplace where car owners can list their vehicles for rent and users can rent them directly. We particularly aim to help tourists visiting a new city who wish to rent a vehicle for short-term travel (typically 1 to 7 days) without the legal hurdles of traditional rental agencies.

🖥️ Dashboard UI Design
1. Login/Register Screen
Plaintext

+============================================================+
|                                                            |
|          🚗  P2P VEHICLE RENTAL SYSTEM v1.0  🚗            |
|                                                            |
|       "Drive Anywhere, Earn Anytime. Secure & Direct."     |
|                                                            |
+============================================================+
|                                                            |
|    Please select an option to continue:                    |
|                                                            |
|    [1] LOGIN                                               |
|    [2] REGISTER NEW ACCOUNT                                |
|    [3] EXIT                                                |
|                                                            |
+============================================================+
> Enter Option: _
2. Main Menu
Plaintext

+============================================================+
|  DASHBOARD  |  User: Solaiman (S254)                       |
+============================================================+
|  💰 WALLET BALANCE: $520.00                                |
+------------------------------------------------------------+
|  🚦 STATUS SUMMARY                                         |
|                                                            |
|  [!] You have (2) items in AGREEMENTS needing attention.   |
|  [*] Active Rentals: 1 car (You are driving)               |
|  [*] Active Listings: 1 car (Earning income)               |
+------------------------------------------------------------+
|  ⚙️ MAIN MENU                                              |
|                                                            |
|  1. RENT A CAR             4. MANAGE WALLET                |
|  2. LIST A CAR             5. VIEW HISTORY                 |
|  3. MY ASSETS              6. LOGOUT                       |
|                                                            |
|  7. AGREEMENTS & ACTIONS  <-- [!] CHECK HERE               |
+============================================================+
> Enter Option (1-7): _
3. Rent a Car (Search & Listings)
Plaintext

+============================================================+
|  DASHBOARD > RENT A CAR                                    |
+============================================================+
|  🔎 SEARCH OPTIONS:                                        |
|  [1] Filter by Date Range                                  |
|  [2] Filter by Brand/Model                                 |
+------------------------------------------------------------+
|  🆕 NEWEST LISTINGS (Last 5)                               |
+------------------------------------------------------------+
|  ID    | Vehicle        | Rate/Day | Owner Rating | Status |
|  V019  | Tesla Model 3  | $90.00   | ★★★★☆        | AVAIL  |
|  V018  | Honda Accord   | $55.00   | ★★★★★        | AVAIL  |
|  V017  | Ford Focus     | $40.00   | ★★★☆☆        | AVAIL  |
|  V016  | Toyota Sienna  | $80.00   | New User     | AVAIL  |
|  V015  | BMW 3 Series   | $110.00  | ★★★★☆        | AVAIL  |
+------------------------------------------------------------+
|  [3] SELECT VEHICLE ID TO BOOK                             |
|  [4] RETURN TO DASHBOARD                                   |
+============================================================+
4. Booking Request (Renter Confirmation)
Plaintext

+------------------------------------------------------------+
|  DASHBOARD > RENT A CAR > BOOKING REQUEST: Tesla (V019)    |
+------------------------------------------------------------+
|  Enter Start Date (DD MM YYYY): 10 01 2026                 |
|  Enter End Date   (DD MM YYYY): 12 01 2026                 |
+------------------------------------------------------------+
|  Duration: 2 Days                                          |
|  Total Cost: $180.00                                       |
+------------------------------------------------------------+
|  NOTICE: Upon confirmation, funds will be held.            |
|  The Owner must approve this request within 24 hours.      |
+------------------------------------------------------------+
|  [1] AGREE & SEND REQUEST (Pay $180.00 Now)                |
|  [2] CANCEL                                                |
+------------------------------------------------------------+
5. Agreements & Actions (Request/Approval Flow)
Plaintext

+============================================================+
|  DASHBOARD > AGREEMENTS & ACTIONS                          |
+============================================================+
|  🚨 PENDING ACTIONS (REQUIRES YOUR INPUT)                  |
|                                                            |
|  A. INCOMING REQUESTS (You are the Owner):                 |
|     [R1] Request for V002 (Audi Q5) from User S254         |
|          -> Dates: 10/01 - 12/01 (Total: $240.00)          |
|          -> Renter Rating: 4.8/5.0                         |
|          [ ACTION: Type 'ACC R1' to Accept, 'REJ R1' to Deny ]
|                                                            |
|  B. PENDING RETURNS (Active Rentals):                      |
|     [R2] Rental A101: Toyota Prius (Due: Today)            |
|          -> ACTION: Mark as Returned                       |
+------------------------------------------------------------+
|  ⏳ YOUR SENT REQUESTS (STATUS CHECK)                      |
|                                                            |
|  1. Request for V009 (Ford): WAITING FOR OWNER APPROVAL    |
|  2. Request for V019 (Tesla): REJECTED (Refunded)          |
+------------------------------------------------------------+
|  📜 PAST AGREEMENTS & REVIEWS                              |
|                                                            |
|  [3] VIEW PAST AGREEMENTS (Write Reviews)                  |
+------------------------------------------------------------+
|  [4] RETURN TO DASHBOARD                                   |
+============================================================+
6. Wallet & Transaction History
Plaintext

+============================================================+
|  DASHBOARD > Wallet                                        |
+============================================================+
|  DATE       | TYPE        | DETAILS             | AMOUNT   |
+-------------+-------------+---------------------+----------+
|  12/12/2025 | Deposit     | Wallet Fund         | +$500.00 |
|  14/12/2025 | Payout      | Rental A105 (Net)   | +$120.00 |
|  15/12/2025 | Payment     | Booking B201        | -$150.00 |
|  16/12/2025 | Withdrawal  | Bank Transfer       | -$100.00 |
+------------------------------------------------------------+
|  [1] Add Balance                                           |
|  [2] Withdraw Balance                                      |
|  [3] EXPORT TO FILE                                        |
|  [4] RETURN TO DASHBOARD                                   |
+============================================================+
👥 Team Allocation & Responsibilities
Solaiman: System Integrator & User Lead
Classes: User

Responsibilities:

Implement User authentication and Wallet management logic.

Handle the high-level logic for Listing and Renting.

Lead the final system integration and main.cpp flow.

Shanto: Asset & Search Specialist
Classes: Vehicle

Responsibilities:

Implement Vehicle object properties and states.

Create the availability search algorithm and filtering logic.

Handle the display of listed vehicles in the UI.

Turjo: Transaction & Financial Officer
Classes: RentalAgreement, CustomDate

Responsibilities:

Implement the Rental Agreement state machine (Request -> Active -> Complete).

Handle date calculations, rental timers, and constraints.

Manage financial records and payout logic.

Ilham: UI/UX & Persistence Engineer
Classes: DataStore, ConsoleUI

Responsibilities:

Implement the DataStore for file saving/fetching (Persistence).

Build the ConsoleUI structure based on ASCII designs.

Handle input validation and edge-case testing.
