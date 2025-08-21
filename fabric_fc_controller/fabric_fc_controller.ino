
#include <M5Unified.h>
#include <M5GFX.h>
#include <MCP3208.h>

#include "ScreenManager.hpp"
#include "Config.hpp"
#include "switch_tinyuse_custom_esp32.h"

#define MAX_ADC_VALUE 3500 // 上限は少し低めに設定
const int ADC_TASK_PRIORITY = 1;    // タスクの優先度
const int ADC_TASK_STACK_SIZE = 4096; // スタックサイズ

M5GFX display;
ScreenManager screen_manager(&display);

MCP3208 adc;
TaskHandle_t adcTaskHandle;
volatile bool adcTaskRunning = false;
portMUX_TYPE adcMux = portMUX_INITIALIZER_UNLOCKED;

NSGamepad Gamepad;
bool resetFlg = false;
HID_NSGamepadReport_Data_t beforeData;

void setup() {
  
  Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg);

  adc.begin(6);
  startADCTask();

  display.begin();
  display.setRotation(0);

  screen_manager.setScreen(SCREEN_MONITOR);

  Gamepad.begin();

  while(!Gamepad.ready()){
      delay(100);
  }
}

void adcReadTask(void *parameter) {
  
  while (adcTaskRunning) {
    
    // 全チャンネルの値を読み取り
    for (int channel = 0; channel < 8; channel++) {
      float value = (float)adc.readADC(channel) / MAX_ADC_VALUE;
      
      if(value > 1.0f) value = 1.0f;

      // クリティカルセクション（データ保護）
      portENTER_CRITICAL(&adcMux);
      Config::getInstance().setPressureValue(channel, value);
      portEXIT_CRITICAL(&adcMux);
    }
    
    controll();

    vTaskDelay(1);
  }
  
  vTaskDelete(NULL);
}

// ADCタスクを開始
void startADCTask() {
  if (!adcTaskRunning) {
    adcTaskRunning = true;
    
    // Core 0でタスクを作成
    xTaskCreatePinnedToCore(
      adcReadTask,           // タスク関数
      "ADC_Read_Task",       // タスク名
      ADC_TASK_STACK_SIZE,   // スタックサイズ
      NULL,                  // パラメータ
      ADC_TASK_PRIORITY,     // 優先度
      &adcTaskHandle,        // タスクハンドル
      0                      // Core 0で実行
    );
    
  }
}

void loop() {

  M5.update();
  
  // タッチ処理
  screen_manager.handleTouch();

  // 画面の更新
  screen_manager.update();
}

void controll(){

  Gamepad.reset();

  // 他のキーが押下されている場合、スタートが無効となるため
  // スタートが押されている場合は、他のボタンを無視する
  if(Config::getInstance().isPressed(BUTTON_START)){

    // スタート押下状態で、他のボタンが押されている場合は
    // ゲームをリセットする
    if(Config::getInstance().isPressed(BUTTON_SELECT)){

      if(resetFlg){
        return;
      }

      //リセットフラグを有効し、複数回の実行を防ぐ
      resetFlg = true;
      resetControll();

    }
    else{
      resetFlg = false;
      Gamepad.press(NSButton_Plus);
    }
  }
  else{
    resetFlg = false;

    if(Config::getInstance().isPressed(BUTTON_SELECT)){
      Gamepad.press(NSButton_Minus);         
    }

    if(Config::getInstance().isPressed(BUTTON_A)){
      Gamepad.press(NSButton_A);
    }    

    if(Config::getInstance().isPressed(BUTTON_B)){
      Gamepad.press(NSButton_B);
    }    

    Gamepad.dPad(Config::getInstance().isPressed(PAD_UP), 
                Config::getInstance().isPressed(PAD_DOWN), 
                Config::getInstance().isPressed(PAD_LEFT), 
                Config::getInstance().isPressed(PAD_RIGHT));

  }

  // 前回と入力が同じ場合は、ボタン操作を送信しない
  if(!Gamepad.compareTo(beforeData)){
    Gamepad.SendReport();
    beforeData = Gamepad.getReportData();
  }

}

/**
 * @brief ファミコンのリセット操作 
 * ZL＋ZR押してメニューのリセットを選ぶ操作
 */
void resetControll(){

  switch(Config::getInstance().getResetType()){
    case WORLD:
      Gamepad.reset();
      Gamepad.press(NSButton_LeftThrottle);
      Gamepad.press(NSButton_RightThrottle);  
      Gamepad.SendReport();
      delay(100);
      
      Gamepad.reset();
      Gamepad.press(NSButton_A);
      Gamepad.SendReport();
      break;

  case FC:
    Gamepad.reset();
    Gamepad.press(NSButton_LeftThrottle);
    Gamepad.press(NSButton_RightThrottle);  
    Gamepad.SendReport();
    delay(100);

    for(int i=0; i<3; i++){
      Gamepad.reset();
      Gamepad.dPad(true, false, false, false);
      Gamepad.SendReport();
      delay(150);
      Gamepad.reset();
      Gamepad.SendReport();
      delay(150);
    }
    
    Gamepad.reset();
    Gamepad.press(NSButton_A);
    Gamepad.SendReport();
    break;
  }
}