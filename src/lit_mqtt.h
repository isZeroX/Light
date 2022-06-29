/**
 * @file lit_mqtt.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-06-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LIT_MQTT_H
#define LIT_MQTT_H

void initMQTT(bool firstRun); //初始化mqtt
bool mqttConnectCheck();      // mqtt连接检测
void sendData(String msg);    //发送至mqtt
void disMqtt();               //断开mqtt
void runMQTT();               //运行

#endif // !LIT_MQTT_H