
#include <M5Unified.h>
#include <M5GFX.h>

#include "ScreenManager.hpp"
#include "Config.hpp"

M5GFX display;
ScreenManager screen_manager(&display);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg);
  
  display.begin();
  display.setRotation(0);

  screen_manager.setScreen(SCREEN_MONITOR);
}

void loop() {

  M5.update();
  
  // タッチ処理
  screen_manager.handleTouch();

  // 画面の更新
  screen_manager.update();

}
