#include "App.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

using namespace std;

// ─── Theme ────────────────────────────────────────────────────────────────────

Theme::Theme()
    // Slate-blue light theme — background layers (cool off-whites)
    : BG      ({241, 245, 249, 255})   // slate-100
    , PANEL   ({255, 255, 255, 255})   // pure white
    , HEADER  ({226, 232, 240, 255})   // slate-200
    , TOPBAR  ({255, 255, 255, 255})   // white topbar
    , INPUT   ({248, 250, 252, 255})   // slate-50
    , ROW_ALT ({241, 245, 249, 255})   // slate-100

    // Accent palette
    , ACCENT  ({ 59, 130, 246, 255})   // blue-500
    , SUCCESS ({ 16, 185, 129, 255})   // emerald-500
    , WARNING ({245, 158,  11, 255})   // amber-500
    , DANGER  ({239,  68,  68, 255})   // red-500

    // Text — dark navy for sharp contrast on white
    , TEXT    ({ 15,  23,  42, 255})   // slate-900
    , SUBTEXT ({100, 116, 139, 255})   // slate-500
    , BORDER  ({203, 213, 225, 255})   // slate-300
{}

// ─── Layout helpers ───────────────────────────────────────────────────────────

int App::contentX()  const { return SIDEBAR_W + PAD; }
int App::contentW()  const { return GetScreenWidth() - contentX() - PAD; }
int App::rightEdge() const { return GetScreenWidth() - PAD; }
int App::screenW()   const { return GetScreenWidth(); }
int App::screenH()   const { return GetScreenHeight(); }

// ─── Utility ──────────────────────────────────────────────────────────────────

Color App::lighten(Color c, int a) {
    return {
        (unsigned char)min(255, (int)c.r + a),
        (unsigned char)min(255, (int)c.g + a),
        (unsigned char)min(255, (int)c.b + a),
        255
    };
}

Color App::darken(Color c, int a) {
    return {
        (unsigned char)max(0, (int)c.r - a),
        (unsigned char)max(0, (int)c.g - a),
        (unsigned char)max(0, (int)c.b - a),
        255
    };
}

string App::fmtMoney(double v) {
    ostringstream ss;
    ss << "$" << fixed << setprecision(2) << v;
    return ss.str();
}

string App::ratingStr(float rating, int count) {
    if (count == 0) return "New User";
    ostringstream ss;
    ss << fixed << setprecision(1) << rating << "/5 (" << count << ")";
    return ss.str();
}

void App::navigate(Screen s) {
    screen         = s;
    focusedInput   = 0;
    agreementsTab  = 0;
    newVehicleType = "4-Seater";
    memset(inputs, 0, sizeof(inputs));
}

void App::showNotif(const string& msg, bool err) {
    notification = msg;
    notifError   = err;
    notifTimer   = 3.0f;
}

void App::handleTextInput() {
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if (focusedInput <= 0) return;

    char* buf = inputs[focusedInput - 1];
    int   len = (int)strlen(buf);

    int key;
    while ((key = GetCharPressed()) > 0
           && key >= 32 && key <= 126 && len < 127)
        buf[len++] = (char)key, buf[len] = '\0';

    if ((IsKeyPressed(KEY_BACKSPACE)
         || IsKeyPressedRepeat(KEY_BACKSPACE)) && len > 0)
        buf[--len] = '\0';

    if (IsKeyPressed(KEY_TAB))
        focusedInput = (focusedInput % MAX_INPUTS) + 1;

    if (IsKeyPressed(KEY_ESCAPE))
        focusedInput = 0;
}

// ─── UI Primitives ────────────────────────────────────────────────────────────

void App::drawPanel(int x, int y, int w, int h, const char* title) {
    DrawRectangleRounded(
        {(float)x, (float)y, (float)w, (float)h}, 0.02f, 8, theme.PANEL
    );
    DrawRectangleLinesEx(
        {(float)x, (float)y, (float)w, (float)h}, 1.0f, theme.BORDER
    );
    if (title) {
        DrawRectangle(x + 1, y + 1, w - 2, 44, theme.HEADER);
        DrawText(title, x + 14, y + 13, 18, theme.TEXT);
        DrawLine(x + 1, y + 45, x + w - 1, y + 45, theme.BORDER);
    }
}

bool App::drawBtn(int x, int y, int w, int h, const char* text, Color col) {
    if (col.r == 0 && col.g == 0 && col.b == 0 && col.a == 0)
        col = theme.ACCENT;
    Rectangle r = {(float)x, (float)y, (float)w, (float)h};
    bool hov = CheckCollisionPointRec(GetMousePosition(), r);
    bool clk = hov && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    DrawRectangleRounded(r, 0.18f, 8, hov ? darken(col, 18) : col);
    int fs = 17, tw = MeasureText(text, fs);
    DrawText(text, x + (w - tw) / 2, y + (h - fs) / 2, fs, WHITE);
    return clk;
}

bool App::drawSmallBtn(
    int x, int y, int w, int h, const char* text, Color col
) {
    if (col.r == 0 && col.g == 0 && col.b == 0 && col.a == 0)
        col = theme.ACCENT;
    Rectangle r = {(float)x, (float)y, (float)w, (float)h};
    bool hov = CheckCollisionPointRec(GetMousePosition(), r);
    bool clk = hov && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    DrawRectangleRounded(r, 0.25f, 8, hov ? darken(col, 18) : col);
    int fs = 14, tw = MeasureText(text, fs);
    DrawText(text, x + (w - tw) / 2, y + (h - fs) / 2, fs, WHITE);
    return clk;
}

void App::drawInput(
    int id, int x, int y, int w, int h,
    const char* ph, bool password
) {
    Rectangle r = {(float)x, (float)y, (float)w, (float)h};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
        && CheckCollisionPointRec(GetMousePosition(), r))
        focusedInput = id;

    bool   focused = (focusedInput == id);
    DrawRectangleRec(r, theme.INPUT);
    DrawRectangleLinesEx(
        r, focused ? 2.0f : 1.0f,
        focused ? theme.ACCENT : theme.BORDER
    );

    char*  buf  = inputs[id - 1];
    string disp = password
                  ? string(strlen(buf), '*')
                  : string(buf);

    if (disp.empty() && !focused && ph[0])
        DrawText(ph, x + 10, y + (h - 17) / 2, 17, theme.SUBTEXT);
    else
        DrawText(disp.c_str(), x + 10, y + (h - 17) / 2, 17, theme.TEXT);

    if (focused && (int)(GetTime() * 2) % 2 == 0) {
        int tw = min(MeasureText(disp.c_str(), 17), w - 20);
        DrawLine(x + 10 + tw + 1, y + 6, x + 10 + tw + 1, y + h - 6,
                 theme.ACCENT);
    }
}

