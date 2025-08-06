#pragma once

#include "BaseScreen.hpp"

class ConfigScreen : public BaseScreen {
private:
  M5Canvas *config_canvas;
  static const int CARD_WIDTH = 210;
  static const int CARD_HEIGHT = 120;  
  static const int CARD_X = 15;
  static const int OPTION_HEIGHT = 35;
private:
  void drawResetType(){

    int card1_y = 35;

    // カード1背景
    config_canvas->fillRoundRect(ConfigScreen::CARD_X, card1_y, 
                                  ConfigScreen::CARD_WIDTH, ConfigScreen::CARD_HEIGHT, 8, 
                                  config_canvas->color565(39, 39, 42));

    // カード1タイトル
    config_canvas->setTextColor(config_canvas->color565(79, 172, 254));
    config_canvas->setTextSize(1);
    config_canvas->drawString("RESET TYPE", ConfigScreen::CARD_X + 15, card1_y + 12);

    // カード1の装飾ライン
    config_canvas->fillRect(ConfigScreen::CARD_X + 15, card1_y + 27,  
                            ConfigScreen::CARD_WIDTH - 30, 1, 
                            config_canvas->color565(71, 85, 105));

    // オプション1-1: Family Computer (タッチエリア拡大)
    int option1_y = card1_y + 40;

    // 選択背景（タッチエリア全体）
    if (Config::getInstance().getResetType() == FC) {
        config_canvas->fillRoundRect(ConfigScreen::CARD_X + 5, option1_y - 5, 
                                      ConfigScreen::CARD_WIDTH - 10, ConfigScreen::OPTION_HEIGHT, 6, 
                                      config_canvas->color565(59, 130, 246));
    }

    // ラジオボタン1-1（大きくする）
    int radio1_x = ConfigScreen::CARD_X + 25;
    int radio1_y = option1_y + 15;
    config_canvas->drawCircle(radio1_x, radio1_y, 8,  config_canvas->color565(248, 250, 252));
    config_canvas->drawCircle(radio1_x, radio1_y, 7, config_canvas->color565(248, 250, 252));
    if (Config::getInstance().getResetType() == FC) {
        config_canvas->fillCircle(radio1_x, radio1_y, 4, config_canvas->color565(248, 250, 252));
    }

    // テキスト1-1（大きくする）
    config_canvas->setTextColor(config_canvas->color565(248, 250, 252));
    config_canvas->setTextSize(1);
    config_canvas->drawString("Family Computer", radio1_x + 20, option1_y + 10);

    // オプション1-2: Powerpak
    int option2_y = option1_y + ConfigScreen::OPTION_HEIGHT + 5;

    // 選択背景
    if (Config::getInstance().getResetType() == WORLD) {
        config_canvas->fillRoundRect(ConfigScreen::CARD_X + 5, option2_y - 5,  
                                        ConfigScreen::CARD_WIDTH - 10, ConfigScreen::OPTION_HEIGHT, 6, 
                                        config_canvas->color565(59, 130, 246));
    }

    // ラジオボタン1-2
    int radio2_x = ConfigScreen::CARD_X + 25;
    int radio2_y = option2_y + 15;
    config_canvas->drawCircle(radio2_x, radio2_y, 8,config_canvas->color565(248, 250, 252) );
    config_canvas->drawCircle(radio2_x, radio2_y, 7, config_canvas->color565(248, 250, 252) );
    if (Config::getInstance().getResetType() == WORLD) {
        config_canvas->fillCircle(radio2_x, radio2_y, 4, config_canvas->color565(248, 250, 252));
    }

    // テキスト1-2
    config_canvas->setTextColor(config_canvas->color565(248, 250, 252));
    config_canvas->setTextSize(1);
    config_canvas->drawString("Nintendo World Champion", radio2_x + 20, option2_y + 10);

  } 

