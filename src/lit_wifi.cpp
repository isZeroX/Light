/**
 * @file litwifi.cpp
 * @author isZeroX@outlook.com
 * @brief
 * 此文件时对wifi进行操作
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <wifiClient.h>
#include <ESP8266WiFi.h>
#include <lit_wifi.h>
#include <lit_eeprom.h>
#include <lit_msgcheck.h>

#define DEBUG

#define MAX_SRV_CLIENTS 3 //最大连接数量
WiFiClient serverClients[MAX_SRV_CLIENTS];
WiFiServer server(8080); //声明服务器对象

/****************************************************************
 * 此区域内函数对程序启动和更改wifi信息后对WiFi重新初始化
 * ****************************************************************/
/**
 * @brief 初始化wifi
 *
 * @param firstRun 首次运行
 * true 首次运行
 * false 非首次运行
 *
 * @return true  eeprom内存在默认wifi
 * @return false eeprom内不存在默认wifi
 */
bool initWIFI(bool firstRun)
{
    if (firstRun)
        setWifiLocal_Data("0001", "root@admin"); //默认信息

    String apName = ("LIGHT+ " + getWifiLocal_ssid());
    String apPass = getWifiLocal_pass();
    IPAddress softLocal(192, 168, 128, 1);
    IPAddress softGateway(192, 168, 128, 1);
    IPAddress softSubnet(255, 255, 255, 0);
    WiFi.mode(WIFI_AP_STA);                                // AP_STA模式
    WiFi.setSleep(false);                                  //关闭STA模式下wifi休眠，提高响应速度
    WiFi.softAPConfig(softLocal, softGateway, softSubnet); //配置访问点
    WiFi.softAP(apName, apPass);
    server.begin();
    server.setNoDelay(true);

#ifdef DEBUG
    Serial.print("==IP address: ");
    Serial.print(WiFi.softAPIP());
    Serial.println("==");
    Serial.print("==softAPName: ");
    Serial.print(apName);
    Serial.println("==");
#endif

    return initWIFILink();
}

/**
 * @brief 初始化wifi连接
 *
 * @return true
 * @return false
 */
bool initWIFILink()
{
    String wifi_ssid = getWifi_ssid(); //调用eeprom中函数，获取wifi ssid
    String wifi_pass = getWifi_pass(); //调用eeprom中函数，获取wifi password
    if (wifiConnectCheck())
    {
        WiFi.disconnect(); //断开连接
        delay(1000);
    }

#ifdef DEBUG
    Serial.print("==Find wifi ");
    Serial.print(">");
    Serial.print(wifi_ssid);
    Serial.print("< >");
    Serial.print(wifi_pass);
    Serial.println("< ==");
    Serial.println("==Link wifi==");
#endif

    if (wifi_ssid != "") //存在要连接wifi
    {
        WiFi.hostname("LIGHT_" + getWifiLocal_ssid()); //更改主机名称
        WiFi.begin(wifi_ssid, wifi_pass);              //连接wifi
        return true;
    }
    return false;
}

/**
 * @brief wifi连接检测
 *
 * @return true 连接中
 * @return false 无连接
 */
bool wifiConnectCheck()
{
    if (WiFi.status() != WL_CONNECTED) //连接中
    {
        Serial.print(".");
        return false;
    }
    return true;
}

/**
 * @brief 本地tpc连接，新接入设备检测
 *
 */
void ClientClick()
{
    //检测新设备接入
    if (server.hasClient())
    {
#ifdef DEBUG
        Serial.println("==new connect");
#endif
        for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        {
            if (!serverClients[i] || !serverClients[i].connected())
            {
                if (serverClients[i])
                    serverClients[i].stop();           //未联接,就释放
                serverClients[i] = server.available(); //分配新的
                continue;
            }
        }
        WiFiClient serverClient = server.available();
        serverClient.stop();
    }
}

/**
 * @brief 消息检测
 *
 * @return true 有设备连接中
 * @return false  无设备连接
 */
bool MessageClick()
{
    bool tcpClien = false;
    String str = "";
    for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
    {
        if (serverClients[i] && serverClients[i].connected())
        {

            tcpClien = true; //有链接存在
            if (serverClients[i].available())
            {
                while (serverClients[i].available())
                {
                    // Serial.write(serverClients[i].read());
                    str += (char)serverClients[i].read();
                }
                message_check(str, MSG_TCP);
            }
        }
    }
    return tcpClien;
}
