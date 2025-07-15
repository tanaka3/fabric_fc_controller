#pragma once

#include <M5GFX.h>
#include "Config.hpp"

class BaseScreen {
protected:
    M5GFX* display;
    Config &config = Config::getInstance();
public:
    BaseScreen(M5GFX* disp) :display(disp) {}
    virtual ~BaseScreen(){}
    
    // 純粋仮想関数：各画面で実装必須
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void handleTouch(int x, int y, bool pressed) = 0;
    
};