void App::drawLabel(int x, int y, const char* text, Color col, int fontSize) {
    if (col.r == 0 && col.g == 0 && col.b == 0 && col.a == 0)
        col = theme.SUBTEXT;
    DrawText(text, x, y, fontSize, col);
}

void App::drawTableHeader(
    int y,
    const vector<pair<int, string>>& cols
) {
    DrawRectangle(contentX(), y, contentW(), 36, theme.HEADER);
    DrawLine(contentX(), y + 36, rightEdge(), y + 36, theme.BORDER);
    for (auto& [x, label] : cols)
        DrawText(label.c_str(), x, y + 10, 13, theme.SUBTEXT);
}

void App::drawTableRow(int y, int h, bool alt) {
    DrawRectangle(
        contentX(), y, contentW(), h,
        alt ? theme.ROW_ALT : theme.PANEL
    );
    DrawLine(contentX(), y, rightEdge(), y, theme.BORDER);
}

void App::drawTopBar() {
    DrawRectangle(0, 0, screenW(), TOP_H, theme.TOPBAR);
    DrawLine(0, TOP_H, screenW(), TOP_H, theme.BORDER);
    DrawText("P2P Car Rental", 20, 16, 22, theme.ACCENT);
    DrawText("F11 = Fullscreen", screenW() - 170, 20, 14, theme.SUBTEXT);

    User* u = store.findUserById(userId);
    if (u) {
        string info = u->getName() + "   Wallet: " + fmtMoney(u->getBalance());
        int tw = MeasureText(info.c_str(), 16);
        DrawText(info.c_str(), screenW() - tw - 180, 19, 16, theme.TEXT);
    }
}

void App::drawNotif() {
    if (notifTimer <= 0) return;
    notifTimer -= GetFrameTime();
    Color c  = notifError ? theme.DANGER : theme.SUCCESS;
    c.a      = (unsigned char)(255 * min(1.0f, notifTimer / 0.4f));
    int nw   = MeasureText(notification.c_str(), 16) + 36;
    int nx   = (screenW() - nw) / 2;
    DrawRectangleRounded(
        {(float)nx, (float)(TOP_H + 6), (float)nw, 36}, 0.4f, 8, c
    );
    Color tc = WHITE; tc.a = c.a;
    DrawText(notification.c_str(), nx + 18, TOP_H + 16, 16, tc);
}

void App::drawSidebarNav() {
    Color sidebarBg = {210, 218, 230, 255};   // slate-300
    DrawRectangle(0, TOP_H, SIDEBAR_W, screenH() - TOP_H, sidebarBg);
    DrawLine(SIDEBAR_W, TOP_H, SIDEBAR_W, screenH(), theme.BORDER);

    struct NavItem { const char* label; Screen s; };
    NavItem items[] = {
        {"Dashboard",  Screen::DASHBOARD},
        {"Rent a Car", Screen::RENT_CAR},
        {"List a Car", Screen::LIST_CAR},
        {"My Assets",  Screen::MY_ASSETS},
        {"Agreements", Screen::AGREEMENTS},
        {"Wallet",     Screen::WALLET},
    };

    int y = TOP_H + 14;
    for (auto& item : items) {
        bool active = (screen == item.s);
        Rectangle r = {8, (float)y, (float)(SIDEBAR_W - 16), 40};
        if (active) {
            DrawRectangleRounded(r, 0.2f, 8, theme.ACCENT);
        } else {
            bool hov = CheckCollisionPointRec(GetMousePosition(), r);
            if (hov) {
                Color hovCol = {185, 198, 216, 255};   // slate-400
                DrawRectangleRounded(r, 0.2f, 8, hovCol);
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    navigate(item.s);
            }
        }
        DrawText(item.label, 22, y + 12, 15, active ? WHITE : theme.TEXT);
        y += 52;
    }

    if (drawSmallBtn(8, screenH() - 54, SIDEBAR_W - 16, 38,
                     "Logout", theme.DANGER))
        navigate(Screen::LOGIN);
}

void App::drawVehicleTypeSelector(int x, int y) {
    struct TypeInfo { const char* type; const char* label;};
    TypeInfo types[] = {
        {"Motorcycle", "Motorcycle"},
        {"2-Seater",   "2-Seater"},
        {"4-Seater",   "4-Seater"},
        {"6-Seater",   "6-Seater"},
    };
    drawLabel(x, y, "VEHICLE TYPE", theme.SUBTEXT);
    int bx = x;
    for (auto& t : types) {
        bool   active = (newVehicleType == t.type);
        int    bw     = 155;
        string lbl    = string(t.label);
        Color col = active ? theme.ACCENT : theme.BORDER;
        if (drawBtn(bx, y + 18, bw, 40, lbl.c_str(), col))
            newVehicleType = t.type;
        bx += bw + 8;
    }
}

// ─── Business Actions ─────────────────────────────────────────────────────────

bool App::tryLogin() {
    User* u = store.findUserByUsername(inputs[0]);
    if (u && u->authenticate(inputs[1])) {
        userId = u->getId();
        return true;
    }
    return false;
}

bool App::tryRegister() {
    string uname   = inputs[0], fullname = inputs[1];
    string license = inputs[2], idNum    = inputs[3];
    string pass    = inputs[4], confirm  = inputs[5];
    string idType  = regIdNID ? "NID" : "Passport";

    if (uname.empty() || fullname.empty() || license.empty()
        || idNum.empty() || pass.empty()) {
        showNotif("All fields are required.", true);
        return false;
    }
    if (idNum.find_first_not_of("0123456789") != string::npos) {
        showNotif("NID/Passport must contain digits only.", true);
        return false;
    }
    if (pass != confirm) {
        showNotif("Passwords do not match.", true);
        return false;
    }

    string newId = store.registerUser(
        uname, fullname, pass, license, idType, idNum
    );
    if (newId.empty())     { showNotif("Username already taken.", true);          return false; }
    if (newId == "ERR_ID") { showNotif("NID/Passport already registered.", true); return false; }
    userId = newId;
    return true;
}

