// language: C++17, file: main.cpp, app: txfeecalc — transaction fee calculator
// Windows 11, MSVC, WinAPI. No external dependencies, fully offline.
// Input: gas limit, gas price (gwei), ETH price. Output: cost in ETH and USD
// for send / swap / mint presets.
#include <windows.h>
#include <cstdio>

static HWND g_gaslimit, g_gwei, g_ethusd, g_result;

static double GetDouble(HWND h) {
    wchar_t buf[64]; GetWindowTextW(h, buf, 64);
    return _wtof(buf);
}

static void Calculate() {
    double gas = GetDouble(g_gaslimit);
    double gwei = GetDouble(g_gwei);
    double ethUsd = GetDouble(g_ethusd);
    if (gas <= 0 || gwei <= 0 || ethUsd <= 0) {
        SetWindowTextW(g_result, L"enter valid numbers in all fields");
        return;
    }
    double ethCost = gwei * 1e-9 * gas;
    wchar_t buf[512];
    swprintf(buf, 512,
             L"this tx:  %.6f ETH   ($%.2f)\r\n"
             L"ETH send (21000 gas):  %.6f ETH ($%.2f)\r\n"
             L"ERC-20 swap (150000):  %.6f ETH ($%.2f)\r\n"
             L"NFT mint (285000):     %.6f ETH ($%.2f)",
             ethCost, ethCost * ethUsd,
             gwei * 1e-9 * 21000, gwei * 1e-9 * 21000 * ethUsd,
             gwei * 1e-9 * 150000, gwei * 1e-9 * 150000 * ethUsd,
             gwei * 1e-9 * 285000, gwei * 1e-9 * 285000 * ethUsd);
    SetWindowTextW(g_result, buf);
}

static LRESULT CALLBACK WndProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        HFONT f = CreateFontW(17, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
        auto label = [&](const wchar_t* t, int x, int y) {
            HWND h = CreateWindowW(L"STATIC", t, WS_CHILD | WS_VISIBLE, x, y, 220, 22, w, nullptr, nullptr, nullptr);
            SendMessageW(h, WM_SETFONT, (WPARAM)f, TRUE);
        };
        auto input = [&](HWND& s, const wchar_t* d, int x, int y) {
            s = CreateWindowW(L"EDIT", d, WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, 150, 26, w, nullptr, nullptr, nullptr);
            SendMessageW(s, WM_SETFONT, (WPARAM)f, TRUE);
        };
        label(L"Gas limit", 24, 22);        input(g_gaslimit, L"150000", 260, 20);
        label(L"Gas price (gwei)", 24, 62); input(g_gwei, L"15", 260, 60);
        label(L"ETH price ($)", 24, 102);   input(g_ethusd, L"3400", 260, 100);
        CreateWindowW(L"BUTTON", L"Calculate", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      24, 144, 150, 34, w, (HMENU)1, nullptr, nullptr);
        g_result = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                                 24, 194, 440, 140, w, nullptr, nullptr, nullptr);
        SendMessageW(g_result, WM_SETFONT, (WPARAM)f, TRUE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == 1) Calculate();
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(w, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, LPWSTR, int show) {
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc; wc.hInstance = inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(13, 17, 23));
    wc.lpszClassName = L"FeeCalcWnd";
    RegisterClassW(&wc);
    HWND w = CreateWindowExW(0, L"FeeCalcWnd", L"FeeCalc — transaction fee calculator (offline)",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 380,
                             nullptr, nullptr, inst, nullptr);
    ShowWindow(w, show);
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0)) { TranslateMessage(&m); DispatchMessageW(&m); }
    return 0;
}
