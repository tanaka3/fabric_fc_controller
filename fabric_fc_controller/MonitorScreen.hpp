#pragma once

#include "BaseScreen.hpp"

class MonitorScreen : public BaseScreen {  
private:
  M5Canvas *controller_canvas;
  M5Canvas *pressure_canvas;
  //float pressure_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // デモ用の圧力値
  //float threshold_values[8] = {0.6, 0.5, 0.7, 0.4, 0.8, 0.3, 0.65, 0.55}; // 各ボタンの基準値（0.0-1.0）

  void drawFamicomController(){
    //frame
    controller_canvas->fillRoundRect(0, 0, 240, 120, 3, controller_canvas->color565(166, 20, 31));

    controller_canvas->fillRoundRect(10, 10, 100, 100, 3, controller_canvas->color565(240, 235, 220));
    controller_canvas->fillRoundRect(10, 50, 220, 60, 3, controller_canvas->color565(240, 235, 220));

    // mark
    controller_canvas->fillRect(20, 20, 15, 15, TFT_BLACK);
    
    // pad
    controller_canvas->fillRect(42, 50, 20, 50, controller_canvas->color565(166, 20, 31));
    controller_canvas->fillRect(27, 65, 50, 20, controller_canvas->color565(166, 20, 31));
  
    controller_canvas->fillRect(44, 52, 16, 46, TFT_BLACK);
    controller_canvas->fillRect(29, 67, 46, 16, TFT_BLACK);

    // start / select 
    controller_canvas->fillRoundRect(92, 80, 60, 20, 4, controller_canvas->color565(166, 20, 31));
    controller_canvas->fillRect(102, 86, 15, 8, TFT_BLACK);
    controller_canvas->fillRect(127, 86, 15, 8, TFT_BLACK);

    // button mark
    controller_canvas->fillRoundRect(170, 65, 20, 8, 3, TFT_BLACK);
    controller_canvas->fillRoundRect(200, 65, 20, 8, 3, TFT_BLACK);

    //button
    controller_canvas->fillCircle(180, 90, 10, controller_canvas->color565(166, 20, 31));
    controller_canvas->fillCircle(210, 90, 10, controller_canvas->color565(166, 20, 31));

    controller_canvas->fillCircle(180, 90, 8, TFT_BLACK);
    controller_canvas->fillCircle(210, 90, 8, TFT_BLACK);

    controller_canvas->pushSprite(0,0);
  }

  void drawPressureValue(){
    // 背景をクリア
    pressure_canvas->fillSprite(TFT_BLACK);
    
    // グリッド線を描画
    int grid_color = pressure_canvas->color565(40, 40, 40);
    for(int i = 0; i <= 10; i++) {
      int y = pressure_canvas->height() - 40 - (i * (pressure_canvas->height() - 100) / 10);
      pressure_canvas->drawFastHLine(0, y, pressure_canvas->width(), grid_color);
    }
    
    // 各センサーのバーを描画
    for(int i = 0; i < 8; i++){
      int bar_width = 24;
      int bar_spacing = 6;
      int total_width = 8 * bar_width + 7 * bar_spacing;
      int start_x = (pressure_canvas->width() - total_width) / 2;
      int x = start_x + i * (bar_width + bar_spacing);
      
      int max_height = pressure_canvas->height() - 100;
      int bar_height = (int)(config.getPressureValue(i) * max_height);
      int y = pressure_canvas->height() - 40 - bar_height;
      int bar_top = pressure_canvas->height() - 40 - max_height;
      
      // バーの背景（暗いグレー）
      pressure_canvas->fillRect(x, pressure_canvas->height() - 40 - max_height, bar_width, max_height, 
                            pressure_canvas->color565(30, 30, 30));
      
      // 基準値ラインを描画
      int threshold_height = (int)(config.getThresholdValue(i) * max_height);
      int threshold_y = pressure_canvas->height() - 40 - threshold_height;
      
      // 基準値ライン（点線風）
      uint16_t threshold_line_color = pressure_canvas->color565(255, 255, 255);
      for(int line_x = x; line_x < x + bar_width; line_x += 3) {
        pressure_canvas->drawPixel(line_x, threshold_y, threshold_line_color);
        pressure_canvas->drawPixel(line_x, threshold_y + 1, threshold_line_color);
      }
      
      // 圧力レベルと基準値に応じた色分け
      uint16_t bar_color;
      if(config.getPressureValue(i) >= config.getThresholdValue(i)) {
        // 基準値以上：赤（警告）
        bar_color = pressure_canvas->color565(255, 50, 50);
      } else {
        // 基準値以下：緑（正常）
        bar_color = pressure_canvas->color565(50, 255, 50);
      }
      
      // メインバー
      pressure_canvas->fillRect(x + 2, y, bar_width - 4, bar_height, bar_color);
      
      // グロー効果（淡い外枠）
      if(config.getPressureValue(i) > 0.1) {
        uint16_t glow_color;
        if(config.getPressureValue(i) >= config.getThresholdValue(i)) {
          // 基準値以上：赤いグロー（警告）
          glow_color = pressure_canvas->color565(255, 100, 100);
        } else {
          // 基準値以下：緑のグロー（正常）
          glow_color = pressure_canvas->color565(100, 255, 100);
        }
        pressure_canvas->drawRect(x + 1, y - 1, bar_width - 2, bar_height + 2, glow_color);
      }
      
      // 数値表示（バーの描画範囲内に制限）
      pressure_canvas->setTextColor(TFT_WHITE);
      pressure_canvas->setTextSize(1);
      String value_str = String((int)(config.getPressureValue(i) * 100)) + "%";
      int text_width = value_str.length() * 6;
      
      // 数値表示位置を計算（バーの最上部より上に行かないように制限）
      int text_y = y - 15;
      if(text_y < bar_top + 2) {
        text_y = bar_top + 2;
      }
      
      pressure_canvas->drawString(value_str, x + (bar_width - text_width)/2, text_y);
      
      // 基準値表示（小さく表示）
      pressure_canvas->setTextColor(pressure_canvas->color565(200, 200, 200));
      String threshold_str = String((int)(config.getThresholdValue(i) * 100));
      int threshold_text_width = threshold_str.length() * 6;
      pressure_canvas->drawString(threshold_str, x + (bar_width - threshold_text_width)/2, threshold_y - 10);
      
      // ボタンアイコン
      drawButtonIcon(i, x + bar_width/2, pressure_canvas->height() - 20);
    }
    
    // ベースライン
    pressure_canvas->drawFastHLine(0, pressure_canvas->height() - 40, pressure_canvas->width(), 
                                pressure_canvas->color565(240, 235, 220));
    
    // タイトル
    pressure_canvas->setTextColor(pressure_canvas->color565(240, 235, 220));
    pressure_canvas->setTextSize(1);
    String title = "PRESSURE SENSORS";
    int title_width = title.length() * 6;
    int title_x = (pressure_canvas->width() - title_width) / 2;
    pressure_canvas->drawString(title, title_x, 25);
    
    pressure_canvas->pushSprite(0, controller_canvas->height());
  }

