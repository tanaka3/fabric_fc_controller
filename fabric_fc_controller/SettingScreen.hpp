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
  M5Canvas *mode_canvas;  // モード切り替え用キャンバス

  int selected_button = -1;  // 設定中のボタンインデックス

  uint8_t update_flags;

  void drawSettingScreen() {
    
    // タイトル
    display->setTextColor(TFT_WHITE);
    display->setTextSize(1);
    String title = "BUTTON SETTINGS";    
    int title_width = title.length() * 6;
    int title_x = (display->width() - title_width) / 2;    
    display->drawString(title, title_x, 10);
    
    // ボタンリスト（左側）
    String button_names[] = {"UP", "DOWN", "LEFT", "RIGHT", "SELECT", "START", "B", "A"};
    
    buttons_canvas->fillSprite(TFT_BLACK);
    for(int i = 0; i < 8; i++) {
      int y_pos = i * 30;
      
      // 選択されたボタンをハイライト（マテリアルデザイン風）
      if(i == selected_button) {
        buttons_canvas->fillRoundRect(0, y_pos, 85, 28, 4, buttons_canvas->color565(33, 150, 243)); // Material Blue
      } else {
        buttons_canvas->fillRoundRect(0, y_pos, 85, 28, 4, buttons_canvas->color565(66, 66, 66)); // Material Grey
      }
      
      // ボタン名
      buttons_canvas->setTextColor(TFT_WHITE);
      buttons_canvas->setTextSize(1);
      buttons_canvas->drawString(button_names[i], 5, y_pos + 2);
      
      // モード表示（A/R）
      PressType button_mode = config.getButtonPressType(i);
      uint16_t mode_color = (button_mode == ABSOLUTE) ? 
                           buttons_canvas->color565(255, 255, 100) : 
                           buttons_canvas->color565(100, 255, 255);
      buttons_canvas->setTextColor(mode_color);
      String mode_str = (button_mode == ABSOLUTE) ? "A" : "R";
      buttons_canvas->drawString(mode_str, 5, y_pos + 12);
      
      // 現在の基準値/相対値
      float value = (button_mode == ABSOLUTE) ? 
                   config.getThresholdValue(i) : 
                   config.getAbsoluteValue(i);
      String value_str = String((int)(value * 100)) + "%";
      buttons_canvas->setTextColor(TFT_WHITE);
      buttons_canvas->drawString(value_str, 45, y_pos + 8);
    }
    buttons_canvas->pushSprite(5, 30);

    // バー/スライダーエリア
    bar_canvas->fillSprite(TFT_BLACK);
    
    // マテリアルデザイン風の+ボタン
    bar_canvas->fillRoundRect(0, 0, 90, 25, 4, bar_canvas->color565(76, 175, 80)); // Material Green
    bar_canvas->setTextColor(TFT_WHITE);
    bar_canvas->setTextSize(2);
    bar_canvas->drawString("+", 40, 5);
    
    // マテリアルデザイン風の-ボタン
    bar_canvas->fillRoundRect(0, 210, 90, 25, 4, bar_canvas->color565(244, 67, 54)); // Material Red
    bar_canvas->setTextColor(TFT_WHITE);
    bar_canvas->setTextSize(2);
    bar_canvas->drawString("-", 40, 215);
    
    // === マテリアルデザイン風スライダー ===
    
    // スライダー背景
    bar_canvas->fillRoundRect(10, 35, 60, 165, 2, bar_canvas->color565(50, 50, 50));
    
    if(selected_button >= 0){
      // 圧力レベルと基準値に応じた色分け
      uint16_t bar_color;
      if(config.isPressed(selected_button)) {
        // ボタンが押された状態：緑（正常）
        bar_color = bar_canvas->color565(50, 255, 50);
      } else {
        // ボタンが押されていない状態：赤
        bar_color = bar_canvas->color565(255, 50, 50);
      }
        
      // メインバー（現在の圧力値）
      int bar_height = (int)(config.getPressureValue(selected_button) * 165);
      int y = 35 + 165 - bar_height;      
      bar_canvas->fillRoundRect(10, y, 60, bar_height, 2, bar_color);
      
      // 絶対値モードの場合のみ基準値ラインを表示
      PressType button_mode = config.getButtonPressType(selected_button);
      if(button_mode == ABSOLUTE) {
        float threshold = config.getThresholdValue(selected_button);
        int threshold_y = 35 + (int)((1.0 - threshold) * 165);
        
        // 基準値ライン（点線風）
        uint16_t threshold_line_color = bar_canvas->color565(255, 255, 255);
        for(int line_x = 10; line_x < 70; line_x += 3) {
          bar_canvas->drawPixel(line_x, threshold_y, threshold_line_color);
          bar_canvas->drawPixel(line_x, threshold_y + 1, threshold_line_color);
        }
      }
    }

    // スライダー目盛り
    bar_canvas->setTextColor(bar_canvas->color565(150, 150, 150));
    bar_canvas->setTextSize(1);
    for(int i = 0; i <= 10; i++) {
      int mark_y = 35 + (165 * i / 10);
      bar_canvas->drawString(String(100 - i * 10), 75, mark_y - 4);
      bar_canvas->drawFastHLine(10, mark_y, 60, bar_canvas->color565(100, 100, 100));
    }
    
    if(selected_button >= 0){
      // スライダーハンドル
      PressType button_mode = config.getButtonPressType(selected_button);
      float value = (button_mode == ABSOLUTE) ? 
                   config.getThresholdValue(selected_button) : 
                   config.getAbsoluteValue(selected_button);
      
      int handle_y = 35 + (int)((1.0 - value) * 165);
      
      // ハンドルの色をモードで変える
      uint16_t handle_color = (button_mode == ABSOLUTE) ? 
                             bar_canvas->color565(255, 200, 100) : 
                             bar_canvas->color565(100, 200, 255);
      
      bar_canvas->fillRoundRect(0, handle_y - 8, 90, 16, 8, handle_color);
      
      // 現在値表示
      bar_canvas->setTextColor(TFT_BLACK);
      bar_canvas->setTextSize(1);
      String current_value = String((int)(value * 100)) + "%";
      int value_width = current_value.length() * 6;
      int value_x = (90 - value_width) / 2; 
      bar_canvas->drawString(current_value, value_x, handle_y - 4);
    }

    bar_canvas->pushSprite(100, 30);
    
    // モード切り替えエリア
    drawModeControls();
  }

  void drawModeControls() {
    mode_canvas->fillSprite(TFT_BLACK);
    
    if(selected_button >= 0) {
      PressType button_mode = config.getButtonPressType(selected_button);
      
      // モード表示
      mode_canvas->setTextColor(TFT_WHITE);
      mode_canvas->setTextSize(1);
      mode_canvas->drawString("MODE:", 5, 5);
      
      // 絶対値ボタン
      uint16_t abs_color = (button_mode == ABSOLUTE) ? 
                          mode_canvas->color565(255, 200, 100) : 
                          mode_canvas->color565(80, 80, 80);
      mode_canvas->fillRoundRect(5, 20, 35, 20, 3, abs_color);
      mode_canvas->setTextColor(TFT_BLACK);
      mode_canvas->drawString("ABS", 12, 26);
      
      // 相対値ボタン
      uint16_t rel_color = (button_mode == RELATIVE) ? 
                          mode_canvas->color565(100, 200, 255) : 
                          mode_canvas->color565(80, 80, 80);
      mode_canvas->fillRoundRect(45, 20, 35, 20, 3, rel_color);
      mode_canvas->setTextColor(TFT_BLACK);
      mode_canvas->drawString("REL", 52, 26);
      
      // モード説明
      mode_canvas->setTextColor(mode_canvas->color565(180, 180, 180));
      mode_canvas->setTextSize(1);
      if(button_mode == ABSOLUTE) {
        mode_canvas->drawString("Fixed threshold", 5, 50);
      } else {
        mode_canvas->drawString("Dynamic baseline", 5, 50);
      }
      
      // リセットボタン
      mode_canvas->fillRoundRect(5, 70, 75, 20, 3, mode_canvas->color565(200, 100, 100));
      mode_canvas->setTextColor(TFT_WHITE);
      mode_canvas->drawString("RESET", 25, 76);
    } else {
      // ボタン未選択時のメッセージ
      mode_canvas->setTextColor(mode_canvas->color565(150, 150, 150));
      mode_canvas->setTextSize(1);
      mode_canvas->drawString("Select button", 5, 20);
      mode_canvas->drawString("to configure", 5, 35);
    }
    
    mode_canvas->pushSprite(200, 30);
  }

  void handleSettingTouch(int x, int y) {
    // ボタン選択エリア（左側）
    if(x >= 5 && x <= 90) {
      int button_height = 30;
      int start_y = 30;
      for(int i = 0; i < 8; i++) {
        if(y >= start_y + i * button_height && y < start_y + (i + 1) * button_height) {
          if(selected_button == i) {
            selected_button = -1;
          } else {
            selected_button = i;
          }
          return;
        }
      }
    }

    if(selected_button >= 0) {
      // +/-ボタン
      if(x >= 100 && x <= 190) {
        if(y >= 30 && y <= 55) { // +ボタン
          PressType button_mode = config.getButtonPressType(selected_button);
          if(button_mode == ABSOLUTE) {
            float new_val = config.getThresholdValue(selected_button) + 0.05f;
            if(new_val > 1.0f) new_val = 1.0f;
            config.setThresholdValue(selected_button, new_val);
          } else {
            float new_val = config.getAbsoluteValue(selected_button) + 0.01f;
            if(new_val > 1.0f) new_val = 1.0f;
            config.setAbsoluteValue(selected_button, new_val);
          }
        } else if(y >= 240 && y <= 265) { // -ボタン
          PressType button_mode = config.getButtonPressType(selected_button);
          if(button_mode == ABSOLUTE) {
            float new_val = config.getThresholdValue(selected_button) - 0.05f;
            if(new_val < 0.0f) new_val = 0.0f;
            config.setThresholdValue(selected_button, new_val);
          } else {
            float new_val = config.getAbsoluteValue(selected_button) - 0.01f;
            if(new_val < 0.01f) new_val = 0.01f;
            config.setAbsoluteValue(selected_button, new_val);
          }
        }
      }
      
      // モード切り替えエリア
      if(x >= 200 && x <= 280 && y >= 50 && y <= 70) {
        if(x >= 205 && x <= 240) { // ABSボタン
          config.setButtonPressType(selected_button, ABSOLUTE);
        } else if(x >= 245 && x <= 280) { // RELボタン
          config.setButtonPressType(selected_button, RELATIVE);
        }
      }
      
      // リセットボタン
      if(x >= 205 && x <= 280 && y >= 100 && y <= 120) {
        // 選択されたボタンの設定をデフォルトにリセット
        PressType button_mode = config.getButtonPressType(selected_button);
        if(button_mode == ABSOLUTE) {
          config.setThresholdValue(selected_button, 0.6f);
        } else {
          config.setAbsoluteValue(selected_button, 0.1f);
        }
      }
    }
  }

  void handleTouchSlider(int x, int y) {
    if(selected_button >= 0) {
      // スライダーエリア（中央）
      if(x >= 100 && x <= 190 && y >= 65 && y <= 230) {
        float slider_ratio = 1.0f - (float)(y - 65) / 165.0f;
        if(slider_ratio < 0.0f) slider_ratio = 0.0f;
        if(slider_ratio > 1.0f) slider_ratio = 1.0f;
        
        PressType button_mode = config.getButtonPressType(selected_button);
        if(button_mode == ABSOLUTE) {
          config.setThresholdValue(selected_button, slider_ratio);
        } else {
          // 相対値は最小値を0.01に制限
          if(slider_ratio < 0.01f) slider_ratio = 0.01f;
          config.setAbsoluteValue(selected_button, slider_ratio);
        }
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
    if (mode_canvas){
      mode_canvas->deleteSprite();      
      delete mode_canvas;  
    }
  }
    
  void init() override{
    // キャンバス初期化
    buttons_canvas = new M5Canvas(display);
    buttons_canvas->setColorDepth(16);
    buttons_canvas->createSprite(85, 240);
    buttons_canvas->setRotation(0);
    
    bar_canvas = new M5Canvas(display);
    bar_canvas->setColorDepth(16);
    bar_canvas->createSprite(90, 240);
    bar_canvas->setRotation(0);
    
    mode_canvas = new M5Canvas(display);
    mode_canvas->setColorDepth(16);
    mode_canvas->createSprite(85, 100);
    mode_canvas->setRotation(0);
        
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