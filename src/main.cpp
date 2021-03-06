/**
 * @file main.cpp
 * @author isXeroX@outlook.com
 * @brief
 * 硬件: esp8266(乐鑫esp12F)
 * 框架 Arduino
 * 环境: vs code x platformio
 * Light+为灯光控制程序，主要对ws2812灯光点阵进行控制，并能够通过手机app和pc端程序进行控制，调节灯光颜色等参数进行补光和照明功能。
 * 主要功能：
 *  灯光颜色调整
 *  灯光亮度调整
 *  灯光点阵模式
 *  灯光群控
 *  服务器连接 和 TCP连接模式 自动切换
 *
 * bug:
 * connect会在断联后tcpLinkFlag无法重新变
 *
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Arduino.h>      //Arduino库
#include <Ticker.h>       // Ticker库
#include <lit_eeprom.h>   //eeprom
#include <lit_wifi.h>     //wifi
#include <lit_led.h>      //led
#include <lit_mqtt.h>     //mqtt
#include <lit_msgcheck.h> //msgcheck

#define Key D8
bool keyFlag = false;

#define DEBUG

void timerCallback();  //定时器回调函数
void chkWlanConnet();  // mqtt运行
void chkTcpMsg();      // tcp运行
void SelectLedColor(); //指示灯选择

#define TimerHz 1            //定时器频率 1000hz 1ms/1
#define SYS_CHECK_TIMER 1000 //系统网络状态检测周期
#define SYS_LED_TIMER 100    //等待网络连接检测周期
Ticker ticker;               //定时器
int sysCheckTimer = 0;       // led等待初始闪烁间隔 0-65535
int sysLedTimer = 0;         // led网络连接闪烁间隔 0-65535
bool ledDelSelWifi = false;  // led指示灯状态指示-闪烁标志
bool ledDelLinkWifi = false; // led等待wifi连接 -闪烁标志
bool firstFlag = false;      // 设备首次运行标志
bool defWifiLink = false;    //用以确认是否存在wifi默认连接信息
bool wifiCheckFlag = false;  // wifi-检测标志
bool mqttCheckFlag = false;  // mqtt-检测标志
bool wifiLinkFlag = false;   // wifi-连接标志
bool mqttLinkFlag = false;   // mqtt-连接标志
bool tcpLinkFlag = false;    // tcp -连接标志

/**
 * @brief 啦啦啦啦
 * 此函数代码只在上电或复位后运行一次
 * 相当于main函数 while前的代码
 *
 */
void setup()
{
  ticker.attach_ms(TimerHz, timerCallback); //定时器1000hz 1ms调用一次
  Serial.begin(115200);                     //串口波特率115200
  firstFlag = initEEPROM();                 //初始化eeprom

#ifdef DEBUG
  Serial.println("\r\n\r\n");
  Serial.println("--------------");
  Serial.println("==esp Init==");
  if (firstFlag) //只有首次运行才有，用来对板子进行基本的初始化
    Serial.println("==First Runing... ==");
#endif // DEBUG

  defWifiLink = initWIFI(firstFlag); //对wifi进行初始化操作
  initMQTT(defWifiLink);             //首次连接初始化
  initLED(firstFlag);                //初始化 led
}

/**
 * @brief 代码初始化后搁着跑
 *
 */
void loop()
{
  chkWlanConnet();  //网络状态检测
  chkSerialMsg();   //串口消息检测
  chkTcpMsg();      // tcp消息检测
  SelectLedColor(); //状态指示灯
  runMQTT();        // mqtt跑
}

void SelectLedColor()
{
  if (!defWifiLink && ledDelSelWifi) //等待设定默认wifi
  {
    setLedColor(COLOR_BLOCK);
    wifiLinkFlag = false;
    mqttLinkFlag = false;
    tcpLinkFlag = false;
    return; //闪烁
  }
  else if (ledDelLinkWifi)
  {
    if (defWifiLink && !wifiLinkFlag) // 等待wifi连接
    {
      setLedColor(COLOR_BLOCK);
      return; //闪烁
    }
    switch ((wifiLinkFlag << 2) + (mqttLinkFlag << 1) + tcpLinkFlag)
    {
    case 0: //都没连接
      setLedColor(COLOR_RED);
      break;
    case 1: //只有tcp连接
      setLedColor(COLOR_CYAN_BLUE);
      break;
    case 4: //只有wifi连接
      setLedColor(COLOR_GREEN);
      break;
    case 5: // wifi和tcp连接
      setLedColor(COLOR_OCEAN_RED);
      break;
    case 6: // wifi和tcp连接
      setLedColor(COLOR_BLUE);
      break;
    case 7:
      setLedColor(COLOR_YELLOW);
      break;
    }
  }
}

/**
 * @brief tcp运行
 *
 * @return true 有设备连接
 * @return false 无设备连接
 */
void chkTcpMsg()
{
  ClientClick();                // 新接入设备检测
  tcpLinkFlag = MessageClick(); //消息检测
}

/**
 * @brief 网络状态检测
 * 用来检测wifi连接状态和mqtt连接状态
 * 在wifi未连接时自动关闭mqtt连接
 *
 */
void chkWlanConnet()
{
  //存在连接wifi和 wifi连接检测标志
  if (wifiCheckFlag && defWifiLink)
  {
    wifiCheckFlag = false;             //清除wifi检测标志
    wifiLinkFlag = wifiConnectCheck(); //获取wifi状态
    if (!wifiLinkFlag)
    {
      mqttLinkFlag = false; // wifi断开  清除mqtt连接标志
    }
    if (wifiLinkFlag && mqttCheckFlag)
    {
      mqttCheckFlag = false;             //清除检测标志
      mqttLinkFlag = mqttConnectCheck(); //获取mqtt状态
      Serial.print("check mqtt connect:");
      Serial.println(mqttLinkFlag);
    }
  }
}

/**
 * @brief 定时器回调函数
 * 主要控制led闪烁频率
 * 以及按键消抖
 * 和按键功能复用
 */
void timerCallback()
{
  if ((sysCheckTimer++) >= SYS_CHECK_TIMER) //每隔1000ms检测以此wifi状态
  {
    sysCheckTimer = 0;
    wifiCheckFlag = true;           // wifi检测标志
    mqttCheckFlag = true;           // mqtt检测标志
    ledDelSelWifi = !ledDelSelWifi; // led指示灯
  }
  if ((sysLedTimer++) >= SYS_LED_TIMER)
  {
    sysLedTimer = 0;
    ledDelLinkWifi = !ledDelLinkWifi; // led指示灯
  }
}
//================================================================
//================================================================