  // ボタンアイコンを描画する関数
  void drawButtonIcon(int button_index, int center_x, int center_y) {
    uint16_t icon_color = pressure_canvas->color565(240, 235, 220);

    switch(button_index) {
      case PAD_UP: // UP (↑)
        pressure_canvas->fillTriangle(center_x, center_y-8, center_x-5, center_y+2, center_x+5, center_y+2, icon_color);
        break;
        
      case PAD_DOWN: // DOWN (↓)
        pressure_canvas->fillTriangle(center_x, center_y+8, center_x-5, center_y-2, center_x+5, center_y-2, icon_color);
        break;
        
      case PAD_LEFT: // LEFT (←)
        pressure_canvas->fillTriangle(center_x-8, center_y, center_x+2, center_y-5, center_x+2, center_y+5, icon_color);
        break;
        
      case PAD_RIGHT: // RIGHT (→)
        pressure_canvas->fillTriangle(center_x+8, center_y, center_x-2, center_y-5, center_x-2, center_y+5, icon_color);
        break;
        
      case BUTTON_SELECT: // SELECT
        pressure_canvas->fillRoundRect(center_x-8, center_y-3, 16, 10, 2, icon_color);
        pressure_canvas->setTextColor(TFT_BLACK);
        pressure_canvas->setTextSize(1);
        pressure_canvas->drawString("SL", center_x-5, center_y-2);
        break;
        
      case BUTTON_START: // START
        pressure_canvas->fillRoundRect(center_x-8, center_y-3, 16, 10, 2, icon_color);
        pressure_canvas->setTextColor(TFT_BLACK);
        pressure_canvas->setTextSize(1);
        pressure_canvas->drawString("ST", center_x-5, center_y-2);
        break;
        
      case BUTTON_B: // B Button
        pressure_canvas->fillCircle(center_x, center_y, 8, icon_color);
        pressure_canvas->setTextColor(TFT_BLACK);
        pressure_canvas->setTextSize(1);
        pressure_canvas->drawString("B", center_x-2, center_y-3);
        break;
        
      case BUTTON_A: // A Button
        pressure_canvas->fillCircle(center_x, center_y, 8, icon_color);
        pressure_canvas->setTextColor(TFT_BLACK);
        pressure_canvas->setTextSize(1);
        pressure_canvas->drawString("A", center_x-2, center_y-3);
        break;
    }
  }  
public:
  MonitorScreen(M5GFX* disp) : BaseScreen(disp) {
  }

  ~MonitorScreen() {
    if (controller_canvas){
      controller_canvas->deleteSprite();
      delete controller_canvas;
    }
    if (pressure_canvas){
      pressure_canvas->deleteSprite();
      delete pressure_canvas;
    }
  }
    
  void init() override{
    controller_canvas = new M5Canvas(display);
    controller_canvas->setColorDepth(16);
    controller_canvas->createSprite(display->width(), display->width()/2);
    controller_canvas->setRotation(0);

    pressure_canvas = new M5Canvas(display);
    pressure_canvas->setColorDepth(16);
    pressure_canvas->createSprite(display->width(), display->height() - display->width()/2);
    pressure_canvas->setRotation(0);
  }
  
  void update() override{
    // デモ用：圧力値をシミュレート
    for(int i = 0; i < 8; i++) {
      config.setPressureValue(i, (sin(millis() * 0.001 + i * 0.5) + 1.0) * 0.5);
      //pressure_values[i] = (sin(millis() * 0.001 + i * 0.5) + 1.0) * 0.5; // 0.0-1.0の範囲
    }    
  }
  
  void draw() override{
    //Serial.println("MonitorScreen draw()");
    drawFamicomController();
    drawPressureValue();
  }


  void handleTouch(int x, int y, bool pressed) override {
  }
};