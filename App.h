#pragma once
#include <string>
#include <vector>
#include <utility>
#include "raylib.h"
#include "DataStore.h"

// ─── Theme ────────────────────────────────────────────────────────────────────
// All UI colours in one place. Swap constructor values to retheme the app.
class Theme {
public:
    // Surfaces
    const Color BG;       // Page / window background
    const Color PANEL;    // Card & panel fill
    const Color HEADER;   // Sidebar + panel title bar
    const Color TOPBAR;   // Top navigation bar
    const Color INPUT;    // Text-field background
    const Color ROW_ALT;  // Alternating table row tint

    // Semantic colours
    const Color ACCENT;   // Primary interactive / highlight
    const Color SUCCESS;  // Positive / green
    const Color WARNING;  // Caution / amber
    const Color DANGER;   // Destructive / red

    // Typography & chrome
    const Color TEXT;     // Body text
    const Color SUBTEXT;  // Secondary / label text
    const Color BORDER;   // Lines & outlines

    Theme();
};

// ─── App ──────────────────────────────────────────────────────────────────────
// Top-level application class.  Owns the DataStore, Theme, and all UI state.
// Only ever instantiated once in main().
class App {
private:
    DataStore store;
    Theme     theme;

    enum class Screen {
        LOGIN, REGISTER, DASHBOARD, RENT_CAR,
        BOOKING, LIST_CAR, MY_ASSETS, AGREEMENTS, WALLET
    };

    Screen screen            = Screen::LOGIN;
    std::string userId;
    std::string selectedVehicleId;
    int         agreementsTab  = 0;
    std::string newVehicleType = "4-Seater";
    bool        regIdNID       = true;

    static const int MAX_INPUTS = 8;
    char inputs[MAX_INPUTS][256];
    int  focusedInput = 0;

    std::string notification;
    bool        notifError = false;
    float       notifTimer = 0.0f;

    // Layout constants
    static const int SIDEBAR_W = 205;
    static const int TOP_H     = 54;
    static const int PAD       = 12;

    // ─── Layout helpers ───────────────────────────────────────────────────────
    int contentX()  const;   // left edge of main content area
    int contentW()  const;   // width of main content area
    int rightEdge() const;   // right edge of main content area
    int screenW()   const;
    int screenH()   const;

    // ─── Utility ──────────────────────────────────────────────────────────────
    static Color       lighten(Color c, int amount);
    static Color       darken(Color c, int amount);
    static std::string fmtMoney(double v);
    static std::string ratingStr(float rating, int count);

    void navigate(Screen s);
    void showNotif(const std::string& msg, bool err = false);
    void handleTextInput();

    // ─── UI primitives ────────────────────────────────────────────────────────
    void drawPanel(int x, int y, int w, int h, const char* title = nullptr);

    // Pass {0,0,0,0} (or omit col) to use theme.ACCENT as the button colour.
    bool drawBtn(
        int x, int y, int w, int h,
        const char* text,
        Color col = {0, 0, 0, 0}
    );
    bool drawSmallBtn(
        int x, int y, int w, int h,
        const char* text,
        Color col = {0, 0, 0, 0}
    );
    void drawInput(
        int id, int x, int y, int w, int h,
        const char* placeholder = "",
        bool password = false
    );
    void drawLabel(
        int x, int y,
        const char* text,
        Color col = {0, 0, 0, 0},
        int fontSize = 16
    );
    void drawTableHeader(
        int y,
        const std::vector<std::pair<int, std::string>>& cols
    );
    void drawTableRow(int y, int h, bool alt);
    void drawTopBar();
    void drawNotif();
    void drawSidebarNav();
    void drawVehicleTypeSelector(int x, int y);

    // ─── Business actions ─────────────────────────────────────────────────────
    bool tryLogin();
    bool tryRegister();
    bool tryBooking();
    bool tryListVehicle();
    bool tryDeposit();
    bool tryWithdraw();

    // ─── Screens ──────────────────────────────────────────────────────────────
    void drawLogin();
    void drawRegister();
    void drawDashboard();
    void drawRentCar();
    void drawBooking();
    void drawListCar();
    void drawMyAssets();
    void drawAgreements();
    void drawWallet();

public:
    // Initialises raylib, runs the main loop, then cleans up.
    void run();
};
