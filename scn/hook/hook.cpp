#include "hook.hpp"

HHOOK hMouseHook;
std::vector<double> clickTimestamps;
WPARAM monitorButton;

int count = 0;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == monitorButton) {
            count++;
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = now.time_since_epoch();
            double timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
            clickTimestamps.push_back(timestamp);
            std::cout << "Mouse event registered. Analysis will start in: " << 50 - count << " clicks more\n";

            if (count >= 50) {
                UnhookWindowsHookEx(hMouseHook);
                PostQuitMessage(0);
                return 0;
            }
        }
    }

    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void install_hook() {
    hMouseHook = SetWindowsHookExA(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    if (!hMouseHook) {
        std::cout << "Failed to install mouse hook. Error code: " << GetLastError() << "\n";
        return;
    }

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}