#pragma once

#include "BaseScreen.hpp"

// ボタンモード設定画面クラス
class ButtonModeScreen : public BaseScreen {  
private:
  M5Canvas *button_grid_canvas;
  M5Canvas *info_canvas;

  int selected_button = -1;  // 選択中のボタンインデックス
  bool mode_changed = false; // 変更フラグ

  void drawButtonModeScreen() {
    // タイトル
    display->setTextColor(TFT_WHITE);
    display->setTextSize(1);
    String title = "BUTTON MODE CONFIGURATION";    
    int title_width = title.length() * 6;
    int title_x = (display->width() - title_width) / 2;    
    display->drawString(title, title_x, 5);
    
    // サブタイトル
    display->setTextColor(display->color565(180, 180, 180));
    display->setTextSize(1);
    String subtitle = "Touch buttons to toggle Absolute/Relative mode";    
    int subtitle_width = subtitle.length() * 6;
    int subtitle_x = (display->width() - subtitle_width) / 2;    
    display->drawString(subtitle, subtitle_x, 18);
    
    // ボタングリッド描画
    drawButtonGrid();
    
    // 情報パネル描画
    drawInfoPanel();
  }

  void drawButtonGrid() {
    button_grid_canvas->fillSprite(TFT_BLACK);
    
    String button_names[] = {"UP", "DOWN", "LEFT", "RIGHT", "SELECT", "START", "B", "A"};
    String button_descriptions[] = {
      "D-Pad Up", "D-Pad Down", "D-Pad Left", "D-Pad Right",
      "Select Button", "Start Button", "B Button", "A Button"
    };
    
    // 2x4のグリッドレイアウト
    int grid_cols = 2;
    int grid_rows = 4;
    int button_width = 110;
    int button_height = 45;
    int spacing_x = 10;
    int spacing_y = 8;
    
    for(int i = 0; i < 8; i++) {
      int col = i % grid_cols;
      int row = i / grid_cols;
      int x = col * (button_width + spacing_x);
      int y = row * (button_height + spacing_y);
      
      PressType button_mode = config.getButtonPressType(i);
      
      // ボタンの背景色（モード別）
      uint16_t bg_color;
      uint16_t border_color;
      if(button_mode == ABSOLUTE) {
        bg_color = button_grid_canvas->color565(70, 50, 20);     // 暗いオレンジ
        border_color = button_grid_canvas->color565(255, 180, 50); // オレンジ
      } else {
        bg_color = button_grid_canvas->color565(20, 50, 70);     // 暗い青
        border_color = button_grid_canvas->color565(100, 200, 255); // 青
      }
      
      // 選択されたボタンのハイライト
      if(i == selected_button) {
        border_color = TFT_WHITE;
        button_grid_canvas->fillRoundRect(x-2, y-2, button_width+4, button_height+4, 6, border_color);
      }
      
      // ボタン背景
      button_grid_canvas->fillRoundRect(x, y, button_width, button_height, 4, bg_color);
      button_grid_canvas->drawRoundRect(x, y, button_width, button_height, 4, border_color);
      
      // ボタン名（大きく）
      button_grid_canvas->setTextColor(TFT_WHITE);
      button_grid_canvas->setTextSize(1);
      int name_width = button_names[i].length() * 6;
      button_grid_canvas->drawString(button_names[i], x + (button_width - name_width)/2, y + 5);
      
      // モード表示（中央大きく）
      String mode_text = (button_mode == ABSOLUTE) ? "ABSOLUTE" : "RELATIVE";
      button_grid_canvas->setTextColor(border_color);
      button_grid_canvas->setTextSize(1);
      int mode_width = mode_text.length() * 6;
      button_grid_canvas->drawString(mode_text, x + (button_width - mode_width)/2, y + 18);
      
      // 現在の設定値
      float value = (button_mode == ABSOLUTE) ? 
                   config.getThresholdValue(i) : 
                   config.getAbsoluteValue(i);
      String value_str = String((int)(value * 100)) + "%";
      button_grid_canvas->setTextColor(button_grid_canvas->color565(200, 200, 200));
      int value_width = value_str.length() * 6;
      button_grid_canvas->drawString(value_str, x + (button_width - value_width)/2, y + 31);
    }
    
    button_grid_canvas->pushSprite(10, 35);
  }

