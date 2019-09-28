#include <linux/input-event-codes.h>
#include "InputHandler.h"
#include "instruction/ModeChangeInstruction.h"
#include "../../lib/keycode/keycode.h"
#include "instruction/CopyInstruction.h"

Instruction *InputHandler::handleKeyPress(unsigned keyCode) {
    if (keyCode == KEY_ESC) {
        return new Instruction(InstructionType::CANCEL);
    }

    if (keyCode == KEY_CAPSLOCK) {
        InputMode mode = getNextMode();

        return new ModeChangeInstruction(mode);
    }

    if (keyCode == KEY_ENTER) {
        return new CopyInstruction();
    }

    if (isModifier(keyCode)) {
        addModifier(keyCode);
    }

    if (keyCode == KEY_C && isModifierActive("CONTROL")) {
        return new Instruction(InstructionType::EXIT);
    }

    return new Instruction(InstructionType::NONE);
}

Instruction *InputHandler::handleKeyRelease(unsigned keyCode) {
    if (isModifier(keyCode)) {
        removeModifier(keyCode);
    }

    return new Instruction(InstructionType::NONE);
}

void InputHandler::removeModifier(unsigned keyCode) {
    activeModifiers.erase(linux_keycode_to_yaml_name(keyCode));
}

bool InputHandler::isModifier(unsigned keyCode) {
    switch (keyCode) {
        case KEY_RIGHTSHIFT:
        case KEY_LEFTSHIFT:
            return true;
        case KEY_LEFTCTRL:
            return true;
    }

    return false;
}

void InputHandler::addModifier(unsigned keyCode) {
    activeModifiers.insert(linux_keycode_to_yaml_name(keyCode));
}

bool InputHandler::isModifierActive(std::string keyName) {
    return activeModifiers.find(keyName) != activeModifiers.end();
}
