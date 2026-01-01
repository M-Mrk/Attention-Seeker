#include "screens/HotkeyScreen.h"

#include "DisplayManager.h"
#include "UsbHandler.h"

namespace {
constexpr uint16_t topBorderColor = 0x4AEF;
constexpr uint16_t topFillColor = 0x2D49;
constexpr uint16_t bottomBorderColor = 0xFBA8;
constexpr uint16_t bottomFillColor = 0xD328;
constexpr uint16_t textColor = 0xFFFF;

struct KeySlot {
    int16_t x;
    int16_t y;
    char label;
    bool topRow;
    int16_t cursorX;
    int16_t cursorY;
};

constexpr KeySlot keySlots[] = {
        {16, 15, 'A', true, 24, 91},   {116, 15, 'B', true, 129, 91},
        {215, 15, 'C', true, 225, 92}, {16, 113, 'D', false, 27, 191},
        {116, 113, 'E', false, 127, 190},
        {215, 113, 'F', false, 224, 190},
};
} // namespace

void HotkeyScreen::init(TFT_eSPI *display) {
    if (!display) {
        displayManager.showError("HotkeyScreen::init: display pointer is null");
        return;
    }

    activeRow = ActiveRow::Top;
    needsRedraw = true;
    display->fillScreen(0x0000);
}

void HotkeyScreen::draw(TFT_eSPI *display) {
    if (!display) {
        displayManager.showError("HotkeyScreen::draw: display pointer is null");
        return;
    }

    if (!needsRedraw)
        return;

    drawKeys(display);
    needsRedraw = false;
}

InputEvent HotkeyScreen::handleInput(InputEvent input) {
    switch (input) {
    case InputEvent::SW1:
        activeRow = (activeRow == ActiveRow::Top) ? ActiveRow::Bottom
                                                                                            : ActiveRow::Top;
        needsRedraw = true;
        return InputEvent::NoneInput;

    case InputEvent::SW2:
    case InputEvent::SW3:
    case InputEvent::SW4: {
        const char key = resolveKeyForInput(input);
        if (key != '\0') {
            usbHandler.sendKey(key);
        }
        return InputEvent::NoneInput;
    }

    default:
        return input;
    }
}

void HotkeyScreen::drawKeys(TFT_eSPI *display) {
    display->fillScreen(0x0000);
    display->setTextWrap(false);
    display->setTextSize(3);
    display->setFreeFont(&FreeSerif18pt7b);

    const bool topActive = activeRow == ActiveRow::Top;

    for (const auto &slot : keySlots) {
        const bool slotActive = slot.topRow ? topActive : !topActive;
        const uint16_t borderColor = slot.topRow ? topBorderColor : bottomBorderColor;
        const uint16_t fillColor = slot.topRow
                                                                     ? (slotActive ? topFillColor : 0x0000)
                                                                     : (slotActive ? bottomFillColor : 0x0000);

        display->drawRect(slot.x, slot.y, 90, 90, borderColor);
        display->drawRect(slot.x + 1, slot.y + 1, 88, 88, borderColor);
        display->fillRect(slot.x + 2, slot.y + 2, 86, 86, fillColor);

        display->setTextColor(textColor);
        display->setCursor(slot.cursorX, slot.cursorY);
        display->print(slot.label);
    }

    display->setFreeFont(nullptr);
    display->setTextSize(1);
    display->setTextColor(0x7BEF);
    display->setCursor(10, 230);
    display->print("SW1 switches row, SW2-4 send");
}

char HotkeyScreen::resolveKeyForInput(InputEvent input) const {
    const bool top = activeRow == ActiveRow::Top;
    switch (input) {
    case InputEvent::SW2:
        return top ? 'A' : 'D';
    case InputEvent::SW3:
        return top ? 'B' : 'E';
    case InputEvent::SW4:
        return top ? 'C' : 'F';
    default:
        return '\0';
    }
}