bool App::tryBooking() {
    VehicleBase* v = store.findVehicleById(selectedVehicleId);
    User*        u = store.findUserById(userId);
    if (!v || !u) { showNotif("Error finding vehicle.", true); return false; }

    int sd = 0, sm = 0, sy2 = 0, ed = 0, em = 0, ey2 = 0;
    if (sscanf(inputs[0], "%d", &sd)  != 1
        || sscanf(inputs[1], "%d", &sm)  != 1
        || sscanf(inputs[2], "%d", &sy2) != 1
        || sscanf(inputs[3], "%d", &ed)  != 1
        || sscanf(inputs[4], "%d", &em)  != 1
        || sscanf(inputs[5], "%d", &ey2) != 1) {
        showNotif("Enter valid dates.", true);
        return false;
    }

    Date start(sd, sm, sy2), end(ed, em, ey2);
    if (!start.isValid() || !end.isValid()) {
        showNotif("Invalid date.", true);                         return false;
    }
    if (!(Date::today() <= start)) {
        showNotif("Start date must be today or later.", true);    return false;
    }
    if (!(start < end)) {
        showNotif("End date must be after start.", true);         return false;
    }
    if (start.daysBetween(end) > 30) {
        showNotif("Maximum rental duration is 30 days.", true);   return false;
    }
    if (!store.isVehicleAvailable(selectedVehicleId, start, end)) {
        showNotif("Vehicle unavailable for those dates.", true);  return false;
    }

    double cost = start.daysBetween(end) * v->getDailyRate();
    if (u->getBalance() < cost) {
        showNotif("Insufficient wallet balance.", true);
        return false;
    }

    User* owner = store.findUserById(v->getOwnerId());
    if (!owner) { showNotif("Owner not found.", true); return false; }

    string aid = store.addAgreement(
        userId, owner->getId(), selectedVehicleId,
        start, end, v->getDailyRate()
    );
    u->deductBalance(cost);
    u->addTransaction(
        Transaction(Date::today(), "Payment", "Booking " + aid, -cost)
    );
    store.save();
    showNotif("Booking request sent! Awaiting owner approval.");
    return true;
}

bool App::tryListVehicle() {
    string make    = inputs[0], model   = inputs[1];
    string yr      = inputs[2], loc     = inputs[3];
    string rateStr = inputs[4], regNum  = inputs[5];

    if (make.empty() || model.empty() || yr.empty()
        || loc.empty() || rateStr.empty() || regNum.empty()) {
        showNotif("All fields are required.", true);
        return false;
    }
    int year = 0; double rate = 0;
    try { year = stoi(yr); rate = stod(rateStr); }
    catch (...) {
        showNotif("Year and rate must be numbers.", true);
        return false;
    }
    if (year < 2000 || year > 2030) {
        showNotif("Enter a valid year (2000-2030).", true);
        return false;
    }
    if (rate < 1.0 || rate > 5000.0) {
        showNotif("Rate must be $1 - $5000/day.", true);
        return false;
    }

    store.addVehicle(
        userId, newVehicleType, make, model,
        year, loc, rate, regNum
    );
    showNotif("Vehicle listed successfully!");
    return true;
}

bool App::tryDeposit() {
    double amount = 0;
    try { amount = stod(inputs[0]); }
    catch (...) { showNotif("Enter a valid amount.", true); return false; }

    if (amount < 1 || amount > 100000) {
        showNotif("Amount must be $1 - $100,000.", true);
        return false;
    }
    User* u = store.findUserById(userId);
    u->creditBalance(amount);
    u->addTransaction(
        Transaction(Date::today(), "Deposit", "Wallet Deposit", amount)
    );
    store.save();
    showNotif("Deposited " + fmtMoney(amount) + " to wallet.");
    return true;
}

bool App::tryWithdraw() {
    double amount = 0;
    try { amount = stod(inputs[0]); }
    catch (...) { showNotif("Enter a valid amount.", true); return false; }

    User* u = store.findUserById(userId);
    if (amount < 1 || amount > u->getBalance()) {
        showNotif("Invalid withdrawal amount.", true);
        return false;
    }
    u->deductBalance(amount);
    u->addTransaction(
        Transaction(Date::today(), "Withdrawal", "Bank Transfer", -amount)
    );
    store.save();
    showNotif("Withdrawal of " + fmtMoney(amount) + " processed.");
    return true;
}

// ─── Screens ──────────────────────────────────────────────────────────────────

void App::drawLogin() {
    int cx = screenW() / 2, cy = screenH() / 2;
    DrawText("P2P Car Rental Marketplace",
        cx - MeasureText("P2P Car Rental Marketplace", 26) / 2,
        cy - 210, 26, theme.ACCENT);
    DrawText("Drive Anywhere. Earn Anytime.",
        cx - MeasureText("Drive Anywhere. Earn Anytime.", 16) / 2,
        cy - 175, 16, theme.SUBTEXT);

    drawPanel(cx - 210, cy - 148, 420, 300, nullptr);

    drawLabel(cx - 190, cy - 125, "USERNAME", theme.SUBTEXT);
    drawInput(1, cx - 190, cy - 107, 380, 42, "Enter username");

    drawLabel(cx - 190, cy - 50, "PASSWORD", theme.SUBTEXT);
    drawInput(2, cx - 190, cy - 32, 380, 42, "Enter password", true);

    if (drawBtn(cx - 190, cy + 34, 183, 44, "LOGIN")) {
        if (!tryLogin()) showNotif("Invalid username or password.", true);
        else navigate(Screen::DASHBOARD);
    }
    if (drawBtn(cx + 7, cy + 34, 183, 44, "REGISTER", theme.BORDER))
        navigate(Screen::REGISTER);

    DrawText("F11 = Toggle Fullscreen",
        cx - MeasureText("F11 = Toggle Fullscreen", 14) / 2,
        cy + 100, 14, theme.SUBTEXT);
}

