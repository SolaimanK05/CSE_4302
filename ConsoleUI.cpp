#include "ConsoleUI.h"
#include "DataStore.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace ftxui;
using namespace std;

static string generateUserID(int n) {
    stringstream ss;
    ss << "U" << setw(3) << setfill('0') << n;
    return ss.str();
}

static string generateVehicleID(int n) {
    stringstream ss;
    ss << "V" << setw(3) << setfill('0') << n;
    return ss.str();
}

static bool is_int(const string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    return true;
}

static bool is_double_like(const string& s) {
    if (s.empty()) return false;
    bool dot = false;
    for (char c : s) {
        if (c == '.') { if (dot) return false; dot = true; }
        else if (!isdigit((unsigned char)c)) return false;
    }
    return true;
}

void ConsoleUI::start() {
    auto screen = ScreenInteractive::TerminalOutput();
    DataStore store;

    vector<User> users = store.loadUsers();
    vector<VehicleRecord> vehicles = store.loadVehicles();
    vector<AgreementRecord> agreements = store.loadAgreements();

    string message = "";

    // Pages: 0 auth, 1 login, 2 register, 3 dashboard, 4 wallet, 5 list car, 6 my assets, 7 Agreements
    int page = 0;
    int current_user_index = -1;

    // ---------- AUTH ----------
    int auth_selected = 0;
    vector<string> auth_entries = {"Login", "Register", "Exit"};
    auto auth_menu = Menu(&auth_entries, &auth_selected);
    auto auth_container = Container::Vertical({ auth_menu });

    // ---------- LOGIN ----------
    string login_input;
    auto login_field = Input(&login_input, "Username");
    auto login_button = Button("Login", [&] {
        message.clear();
        if (DataStore::trim(login_input).empty()) {
            message = "Username cannot be empty.";
            return;
        }
        for (int i = 0; i < (int)users.size(); i++) {
            if (users[i].name == login_input) {
                current_user_index = i;
                page = 3;
                return;
            }
        }
        message = "User not found.";
    });
    auto login_container = Container::Vertical({ login_field, login_button });

    // ---------- REGISTER ----------
    string reg_input;
    auto reg_field = Input(&reg_input, "New Username");
    auto reg_button = Button("Register", [&] {
        message.clear();
        string name = DataStore::trim(reg_input);
        if (name.empty()) {
            message = "Username cannot be empty.";
            return;
        }
        for (auto& u : users) {
            if (u.name == name) {
                message = "Username already exists.";
                return;
            }
        }
        User u;
        u.id = generateUserID((int)users.size() + 1);
        u.name = name;
        u.balance = 0.0;

        users.push_back(u);
        store.saveUsers(users);

        message = "Registered successfully!";
        page = 0;
    });
    auto reg_container = Container::Vertical({ reg_field, reg_button });

    // ---------- DASHBOARD ----------
    int dash_selected = 0;
    vector<string> dash_entries = {
        "Rent a Car (placeholder)",
        "List a Car",
        "My Assets",
        "Manage Wallet",
        "View History (placeholder)",
        "Agreements",
        "Logout"
    };
    auto dash_menu = Menu(&dash_entries, &dash_selected);
    auto dash_container = Container::Vertical({ dash_menu });

    // ---------- WALLET ----------
    string amount_input;
    auto amount_field = Input(&amount_input, "Amount");
    int wallet_selected = 0;
    vector<string> wallet_entries = {"Add Balance", "Withdraw", "Back"};
    auto wallet_menu = Menu(&wallet_entries, &wallet_selected);
    auto wallet_container = Container::Vertical({ amount_field, wallet_menu });

    // ---------- LIST A CAR (YOUR JOB) ----------
    string car_make, car_model, car_year, car_location, car_rate;

    auto make_field = Input(&car_make, "Make (e.g., Toyota)");
    auto model_field = Input(&car_model, "Model (e.g., Corolla)");
    auto year_field = Input(&car_year, "Year (e.g., 2022)");
    auto loc_field  = Input(&car_location, "Location (e.g., Downtown)");
    auto rate_field = Input(&car_rate, "Rate per day (e.g., 45.00)");

    auto list_confirm = Button("Confirm Listing", [&] {
        message.clear();

        if (current_user_index < 0) { message = "Not logged in."; return; }

        string mk = DataStore::trim(car_make);
        string md = DataStore::trim(car_model);
        string yr = DataStore::trim(car_year);
        string lc = DataStore::trim(car_location);
        string rt = DataStore::trim(car_rate);

        if (mk.empty() || md.empty() || yr.empty() || lc.empty() || rt.empty()) {
            message = "All fields are required.";
            return;
        }
        if (!is_int(yr)) { message = "Year must be a number."; return; }
        if (!is_double_like(rt)) { message = "Rate must be numeric (e.g., 45 or 45.00)."; return; }

        int year = stoi(yr);
        double rate = stod(rt);

        if (year < 1980 || year > 2050) { message = "Year looks invalid."; return; }
        if (rate <= 0) { message = "Rate must be > 0."; return; }

        VehicleRecord v;
        v.vehicle_id = generateVehicleID((int)vehicles.size() + 1);
        v.owner_id = users[current_user_index].id;
        v.make = mk;
        v.model = md;
        v.year = year;
        v.location = lc;
        v.rate_per_day = rate;
        v.status = "AVAIL"; // placeholder; availability logic belongs to teammate

        vehicles.push_back(v);
        store.saveVehicles(vehicles);

        // Clear fields
        car_make.clear(); car_model.clear(); car_year.clear(); car_location.clear(); car_rate.clear();

        message = "Vehicle listed successfully!";
        page = 3;
    });

    auto list_cancel = Button("Cancel", [&] {
        message.clear();
        car_make.clear(); car_model.clear(); car_year.clear(); car_location.clear(); car_rate.clear();
        page = 3;
    });

    auto list_container = Container::Vertical({
        make_field, model_field, year_field, loc_field, rate_field,
        Container::Horizontal({ list_confirm, list_cancel })
    });

    // ---------- MY ASSETS (YOUR JOB) ----------
    auto assets_back = Button("Back to Dashboard", [&] {
        message.clear();
        page = 3;
    });
    auto assets_container = Container::Vertical({ assets_back });


    // ---------- AGREEMENTS ----------
    auto agreements_back = Button("Back to Dashboard", [&] {
    message.clear();
    page = 3;
    });
    auto agreements_container = Container::Vertical({ agreements_back });


    // ---------- TAB CONTAINER (focus works) ----------
    auto tab_container = Container::Tab({
        auth_container,   // 0
        login_container,  // 1
        reg_container,    // 2
        dash_container,   // 3
        wallet_container, // 4
        list_container,   // 5
        assets_container, //6
        agreements_container // 7
    }, &page);

    // ---------- RENDER ----------
    auto renderer = Renderer(tab_container, [&] {
        auto error_line = message.empty()
            ? emptyElement()
            : text(message) | color(Color::RedLight) | center;

        if (page == 0) {
            return vbox({
                text("P2P VEHICLE RENTAL SYSTEM") | bold | center,
                separator(),
                auth_menu->Render() | border,
                error_line
            });
        }

        if (page == 1) {
            return vbox({
                text("LOGIN") | bold | center,
                separator(),
                login_field->Render() | border,
                login_button->Render() | center,
                error_line,
                text("Esc = back") | dim | center
            }) | border;
        }

        if (page == 2) {
            return vbox({
                text("REGISTER") | bold | center,
                separator(),
                reg_field->Render() | border,
                reg_button->Render() | center,
                error_line,
                text("Esc = back") | dim | center
            }) | border;
        }

        if (page == 3) {
            auto& u = users[current_user_index];
            ostringstream bal; bal << fixed << setprecision(2) << u.balance;

            return vbox({
                hbox({ text("DASHBOARD") | bold, filler(), text("User: " + u.name + " (" + u.id + ")") }),
                text("Wallet Balance: $" + bal.str()),
                separator(),
                dash_menu->Render() | border,
                error_line,
                text("Enter to select • Esc = logout") | dim | center
            }) | border;
        }

        if (page == 4) {
            auto& u = users[current_user_index];
            ostringstream bal; bal << fixed << setprecision(2) << u.balance;

            return vbox({
                text("WALLET") | bold | center,
                text("Balance: $" + bal.str()) | center,
                separator(),
                amount_field->Render() | border,
                wallet_menu->Render() | border,
                error_line,
                text("Enter to confirm • Esc = back") | dim | center
            }) | border;
        }

        if (page == 5) {
            return vbox({
                text("LIST A CAR") | bold | center,
                separator(),
                make_field->Render() | border,
                model_field->Render() | border,
                year_field->Render() | border,
                loc_field->Render() | border,
                rate_field->Render() | border,
                hbox({ list_confirm->Render(), filler(), list_cancel->Render() }),
                error_line,
                text("Tab/Shift+Tab to switch fields • Esc = cancel") | dim | center
            }) | border;
        }

        // page == 6 : MY ASSETS
       // page == 6 : MY ASSETS
if (page == 6) {
    auto& u = users[current_user_index];
    vector<Element> rows;
    rows.push_back(hbox({
        text("ID") | bold, filler(),
        text("Vehicle") | bold, filler(),
        text("Year") | bold, filler(),
        text("Rate/Day") | bold, filler(),
        text("Location") | bold
    }));

    int count = 0;
    for (auto& v : vehicles) {
        if (v.owner_id != u.id) continue;
        count++;
        ostringstream rate; rate << fixed << setprecision(2) << v.rate_per_day;
        rows.push_back(hbox({
            text(v.vehicle_id), filler(),
            text(v.make + " " + v.model), filler(),
            text(to_string(v.year)), filler(),
            text("$" + rate.str()), filler(),
            text(v.location)
        }));
    }

    if (count == 0)
        rows.push_back(text("No vehicles listed yet.") | dim);

    return vbox({
        text("MY ASSETS") | bold | center,
        text("Owner: " + u.name + " (" + u.id + ")") | center,
        separator(),
        vbox(rows) | border,
        assets_back->Render() | center,
        error_line
    }) | border;
}


        // page == 7 : AGREEMENTS
if (page == 7) {
    auto& u = users[current_user_index];
    vector<Element> rows;

    rows.push_back(hbox({
        text("ID") | bold, filler(),
        text("Vehicle") | bold, filler(),
        text("Status") | bold, filler(),
        text("Cost") | bold
    }));

    int count = 0;
    for (auto& a : agreements) {
        if (a.owner_id != u.id && a.renter_id != u.id)
            continue;

        count++;
        rows.push_back(hbox({
            text(a.agreement_id), filler(),
            text(a.vehicle_id), filler(),
            text(a.status), filler(),
            text("$" + to_string((int)a.total_cost))
        }));
    }

    if (count == 0)
        rows.push_back(text("No agreements yet.") | dim);

    return vbox({
        text("AGREEMENTS & ACTIONS") | bold | center,
        separator(),
        vbox(rows) | border,
        agreements_back->Render() | center
    }) | border;
}
return text("Unknown page") | center;

    });

    // ---------- EVENTS ----------
    renderer = CatchEvent(renderer, [&](Event e) {
        // AUTH menu
        if (page == 0 && e == Event::Return) {
            message.clear();
            if (auth_selected == 0) page = 1;
            else if (auth_selected == 1) page = 2;
            else screen.ExitLoopClosure()();
            return true;
        }

        // DASHBOARD enter
        if (page == 3 && e == Event::Return) {
            message.clear();
            // 1 List a Car, 2 My Assets, 3 Wallet, 6 Logout
            if (dash_selected == 1) { page = 5; return true; }
            if (dash_selected == 2) { page = 6; return true; }
            if (dash_selected == 3) { page = 4; return true; }
            if (dash_selected == 5) { page = 7; return true; }
            if (dash_selected == 6) { page = 0; return true; }

            message = "Placeholder (will be connected later).";
            return true;
        }

        // WALLET enter
        if (page == 4 && e == Event::Return) {
            message.clear();
            string amt_s = DataStore::trim(amount_input);
            if (wallet_selected == 2) { page = 3; return true; }

            if (!is_double_like(amt_s)) { message = "Invalid amount."; return true; }
            double amt = stod(amt_s);
            if (amt <= 0) { message = "Amount must be > 0."; return true; }

            auto& u = users[current_user_index];
            if (wallet_selected == 0) {
                u.balance += amt;
                message = "Balance added.";
            } else if (wallet_selected == 1) {
                if (u.balance < amt) { message = "Insufficient balance."; return true; }
                u.balance -= amt;
                message = "Withdraw successful.";
            }
            store.saveUsers(users);
            amount_input.clear();
            return true;
        }

        // ESC behavior
        if (e == Event::Escape) {
            message.clear();
            if (page == 1 || page == 2) { page = 0; return true; }
            if (page == 4 || page == 5 || page == 6) { page = 3; return true; }
            if (page == 3) { page = 0; return true; } // logout on Esc from dashboard
            return false;
        }

        return false;
    });

    // Prevent dashboard access without login
    renderer = CatchEvent(renderer, [&](Event e) {
        if ((page == 3 || page == 4 || page == 5 || page == 6) && current_user_index < 0) {
            page = 0;
            message = "Please login first.";
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}
