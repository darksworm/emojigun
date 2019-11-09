#include <linux/input-event-codes.h>
#include <QtCore/Qt>
#include "InputHandler.h"
#include "instruction/ModeChangeInstruction.h"
#include "../../lib/keycode/keycode.h"
#include "instruction/CopyInstruction.h"
#include "../../gui/ImagePickerDrawer.h"
#include "instruction/MoveInstruction.h"

InputInstruction *InputHandler::handleKeyPress(unsigned keyCode) {
    if (keyCode == Qt::Key_Escape) {
        return new InputInstruction(InputInstructionType::CANCEL);
    }

    if (keyCode == Qt::Key_CapsLock) {
        InputMode mode = getNextMode();

        return new ModeChangeInstruction(mode);
    }

    if (keyCode == Qt::Key_Enter) {
        return new CopyInstruction();
    }

    if (isModifier(keyCode)) {
        addModifier(keyCode);
    }

    if (keyCode == Qt::Key_C && isModifierActive("CONTROL")) {
        return new InputInstruction(InputInstructionType::EXIT);
    }

    ImagePickerMove move = ImagePickerMove::NONE;

    switch (keyCode) {
        case Qt::Key_Left:
            move = ImagePickerMove::LEFT;
            break;
        case Qt::Key_Right:
            move = ImagePickerMove::RIGHT;
            break;
        case Qt::Key_Down:
            move = ImagePickerMove::DOWN;
            break;
        case Qt::Key_Up:
            move = ImagePickerMove::UP;
            break;
        case Qt::Key_PageDown:
            move = ImagePickerMove::PG_DOWN;
            break;
        case Qt::Key_PageUp:
            move = ImagePickerMove::PG_UP;
            break;
        case Qt::Key_Home:
            move = ImagePickerMove::HOME;
            break;
        case Qt::Key_End:
            move = ImagePickerMove::END;
            break;
        default:
            move = ImagePickerMove::NONE;
            break;
    }

    if (move != ImagePickerMove::NONE) {
        return new MoveInstruction(move, 1);
    }

    return new InputInstruction(InputInstructionType::NONE);
}

InputInstruction *InputHandler::handleKeyRelease(unsigned keyCode) {
    if (isModifier(keyCode)) {
        removeModifier(keyCode);
    }

    return new InputInstruction(InputInstructionType::NONE);
}

void InputHandler::removeModifier(unsigned keyCode) {
    activeModifiers.erase(linux_keycode_to_yaml_name(keyCode));
}

bool InputHandler::isModifier(unsigned keyCode) {
    switch (keyCode) {
        case Qt::Key_Shift:
            return true;
        case Qt::Key_Control:
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