void App::drawRegister() {
    int cx = screenW() / 2, cy = screenH() / 2;
    DrawText("Create Account",
        cx - MeasureText("Create Account", 24) / 2,
        cy - 285, 24, theme.ACCENT);

    drawPanel(cx - 220, cy - 255, 440, 510, nullptr);
    int y = cy - 232;

    drawLabel(cx - 200, y, "USERNAME", theme.SUBTEXT);          y += 16;
    drawInput(1, cx - 200, y, 400, 40, "Choose a username");    y += 54;

    drawLabel(cx - 200, y, "FULL NAME", theme.SUBTEXT);         y += 16;
    drawInput(2, cx - 200, y, 400, 40, "Your full name");       y += 54;

    drawLabel(cx - 200, y, "DRIVING LICENSE NUMBER", theme.SUBTEXT); y += 16;
    drawInput(3, cx - 200, y, 400, 40, "e.g. DL-1234567");      y += 54;

    string idLabel = regIdNID ? "NATIONAL ID NUMBER" : "PASSPORT NUMBER";
    drawLabel(cx - 200, y, idLabel.c_str(), theme.SUBTEXT);     y += 16;
    drawInput(4, cx - 200, y, 280, 40,
              regIdNID ? "Enter NID number" : "Enter passport number");
    if (drawSmallBtn(cx + 92, y, 108, 40,
                     regIdNID ? "Switch: Passport" : "Switch: NID",
                     theme.SUBTEXT))
        regIdNID = !regIdNID;
    y += 54;

    drawLabel(cx - 200, y, "PASSWORD", theme.SUBTEXT);          y += 16;
    drawInput(5, cx - 200, y, 400, 40, "Create password", true); y += 54;

    drawLabel(cx - 200, y, "CONFIRM PASSWORD", theme.SUBTEXT);  y += 16;
    drawInput(6, cx - 200, y, 400, 40, "Repeat password", true); y += 52;

    if (drawBtn(cx - 200, y, 400, 44, "CREATE ACCOUNT", theme.SUCCESS))
        if (tryRegister()) navigate(Screen::DASHBOARD);

    if (drawSmallBtn(cx - 200, y + 58, 200, 32, "Back to Login", theme.BORDER))
        navigate(Screen::LOGIN);
}

void App::drawDashboard() {
    User* u = store.findUserById(userId);
    if (!u) return;
    drawTopBar(); drawSidebarNav();

    int y = TOP_H + 12;

    auto pending    = store.getAgreementsAsOwner(userId);
    auto myRentals  = store.getAgreementsAsRenter(userId);
    auto myVehicles = store.getVehiclesForOwner(userId);

    int pendingCount = 0, activeRentals = 0;
    for (auto* a : pending)
        if (a->getStatus() == AgreementStatus::PENDING) pendingCount++;
    for (auto* a : myRentals)
        if (a->getStatus() == AgreementStatus::ACTIVE)  activeRentals++;

    struct Stat { string label, value; Color col; };
    vector<Stat> stats = {
        {"WALLET BALANCE",  fmtMoney(u->getBalance()),      theme.SUCCESS},
        {"ACTIVE RENTALS",  to_string(activeRentals),        theme.ACCENT},
        {"YOUR LISTINGS",   to_string(myVehicles.size()),    theme.WARNING},
        {"PENDING ACTIONS", to_string(pendingCount),
            pendingCount > 0 ? theme.DANGER : theme.SUBTEXT},
    };
    int cardW = (contentW() - 9 * 3) / 4;
    for (int i = 0; i < 4; i++) {
        int cx = contentX() + i * (cardW + 9);
        drawPanel(cx, y, cardW, 86, nullptr);
        DrawText(stats[i].value.c_str(), cx + 14, y + 14, 28, stats[i].col);
        DrawText(stats[i].label.c_str(), cx + 14, y + 56, 12, theme.SUBTEXT);
    }
    y += 100;

    auto available = store.getAvailableVehicles(userId);
    int  tableH    = 50 + 36 + max((int)available.size(), 1) * 48 + 10;
    drawPanel(contentX(), y, contentW(), tableH, "Recent Available Vehicles");

    int c1 = contentX()+8,  c2 = contentX()+70,  c3 = contentX()+240;
    int c4 = contentX()+420, c5 = contentX()+530, c6 = contentX()+660;
    int actX = rightEdge() - 110;
    drawTableHeader(y + 50, {
        {c1,"ID"},{c2,"Type"},{c3,"Vehicle"},
        {c4,"Location"},{c5,"Rate/Day"},{c6,"Owner Rating"},{actX,""}
    });

    int count = min((int)available.size(), 6);
    for (int i = 0; i < count; i++) {
        VehicleBase* v = available[available.size() - 1 - i];
        User* o = store.findUserById(v->getOwnerId());
        int ry = y + 90 + i * 48;
        drawTableRow(ry, 48, i % 2 == 0);
        DrawText(v->getId().c_str(),                c1, ry+15, 14, theme.SUBTEXT);
        DrawText(v->getVehicleType().c_str(),        c2, ry+15, 14, theme.ACCENT);
        DrawText(v->getDisplayName().c_str(),        c3, ry+15, 14, theme.TEXT);
        DrawText(v->getLocation().c_str(),           c4, ry+15, 14, theme.TEXT);
        DrawText(fmtMoney(v->getDailyRate()).c_str(),c5, ry+15, 14, theme.SUCCESS);
        if (o) DrawText(
            ratingStr(o->getRating(), o->getRatingCount()).c_str(),
            c6, ry+15, 14, theme.WARNING
        );
        if (drawSmallBtn(actX, ry + 10, 90, 28, "Book Now")) {
            selectedVehicleId = v->getId();
            navigate(Screen::BOOKING);
        }
    }
    if (available.empty())
        DrawText("No vehicles available.",
            contentX() + (contentW() - 200) / 2, y + 110, 16, theme.SUBTEXT);

    if (pendingCount > 0) {
        int notifY = y + tableH + 10;
        drawPanel(contentX(), notifY, contentW(), 50, nullptr);
        string msg = "  You have " + to_string(pendingCount)
                     + " pending booking request(s) as owner. "
                       "Go to Agreements.";
        DrawText(msg.c_str(), contentX() + 14, notifY + 16, 15, theme.WARNING);
    }
}