  void drawInfoPanel() {
    info_canvas->fillSprite(TFT_BLACK);
    
    // 統計情報
    int absolute_count = 0;
    int relative_count = 0;
    for(int i = 0; i < 8; i++) {
      if(config.getButtonPressType(i) == ABSOLUTE) {
        absolute_count++;
      } else {
        relative_count++;
      }
    }
    
    // パネルタイトル
    info_canvas->setTextColor(TFT_WHITE);
    info_canvas->setTextSize(1);
    info_canvas->drawString("MODE SUMMARY", 5, 5);
    
    // 統計表示
    info_canvas->setTextColor(info_canvas->color565(255, 180, 50));
    info_canvas->drawString("Absolute: " + String(absolute_count) + " buttons", 5, 25);
    
    info_canvas->setTextColor(info_canvas->color565(100, 200, 255));
    info_canvas->drawString("Relative: " + String(relative_count) + " buttons", 5, 40);
    
    // 説明
    info_canvas->setTextColor(info_canvas->color565(150, 150, 150));
    info_canvas->setTextSize(1);
    info_canvas->drawString("ABSOLUTE: Fixed pressure threshold", 5, 65);
    info_canvas->drawString("RELATIVE: Dynamic baseline tracking", 5, 80);
    
    // 推奨設定表示
    info_canvas->setTextColor(info_canvas->color565(100, 255, 100));
    info_canvas->drawString("RECOMMENDED:", 5, 105);
    info_canvas->drawString("D-Pad + B: Relative (sensitive)", 5, 120);
    info_canvas->drawString("A, Start, Select: Absolute (stable)", 5, 135);
    
    // 選択されたボタンの詳細情報
    if(selected_button >= 0) {
      String button_names[] = {"UP", "DOWN", "LEFT", "RIGHT", "SELECT", "START", "B", "A"};
      
      info_canvas->drawFastHLine(0, 160, info_canvas->width(), info_canvas->color565(100, 100, 100));
      
      info_canvas->setTextColor(TFT_WHITE);
      info_canvas->drawString("SELECTED: " + button_names[selected_button], 5, 170);
      
      PressType button_mode = config.getButtonPressType(selected_button);
      String mode_text = (button_mode == ABSOLUTE) ? "ABSOLUTE" : "RELATIVE";
      uint16_t mode_color = (button_mode == ABSOLUTE) ? 
                           info_canvas->color565(255, 180, 50) : 
                           info_canvas->color565(100, 200, 255);
      
      info_canvas->setTextColor(mode_color);
      info_canvas->drawString("Mode: " + mode_text, 5, 185);
      
      float value = (button_mode == ABSOLUTE) ? 
                   config.getThresholdValue(selected_button) : 
                   config.getAbsoluteValue(selected_button);
      info_canvas->setTextColor(info_canvas->color565(200, 200, 200));
      info_canvas->drawString("Threshold: " + String((int)(value * 100)) + "%", 5, 200);
      
      // 現在の圧力値とボタン状態
      bool is_pressed = config.isPressed(selected_button);
      float current_pressure = config.getPressureValue(selected_button);
      
      info_canvas->setTextColor(is_pressed ? info_canvas->color565(100, 255, 100) : 
                                           info_canvas->color565(255, 100, 100));
      info_canvas->drawString("Status: " + String(is_pressed ? "PRESSED" : "RELEASED"), 5, 215);
      info_canvas->drawString("Pressure: " + String((int)(current_pressure * 100)) + "%", 5, 230);
    }
    
    info_canvas->pushSprite(10, 235);
  }

  void handleButtonModeTouch(int x, int y) {
    // ボタングリッドエリア
    if(x >= 10 && x <= 240 && y >= 35 && y <= 215) {
      int grid_cols = 2;
      int button_width = 110;
      int button_height = 45;
      int spacing_x = 10;
      int spacing_y = 8;
      
      int adjusted_x = x - 10;
      int adjusted_y = y - 35;
      
      int col = adjusted_x / (button_width + spacing_x);
      int row = adjusted_y / (button_height + spacing_y);
      
      if(col >= 0 && col < grid_cols && row >= 0 && row < 4) {
        int button_index = row * grid_cols + col;
        
        if(button_index >= 0 && button_index < 8) {
          // 同じボタンをタップした場合はモード切り替え
          if(selected_button == button_index) {
            PressType current_mode = config.getButtonPressType(button_index);
            PressType new_mode = (current_mode == ABSOLUTE) ? RELATIVE : ABSOLUTE;
            config.setButtonPressType(button_index, new_mode);
            mode_changed = true;
          } else {
            // 異なるボタンを選択
            selected_button = button_index;
          }
        }
      }
    }
    
    // 推奨設定の適用（ダブルタップで全体設定）
    // この機能は必要に応じて追加可能
  }

  void applyRecommendedSettings() {
    // 十字キー + Bボタンを相対モードに
    config.setButtonPressType(PAD_LEFT, RELATIVE);
    config.setButtonPressType(PAD_UP, RELATIVE);
    config.setButtonPressType(PAD_DOWN, RELATIVE);
    config.setButtonPressType(PAD_RIGHT, RELATIVE);
    config.setButtonPressType(BUTTON_B, RELATIVE);
    
    // その他を絶対モードに
    config.setButtonPressType(BUTTON_SELECT, ABSOLUTE);
    config.setButtonPressType(BUTTON_START, ABSOLUTE);
    config.setButtonPressType(BUTTON_A, ABSOLUTE);
    
    mode_changed = true;
  }

public:
  ButtonModeScreen(M5GFX* disp) : BaseScreen(disp) {
  }

  ~ButtonModeScreen() {
    if (button_grid_canvas) {
      button_grid_canvas->deleteSprite();
      delete button_grid_canvas;
    }
    if (info_canvas) {
      info_canvas->deleteSprite();      
      delete info_canvas;  
    }
  }
    
  void init() override {
    // キャンバス初期化
    button_grid_canvas = new M5Canvas(display);
    button_grid_canvas->setColorDepth(16);
    button_grid_canvas->createSprite(230, 200);
    button_grid_canvas->setRotation(0);
    
    info_canvas = new M5Canvas(display);
    info_canvas->setColorDepth(16);
    info_canvas->createSprite(230, 250);
    info_canvas->setRotation(0);
    
    selected_button = -1;
    mode_changed = false;
  }
  
  void update() override {
    // デモ用：圧力値をシミュレート（実際の実装では実センサー値を使用）
    for(int i = 0; i < 8; i++) {
      config.setPressureValue(i, (sin(millis() * 0.001 + i * 0.5) + 1.0) * 0.5);
    }
  }
  
  void draw() override {
    drawButtonModeScreen();
  }

  void handleTouch(int x, int y, bool pressed) override {
    auto t = M5.Touch.getDetail();
    if(t.wasPressed()) {
      handleButtonModeTouch(x, y);
    }
  }
  
  // 設定が変更されたかどうかを返す
  bool hasChanges() const {
    return mode_changed;
  }
  
  // 変更フラグをリセット
  void resetChangeFlag() {
    mode_changed = false;
  }
};