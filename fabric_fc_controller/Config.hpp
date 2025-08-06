#pragma once

#include <EEPROM.h>

// 更新フラグ用のenum
enum ResetType {
    FC = 0,
    WORLD = 1,
};

enum PressType {
    ABSOLUTE = 0,
    RELATIVE,
};

enum FabricFC{
    PAD_LEFT = 0,
    PAD_UP,
    PAD_RIGHT,    
    PAD_DOWN,
    BUTTON_SELECT,
    BUTTON_START,
    BUTTON_B,
    BUTTON_A,
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
        setDefaultValues();
        // ベースライン値を初期化
        for (int i = 0; i < 8; i++) {
            baseline_values[i] = 0.0;
            previous_values[i] = 0.0;
            button_states[i] = false;
        }
        /*
        // EEPROM初期化（512バイト確保）
        if (!EEPROM.begin(512)) {
            Serial.println("EEPROM初期化に失敗しました");
            // デフォルト値を設定
            setDefaultValues();
            initialized = true;
            return;
        }
        
        // EEPROMから設定値を読み込み
        loadFromEEPROM();
        */
        initialized = true;
    }

     // EEPROMから設定値を読み込み
    void loadFromEEPROM() {
        // マジックナンバーをチェック（初回起動判定）
        uint32_t magic = 0;
        
        // バイト単位でマジックナンバーを読み込み
        for (int i = 0; i < 4; i++) {
            ((uint8_t*)&magic)[i] = EEPROM.read(EEPROM_MAGIC_ADDR + i);
        }
        
        if (magic != EEPROM_MAGIC_NUMBER) {
            // 初回起動時：デフォルト値を設定してEEPROMに保存
            setDefaultValues();
            saveToEEPROM();
        } else {
            // 既存データを読み込み
            // threshold_values を読み込み
            for (int i = 0; i < 8; i++) {
                uint8_t* float_bytes = (uint8_t*)&threshold_values[i];
                for (int j = 0; j < sizeof(float); j++) {
                    float_bytes[j] = EEPROM.read(EEPROM_THRESHOLD_ADDR + (i * sizeof(float)) + j);
                }
            }
            
            // absolute_values を読み込み
            for (int i = 0; i < 8; i++) {
                uint8_t* float_bytes = (uint8_t*)&absolute_values[i];
                for (int j = 0; j < sizeof(float); j++) {
                    float_bytes[j] = EEPROM.read(EEPROM_ABSOLUTE_ADDR + (i * sizeof(float)) + j);
                }
            }
            
            // reset_type を読み込み
            reset_type = (ResetType)EEPROM.read(EEPROM_RESET_TYPE_ADDR);
            
            // press_type を読み込み
            press_type = (PressType)EEPROM.read(EEPROM_PRESS_TYPE_ADDR);
        }
    }
    
    // デフォルト値を設定
    void setDefaultValues() {
        // デフォルト閾値
        float default_threshold[8] = {0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6};
        for (int i = 0; i < 8; i++) {
            threshold_values[i] = default_threshold[i];
        }
        
        // デフォルト相対値
        float default_absolute[8] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
        for (int i = 0; i < 8; i++) {
            absolute_values[i] = default_absolute[i];
        }
        
        // デフォルト設定
        reset_type = FC;
        press_type = RELATIVE;//ABSOLUTE;
    }
    
    // 設定値をEEPROMに保存
    void saveToEEPROM() {
        /*
        // マジックナンバーを書き込み
        uint32_t magic = EEPROM_MAGIC_NUMBER;
        for (int i = 0; i < 4; i++) {
            EEPROM.write(EEPROM_MAGIC_ADDR + i, ((uint8_t*)&magic)[i]);
        }
        
        // threshold_values を書き込み
        for (int i = 0; i < 8; i++) {
            uint8_t* float_bytes = (uint8_t*)&threshold_values[i];
            for (int j = 0; j < sizeof(float); j++) {
                EEPROM.write(EEPROM_THRESHOLD_ADDR + (i * sizeof(float)) + j, float_bytes[j]);
            }
        }
        
        // absolute_values を書き込み
        for (int i = 0; i < 8; i++) {
            uint8_t* float_bytes = (uint8_t*)&absolute_values[i];
            for (int j = 0; j < sizeof(float); j++) {
                EEPROM.write(EEPROM_ABSOLUTE_ADDR + (i * sizeof(float)) + j, float_bytes[j]);
            }
        }
        
        // reset_type を書き込み
        EEPROM.write(EEPROM_RESET_TYPE_ADDR, (uint8_t)reset_type);
        
        // press_type を書き込み
        EEPROM.write(EEPROM_PRESS_TYPE_ADDR, (uint8_t)press_type);
        
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

    // 有効かどうか（絶対値・相対値両対応）
    bool isPressed(int index){
        if (press_type == ABSOLUTE) {
            return getPressureValue(index) >= getThresholdValue(index);
        } else {
            return isPressedRelative(index);
        }
    }
    
    // 相対値による判定（改善版）
    bool isPressedRelative(int index) {
        if (index < 0 || index >= 8) return false;
        
        float current_value = getPressureValue(index);
        float baseline = baseline_values[index];
        float threshold = absolute_values[index]; // 相対値の閾値（例：0.1 = 10%）
        float previous_value = previous_values[index];
        
        // 現在の状態を取得
        bool current_state = button_states[index];
        
        // 変動方向を判定（前回値と比較）
        bool is_increasing = current_value > previous_value;
        bool is_decreasing = current_value < previous_value;
        
        if (!current_state) {
            // ボタンがOFFの場合
            if (is_increasing) {
                // 圧力値が上昇に転じた場合
                if (baseline_values[index] == 0.0 || current_value < baseline) {
                    // 初回または基準点より下がった場合は基準点を更新
                    baseline_values[index] = current_value;
                } else {
                    // 基準点からの増加率をチェック
                    float increase_rate = (current_value - baseline) / baseline;
                    if (increase_rate >= threshold) {
                        // 一定％増えたらONにする
                        button_states[index] = true;
                        previous_values[index] = current_value;
                        return true;
                    }
                }
            } else if (is_decreasing) {
                // 圧力が下がった場合は基準点を修正
                baseline_values[index] = current_value;
            }
        } else {
            // ボタンがONの場合
            if (is_decreasing) {
                // 圧力が減少に転じた場合
                if (current_value > baseline) {
                    // 基準点より上がった場合は基準点を更新
                    baseline_values[index] = current_value;
                } else {
                    // 基準点からの減少率をチェック
                    float decrease_rate = (baseline - current_value) / baseline;
                    if (decrease_rate >= threshold) {
                        // 一定％減ったらOFFにする
                        button_states[index] = false;
                        previous_values[index] = current_value;
                        return false;
                    }
                }
            } else if (is_increasing) {
                // 圧力が上がった場合は基準点を修正
                baseline_values[index] = current_value;
            }
        }
        
        // 前回値を更新
        previous_values[index] = current_value;
        return current_state;
    }
    
    // 現在の圧力値をベースラインとして設定（手動キャリブレーション用）
    void calibrateBaseline(int index) {
        if (index >= 0 && index < 8) {
            baseline_values[index] = pressure_values[index];
            previous_values[index] = pressure_values[index];
            button_states[index] = false;
        }
    }
    
    // 全てのボタンをキャリブレーション（手動キャリブレーション用）
    void calibrateAllBaselines() {
        for (int i = 0; i < 8; i++) {
            calibrateBaseline(i);
        }
    }

    // 圧力値の管理（自動ベースライン更新対応）
    void setPressureValue(int index, float value) {
        if (index >= 0 && index < 8) {
            pressure_values[index] = value;
            
            // 相対値モードの場合、自動ベースライン更新は isPressedRelative() 内で処理
            // ここでは値の設定のみ行う
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
            saveToEEPROM(); // 変更時に自動保存
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
        saveToEEPROM(); // 変更時に自動保存
    }
    
    // 全ての閾値を取得
    const float* getThresholdValues() const {
        return threshold_values;
    }
    
    // 相対値の閾値管理（EEPROMへの自動保存付き）
    void setAbsoluteValue(int index, float value) {
        if (index >= 0 && index < 8 && value >= 0.0 && value <= 1.0) {
            absolute_values[index] = value;
            saveToEEPROM(); // 変更時に自動保存
        }
    }
    
    float getAbsoluteValue(int index) const {
        if (index >= 0 && index < 8) {
            return absolute_values[index];
        }
        return 0.1; // デフォルト値
    }
    
    // 全ての相対値閾値を設定（EEPROMへの自動保存付き）
    void setAbsoluteValues(const float values[8]) {
        for (int i = 0; i < 8; i++) {
            if (values[i] >= 0.0 && values[i] <= 1.0) {
                absolute_values[i] = values[i];
            }
        }
        saveToEEPROM(); // 変更時に自動保存
    }
    
    // 全ての相対値閾値を取得
    const float* getAbsoluteValues() const {
        return absolute_values;
    }
    
    // 閾値を工場出荷時の値にリセット
    void resetThresholdValues() {
        setDefaultValues();
        saveToEEPROM();
    }
    
    // 手動でEEPROMに保存（通常は自動保存されるため不要）
    void saveConfig() {
        saveToEEPROM();
    }

    void setResetType(ResetType type){
        reset_type = type;
        saveToEEPROM(); // 変更時に自動保存
    }

    ResetType getResetType(){
        return reset_type;
    }

    void setPressType(PressType type){
        press_type = type;
        // 相対値モードに切り替える際は全ボタンの状態をリセット
        if (type == RELATIVE) {
            for (int i = 0; i < 8; i++) {
                button_states[i] = false;
                // 初期化時に現在の圧力値をベースラインとして設定
                baseline_values[i] = pressure_values[i];
                previous_values[i] = pressure_values[i];
            }
        }
        saveToEEPROM(); // 変更時に自動保存
    }

    PressType getPressType(){
        return press_type;
    }    
    
private:
    // EEPROM関連の定数
    static constexpr uint32_t EEPROM_MAGIC_NUMBER = 0x12345678;  // マジックナンバー
    static constexpr int EEPROM_MAGIC_ADDR = 0;                  // マジックナンバー保存位置 (4 bytes)
    static constexpr int EEPROM_THRESHOLD_ADDR = 4;              // 閾値保存開始位置 (32 bytes: 8 * 4)
    static constexpr int EEPROM_ABSOLUTE_ADDR = 36;              // 相対値保存開始位置 (32 bytes: 8 * 4)
    static constexpr int EEPROM_RESET_TYPE_ADDR = 68;            // reset_type保存位置 (1 byte)
    static constexpr int EEPROM_PRESS_TYPE_ADDR = 69;            // press_type保存位置 (1 byte)
    // 合計使用量: 70 bytes
    
    // ベースライン値を設定（内部使用）
    void setBaselineValue(int index, float value) {
        if (index >= 0 && index < 8) {
            baseline_values[index] = value;
            previous_values[index] = value;
            button_states[index] = false; // ベースライン設定時は状態をリセット
        }
    }
    
    // ベースライン値を取得（内部使用）
    float getBaselineValue(int index) const {
        if (index >= 0 && index < 8) {
            return baseline_values[index];
        }
        return 0.0;
    }
        
    // メンバー変数
    bool initialized = false;

    ResetType reset_type = FC;
    PressType press_type = ABSOLUTE;

    float pressure_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // デモ用の圧力値
    float threshold_values[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // 各ボタンの基準値（0.0-1.0）
    float absolute_values[8] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}; // 各ボタンの相対値 (0.0-1.0)
    
    // 相対値判定用の追加メンバー
    float baseline_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // ベースライン値（基準点）
    float previous_values[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // 前回の圧力値（変動方向判定用）
    bool button_states[8] = {false, false, false, false, false, false, false, false}; // ボタンの現在状態
};