void App::drawRentCar() {
    drawTopBar(); drawSidebarNav();
    int y = TOP_H + 12;

    drawPanel(contentX(), y, contentW(), 56, nullptr);
    drawLabel(contentX() + 14, y + 10, "FILTER", theme.SUBTEXT);
    drawInput(1, contentX() + 75, y + 8, contentW() - 250, 40,
              "Search by make, model or type...");
    if (drawSmallBtn(rightEdge() - 160, y + 8, 150, 40, "Search")) {}

    y += 68;
    string filter   = inputs[0];
    auto   vehicles = store.getAvailableVehicles(userId, filter);

    int tableH = 50 + 36 + max((int)vehicles.size(), 1) * 50 + 10;
    drawPanel(contentX(), y, contentW(), tableH, "Available Vehicles");

    int c1 = contentX()+8,  c2 = contentX()+68,  c3 = contentX()+148;
    int c4 = contentX()+350, c5 = contentX()+470, c6 = contentX()+580;
    int actX = rightEdge() - 100;
    drawTableHeader(y + 50, {
        {c1,"ID"},{c2,"Type"},{c3,"Vehicle"},
        {c4,"Location"},{c5,"Rate/Day"},{c6,"Owner Rating"},{actX,""}
    });

    for (int i = 0; i < (int)vehicles.size(); i++) {
        VehicleBase* v = vehicles[i];
        User* o = store.findUserById(v->getOwnerId());
        int ry = y + 90 + i * 50;
        drawTableRow(ry, 50, i % 2 == 0);
        DrawText(v->getId().c_str(),                 c1, ry+16, 14, theme.SUBTEXT);
        DrawText(v->getVehicleType().c_str(),         c2, ry+16, 14, theme.ACCENT);
        DrawText(v->getDisplayName().c_str(),         c3, ry+16, 14, theme.TEXT);
        DrawText(v->getLocation().c_str(),            c4, ry+16, 14, theme.TEXT);
        DrawText(fmtMoney(v->getDailyRate()).c_str(),  c5, ry+16, 14, theme.SUCCESS);
        if (o) DrawText(
            ratingStr(o->getRating(), o->getRatingCount()).c_str(),
            c6, ry+16, 14, theme.WARNING
        );
        if (drawSmallBtn(actX, ry + 12, 86, 26, "Book >>")) {
            selectedVehicleId = v->getId();
            navigate(Screen::BOOKING);
        }
    }
    if (vehicles.empty())
        DrawText("No vehicles match your search.",
            contentX() + (contentW() - 280) / 2, y + 110, 16, theme.SUBTEXT);
}

void App::drawBooking() {
    VehicleBase* v = store.findVehicleById(selectedVehicleId);
    if (!v) { navigate(Screen::RENT_CAR); return; }
    drawTopBar(); drawSidebarNav();

    User* owner = store.findUserById(v->getOwnerId());
    int y = TOP_H + 12;
    drawPanel(contentX(), y, contentW(), 520,
              ("Booking: " + v->getDisplayName()).c_str());

    int lx = contentX() + 14, ly = y + 58;

    // Lambda to draw a labelled info row
    auto row = [&](const char* label, const string& val,
                   Color c = {0, 0, 0, 0}) {
        if (c.r == 0 && c.g == 0 && c.b == 0 && c.a == 0) c = theme.TEXT;
        drawLabel(lx, ly, label, theme.SUBTEXT);
        DrawText(val.c_str(), lx + 160, ly, 15, c);
        ly += 28;
    };

    row("Vehicle ID:",  v->getId());
    row("Type:",        v->getVehicleType()
                        + "  (" + to_string(v->getSeats()) + " seats)",
                        theme.ACCENT);
    row("Category:",    v->getCategory());
    row("Location:",    v->getLocation());
    row("Daily Rate:",  fmtMoney(v->getDailyRate()), theme.SUCCESS);
    row("Base Rate:",
        fmtMoney(v->getBaseRate()) + "/day (class default)", theme.SUBTEXT);
    if (owner) {
        row("Owner:",        owner->getName());
        row("Owner Rating:", ratingStr(owner->getRating(),
                                       owner->getRatingCount()), theme.WARNING);
    }

    ly += 8;
    DrawLine(lx, ly, rightEdge() - 14, ly, theme.BORDER);
    ly += 14;

    drawLabel(lx, ly, "START DATE", theme.SUBTEXT); ly += 16;
    DrawText("Day",   lx,       ly, 13, theme.SUBTEXT);
    DrawText("Month", lx + 78,  ly, 13, theme.SUBTEXT);
    DrawText("Year",  lx + 160, ly, 13, theme.SUBTEXT); ly += 16;
    drawInput(1, lx,       ly, 68, 40, "DD");
    drawInput(2, lx + 78,  ly, 72, 40, "MM");
    drawInput(3, lx + 160, ly, 90, 40, "YYYY"); ly += 54;

    drawLabel(lx, ly, "END DATE", theme.SUBTEXT); ly += 16;
    DrawText("Day",   lx,       ly, 13, theme.SUBTEXT);
    DrawText("Month", lx + 78,  ly, 13, theme.SUBTEXT);
    DrawText("Year",  lx + 160, ly, 13, theme.SUBTEXT); ly += 16;
    drawInput(4, lx,       ly, 68, 40, "DD");
    drawInput(5, lx + 78,  ly, 72, 40, "MM");
    drawInput(6, lx + 160, ly, 90, 40, "YYYY"); ly += 52;

    int sd = 0, sm = 0, sy2 = 0, ed = 0, em = 0, ey2 = 0;
    bool ok = sscanf(inputs[0], "%d", &sd)  == 1
           && sscanf(inputs[1], "%d", &sm)  == 1
           && sscanf(inputs[2], "%d", &sy2) == 1
           && sscanf(inputs[3], "%d", &ed)  == 1
           && sscanf(inputs[4], "%d", &em)  == 1
           && sscanf(inputs[5], "%d", &ey2) == 1;
    if (ok) {
        Date s(sd, sm, sy2), e(ed, em, ey2);
        if (s.isValid() && e.isValid() && s < e) {
            int    days    = s.daysBetween(e);
            double cost    = days * v->getDailyRate();
            string preview = to_string(days) + " day(s)   Total: "
                             + fmtMoney(cost);
            DrawText(preview.c_str(), lx, ly, 16, theme.ACCENT);
            ly += 30;
        }
    }

    DrawText("Funds are held on request. Owner must approve within 24 hours.",
             lx, ly, 14, theme.SUBTEXT);
    ly += 36;

    if (drawBtn(lx, ly, 220, 44, "CONFIRM REQUEST", theme.SUCCESS))
        if (tryBooking()) navigate(Screen::AGREEMENTS);
    if (drawBtn(lx + 232, ly, 120, 44, "BACK", theme.BORDER))
        navigate(Screen::RENT_CAR);
}

