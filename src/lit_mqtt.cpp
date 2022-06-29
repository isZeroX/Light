/**
 * @file lit_mqtt.cpp
 * @author isZeroX@outlook.com
 * @brief
 * @version 0.1
 * @date 2022-06-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <lit_eeprom.h>
#include <lit_led.h>
#include <lit_msgcheck.h>

#define DEBUG
// #define DEBUG_INIT

WiFiClient espClient;
PubSubClient client(espClient);

String mqttHost = "";      // host
String mqttId = "";        //设备id
String mqttPubClient = ""; //发布主题,初始化函数中初始化
bool mqttInitFlag = false; // mqtt初始化完成标志

#define SUB_NUM 3 //订阅主题上限

/**
 * @brief 统一发送 串口-mqtt
 *
 * @param msg 发送数据
 */
void sendData(String msg)
{
    client.publish(mqttPubClient.c_str(), (char *)msg.c_str());
    // Serial.print("e>>");
    // Serial.println(msg);
}

/**
 * @brief mqttCallback回调函数
 *
 * @param topic 接受数据
 * @param payload 数据
 * @param length 长度
 */
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    String str = "";
    for (unsigned int i = 0; i < length; i++)
    {
        str += (char)payload[i];
    }
    message_check(str, MSG_MQTT);
}

/**
 * @brief 始化mqtt
 *
 * @param wifiLink
 * false 初始化变量后return
 * true 初始化mqtt
 */
void initMQTT(bool wifiLink)
{
#ifdef DEBUG_INIT
    setServerIp(192, 168, 1, 104);
    setServerIp(192, 168, 10, 101);
    setSeerverPort(1883);
    subClear();
    setPubClient("esp0001");
    setSubClient("phone0001", 1);
#endif // DEBUG_INIT

    mqttHost = getServerIp();
    mqttId = ("LIGHT+ " + getWifiLocal_ssid());
    mqttPubClient = getPubClient();

#ifdef DEBUG

    Serial.print("==");
    Serial.print(mqttHost);
    Serial.print(":");
    Serial.println(getServerPort());
    Serial.println("==init mqtt.");
    Serial.print("==pubClient:");
    Serial.println(mqttPubClient);

#endif // DEBUG

    //没有发布，没有订阅，没有连接wifi均返回
    if (mqttPubClient == "" || !getSubNum() || !wifiLink)
    {
        if (mqttPubClient == "")
        {
            Serial.println("mqtt not pub .");
        }
        if (!getSubNum())
        {
            Serial.println("mqtt not sub .");
        }
        if (!wifiLink)
        {
            Serial.println("mqtt not link .");
        }

        mqttInitFlag = false;
        return;
    }

    client.setServer(mqttHost.c_str(), getServerPort()); //服务器和端口
    client.setKeepAlive(getHeart());                     //设置心跳间隔时间
    client.setCallback(mqttCallback);                    //设置回调函数
    mqttInitFlag = true;                                 //
}

/**
 * @brief 断开mqtt
 *
 */
void disMqtt()
{
    client.disconnect();
}

/**
 * @brief mqtt连接检查
 *
 * @return true mqtt已经连接
 * @return false mqtt未连接
 */
bool mqttConnectCheck()
{
    if (client.connected()) //如果已经正常连接
    {
        return true;
    }
    if (!client.connected() && mqttInitFlag) //如果未连接并且初始化完成
    {
        if (client.connect(mqttId.c_str()))
        {
            Serial.println();
            Serial.println("==emqx link ok!");

            uint8_t subNum = getSubNum();
            if (subNum & 0x01)
            {
                client.subscribe(getSubClient1().c_str()); //订阅报文1
                Serial.print("SUB1:");
                Serial.println(getSubClient1());
            }
            if ((subNum >> 1) & 0x01)
            {
                client.subscribe(getSubClient2().c_str()); //订阅报文2
                Serial.print("SUB2:");
                Serial.println(getSubClient2());
            }
            if ((subNum >> 2) & 0x01)
            {
                client.subscribe(getSubClient3().c_str()); //订阅报文3
                Serial.print("SUB3:");
                Serial.println(getSubClient3());
            }

            sendData(CALL_ESP);
            return true;
        }
        else
        {
            Serial.print("==emqx link err! ");
            Serial.println(client.state());
            Serial.println("");
            return false;
        }
    }

    return false;
}

/**
 * @brief
 *
 */
void runMQTT()
{
    client.loop();
}
