# Manual Test Cases — P2P Car Rental Marketplace

All tests are performed through the running application UI unless stated
otherwise. Load the seed data before each session by copying
`tests/data_seed/` to `data/` (see "How to Run" below).

---

## How to Run

### Standard UI tests (TC-R, TC-L, TC-V, TC-B, TC-W, TC-A)

**Windows (Command Prompt):**

```cmd
xcopy tests\data_seed\* data\ /Y
make run
```

**Mac / Linux:**

```bash
cp -r tests/data_seed/. data/
make run
```

Interact with the application as described in each test case.
Expected outcomes appear in the "Expected Result" column.

### Reset between tests

Running tests that log in and make bookings will modify `data/`.
Restore the seed before starting the next test:

**Windows:**

```cmd
xcopy tests\data_seed\* data\ /Y
```

**Mac / Linux:**

```bash
cp -r tests/data_seed/. data/
```

### Corrupt-data / error-handling tests (TC-D)

**Windows (Command Prompt):**

```cmd
xcopy tests\data_corrupt\* data\ /Y
make run 2> tests\stderr_output.txt
type tests\stderr_output.txt
```

**Mac / Linux:**

```bash
cp -r tests/data_corrupt/. data/
make run 2> tests/stderr_output.txt
cat tests/stderr_output.txt
```

### First-launch test (TC-D01)

**Windows (Command Prompt):**

```cmd
rmdir /S /Q data
make run
```

**Mac / Linux:**

```bash
rm -rf data/
make run
```

### Delete a single file (used in TC-D06, TC-D07)

**Windows:**

```cmd
del data\agreements.txt
del data\users.txt
```

**Mac / Linux:**

```bash
rm data/agreements.txt
rm data/users.txt
```

---

## Seed Users

These accounts are available after loading `tests/data_seed/`:

| Username | Password      | Balance | Notes                                                             |
| -------- | ------------- | ------- | ----------------------------------------------------------------- |
| `alice`  | `password123` | $500    | Owns V1 (Corolla, listed), V2 (Bike, listed), V4 (BMW, unlisted)  |
| `bob`    | `qwerty`      | $150    | Owns V3 (Tucson, listed). Has rented alice's Corolla (A1, ACTIVE) |
| `carol`  | `carol99`     | $0      | Has one completed rental (A2, COMPLETED). Has already rated.      |

---

## Registration (TC-R)

Load seed data before this section.

| ID     | Steps                                                                        | Input                                                                                                                            | Expected Result                                                |
| ------ | ---------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------- |
| TC-R01 | Go to Register screen. Fill all fields correctly. Click CREATE ACCOUNT.      | Username: `newuser` / Full name: `Test User` / License: `DL-9999` / NID: `5556667770` / Password: `pass123` / Confirm: `pass123` | Account created, redirected to Dashboard.                      |
| TC-R02 | Go to Register. Leave Full Name blank. Click CREATE ACCOUNT.                 | All fields filled except Full Name (empty).                                                                                      | Red notification: **"All fields are required."**               |
| TC-R03 | Fill all fields but enter different values in Password and Confirm Password. | Password: `abc123` / Confirm: `xyz999`                                                                                           | Red notification: **"Passwords do not match."**                |
| TC-R04 | Register with username `alice` (already in seed data).                       | Username: `alice` — all other fields unique.                                                                                     | Red notification: **"Username already taken."**                |
| TC-R05 | Register with NID `1234567890` (already used by alice in seed data).         | Username: `uniqueuser` / NID: `1234567890`                                                                                       | Red notification: **"NID/Passport already registered."**       |
| TC-R06 | Register with a non-numeric NID.                                             | NID: `ABCD1234`                                                                                                                  | Red notification: **"NID/Passport must contain digits only."** |
| TC-R07 | Register with Passport toggle selected, enter a non-numeric passport number. | Click "Switch: Passport". Passport number: `P-99Z`                                                                               | Red notification: **"NID/Passport must contain digits only."** |

---

## Login (TC-L)

Load seed data before this section.

| ID     | Steps                                                | Input                                       | Expected Result                                                              |
| ------ | ---------------------------------------------------- | ------------------------------------------- | ---------------------------------------------------------------------------- |
| TC-L01 | Enter valid credentials. Click LOGIN.                | Username: `alice` / Password: `password123` | Redirected to Dashboard. Top bar shows "Alice Rahman".                       |
| TC-L02 | Enter correct username, wrong password. Click LOGIN. | Username: `alice` / Password: `wrongpass`   | Red notification: **"Invalid username or password."** Stays on Login screen. |
| TC-L03 | Enter a username that does not exist.                | Username: `nobody` / Password: `anything`   | Red notification: **"Invalid username or password."**                        |

