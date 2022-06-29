/**
 * @file lit_led.h
 * @author your name (you@domain.com)
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

#ifndef LIT_LED_H
#define LIT_LED_H

//三色led
#define COLOR_BLOCK 0     //全灭
#define COLOR_BLUE 1      //蓝色
#define COLOR_GREEN 2     //绿色
#define COLOR_CYAN_BLUE 3 //青色
#define COLOR_RED 4       //红色
#define COLOR_OCEAN_RED 5 //洋红色
#define COLOR_YELLOW 6    //黄色
#define COLOR_WHITE 7     //白色

void initLED(bool firstRun);                                      //初始化led
void LedUpdate_Bright(uint8_t light);                             //更新亮度
void ledUpdate_Color(uint8_t r, uint8_t g, uint8_t b, bool save); //更新颜色
void setLedColor(uint8_t color);                                  //设定三色led颜色
uint8_t getLedStateColor();                                       //获取状态指示led颜色

#endif // !LIT_LED_H
