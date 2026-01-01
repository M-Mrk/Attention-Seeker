#ifndef HOTKEYSCREEN_H
#define HOTKEYSCREEN_H

#include <TFT_eSPI.h>
#include "../Screen.h"

class HotkeyScreen : public Screen {
public:
  HotkeyScreen() = default;

  void init(TFT_eSPI *display) override;
  void draw(TFT_eSPI *display) override;
  InputEvent handleInput(InputEvent input) override;
  void end() override {}

private:
  enum class ActiveRow { Top, Bottom };

  ActiveRow activeRow = ActiveRow::Top;
  bool needsRedraw = true;

  void drawKeys(TFT_eSPI *display);
  char resolveKeyForInput(InputEvent input) const;
};

#endif
