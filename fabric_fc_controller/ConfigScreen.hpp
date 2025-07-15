#pragma once

#include "BaseScreen.hpp"

class ConfigScreen : public BaseScreen {
public:
  ConfigScreen(M5GFX* disp) : BaseScreen(disp) {
  }

  ~ConfigScreen() {
  }
    
  void init() override{
  }
  
  void update() override{
  }
  
  void draw() override{
    // 背景の該当部分をクリア
      display->fillRect(0, 0, 240, 320, TFT_BLUE);    
       
  }

  void handleTouch(int x, int y, bool pressed) override {

  }
};