void App::drawListCar() {
    drawTopBar(); drawSidebarNav();
    int y = TOP_H + 12;
    drawPanel(contentX(), y, contentW(), 490, "List Your Vehicle");

    int lx = contentX() + 14;
    drawVehicleTypeSelector(lx, y + 56);

    int fy = y + 120;
    struct Field { const char* label; int id; const char* ph; };
    Field fields[] = {
        {"MAKE",                1, "e.g. Toyota"},
        {"MODEL",               2, "e.g. Corolla"},
        {"YEAR",                3, "e.g. 2022"},
        {"LOCATION",            4, "e.g. Downtown"},
        {"DAILY RATE ($)",      5, "e.g. 45.00"},
        {"REGISTRATION NUMBER", 6, "e.g. DHK-1234"},
    };
    int fw = min(contentW() - 28, 500);
    for (auto& f : fields) {
        drawLabel(lx, fy, f.label, theme.SUBTEXT); fy += 16;
        drawInput(f.id, lx, fy, fw, 40, f.ph);    fy += 54;
    }

    if (drawBtn(lx, fy, 220, 44, "LIST VEHICLE", theme.SUCCESS))
        if (tryListVehicle()) navigate(Screen::MY_ASSETS);
    if (drawBtn(lx + 232, fy, 120, 44, "CANCEL", theme.BORDER))
        navigate(Screen::DASHBOARD);
}

void App::drawMyAssets() {
    drawTopBar(); drawSidebarNav();
    int y = TOP_H + 12;
    auto myVehicles = store.getVehiclesForOwner(userId);

    int tableH = 50 + 36 + max((int)myVehicles.size(), 1) * 54 + 10;
    drawPanel(contentX(), y, contentW(), tableH, "My Listed Vehicles");

    int c1 = contentX()+8,  c2 = contentX()+68,  c3 = contentX()+148;
    int c4 = contentX()+330, c5 = contentX()+460;
    int c6 = contentX()+560, c7 = contentX()+670;
    int actX = rightEdge() - 110;
    drawTableHeader(y + 50, {
        {c1,"ID"},{c2,"Type"},{c3,"Vehicle"},{c4,"Reg #"},
        {c5,"Location"},{c6,"Rate/Day"},{c7,"Status"},{actX,""}
    });

    for (int i = 0; i < (int)myVehicles.size(); i++) {
        VehicleBase* v      = myVehicles[i];
        bool         rented = store.isVehicleCurrentlyRented(v->getId());
        int ry = y + 90 + i * 54;
        drawTableRow(ry, 54, i % 2 == 0);

        DrawText(v->getId().c_str(),               c1, ry+18, 14, theme.SUBTEXT);
        DrawText(v->getVehicleType().c_str(),       c2, ry+18, 14, theme.ACCENT);
        DrawText(v->getDisplayName().c_str(),       c3, ry+18, 14, theme.TEXT);
        DrawText(v->getRegistrationNum().c_str(),   c4, ry+18, 14, theme.TEXT);
        DrawText(v->getLocation().c_str(),          c5, ry+18, 14, theme.TEXT);
        DrawText(fmtMoney(v->getDailyRate()).c_str(),c6, ry+18, 14, theme.SUCCESS);
        DrawText(rented ? "RENTED" : "AVAILABLE",  c7, ry+18, 14,
                 rented ? theme.WARNING : theme.SUCCESS);

        if (!rented) {
            bool listed = v->isListed();
            if (drawSmallBtn(actX, ry + 14, 96, 26,
                             listed ? "Unlist" : "Relist",
                             listed ? theme.DANGER : theme.SUCCESS)) {
                v->setListed(!listed); store.save();
                showNotif(v->isListed() ? "Vehicle listed."
                                        : "Vehicle unlisted.");
            }
        }
    }
    if (myVehicles.empty())
        DrawText("You have no listed vehicles.",
            contentX() + (contentW() - 240) / 2, y + 110, 16, theme.SUBTEXT);
}

