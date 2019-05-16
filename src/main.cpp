#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml-cpp/yaml.h>

#include "gui/WindowManager.h"
#include "input/KeyboardManager.h"
#include "lib/keycode/keycode.h"
#include "hotkey/hotkeyloader_yaml.h"
#include "gui/drawer/ShapeDrawerFactory.h"
#include "gui/HotkeyPickerDrawer.h"

int main(int argc, char *argv[]) {
    std::vector<Hotkey> hotkeys;

    // TODO: config manager?
    load_hotkeys_yaml((char *) "../static/sampleconfig.yaml", &hotkeys);

    auto *windowManager = new WindowManager();
    auto *keyboardManager = new KeyboardManager();
    auto hotkeyPickerDrawer = new HotkeyPickerDrawer(windowManager, ShapeType::RECTANGLE, &hotkeys);

    Display *display = windowManager->getDisplay();
    Window window = windowManager->getWindow();

    if (!keyboardManager->openKeyboard()) {
        printf("Couldn't open keyboard");
    }

    int keep_running = 1;
    XEvent event;

    hotkeyPickerDrawer->drawFrame(&* hotkeys.begin());

    while (keep_running) {
        // read xevents
        while (XPending(display)) {
            XNextEvent(display, &event);
            switch (event.type) {
                case ClientMessage:
                    if (event.xclient.message_type == XInternAtom(display, "WM_PROTOCOLS", 1) &&
                        (Atom) event.xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", 1))
                        keep_running = 0;
                    break;
                    // Manage events...
                default:
                    break;
            }
        }


        // todo: handle exception
        auto keyCode = (unsigned) keyboardManager->readKeypress();

        if (keyCode == 0) {
            continue;
        }

        printf("RAW: %s FORMATTED: %s\n", keycode_linux_rawname(keyCode),
               keycode_linux_name(keycode_linux_to_hid(keyCode)));

        // exit the loop if exit button is pressed
        if (keyCode == 1) {
            keep_running = 0;
        } else {
            hotkeyPickerDrawer->drawFrame(hotkeyPickerDrawer->getSelectedHotkey());
        }
    }

    keyboardManager->closeKeyboard();
    windowManager->destroyWindow();

    return 0;
}
