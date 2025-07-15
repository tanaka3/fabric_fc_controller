#pragma once

#include <EEPROM.h>

// 更新フラグ用のenum
enum ResetType {
    FC = 0,
    WORLD = 1,
};

class Config{
  private:
    Config() {
        init();
    }
    
    // コピーコンストラクタとコピー代入演算子を削除
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // 初期化処理
    void init() {
        // EEPROM初期化（512バイト確保）
        EEPROM.begin(512);
        
        // EEPROMから閾値を読み込み
        loadThresholdValues();
        
        initialized = true;
    }

    // EEPROMから閾値を読み込み
    void loadThresholdValues() {
        // マジックナンバーをチェック（初回起動判定）
        uint32_t magic = 0;
        EEPROM.get(EEPROM_MAGIC_ADDR, magic);
        
        if (magic != EEPROM_MAGIC_NUMBER) {
            // 初回起動時：デフォルト値を設定してEEPROMに保存
            setDefaultThresholdValues();
            saveThresholdValues();
        } else {
            // 既存データを読み込み
            for (int i = 0; i < 8; i++) {
                EEPROM.get(EEPROM_THRESHOLD_ADDR + (i * sizeof(float)), threshold_values[i]);
            }
        }
    }
    
    // デフォルト閾値を設定
    void setDefaultThresholdValues() {
        float default_values[8] = {0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8};
        for (int i = 0; i < 8; i++) {
            threshold_values[i] = default_values[i];
        }
    }
    
    // 閾値をEEPROMに保存
    void saveThresholdValues() {
        /*
        // マジックナンバーを書き込み
        EEPROM.put(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_NUMBER);
        
        // 閾値を書き込み
        for (int i = 0; i < 8; i++) {
            EEPROM.put(EEPROM_THRESHOLD_ADDR + (i * sizeof(float)), threshold_values[i]);
        }
        
        // EEPROMに実際に書き込み
        EEPROM.commit();
        */
    }

public:
    // インスタンス取得メソッド（Arduino用シンプル版）
    static Config& getInstance() {
        static Config instance;
        return instance;
    }
    
    // 初期化済みかどうかのチェック
    bool isInitialized() const {
        return initialized;
    }

    // 圧力値の管理
    void setPressureValue(int index, float value) {
        if (index >= 0 && index < 8) {
            pressure_values[index] = value;
        }
    }
    
    float getPressureValue(int index) const {
        if (index >= 0 && index < 8) {
            return pressure_values[index];
        }
        return 0.0; // エラー時のデフォルト値
    }
    
    // 全ての圧力値を設定
    void setPressureValues(const float values[8]) {
        for (int i = 0; i < 8; i++) {
            pressure_values[i] = values[i];
        }
    }
    
    // 全ての圧力値を取得
    const float* getPressureValues() const {
        return pressure_values;
    }
    
    // 閾値の管理（EEPROMへの自動保存付き）
    void setThresholdValue(int index, float value) {
        if (index >= 0 && index < 8) {
            threshold_values[index] = value;
            saveThresholdValues(); // 変更時に自動保存
        }
    }
    
    float getThresholdValue(int index) const {
        if (index >= 0 && index < 8) {
            return threshold_values[index];
        }
        return 0.0; // エラー時のデフォルト値
    }
    
    // 全ての閾値を設定（EEPROMへの自動保存付き）
    void setThresholdValues(const float values[8]) {
        for (int i = 0; i < 8; i++) {
            threshold_values[i] = values[i];
        }
        saveThresholdValues(); // 変更時に自動保存
    }
    
    // 全ての閾値を取得
    const float* getThresholdValues() const {
        return threshold_values;
    }
    
    // 閾値を工場出荷時の値にリセット
    void resetThresholdValues() {
        setDefaultThresholdValues();
        saveThresholdValues();
    }
    
    // 手動でEEPROMに保存（通常は自動保存されるため不要）
    void saveToEEPROM() {
        saveThresholdValues();
    }

private:
    // EEPROM関連の定数
    static constexpr uint32_t EEPROM_MAGIC_NUMBER = 0x12345678;  // マジックナンバー
    static constexpr int EEPROM_MAGIC_ADDR = 0;                  // マジックナンバー保存位置
    static constexpr int EEPROM_THRESHOLD_ADDR = 4;              // 閾値保存開始位置
        
    // メンバー変数
    bool initialized = false;

    ResetType reset_type = FC;

    float pressure_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // デモ用の圧力値
    float threshold_values[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // 各ボタンの基準値（0.0-1.0）


};