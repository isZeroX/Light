/**
 * @file lit_msgcheck.cpp
 * @author isZeroX@outlook.com
 * @brief
 * 消息检测
 * @version 0.1
 * @date 2022-06-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <ArduinoJson.h>
#include <lit_msgcheck.h>
#include <lit_eeprom.h>
#include <lit_Led.h>
#include <lit_mqtt.h>
#include <lit_wifi.h>

#define USE_JSON //数据传输比较慢

//获取设备信息
#define GETDATA_ALL 0
#define GETDATA_WIFI 1
#define GETDATA_MQTT 2
#define GETDATA_TCP 3
#define GETDATA_LIGHT 4

extern bool defWifiLink;
extern bool wifiLinkFlag;

//串口检测
void chkSerialMsg()
{
    String comdata = "";
    while (Serial.available() > 0)
    {
        comdata += char(Serial.read());
        delay(1);
    }
    if (comdata.length() > 0)
    {
        message_check(comdata, MSG_SERIAL);
    }
}

/**
 * @brief 消息检测
 *
 * @param msg 消息内容
 * @param source 消息来源
 */
void message_check(String msg, uint8_t source)
{
#ifdef USE_JSON
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (!err) //检查反序列化是否成功
    {
        ////////////////////////////////////////////////////////////////////////
        // user code begin
        String mode = doc["m"]; // mode
        if (mode == "l")        // led
        {
            // r - g - b - a 红-绿-蓝-存储
            ledUpdate_Color(doc["r"], doc["g"], doc["b"], doc["s"]);
        }
        else if (mode == "b") // led
        {
            // r - g - b - a 红-绿-蓝-存储
            LedUpdate_Bright(doc["b"]);
        }
        else if (mode == "ls")
        {

            setWifi_Data(doc["s"], doc["p"]);
            if ((String)doc["s"] == "")
            {
                Serial.println("turn color");
                setLedColor(COLOR_RED);
            }
            defWifiLink = initWIFILink(); //直接初始化wifi
        }
        else if (mode == "di")
        {
            setWifiLocal_Data(doc["s"], doc["p"]);
        }
        else if (mode == "ms")
        {
            setServerIp(doc["a"], doc["b"], doc["c"], doc["d"]);
            setSeerverPort(doc["p"]);
            initMQTT(wifiLinkFlag);
        }
        else if (mode == "pc")
        {
            setPubClient(doc["p"]);
            if (doc["r"])
            {
                disMqtt();
            }
            initMQTT(wifiLinkFlag);
        }
        else if (mode == "sc")
        {
            setSubClient(doc["s"], doc["n"]);
            if (doc["r"])
            {
                disMqtt();
            }
            initMQTT(wifiLinkFlag);
        }
        // user code end
        ////////////////////////////////////////////////////////////////////////
    }

    ////////////////////////////////////////////////////////////////////////
    // user code begin
    else if (source == MSG_MQTT && msg == CALL_PHONE)
    {
        sendData(CALL_ESP);
        // Serial.println(ESPCALL);
        // client.publish(mqttPubClient.c_str(), ESPCALL); // light ok
    }
    // user code end
    ////////////////////////////////////////////////////////////////////////
    Serial.print("c<<");
    Serial.println(msg);
#else

#endif
}

/**
 * @brief 获取esp数据
 *  GETDATA_ALL 0
 *  GETDATA_WIFI 1
 *  GETDATA_MQTT 2
 *  GETDATA_TCP 3
 *  GETDATA_LIGHT 4
 * @param msg 获取类型
 * @return String 返回数据
 */
String getEspData(uint8_t msg)
{
    StaticJsonDocument<256> doc;

    switch (msg)
    {
    case GETDATA_ALL:
        break;
    case GETDATA_WIFI:
        doc["m"] = "wifi";         //添加一个字符串对象节点
        doc["s"] = getWifi_ssid(); //添加一个字符串对象节点
        doc["p"] = getWifi_pass(); //添加一个字符串对象节点
        break;
    case GETDATA_MQTT:

        break;
    case GETDATA_TCP:

        break;
    case GETDATA_LIGHT:
        break;
    }
    String espData = "";
    serializeJson(doc, espData);
    // tcp wifi信息

    //服务器信息
    //灯光信息
    //当前状态
    return espData;
}
