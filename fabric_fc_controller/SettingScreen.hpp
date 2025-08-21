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
  M5Canvas *bar_canvas;
  M5Canvas *buttons_canvas;

  int selected_button = -1;  // 設定中のボタンインデックス
  //float pressure_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // デモ用の圧力値
  //float threshold_values[8] = {1.0, 0.5, 0.7, 0.4, 0.8, 0.3, 0.65, 0.55}; // 各ボタンの基準値（0.0-1.0）  

  uint8_t update_flags;


  void drawSettingScreen() {
    
    // タイトル
    display->setTextColor(TFT_WHITE);
    display->setTextSize(1);
    String title = "THRESHOLD SETTINGS";    
    int title_width = title.length() * 6;
    int title_x = (display->width() - title_width) / 2;    
    display->drawString(title, title_x, 20);
    
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
      float value = config.getPressType() == ABSOLUTE ?  config.getThresholdValue(i) : config.getAbsoluteValue(i);
      String value_str = String((int)( value * 100)) + "%";
      buttons_canvas->drawString(value_str, 60, y_pos + 12);
    }
    buttons_canvas->pushSprite(10, 40);

  //
    bar_canvas->fillSprite(TFT_BLACK);
    
    // マテリアルデザイン風の+ボタン
    bar_canvas->fillRoundRect(0, 0, 90, 30, 4, bar_canvas->color565(76, 175, 80)); // Material Green
    bar_canvas->setTextColor(TFT_WHITE);
    bar_canvas->setTextSize(2);
    bar_canvas->drawString("+", 40, 8);
    
    // マテリアルデザイン風の-ボタン
    bar_canvas->fillRoundRect(0, 245, 90, 30, 4, bar_canvas->color565(244, 67, 54)); // Material Red
    bar_canvas->setTextColor(TFT_WHITE);
    bar_canvas->setTextSize(2);
    bar_canvas->drawString("-", 40, 253);
    
    // === マテリアルデザイン風スライダー ===
    
      // スライダー背景
    bar_canvas->fillRoundRect(10, 45, 60, 185, 2, bar_canvas->color565(50, 50, 50));
    
   if( selected_button >= 0){
      // 圧力レベルと基準値に応じた色分け
      uint16_t bar_color;
      if(config.isPressed(selected_button)) {
        // 基準値以下：緑（正常）
        bar_color = bar_canvas->color565(50, 255, 50);
      } else {
        // 基準値以上：赤（警告）
        bar_color = bar_canvas->color565(255, 50, 50);
      }
        
      // メインバー
      int bar_height = (int)(config.getPressureValue(selected_button) * 185);
      int y = 45 + 185 - bar_height;      
      bar_canvas->fillRoundRect(10, y, 60, bar_height, 2, bar_color);
      
    }

    // スライダー目盛り
    bar_canvas->setTextColor(bar_canvas->color565(150, 150, 150));
    bar_canvas->setTextSize(1);
    for(int i = 0; i <= 10; i++) {
      int mark_y = 45 + (185 * i / 10);
      bar_canvas->drawString(String(100 - i * 10), 75, mark_y - 4);
      bar_canvas->drawFastHLine(10, mark_y, 60, bar_canvas->color565(100, 100, 100));
    }
    
    if( selected_button >= 0){
      // スライダーハンドル

      float value = config.getPressType() == ABSOLUTE ?  config.getThresholdValue(selected_button) : config.getAbsoluteValue(selected_button);
      int handle_y = 45 + (int)((1.0 - value) * 185);
      bar_canvas->fillRoundRect(0, handle_y - 8, 90, 16, 8, bar_canvas->color565(100, 200, 255));
      
      // 現在値表示
      bar_canvas->setTextColor(TFT_WHITE);
      bar_canvas->setTextSize(2);
      String current_value = String((int)(value * 100)) + "%";
      int value_width = current_value.length() * 12;
      int value_x = (90 - value_width) / 2; 

      bar_canvas->drawString(current_value, 25, handle_y - 8);
    }

    bar_canvas->pushSprite(130, 40);
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
          if(config.getPressType() == ABSOLUTE){
            config.setThresholdValue(selected_button, config.getThresholdValue(selected_button)+ 0.05);
            if(config.getThresholdValue(selected_button) > 1.0) config.setThresholdValue(selected_button, 1.0f);
          }
          else{
            config.setAbsoluteValue(selected_button, config.getAbsoluteValue(selected_button)+ 0.05);
            if(config.getAbsoluteValue(selected_button) > 1.0) config.setAbsoluteValue(selected_button, 1.0f);

          }
          //setting_changed = true;
        } else if(y >= 290 && y <= 320) { // -ボタン
          if(config.getPressType() == ABSOLUTE){        
            config.setThresholdValue(selected_button, config.getThresholdValue(selected_button) - 0.05);
            if(config.getThresholdValue(selected_button) < 0.0) config.setThresholdValue(selected_button, 0.0f);
          }
          else{
            config.setAbsoluteValue(selected_button, config.getAbsoluteValue(selected_button) - 0.05);
            if(config.getAbsoluteValue(selected_button) < 0.0) config.setAbsoluteValue(selected_button, 0.0f);
          }
          //setting_changed = true;
        }
      }
    }
  }

  void handleTouchSlider(int x, int y) {
    if( selected_button >= 0){

      // スライダーエリア（右側）
      if(x >= 130 && x <= 220 && y >= 75 && y <= 280) {
        float slider_ratio = 1.0 - (float)(y - 85) / 185.0;
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
    if (bar_canvas){
      bar_canvas->deleteSprite();      
      delete bar_canvas;  
    }  
  }
    

  void init() override{
    // キャンバス初期化
    buttons_canvas = new M5Canvas(display);
    buttons_canvas->setColorDepth(16);
    buttons_canvas->createSprite(90, 275);
    buttons_canvas->setRotation(0);
    
    bar_canvas = new M5Canvas(display);
    bar_canvas->setColorDepth(16);
    bar_canvas->createSprite(110, 275);
    bar_canvas->setRotation(0);
        
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