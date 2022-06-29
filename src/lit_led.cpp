/**
 * @file lit_led.cpp
 * @author isZerox@outlook.com
 * @brief 
 * 灯光驱动-用于对led灯光修改 和状态指示灯修改
 * rgb绿色和ws2812硬件引脚重复了，下个版本修复
 * 
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Arduino.h>
#include <FastLED.h>
#include <lit_eeprom.h>
#include <lit_Led.h>

#define DEBUG

#define NUM_LEDS 16        // led数量
#define LEDFOFF 0          // io端口赋值
#define LEDFON 1           // io端口赋值
#define LEDS_PIN_R D4      // gpio2 红色
#define LEDS_PIN_G D2      // gpio4 绿色
#define LEDS_PIN_B D1      // gpio5 蓝色
#define LED2812_PIN D2     // gpio4 WS2812
uint8_t ledStateColor = 0; // led颜色-状态指示led
uint8_t led_bright = 0;    //亮度变量
CRGB leds[NUM_LEDS];

/**
 * @brief 灯光更新
 *
 * @param r 红色数值
 * @param g 绿色数值
 * @param b 蓝色数值
 * @param save 是否保存
 */
void ledUpdate_Color(uint8_t r, uint8_t g, uint8_t b, bool save)
{
    CRGB rgbColor(r, g, b);
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        fill_solid(leds, NUM_LEDS, rgbColor);
        FastLED.show();
        delay(1);
        FastLED.show();
    }
    if (save) //因eeprom只有100w次数擦写上限，所有确定后再保存至eeprom
    {
        setLedData(r, g, b);
    }
}

/**
 * @brief 单颗led更新
 *
 * @param r 红色数值
 * @param g 绿色数值
 * @param b 蓝色数值
 * @param num 灯珠位置
 */
void ledUpdate_ColorOne(uint8_t r, uint8_t g, uint8_t b, uint8_t add)
{
    if (add < NUM_LEDS)
    {
    }
}

/**
 * @brief 灯光亮度更新
 *
 * @param bright 亮度数值 0-255
 */
void LedUpdate_Bright(uint8_t bright)
{
    FastLED.setBrightness(bright);
    ledUpdate_Color(getLedR(), getLedG(), getLedB(), false); //获取eeprom中存储的灯光信息,并显示
    FastLED.show();
    setLedBright(bright);
    led_bright = bright;
}

/**
 * @brief led初始化
 *
 * @param firstRun 首次运行
 */
void initLED(bool firstRun)
{

#ifdef DEBUG

    Serial.println("==Init LED==");

#endif // DEBUG

    if (firstRun || getLedDefDate()) //获取灯光数据信息
    {
        setLedData(255, 0, 0);
        setLedBright(32);
    }
    led_bright = getLedBright();                               //亮度
    pinMode(LED2812_PIN, OUTPUT);                              // ws2812引脚
    pinMode(LEDS_PIN_R, OUTPUT);                               //红色引脚
    pinMode(LEDS_PIN_G, OUTPUT);                               // 绿色引脚IO冲突
    pinMode(LEDS_PIN_B, OUTPUT);                               //蓝色引脚
    setLedColor(COLOR_RED);                                    //设定状态指示
    FastLED.addLeds<WS2812, LED2812_PIN, GRB>(leds, NUM_LEDS); // led初始化
    FastLED.setBrightness(led_bright);                         //设定亮度
    ledUpdate_Color(getLedR(), getLedG(), getLedB(), false);   //获取eeprom中存储的灯光信息,并显示

#ifdef DEBUG
    Serial.print("==LED_R:");
    Serial.print(getLedR());
    Serial.print("  >LED_G:");
    Serial.print(getLedG());
    Serial.print("  >LED_B:");
    Serial.print(getLedB());
    Serial.print("  >LED_Br:");
    Serial.print(getLedBright());
    Serial.println();
#endif // DEBUG
}

/**
 * @brief Set the Led Color object
 * COLOR_BLOCK 0     //全灭
 * COLOR_BLUE 1      //蓝色
 * COLOR_GREEN 2     //绿色
 * COLOR_CYAN_BLUE 3 //青色
 * COLOR_RED 4       //红色
 * COLOR_OCEAN_RED 5 //洋红色
 * COLOR_YELLO 6     //黄色
 * COLOR_WHITE 7     //白色
 * @param color
 */
void setLedColor(uint8_t color)
{
    digitalWrite(LEDS_PIN_B, !(color & 0x01));
    digitalWrite(LEDS_PIN_G, !((color >> 1) & 0x01));
    digitalWrite(LEDS_PIN_R, !((color >> 2) & 0x01));
    ledStateColor = color;
}

/**
 * @brief 获取led当前状态
 *
 * @return uint8_t led状态
 * COLOR_BLOCK 0     //全灭
 * COLOR_BLUE 1      //蓝色
 * COLOR_GREEN 2     //绿色
 * COLOR_CYAN_BLUE 3 //青色
 * COLOR_RED 4       //红色
 * COLOR_OCEAN_RED 5 //洋红色
 * COLOR_YELLO 6     //黄色
 * COLOR_WHITE 7     //白色
 */
uint8_t getLedStateColor()
{
    return ledStateColor;
}