---

## Vehicle Listing (TC-V)

Load seed data. Log in as `bob` (password: `qwerty`).

| ID     | Steps                                                                     | Input                                                                                                    | Expected Result                                                                     |
| ------ | ------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------- |
| TC-V01 | Go to List a Car. Fill all fields. Select 4-Seater. Click LIST VEHICLE.   | Make: `Suzuki` / Model: `Swift` / Year: `2021` / Location: `Dhanmondi` / Rate: `40.00` / Reg: `DHK-7777` | Notification: **"Vehicle listed successfully!"** Vehicle appears in My Assets.      |
| TC-V02 | Go to List a Car. Leave Model blank. Click LIST VEHICLE.                  | Make: `Toyota` / Model: _(empty)_ / Year: `2021` / Location: `Dhaka` / Rate: `45` / Reg: `DHK-1111`      | Red notification: **"All fields are required."**                                    |
| TC-V03 | Enter a non-numeric year.                                                 | Year: `abcd` (all other fields valid)                                                                    | Red notification: **"Year and rate must be numbers."**                              |
| TC-V04 | Enter a year outside the allowed range.                                   | Year: `1990`                                                                                             | Red notification: **"Enter a valid year (2000-2030)."**                             |
| TC-V05 | Enter a daily rate below the minimum.                                     | Rate: `0.50`                                                                                             | Red notification: **"Rate must be $1 - $5000/day."**                                |
| TC-V06 | List a vehicle (run TC-V01 first). Go to My Assets and click Unlist.      | —                                                                                                        | Button changes to "Relist". Vehicle no longer visible to other users in Rent a Car. |
| TC-V07 | In My Assets, check V1 (Toyota Corolla — currently RENTED per seed data). | Log in as `alice`. Go to My Assets.                                                                      | Unlist button is hidden for V1 while it is rented.                                  |

---

## Booking (TC-B)

Load seed data. Log in as `carol` (password: `carol99`).
Carol starts with $0 — deposit $200 via Wallet before TC-B01.

| ID     | Steps                                                                                                     | Input                                                                        | Expected Result                                                                                     |
| ------ | --------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| TC-B01 | Rent a Car → find V3 (Hyundai Tucson, $80/day) → click Book. Enter valid future dates for a 2-day rental. | Start: today + 2 days / End: today + 4 days                                  | Notification: **"Booking request sent! Awaiting owner approval."** Carol's balance reduced by $160. |
| TC-B02 | Booking screen — enter letters instead of dates.                                                          | Day: `xx` / Month: `yy` / Year: `zzzz`                                       | Red notification: **"Enter valid dates."**                                                          |
| TC-B03 | Enter a start date in the past.                                                                           | Start day: `1` / month: `1` / year: `2025` / End: `5/1/2025`                 | Red notification: **"Start date must be today or later."**                                          |
| TC-B04 | Enter end date equal to start date.                                                                       | Start and End set to the same future date                                    | Red notification: **"End date must be after start."**                                               |
| TC-B05 | Enter a rental period longer than 30 days.                                                                | Start: `10/3/2026` / End: `10/4/2026` (31 days)                              | Red notification: **"Maximum rental duration is 30 days."**                                         |
| TC-B06 | Attempt to book when wallet balance is too low.                                                           | Log in as carol with $0 (do not deposit). Try to book any vehicle for 1 day. | Red notification: **"Insufficient wallet balance."**                                                |
| TC-B07 | Attempt to book V1 (Corolla) during its occupied dates from seed data.                                    | Start: `15` / `3` / `2026` — End: `18` / `3` / `2026`                        | Red notification: **"Vehicle unavailable for those dates."**                                        |
| TC-B08 | Check that an owner cannot book their own vehicles.                                                       | Log in as `alice`. Go to Rent a Car.                                         | Alice's own vehicles (V1, V2, V4) do not appear in the listing at all.                              |

---

## Wallet (TC-W)

Load seed data. Log in as any user.

