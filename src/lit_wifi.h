/**
 * @file lit_wifi.h
 * @author isZeroX@outlook.com
 * @brief
 * 此文件用来进行在EEPROM中对默认连接wifi进行存储，当存在默认连接wifi时将自动进行连接，如果不存在将返回false，等待处理
 * 头文件中包含外部可以调用的功能，包括对wifi ssid和password更改的函数、获取wifi信息的函数、对wifi进行初始化的函数
 * 文件中包含对wifi ssid和password的读取以及设定函数
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LIT_WIFI_H
#define LTI_WIFI_H

bool initWIFI(bool wifiLinkFlag); //对wifi进行初始化
void ClientClick();               //新设备接入
bool MessageClick();              //新消极检测
bool wifiConnectCheck();          // wifi连接状态检测
bool initWIFILink();              //初始化wifi连接
#endif                            // !LIT_WIFI_H
