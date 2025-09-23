#pragma once

#include <M5GFX.h>
#include "BaseScreen.hpp"

#include "SettingScreen.hpp"
#include "ConfigScreen.hpp"
#include "MonitorScreen.hpp"
#include "ButtonModeScreen.hpp"

#include <vector>

// 更新フラグ用のenum
enum SwipType {
    NONE_SWIP = 0,
    LEFT_SWIP = 1,
    RIGHT_SWIP = 2,
};

// 画面タイプ列挙
enum ScreenType {
    SCREEN_CONFIG = 0,
    SCREEN_MONITOR,
    SCREEN_SETTING,
    SCREEN_BUTTON_MODE,
    SCREEN_END
};

class ScreenManager {
private:
    M5GFX *display;
    BaseScreen* current_screen;

    uint8_t current_screen_type = SCREEN_MONITOR;

    ScreenType screen_list[SCREEN_END] = {SCREEN_CONFIG, SCREEN_BUTTON_MODE ,SCREEN_MONITOR, SCREEN_SETTING};
    
    bool clear_screen = false;

    //swip
    // スワイプ検出用の変数
    int touchStartX = 0;
    int touchStartY = 0;
    int touchEndX = 0;
    int touchEndY = 0;
    int lastTouchX = 0;  // 前回のタッチX座標
    unsigned long touchStartTime = 0;  // タッチ開始時刻
    bool touchStarted = false;
    bool swipeValid = true;  // スワイプが有効かどうか
    int swipeDirection = 0;  // スワイプ方向 (1:右, -1:左, 0:未定)

    // 設定可能なパラメータ
    const int SWIPE_THRESHOLD = 50;      // スワイプと判定する最小距離（ピクセル）
    const int VERTICAL_TOLERANCE = 160;   // 縦方向の許容範囲
    const unsigned long MAX_SWIPE_TIME = 800;  // 最大スワイプ時間（ミリ秒）
    const unsigned long MIN_SWIPE_TIME = 50;   // 最小スワイプ時間（ミリ秒）
    const int DIRECTION_THRESHOLD = 10;  // 方向判定のしきい値

    SwipType doSwip(int touch_x, int touch_y){
        // タッチ情報を取得
        auto touch_detail = M5.Touch.getDetail();
        
        if (touch_detail.wasPressed()) {

            // タッチが開始された
            touchStartX = touch_x;
            touchStartY = touch_y;
            lastTouchX = touchStartX;
            touchStartTime = millis();
            touchStarted = true;
            swipeValid = true;
            swipeDirection = 0;
        
        } else if (touch_detail.isPressed() && touchStarted) {
            // タッチ中 - 方向の一貫性をチェック
            int currentX = touch_x;
            int deltaFromLast = currentX - lastTouchX;
            int deltaFromStart = currentX - touchStartX;
            
            // 方向が一定以上変化した場合のみチェック
            if (abs(deltaFromLast) > DIRECTION_THRESHOLD) {
                
                // 初回の方向設定
                if (swipeDirection == 0 && abs(deltaFromStart) > DIRECTION_THRESHOLD) {
                    swipeDirection = (deltaFromStart > 0) ? 1 : -1;  // 1:右, -1:左
                }
                
                // 方向の一貫性チェック
                if (swipeDirection != 0) {
                if ((swipeDirection == 1 && deltaFromLast < -DIRECTION_THRESHOLD) ||
                    (swipeDirection == -1 && deltaFromLast > DIRECTION_THRESHOLD)) {
                    // 逆方向への移動を検出
                    swipeValid = false;
                }
                }
                
                lastTouchX = currentX;
            }
            
        } else if (touch_detail.wasReleased() && touchStarted) {
            // タッチが離された
            touchEndX = touch_x;
            touchEndY = touch_y;
            unsigned long swipeTime = millis() - touchStartTime;
            
            touchStarted = false;

            int deltaX = touchEndX - touchStartX;
            int deltaY = abs(touchEndY - touchStartY);

            // 時間チェック
            bool timeValid = (swipeTime >= MIN_SWIPE_TIME && swipeTime <= MAX_SWIPE_TIME);
            // 距離チェック
            bool distanceValid = (abs(deltaX) > SWIPE_THRESHOLD && deltaY < VERTICAL_TOLERANCE);  

            // 全体的な有効性チェック
            bool overallValid = timeValid && distanceValid && swipeValid;
            
            if (overallValid) {
                if (deltaX > 0) {
                    return RIGHT_SWIP;
                } else {
                    return LEFT_SWIP;
                }
            }   
        }

        return NONE_SWIP;
    }     
    
public:
    ScreenManager(M5GFX* m5gfx) : display(m5gfx), current_screen(nullptr) {
        //setScreen(SCREEN_MONITOR);
    }
    
    ~ScreenManager() {
        if (current_screen) delete current_screen;
    }
    
    // void setScreen(BaseScreen* screen) {
    //     if (current_screen) delete current_screen;
    //     current_screen = screen;

    //     if (current_screen) current_screen->init();
    // }

    // 方法1: 画面タイプを指定して自動生成
    bool setScreen(ScreenType type) {

        BaseScreen *screen;
        
        switch(type) {
            case SCREEN_SETTING:
                screen = new SettingScreen(display);
                break;
            case SCREEN_MONITOR:
                screen = new MonitorScreen(display);
                break;
            case SCREEN_CONFIG:
                screen = new ConfigScreen(display);
                break;
            case SCREEN_BUTTON_MODE:
                screen = new ButtonModeScreen(display);
                break;
        }
        
        if (screen) {
            display->fillScreen(TFT_BLACK);

            if (current_screen) {
                delete current_screen;
                current_screen = nullptr;
            }
            
            clear_screen = true;

            current_screen = screen;

            current_screen_type = type;
            current_screen->init();
            return true;
        }
        return false;
    }

    
    void update() {
        if(clear_screen){
            display->fillRect(0, 0, display->width(), display->height(), TFT_BLACK); 
            clear_screen = false;
        }

        if (current_screen) {
            current_screen->update();
            current_screen->draw();
        }
    }
    
    void handleTouch() {
        if (!current_screen) return;
        
        auto t = M5.Touch.getDetail();

        int touch_x = 240 - t.y;
        int touch_y = t.x;

        current_screen->handleTouch(touch_x, touch_y, true);

        // 画面切り替え
        SwipType swip_state = doSwip(touch_x, touch_y);

        if(swip_state != NONE_SWIP){    
            int index = current_screen_type + (swip_state ==  LEFT_SWIP  ? 1 : -1);
            if(index >= 0 && index < (int)SCREEN_END){
                setScreen(screen_list[index]);
            }
        }        
    }
};
