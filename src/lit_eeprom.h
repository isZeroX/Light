/**
 * @file lit_eeprom.h
 * @author isZeroX@outlook.com
 * @brief
 * 此文件主要对eeprom内数据进行操作，其中存储有wifi ssid password 和连接状态信息
 * 此文件initEEPROM应当被优先调用
 * 此文件中wifi ssid password获取和修改将允许被调用
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LIT_EEPROM_H
#define LIT_EEPROM_H

#include <WString.h>

bool initEEPROM();                                          //对eeprom进行初始化
void clearEEPROM();                                         //清除EEPROM数据
void clearWifi();                                           //清除wifi
String getWifi_ssid();                                      //获取wifi ssid
String getWifi_pass();                                      //获取wifi password
String getWifiLocal_ssid();                                 //获取wifi ssid
String getWifiLocal_pass();                                 //获取wifi password
void setWifi_Data(String wifi_ssid, String wifi_pass);      //重新设定wifi ssid 和 password
void setWifiLocal_Data(String wifi_ssid, String wifi_pass); //重新设定wifi ssid 和 password
String getServerIp();                                       //获取服务器Ip
//设置服务器ip
void setServerIp(uint8_t ipa, uint8_t ipb, uint8_t ipc, uint8_t ipd);
uint16_t getServerPort();                         //获取服务器端口
void setSeerverPort(uint16_t port);               //设定服务器端口
String getDeviceName();                           //获取本地id
void setDeviceName(String deviceName);            //设置本地id
uint8_t getHeart();                               //获取心跳
void setHeart(uint8_t heartbeat);                 //设定心跳
String getPubClient();                            //获取发布主题
void setPubClient(String pubclient);              //设定发布主题
uint8_t getSubNum();                              //获取订阅主题数量
String getSubClient1();                           //获取订阅主题1
String getSubClient2();                           //获取订阅主题2
String getSubClient3();                           //获取订阅主题3
bool setSubClient(String subClient, uint8_t add); //设定订阅主题
void subClear();                                  //清除所有订阅

// user code begin

bool getLedDefDate();                             //获取灯光信息
uint8_t getLedBright();                           //获取亮度
void setLedBright(uint8_t bright);                //设定亮度
void setLedData(uint8_t r, uint8_t g, uint8_t b); //设置灯光数据
uint8_t getLedR();                                //获取红色
uint8_t getLedG();                                //获取绿色
uint8_t getLedB();                                //获取蓝色

void debugInit();//debug

// user code end

#endif // !LIT_EEPROM_H