void App::drawAgreements() {
    drawTopBar(); drawSidebarNav();
    int y = TOP_H + 10;

    const char* tabs[] = {"Incoming Requests", "My Rentals", "Completed"};
    for (int i = 0; i < 3; i++) {
        bool active = (agreementsTab == i);
        Rectangle r = {
            (float)(contentX() + i * 190), (float)y, 182, 38
        };
        DrawRectangleRounded(r, 0.2f, 8,
                             active ? theme.ACCENT : theme.PANEL);
        DrawRectangleLinesEx(r, 1.0f, theme.BORDER);
        DrawText(tabs[i], (int)r.x + 10, (int)r.y + 11, 14,
                 active ? WHITE : theme.TEXT);
        if (!active
            && CheckCollisionPointRec(GetMousePosition(), r)
            && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            agreementsTab = i;
            memset(inputs, 0, sizeof(inputs));
        }
    }
    int cy2 = y + 48;

    if (agreementsTab == 0) {
        auto incoming     = store.getAgreementsAsOwner(userId);
        int  pendingCount = 0;
        for (auto* a : incoming)
            if (a->getStatus() == AgreementStatus::PENDING) pendingCount++;

        int tableH = 50 + 36 + max(pendingCount, 1) * 72 + 10;
        drawPanel(contentX(), cy2, contentW(), tableH,
                  "Booking Requests for Your Vehicles");

        int c1 = contentX()+8,  c2 = contentX()+90;
        int c3 = contentX()+280, c4 = contentX()+430, c5 = contentX()+610;
        int actX = rightEdge() - 180;
        drawTableHeader(cy2 + 50, {
            {c1,"Req ID"},{c2,"Vehicle"},{c3,"Renter"},
            {c4,"Dates"},{c5,"Total"},{actX,"Actions"}
        });

        int ry = cy2 + 90, shown = 0;
        for (auto* a : incoming) {
            if (a->getStatus() != AgreementStatus::PENDING) continue;
            VehicleBase* v = store.findVehicleById(a->getVehicleId());
            User*        r = store.findUserById(a->getRenterId());
            drawTableRow(ry, 68, shown % 2 == 0);
            string dates = a->getStartDate().toString()
                           + " - " + a->getEndDate().toString();
            DrawText(a->getId().c_str(),                    c1, ry+22, 13, theme.SUBTEXT);
            DrawText(v ? v->getDisplayName().c_str() : "?", c2, ry+22, 13, theme.TEXT);
            DrawText(r ? r->getName().c_str() : "?",        c3, ry+22, 13, theme.TEXT);
            DrawText(dates.c_str(),                         c4, ry+22, 13, theme.TEXT);
            DrawText(fmtMoney(a->getTotalCost()).c_str(),   c5, ry+22, 13, theme.SUCCESS);

            if (drawSmallBtn(actX, ry + 10, 82, 26, "ACCEPT", theme.SUCCESS)) {
                a->setStatus(AgreementStatus::ACTIVE);
                User* owner = store.findUserById(userId);
                if (owner) {
                    double p = a->getOwnerPayout();
                    owner->creditBalance(p);
                    owner->addTransaction(Transaction(
                        Date::today(), "Payout",
                        "Rental " + a->getId(), p
                    ));
                }
                store.save();
                showNotif("Booking accepted. Payout credited.");
            }
            if (drawSmallBtn(actX + 92, ry + 10, 82, 26,
                             "REJECT", theme.DANGER)) {
                a->setStatus(AgreementStatus::REJECTED);
                User* renter = store.findUserById(a->getRenterId());
                if (renter) {
                    renter->creditBalance(a->getTotalCost());
                    renter->addTransaction(Transaction(
                        Date::today(), "Refund",
                        "Rejected " + a->getId(), a->getTotalCost()
                    ));
                }
                store.save();
                showNotif("Rejected. Renter refunded.");
            }
            ry += 72; shown++;
        }
        if (shown == 0)
            DrawText("No pending requests.",
                contentX() + (contentW() - 200) / 2,
                cy2 + 120, 16, theme.SUBTEXT);
    }

    else if (agreementsTab == 1) {
        auto myRentals = store.getAgreementsAsRenter(userId);
        int tableH = 50 + 36 + max((int)myRentals.size(), 1) * 54 + 10;
        drawPanel(contentX(), cy2, contentW(), tableH,
                  "Your Rental Requests");

        int c1 = contentX()+8,  c2 = contentX()+80;
        int c3 = contentX()+280, c4 = contentX()+450, c5 = contentX()+560;
        int actX = rightEdge() - 110;
        drawTableHeader(cy2 + 50, {
            {c1,"ID"},{c2,"Vehicle"},{c3,"Dates"},
            {c4,"Total"},{c5,"Status"},{actX,""}
        });

        for (int i = 0; i < (int)myRentals.size(); i++) {
            auto* a = myRentals[i];
            VehicleBase* v = store.findVehicleById(a->getVehicleId());
            int ry = cy2 + 90 + i * 54;
            drawTableRow(ry, 54, i % 2 == 0);
            string dates = a->getStartDate().toString()
                           + " - " + a->getEndDate().toString();
            Color scol =
                a->getStatus() == AgreementStatus::ACTIVE   ? theme.SUCCESS :
                a->getStatus() == AgreementStatus::PENDING  ? theme.WARNING :
                a->getStatus() == AgreementStatus::REJECTED ? theme.DANGER
                                                             : theme.SUBTEXT;
            DrawText(a->getId().c_str(),                    c1, ry+18, 14, theme.SUBTEXT);
            DrawText(v ? v->getDisplayName().c_str() : "?", c2, ry+18, 14, theme.TEXT);
            DrawText(dates.c_str(),                         c3, ry+18, 14, theme.TEXT);
            DrawText(fmtMoney(a->getTotalCost()).c_str(),   c4, ry+18, 14, theme.SUCCESS);
            DrawText(a->statusToString().c_str(),           c5, ry+18, 14, scol);
            if (a->getStatus() == AgreementStatus::ACTIVE)
                if (drawSmallBtn(actX, ry + 14, 96, 26, "Return")) {
                    a->setStatus(AgreementStatus::COMPLETED);
                    store.save();
                    showNotif("Vehicle marked as returned.");
                }
        }
        if (myRentals.empty())
            DrawText("No rental requests.",
                contentX() + (contentW() - 180) / 2,
                cy2 + 120, 16, theme.SUBTEXT);
    }

    else {
        auto renterAgreements = store.getAgreementsAsRenter(userId);
        auto ownerAgreements  = store.getAgreementsAsOwner(userId);

        int tableH = 560;
        drawPanel(contentX(), cy2, contentW(), tableH,
                  "Completed Agreements & Reviews");

        int c1 = contentX()+8,  c2 = contentX()+90;
        int c3 = contentX()+270, c4 = contentX()+370, c5 = contentX()+470;
        int rc = rightEdge() - 180;
        drawTableHeader(cy2 + 50, {
            {c1,"ID"},{c2,"Vehicle"},{c3,"Role"},
            {c4,"Total"},{c5,"Status"},{rc,"Rate the other party (1-5)"}
        });

        int ry = cy2 + 90;

        // Lambda draws one completed-agreement review row
        auto drawReviewRow = [&](RentalAgreement* a, bool asRenter) {
            if (a->getStatus() != AgreementStatus::COMPLETED) return;
            VehicleBase* v     = store.findVehicleById(a->getVehicleId());
            User*        other = asRenter
                ? store.findUserById(a->getOwnerId())
                : store.findUserById(a->getRenterId());
            drawTableRow(ry, 56, true);
            DrawText(a->getId().c_str(),                  c1, ry+18, 13, theme.SUBTEXT);
            DrawText(v ? v->getDisplayName().c_str() : "?",c2, ry+18, 13, theme.TEXT);
            DrawText(asRenter ? "Renter" : "Owner",       c3, ry+18, 13, theme.TEXT);
            DrawText(fmtMoney(a->getTotalCost()).c_str(),  c4, ry+18, 13, theme.SUCCESS);
            DrawText("DONE",                               c5, ry+18, 13, theme.SUCCESS);

            bool reviewed = asRenter ? a->renterReviewed()
                                     : a->ownerReviewed();
            if (!reviewed) {
                for (int s = 1; s <= 5; s++) {
                    if (drawSmallBtn(rc + (s - 1) * 34, ry + 15, 28, 26,
                                     to_string(s).c_str(), theme.WARNING)) {
                        if (asRenter) {
                            a->setRatingForOwner(s);
                            a->setRenterReviewed(true);
                        } else {
                            a->setRatingForRenter(s);
                            a->setOwnerReviewed(true);
                        }
                        if (other) other->addRating(s);
                        store.save();
                        showNotif("Rating submitted!");
                    }
                }
            } else {
                int given = asRenter ? a->getRatingForOwner()
                                     : a->getRatingForRenter();
                DrawText(("Rated: " + to_string(given) + "/5").c_str(),
                         rc, ry + 18, 13, theme.WARNING);
            }
            ry += 60;
        };

        for (auto* a : renterAgreements) drawReviewRow(a, true);
        for (auto* a : ownerAgreements)  drawReviewRow(a, false);
        if (ry == cy2 + 90)
            DrawText("No completed agreements yet.",
                contentX() + (contentW() - 260) / 2,
                cy2 + 200, 16, theme.SUBTEXT);
    }
}