  void drawPressType(){
    int card2_y = ConfigScreen::CARD_HEIGHT + 45;

    // カード2背景
    config_canvas->fillRoundRect(ConfigScreen::CARD_X, card2_y,  
                                  ConfigScreen::CARD_WIDTH, ConfigScreen::CARD_HEIGHT, 8, 
                                  config_canvas->color565(39, 39, 42));

    // カード2タイトル
    config_canvas->setTextColor(config_canvas->color565(79, 172, 254));
    config_canvas->setTextSize(1);
    config_canvas->drawString("PRESS TYPE", ConfigScreen::CARD_X + 15, card2_y + 12);

    // カード2の装飾ライン
    config_canvas->fillRect(ConfigScreen::CARD_X + 15, card2_y + 27,  
                            ConfigScreen::CARD_WIDTH - 30, 1, 
                            config_canvas->color565(71, 85, 105));

    // オプション2-1: Standard
    int option3_y = card2_y + 40;

    // 選択背景
    if (Config::getInstance().getPressType() == ABSOLUTE) {
        config_canvas->fillRoundRect(ConfigScreen::CARD_X + 5, option3_y - 5,  
                                      ConfigScreen::CARD_WIDTH - 10, ConfigScreen::OPTION_HEIGHT, 6, 
                                      config_canvas->color565(59, 130, 246));
    }

    // ラジオボタン2-1
    int radio3_x = ConfigScreen::CARD_X + 25;
    int radio3_y = option3_y + 15;
    config_canvas->drawCircle(radio3_x, radio3_y, 8, config_canvas->color565(248, 250, 252) );
    config_canvas->drawCircle(radio3_x, radio3_y, 7, config_canvas->color565(248, 250, 252) );
    if (Config::getInstance().getPressType() == ABSOLUTE) {
        config_canvas->fillCircle(radio3_x, radio3_y, 4, config_canvas->color565(248, 250, 252));
    }

    // テキスト2-1
    config_canvas->setTextColor(config_canvas->color565(248, 250, 252) );
    config_canvas->setTextSize(1);
    config_canvas->drawString("Absolute", radio3_x + 20, option3_y + 10);

    // オプション2-2: Championships
    int option4_y = option3_y + ConfigScreen::OPTION_HEIGHT + 5;

    // 選択背景
    if (Config::getInstance().getPressType() == RELATIVE) {
        config_canvas->fillRoundRect(ConfigScreen::CARD_X + 5, option4_y - 5,  
                                      ConfigScreen::CARD_WIDTH - 10, ConfigScreen::OPTION_HEIGHT, 6, 
                                      config_canvas->color565(59, 130, 246));
    }

    // ラジオボタン2-2
    int radio4_x = ConfigScreen::CARD_X + 25;
    int radio4_y = option4_y + 15;
    config_canvas->drawCircle(radio4_x, radio4_y, 8, config_canvas->color565(248, 250, 252));
    config_canvas->drawCircle(radio4_x, radio4_y, 7, config_canvas->color565(248, 250, 252));
    if (Config::getInstance().getPressType() == RELATIVE) {
        config_canvas->fillCircle(radio4_x, radio4_y, 4, config_canvas->color565(248, 250, 252));
    }

    // テキスト2-2
    config_canvas->setTextColor(config_canvas->color565(248, 250, 252));
    config_canvas->setTextSize(1);
    config_canvas->drawString("RELATIVE", radio4_x + 20, option4_y + 10);
  }

public:
  ConfigScreen(M5GFX* disp) : BaseScreen(disp) {
  }

  ~ConfigScreen() {
    if (config_canvas){
      config_canvas->deleteSprite();
      delete config_canvas;
    }    
  }
    
  void init() override{
    config_canvas = new M5Canvas(display);
    config_canvas->setColorDepth(16);
    config_canvas->setRotation(0);   
    config_canvas->createSprite(240, 320);
 
  }
  
  void update() override{
  }
  
  void draw() override{

    // 背景をクリア
    config_canvas->fillSprite(TFT_BLACK);

    // タイトル部分を改善
    config_canvas->setTextColor(TFT_WHITE);
    config_canvas->setTextSize(1);
    String title = "CONTROLLER SETTINGS";    
    int title_width = title.length() * 6;
    int title_x = (display->width() - title_width) / 2;    
    config_canvas->drawString(title, title_x, 20);

    // カード共通設定
    drawResetType();
    drawPressType();

    config_canvas->pushSprite(0, 0);
  }

  void handleTouch(int x, int y, bool pressed) override {
    auto t = M5.Touch.getDetail();
    if(t.wasPressed()) {
      if(x >= 20 && x <= 80){
          if(y >= 65 && y <= 105){
            Config::getInstance().setResetType(FC);
          }

          if(y >= 115 && y <= 155){
            Config::getInstance().setResetType(WORLD);
          }

          if(y >= 195 && y <= 235){
            Config::getInstance().setPressType(ABSOLUTE);
          }

          if(y >= 245 && y <= 285){
            Config::getInstance().setPressType(RELATIVE);
          }

      }
    }
  }
};