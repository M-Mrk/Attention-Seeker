#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <Arduino.h>

class LightManager {
private:
  int lightLevel;

public:
  LightManager();

  void init();
  void setLightLevel(int level);
  int getLightLevel();
};

extern LightManager lightManager;

#endif