void App::drawWallet() {
    drawTopBar(); drawSidebarNav();
    User* u = store.findUserById(userId);
    if (!u) return;

    int y = TOP_H + 12;
    drawPanel(contentX(), y, contentW(), 96, nullptr);
    DrawText("WALLET BALANCE",
             contentX() + 18, y + 14, 13, theme.SUBTEXT);
    DrawText(fmtMoney(u->getBalance()).c_str(),
             contentX() + 18, y + 34, 30, theme.SUCCESS);
    DrawText(("Role: " + u->getRole()).c_str(),
             contentX() + 18, y + 74, 13, theme.SUBTEXT);
    DrawText(("License: " + u->getLicenseNumber()).c_str(),
             contentX() + 140, y + 74, 13, theme.SUBTEXT);
    DrawText((u->getIdType() + ": " + u->getIdNumber()).c_str(),
             contentX() + 360, y + 74, 13, theme.SUBTEXT);
    y += 108;

    int hw = (contentW() - 10) / 2;
    drawPanel(contentX(), y, hw, 114, "Deposit Funds");
    drawLabel(contentX() + 14, y + 52, "AMOUNT ($)", theme.SUBTEXT);
    drawInput(1, contentX() + 14, y + 68, hw - 130, 38, "Enter amount");
    if (drawSmallBtn(contentX() + hw - 110, y + 68, 98, 38,
                     "DEPOSIT", theme.SUCCESS)) tryDeposit();

    drawPanel(contentX() + hw + 10, y, hw, 114, "Withdraw Funds");
    drawInput(2, contentX() + hw + 24, y + 68, hw - 130, 38, "Enter amount");
    if (drawSmallBtn(contentX() + hw * 2 - 96, y + 68, 98, 38,
                     "WITHDRAW", theme.DANGER)) tryWithdraw();
    y += 126;

    auto& txs    = u->getTransactions();
    int   tableH = 50 + 36 + max((int)txs.size(), 1) * 46 + 10;
    drawPanel(contentX(), y, contentW(), tableH, "Transaction History");

    int c1 = contentX() + 8,  c2 = contentX() + 130;
    int c3 = contentX() + 270, c4 = rightEdge() - 140;
    drawTableHeader(y + 50, {
        {c1,"Date"},{c2,"Type"},{c3,"Details"},{c4,"Amount"}
    });

    for (int i = 0; i < (int)txs.size(); i++) {
        auto& tx = txs[txs.size() - 1 - i];
        int ry = y + 90 + i * 46;
        drawTableRow(ry, 46, i % 2 == 0);
        DrawText(tx.date.toString().c_str(), c1, ry+14, 14, theme.SUBTEXT);
        DrawText(tx.type.c_str(),            c2, ry+14, 14, theme.TEXT);
        DrawText(tx.details.c_str(),         c3, ry+14, 14, theme.TEXT);
        Color  ac   = tx.amount >= 0 ? theme.SUCCESS : theme.DANGER;
        string sign = tx.amount >= 0 ? "+" : "";
        DrawText((sign + fmtMoney(abs(tx.amount))).c_str(),
                 c4, ry + 14, 14, ac);
    }
    if (txs.empty())
        DrawText("No transactions yet.",
            contentX() + (contentW() - 180) / 2, y + 120, 16, theme.SUBTEXT);
}

// ─── Main Loop ────────────────────────────────────────────────────────────────

void App::run() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 800, "P2P Car Rental Marketplace");
    SetTargetFPS(60);
    store.load();
    memset(inputs, 0, sizeof(inputs));

    while (!WindowShouldClose()) {
        handleTextInput();
        BeginDrawing();
        ClearBackground(theme.BG);

        switch (screen) {
            case Screen::LOGIN: drawLogin(); break;
            case Screen::REGISTER: drawRegister(); break;
            case Screen::DASHBOARD: drawDashboard(); break;
            case Screen::RENT_CAR: drawRentCar(); break;
            case Screen::BOOKING: drawBooking(); break;
            case Screen::LIST_CAR: drawListCar(); break;
            case Screen::MY_ASSETS: drawMyAssets(); break;
            case Screen::AGREEMENTS: drawAgreements(); break;
            case Screen::WALLET: drawWallet(); break;
        }

        drawNotif();
        EndDrawing();
    }

    store.save();
    CloseWindow();
}
