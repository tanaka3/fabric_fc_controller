#pragma once

#include "BaseScreen.hpp"

// 更新フラグ用のenum
enum SettingUpdateFlags {
    UPDATE_NONE = 0,
    UPDATE_TITLE = 1,
    UPDATE_BUTTON_LIST = 2,
    UPDATE_VARIABLE_CONTROLS = 4,
    UPDATE_SLIDER = 8,
    UPDATE_ALL = 15  // 全フラグのOR
};

// 設定画面クラス
class SettingScreen : public BaseScreen {  
private:
  M5Canvas *var_canvas;
  M5Canvas *buttons_canvas;

  int selected_button = -1;  // 設定中のボタンインデックス
  float pressure_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // デモ用の圧力値
  //float threshold_values[8] = {1.0, 0.5, 0.7, 0.4, 0.8, 0.3, 0.65, 0.55}; // 各ボタンの基準値（0.0-1.0）  

  uint8_t update_flags;


  void drawSettingScreen() {
    
    // タイトル
    display->setTextColor(TFT_WHITE);
    display->setTextSize(2);
    display->drawString("THRESHOLD SETTING", 20, 10);
    
    // ボタンリスト（左側）
    String button_names[] = {"UP", "DOWN", "LEFT", "RIGHT", "SELECT", "START", "B", "A"};
    
    for(int i = 0; i < 8; i++) {
      int y_pos = i * 35;
      
      // 選択されたボタンをハイライト（マテリアルデザイン風）
      if(i == selected_button) {
        buttons_canvas->fillRoundRect(0, y_pos, 90, 30, 4, buttons_canvas->color565(33, 150, 243)); // Material Blue
      } else {
        buttons_canvas->fillRoundRect(0, y_pos, 90, 30, 4, buttons_canvas->color565(66, 66, 66)); // Material Grey
      }
      
      // ボタン名
      buttons_canvas->setTextColor(TFT_WHITE);
      buttons_canvas->setTextSize(1);
      buttons_canvas->drawString(button_names[i], 10, y_pos + 12);
      
      // 現在の基準値
      String value_str = String((int)(config.getThresholdValue(i) * 100)) + "%";
      buttons_canvas->drawString(value_str, 60, y_pos + 12);
    }
    buttons_canvas->pushSprite(10, 40);

  //
    var_canvas->fillSprite(TFT_BLACK);
    
    // マテリアルデザイン風の+ボタン
    var_canvas->fillRoundRect(0, 0, 90, 30, 4, var_canvas->color565(76, 175, 80)); // Material Green
    var_canvas->setTextColor(TFT_WHITE);
    var_canvas->setTextSize(2);
    var_canvas->drawString("+", 40, 8);
    
    // マテリアルデザイン風の-ボタン
    var_canvas->fillRoundRect(0, 245, 90, 30, 4, var_canvas->color565(244, 67, 54)); // Material Red
    var_canvas->setTextColor(TFT_WHITE);
    var_canvas->setTextSize(2);
    var_canvas->drawString("-", 40, 253);
    
    // === マテリアルデザイン風スライダー ===
    
      // スライダー背景
    var_canvas->fillRoundRect(10, 45, 60, 185, 2, var_canvas->color565(50, 50, 50));
    
    // スライダー目盛り
    var_canvas->setTextColor(var_canvas->color565(150, 150, 150));
    var_canvas->setTextSize(1);
    for(int i = 0; i <= 10; i++) {
      int mark_y = 45 + (185 * i / 10);
      var_canvas->drawString(String(100 - i * 10), 75, mark_y - 4);
      var_canvas->drawFastHLine(10, mark_y, 60, var_canvas->color565(100, 100, 100));
    }
    
    if( selected_button >= 0){
      // スライダーハンドル
      int handle_y = 85 + (int)((1.0 - config.getThresholdValue(selected_button)) * 185);
      var_canvas->fillRoundRect(0, handle_y - 8, 90, 16, 8, var_canvas->color565(100, 200, 255));
      
      // 現在値表示
      var_canvas->setTextColor(TFT_WHITE);
      var_canvas->setTextSize(2);
      String current_value = String((int)(config.getThresholdValue(selected_button) * 100)) + "%";
      var_canvas->drawString(current_value, 25, handle_y - 8);
    }
      
    var_canvas->pushSprite(130, 40);
  }

  void handleSettingTouch(int x, int y) {
    // ボタン選択エリア（左側）
    if(x < 120) {
      int button_height = 35;
      int start_y = 50;
      for(int i = 0; i < 8; i++) {
        if(y >= start_y + i * button_height && y < start_y + (i + 1) * button_height) {

          if( selected_button == i){
            selected_button = -1;
          }
          else{
            selected_button = i;
          }
          return;
        }
      }
    }

    if(selected_button >= 0){
      // +/-ボタン
      if(x >= 140 && x <= 220) {
        if(y >= 50 && y <= 80) { // +ボタン
          config.setThresholdValue(selected_button, config.getThresholdValue(selected_button)+ 0.05);
          if(config.getThresholdValue(selected_button) > 1.0) config.setThresholdValue(selected_button, 1.0f);
          //setting_changed = true;
        } else if(y >= 290 && y <= 320) { // -ボタン
          config.setThresholdValue(selected_button, config.getThresholdValue(selected_button) - 0.05);
          if(config.getThresholdValue(selected_button) < 0.0) config.setThresholdValue(selected_button, 0.0f);
          //setting_changed = true;
        }
      }
    }
  }

  void handleTouchSlider(int x, int y) {
    if( selected_button >= 0){

      // スライダーエリア（右側）
      if(x >= 130 && x <= 220 && y >= 85 && y <= 225) {
        float slider_ratio = (float)(y - 100) / 185.0;
        slider_ratio = 1.0 - slider_ratio; // 上が1.0、下が0.0になるように反転
        if(slider_ratio < 0.0) slider_ratio = 0.0;
        if(slider_ratio > 1.0) slider_ratio = 1.0;
        
        config.setThresholdValue(selected_button, slider_ratio);
        //setting_changed = true;
      }
    }
  }

public:
  SettingScreen(M5GFX* disp) : BaseScreen(disp) {
  }

  ~SettingScreen() {
    if (buttons_canvas){
      buttons_canvas->deleteSprite();
      delete buttons_canvas;
    }
    if (var_canvas){
      var_canvas->deleteSprite();      
      delete var_canvas;  
    }  
  }
    

  void init() override{
    // キャンバス初期化
    buttons_canvas = new M5Canvas(display);
    buttons_canvas->setColorDepth(16);
    buttons_canvas->createSprite(90, 275);
    buttons_canvas->setRotation(0);
    
    var_canvas = new M5Canvas(display);
    var_canvas->setColorDepth(16);
    var_canvas->createSprite(110, 275);
    var_canvas->setRotation(0);
        
    update_flags = UPDATE_ALL;
  }
  
  void update() override{
  }
  
  void draw() override{
    // 背景の該当部分をクリア
    drawSettingScreen();
  }

  void handleTouch(int x, int y, bool pressed) override {
    auto t = M5.Touch.getDetail();
    if(t.wasPressed()) {
      handleSettingTouch(x, y);
    }
    handleTouchSlider(x, y);
  }
};