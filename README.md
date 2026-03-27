# P2P Car Rental Marketplace — Developer Documentation

A peer-to-peer vehicle rental desktop application written in C++17, using
[raylib](https://www.raylib.com) for rendering. Users can list their own
vehicles for rent and book vehicles from other users — all from a single
account. Payments go through an in-app wallet, and both parties can rate each
other after a rental completes.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Architecture at a Glance](#2-architecture-at-a-glance)
3. [Class Reference](#3-class-reference)
   - [Date](#31-date)
   - [Person](#32-person)
   - [Transaction](#33-transaction)
   - [User](#34-user)
   - [VehicleBase, Car, Bike](#35-vehiclebase-car-bike)
   - [RentalAgreement](#36-rentalagreement)
   - [DataStore](#37-datastore)
   - [App & Theme](#38-app--theme)
4. [Data Storage Format](#4-data-storage-format)
5. [Key Flows](#5-key-flows)
6. [Build Instructions](#6-build-instructions)
7. [Naming & Coding Conventions](#7-naming--coding-conventions)
8. [Known Constraints](#8-known-constraints)

---

## 1. Project Overview

The application is a single-executable desktop program. There is no server,
no database, and no network layer — everything lives in plain-text files under
`data/` and in memory while the app is running. When the window closes, the
current state is flushed to disk.

**What a user can do:**

- Register with a full name, driving licence, and a government ID (NID or
  Passport). The ID number must be unique across all accounts.
- Deposit and withdraw funds from their wallet.
- List a vehicle (Motorcycle, 2-Seater, 4-Seater, or 6-Seater Car) at a
  daily rate they choose.
- Browse and book vehicles listed by other users for a date range.
- Accept or reject incoming booking requests on vehicles they own.
- Mark an active rental as returned; both parties can then leave a 1–5 star
  rating.

**Platform fee:** The system takes 10 % of every rental's total cost
(`PLATFORM_FEE_RATE = 0.10`). The vehicle owner receives the remaining 90 %.

---

## 2. Architecture at a Glance

```
main.cpp
  └─ App                    ← owns everything; drives the raylib loop
       ├─ Theme              ← all UI colours in one struct
       └─ DataStore          ← owns/persists all domain objects
            ├─ vector<User>
            ├─ vector<VehicleBase*>   ← heap-allocated; polymorphic
            └─ vector<RentalAgreement>
```

**Inheritance tree:**

```
Person  (abstract)
  └─ User

VehicleBase  (abstract)
  └─ Car  (abstract)
  │    ├─ TwoSeaterCar
  │    ├─ FourSeaterCar
  │    └─ SixSeaterCar
  └─ Bike
```

`App` is the only class that talks to `DataStore`. The UI layer (everything
in `App.cpp`) never directly touches the domain files; it calls `DataStore`
methods and lets the store decide what to persist.

---

## 3. Class Reference

### 3.1 Date

**Files:** `Date.h` / `Date.cpp`

A lightweight calendar date (day/month/year). Valid range is 2024–2035 — this
is enforced in `isValid()` and is intentional for the scope of this project.

Internally converts dates to a [Julian Day Number](https://en.wikipedia.org/wiki/Julian_day)
for arithmetic, which makes subtraction and comparison trivially correct across
month and year boundaries without any manual carry logic.

| Method | Description |
|---|---|
| `Date::today()` | Static; reads the system clock via `localtime`. |
| `isValid()` | Checks range, month bounds, and leap-year February. |
| `daysBetween(other)` | Returns `other - this` in days (can be negative). |
| `toString()` | Returns `"DD/MM/YYYY"`. |
| Operators `<`, `<=`, `>=`, `==` | All delegate to `toJulian()`. |

**Why Julian numbers?** Comparing two dates like `2025-02-28 < 2025-03-01`
would require checking month roll-overs manually. Converting both to a single
integer removes all that complexity.

---

### 3.2 Person

**Files:** `Person.h` / `Person.cpp`

Abstract base for anyone who has an account in the system. Holds identity
fields that any type of person would need.

```
Person
  ├── name            — full display name
  ├── licenseNumber   — driving licence
  ├── idType          — "NID" or "Passport"
  └── idNumber        — the actual ID value (must be unique at registration)
```

`getRole()` is a pure virtual method. Right now `User` is the only concrete
subclass and returns `"Member"`. The pure virtual is there so that a future
`Admin` or `Staff` subclass would be forced to declare its own role without
needing to touch `Person`.

---

### 3.3 Transaction

**File:** `User.h` (declared as a `struct`, defined in `User.cpp`)

A plain data-holder. It is a `struct` (not a class) because every field is
public — there is no behaviour to protect.

```cpp
struct Transaction {
    Date        date;
    std::string type;     // e.g. "Deposit", "Withdrawal", "Booking Payment"
    std::string details;  // human-readable description
    double      amount;   // positive = credit, negative = debit
};
```

Transactions are never deleted — they accumulate on the `User` and are
persisted to `transactions.txt`. The wallet screen renders them in reverse
order (most recent first) so the top of the list always shows the latest
activity.

---

### 3.4 User

**Files:** `User.h` / `User.cpp`

The only concrete `Person` subclass. A `User` is simultaneously a potential
vehicle owner and a renter — the system does not separate these roles into
different accounts.

**Fields on top of Person:**

```
User
  ├── id            — assigned by DataStore ("U1", "U2", …)
  ├── username      — unique login handle
  ├── password      — stored as plain text (no hashing — acceptable for this
  │                   academic scope, but never do this in production)
  ├── balance       — wallet balance in USD
  ├── rating        — running average (float, 0.0–5.0)
  ├── ratingCount   — number of ratings received
  └── transactions  — vector<Transaction>; append-only
```

**Balance methods:**

```cpp
bool deductBalance(double amount);   // returns false if insufficient funds
void creditBalance(double amount);   // always succeeds; no cap
```

`deductBalance` returns a `bool` instead of throwing — the caller (in
`App.cpp`) checks the return value and shows a notification to the user if
it fails. Throwing here would be an overreaction for something as routine as
"you don't have enough money."

**Rating system:**

```cpp
void addRating(int stars);
```

Uses a running average formula:

```
newAverage = (currentAverage × count + newStars) / (count + 1)
```

This avoids storing every individual rating while keeping the average accurate.
`getRatingDisplay()` returns `"New"` if no ratings exist yet — showing `0.0/5`
for a brand-new user would be misleading.

---

### 3.5 VehicleBase, Car, Bike

**Files:** `VehicleBase.h/.cpp`, `Car.h` (header-only), `Bike.h` (header-only)

`VehicleBase` is an abstract class that carries all the shared state. The four
pure virtuals define what each concrete vehicle type must report:

```cpp
virtual double      getBaseRate()    const = 0;  // default daily rate
virtual std::string getVehicleType() const = 0;  // "4-Seater", "Motorcycle", …
virtual int         getSeats()       const = 0;
virtual std::string getCategory()    const = 0;  // "Car" or "Bike"
```

**Base rates (hard-coded per type):**

| Type | Base Rate/day |
|---|---|
| Motorcycle | $12 |
| 2-Seater Car | $25 |
| 4-Seater Car | $45 |
| 6-Seater Car | $75 |

`getDailyRate()` returns the owner's custom rate if one was set, otherwise
falls back to `getBaseRate()`. This means an owner can price above or below
the default without the system breaking.

`VehicleBase` objects are **always heap-allocated** and owned by `DataStore`.
The `DataStore` destructor walks `vehicles` and `delete`s each pointer.
Copy and assignment of `DataStore` are deleted to prevent accidental double-free.

**Why separate `Car` and `Bike` from `VehicleBase`?** A future feature could
add behaviour specific to four-wheel vehicles (insurance type, number of doors)
without touching `Bike` or `VehicleBase`. The intermediate `Car` class acts as
that extension point.

---

### 3.6 RentalAgreement

**Files:** `RentalAgreement.h` / `RentalAgreement.cpp`

Represents a rental contract between a renter and a vehicle owner.

```
RentalAgreement
  ├── id, renterId, ownerId, vehicleId
  ├── startDate, endDate
  ├── totalCost       — days × dailyRate
  ├── platformFee     — totalCost × 0.10
  ├── status          — PENDING → ACTIVE → COMPLETED (or REJECTED/CANCELLED)
  └── review fields   — renterHasReviewed, ownerHasReviewed, ratings
```

**Status lifecycle:**

```
[created] → PENDING
               ├── owner accepts → ACTIVE
               ├── owner rejects → REJECTED
               └── renter cancels → CANCELLED
ACTIVE → renter marks returned → COMPLETED
COMPLETED → both parties can leave a 1–5 star rating (once each)
```

`getOwnerPayout()` = `totalCost - platformFee`. This is what gets credited to
the owner's wallet when the booking is accepted.

`PLATFORM_FEE_RATE` is a `static constexpr double` inside the class — it is
a compile-time constant that does not waste storage and cannot be changed at
runtime.

---

### 3.7 DataStore

**Files:** `DataStore.h` / `DataStore.cpp`

The single source of truth for all domain data. It reads from and writes to
four pipe-delimited text files. Everything else in the application goes through
`DataStore` to read or modify data.

**Responsibilities:**

- Load all four files on startup (`load()`).
- Flush all four files on any mutation (`save()` after every write operation).
- Generate sequential IDs (`nextId("U")` → `"U3"`, etc.).
- Perform availability checking for vehicles across date ranges.
- Own (and eventually free) all `VehicleBase*` pointers.

**Availability check logic (`isVehicleAvailable`):**

A vehicle is considered unavailable for `[start, end)` if there exists any
agreement for that vehicle that is not REJECTED/CANCELLED/COMPLETED **and**
whose date range overlaps with the requested range. Overlap is detected with:

```cpp
!(end <= a.getStartDate() || start >= a.getEndDate())
```

This is the standard interval-overlap check: two intervals do NOT overlap when
one ends before the other starts.

**Error handling on load:**

Malformed lines (fewer fields than expected, non-numeric where numeric is
required) are logged to `stderr` and skipped. The app continues with whatever
valid data remains. This is a deliberate design choice — a single corrupt line
in `users.txt` should not prevent the other 50 users from logging in.

---

### 3.8 App & Theme

**Files:** `App.h` / `App.cpp`

`App` is the top-level class. It is instantiated exactly once in `main()`.

`Theme` is a plain struct of `Color` values (raylib's `Color` type is 4 bytes:
R, G, B, A). All colours are initialised in the constructor using a slate-blue
palette. To retheme the entire app, you only need to change values in
`Theme::Theme()`.

**Screen enum:**

```cpp
enum class Screen {
    LOGIN, REGISTER, DASHBOARD, RENT_CAR,
    BOOKING, LIST_CAR, MY_ASSETS, AGREEMENTS, WALLET
};
```

Each enum value maps to one `draw*()` method. The `run()` loop switches on
`screen` every frame, calling the matching draw function. Transitioning between
screens calls `navigate(Screen s)`, which resets input buffers and resets
focused input — otherwise text typed on one screen could bleed into the next.

**Input system:**

There is a fixed-size array `char inputs[8][256]`. Each text field on any
screen is assigned a numeric ID (1–8). `focusedInput` tracks which field is
currently active. `handleTextInput()` reads `GetCharPressed()` from raylib
each frame and appends to the focused buffer. `Tab` advances to the next
field; `Escape` clears focus.

**Notification system:**

`showNotif(msg, isError)` sets a message, a flag, and a 3-second countdown
timer. `drawNotif()` renders a small toast-style banner at the top of the
screen if the timer is still running. Error notifications render in red;
success notifications in green.

---

## 4. Data Storage Format

All files live in `data/`, created automatically on first run. Fields are
separated by `|`. There is no quoting — field values must not contain `|`.

### users.txt

```
id | username | password | name | balance | rating | ratingCount | licenseNumber | idType | idNumber
```

Example:
```
U1|alice|password123|Alice Smith|500.00|4.5|2|DL-001|NID|1234567890
```

### transactions.txt

One row per transaction. `userId` links back to `users.txt`.

```
userId | day | month | year | type | details | amount
```

### vehicles.txt

```
id | ownerId | make | model | year | location | dailyRate | listed(0/1) | regNum | vehicleType
```

`vehicleType` is used by `DataStore::createVehicle()` to instantiate the right
subclass on load.

### agreements.txt

```
id | renterId | ownerId | vehicleId |
startDay | startMonth | startYear |
endDay | endMonth | endYear |
totalCost | platformFee | status |
renterReviewed(0/1) | ownerReviewed(0/1) |
ratingForOwner | ratingForRenter
```

---

## 5. Key Flows

### Registration

1. User fills in username, full name, password, licence, ID type, ID number.
2. `DataStore::registerUser()` checks that the username is not taken and that
   the ID number (if provided) is unique.
3. A new `User` is created with `id = nextId("U")` and appended to `users`.
4. `save()` is called immediately.

### Booking

1. The renter selects a vehicle and picks a date range.
2. `DataStore::isVehicleAvailable()` confirms there are no conflicting
   agreements.
3. `DataStore::addAgreement()` creates a new `RentalAgreement` with status
   `PENDING` and calls `save()`. No money moves at this point.
4. The vehicle owner sees the request in the Agreements → Pending tab and can
   accept or reject.
5. On **accept**: the renter's balance is debited `totalCost`; the owner's
   balance is credited `getOwnerPayout()`; both get transaction records;
   status becomes `ACTIVE`.
6. On **return**: the renter marks the vehicle as returned; status becomes
   `COMPLETED`.
7. Both parties can now rate each other (once). `User::addRating()` updates
   the running average.

### Wallet deposit / withdrawal

- Deposit: `creditBalance(amount)` is called; a `Transaction` record is
  appended; `save()` is called.
- Withdrawal: `deductBalance(amount)` is called first. If it returns `false`
  (insufficient funds), a notification is shown and nothing is saved.

---

## 6. Build Instructions

Requires [raylib](https://www.raylib.com) installed under
`C:/msys64/ucrt64` (MSYS2/UCRT64 on Windows).

```bash
make        # produces CarRental.exe
make run    # build and launch immediately
make clean  # remove the executable
```

On Linux/macOS, update the `RAYLIB_PATH` and library flags in `Makefile` to
match your local raylib installation.

**First run:** If `data/` does not exist, `DataStore::load()` creates it via
`std::filesystem::create_directory`. The app starts with no users and no
vehicles.

**Seeding test data:**

```bash
# Windows
xcopy tests\data_seed\* data\ /Y

# Linux / macOS
cp -r tests/data_seed/. data/
```

Seed accounts: `alice / password123`, `bob / qwerty`, `carol / carol99`.

---

## 7. Naming & Coding Conventions

| Element | Style | Example |
|---|---|---|
| Classes / Enums | PascalCase | `DataStore`, `AgreementStatus` |
| Methods / local variables | camelCase | `getRatingDisplay()`, `dailyRate` |
| Constants | UPPER_SNAKE_CASE | `PLATFORM_FEE_RATE`, `SIDEBAR_W` |
| Files | PascalCase matching class | `DataStore.h` / `DataStore.cpp` |

**Headers never use `using namespace std`.** All headers use the `std::` prefix
explicitly. `.cpp` files may use `using namespace std` for brevity.

**`const` correctness:** Every method that does not modify the object is marked
`const`. Large parameters (strings, `Date`) are passed as `const&` to avoid
copying.

**Header-only classes (`Car.h`, `Bike.h`):** Allowed only when the entire class
body is trivial one-liners returning compile-time constants. Anything with real
logic gets a `.cpp`.

---

## 8. Known Constraints

- **Passwords are stored in plaintext.** Acceptable for a university project;
  unacceptable for anything deployed to real users.
- **`|` cannot appear in any user-entered field.** There is no escaping in the
  file format. The UI does not validate for this; a pipe character in a name
  would corrupt the data file for that record.
- **Valid year range is 2024–2035.** Hard-coded in `Date::isValid()`. If the
  project were extended beyond 2035, this would need updating.
- **Copy/assignment of `DataStore` is deleted.** `DataStore` owns raw
  pointers; moving to `std::unique_ptr<VehicleBase>` would remove this
  restriction while keeping the same semantics.
- **No thread safety.** The entire application runs on a single thread.
  `DataStore` does not need a mutex.
- **No formal unit tests.** Test coverage is manual; see
  `tests/TEST_CASES.md`.