| ID     | Steps                                                                  | Input                                     | Expected Result                                                                                             |
| ------ | ---------------------------------------------------------------------- | ----------------------------------------- | ----------------------------------------------------------------------------------------------------------- |
| TC-W01 | Wallet → Deposit Funds. Enter valid amount. Click DEPOSIT.             | Amount: `250`                             | Notification: **"Deposited $250.00 to wallet."** Balance increases by $250. Transaction appears in history. |
| TC-W02 | Deposit — enter text instead of a number.                              | Amount: `abc`                             | Red notification: **"Enter a valid amount."**                                                               |
| TC-W03 | Deposit — enter amount below minimum.                                  | Amount: `0`                               | Red notification: **"Amount must be $1 - $100,000."**                                                       |
| TC-W04 | Deposit — enter amount above maximum.                                  | Amount: `200000`                          | Red notification: **"Amount must be $1 - $100,000."**                                                       |
| TC-W05 | Wallet → Withdraw. Enter amount less than or equal to current balance. | Amount: `50` (ensure balance >= 50 first) | Notification: **"Withdrawal of $50.00 processed."** Balance decreases. Transaction logged.                  |
| TC-W06 | Withdraw an amount greater than current balance.                       | Amount: `999999`                          | Red notification: **"Invalid withdrawal amount."**                                                          |
| TC-W07 | Withdraw — enter a non-numeric value.                                  | Amount: `fifty`                           | Red notification: **"Enter a valid amount."**                                                               |

---

## Agreements & Reviews (TC-A)

Load seed data. Seed provides A1 (ACTIVE: bob rented alice's Corolla)
and A2 (COMPLETED: carol rented bob's Tucson, both parties already reviewed).

To get a fresh PENDING request for TC-A01 and TC-A02, log in as carol,
deposit $200, and book V3 (Tucson) for any available future dates first.

| ID     | Who                               | Steps                                                                       | Expected Result                                                                            |
| ------ | --------------------------------- | --------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| TC-A01 | Log in as `bob`.                  | Agreements → Incoming Requests. Find the PENDING request. Click **ACCEPT**. | Status changes to ACTIVE. Bob's balance credited with owner payout (90% of total).         |
| TC-A02 | Log in as `bob`.                  | Agreements → Incoming Requests. Click **REJECT** on a PENDING request.      | Status becomes REJECTED. Renter's balance refunded in full.                                |
| TC-A03 | Log in as `bob`.                  | Agreements → My Rentals. Find A1 (ACTIVE). Click **Return**.                | Status changes to COMPLETED.                                                               |
| TC-A04 | Log in as `carol`.                | Agreements → Completed. Find A2. Click a star rating (1–5) for the owner.   | Rating submitted. Owner's (bob's) rating updates. Star buttons replaced with "Rated: X/5". |
| TC-A05 | (Continue as carol after TC-A04.) | Go back to Agreements → Completed. Look at A2 again.                        | Star buttons are gone — shows "Rated: X/5". Cannot submit a second rating.                 |

---

## Data / Error Handling (TC-D)

These tests verify the application handles file and data problems gracefully
without crashing.

| ID                               | Setup                                                                                                        | Steps                                                  | Expected Result                                                                                                                                   |
| -------------------------------- | ------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| TC-D01 — Fresh install           | **Windows:** `rmdir /S /Q data` **Mac/Linux:** `rm -rf data/`                                                | Run the application.                                   | App launches normally. `data/` is created automatically. Login and Register both work. No crash.                                                  |
| TC-D02 — Malformed user lines    | Load corrupt data (see How to Run). Run app, capture stderr.                                                 | Inspect `stderr_output.txt`.                           | Log contains `[DataStore] Skipping malformed user line: GARBAGE_LINE_NO_DELIMITERS` and similar. Only valid users U1 and U4 are loaded. No crash. |
| TC-D03 — Bad numeric field       | (Same corrupt data.)                                                                                         | Log in as `gooduser` (password: `pass`). Check wallet. | Balance shown is $100.00. U2 (bad balance field) was skipped and logged to stderr. No crash.                                                      |
| TC-D04 — Bad vehicle year        | (Same corrupt data.)                                                                                         | Go to Rent a Car.                                      | V1 and V3 listed. V2 (bad year) defaults to 2024 with a warning in stderr. No crash.                                                              |
| TC-D05 — Short vehicle line      | (Same corrupt data.)                                                                                         | Inspect `stderr_output.txt`.                           | Log shows `[DataStore] Skipping malformed vehicle line: TOOSHORT\|LINE`.                                                                          |
| TC-D06 — Missing agreements file | Load seed data. **Windows:** `del data\agreements.txt` **Mac/Linux:** `rm data/agreements.txt` Then run app. | Log in as alice. Go to Agreements.                     | App loads with 0 agreements. All agreement tables show empty. No crash.                                                                           |
| TC-D07 — Missing users file      | Load seed data. **Windows:** `del data\users.txt` **Mac/Linux:** `rm data/users.txt` Then run app.           | Attempt to log in as alice.                            | Login fails (no users loaded). Register creates new users normally. No crash.                                                                     |
