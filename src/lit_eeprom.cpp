/**
 * @file lit_eeprom.cpp
 * @author isZeroX@outlook.com
 * @brief
 * 此文件主要对eeprom内数据进行操作，
 * 本地设备wifi ssid password
 * 连接设备wifi ssid password
 * mqtt server host port和订阅主题发布主题等信息
 * 此文件initEEPROM应当被优先调用
 * 此文件中wifi ssid password获取和修改将允许被调用
 * 以及其他用户需要写入eeprom中的数据
 *
 * @version 0.1
 * @date 2022-06-13
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <EEPROM.h>
#include <lit_eeprom.h>

////////////////////////////////////////////////////////////////////////
// user code begin
// 16-32位
#define EEPROM_LED_MODE 16   // LED灯光模式 0单色，1用户自定义
#define EEPROM_LED_RED 17    // led 灯光颜色存放地址 红色128 绿色129 蓝色130
#define EEPROM_LED_GREEN 18  // led 灯光颜色存放地址 红色128 绿色129 蓝色130
#define EEPROM_LED_BLUE 19   // led 灯光颜色存放地址 红色128 绿色129 蓝色130
#define EEPROM_LED_BRIGHT 20 // led 灯光颜色存放地址 红色128 绿色129 蓝色130
//数据起始地址-配合各个数据长度长度使用
#define EEPROM_LED_MODE2_RED_ADD 320    //用户自定义-红色 16
#define EEPROM_LED_MODE2_GREEN_ADD 336  //用户自定义-绿色
#define EEPROM_LED_MODE2_BLUE_ADD 352   //用户自定义-蓝色
#define EEPROM_LED_MODE2_BRIGHT_ADD 368 //用户自定义-亮度

#define EEPROM_LED_MODE_1 '1' // LED灯光模式 0单色，1用户自定义

// user code end
////////////////////////////////////////////////////////////////////////

//#define的一些数据位置信息和数据长度信息
#define EEPROM_SIZE 4096 // eeprom空间

// 0-15位
#define EEPROM_FIRSTRUN 0        // 首次运行标志
#define EEPROM_SSID_LEN 1        // wifi ssid长度
#define EEPROM_PASS_LEN 2        // wifi password长度
#define EEPROM_LOCALSSID_LEN 3   //本地wifi ssid长度
#define EEPROM_LOCALPASS_LEN 4   //本地wifi password长度
#define EEPROM_PUB_LEN 5         //发送标志长度
#define EEPROM_SUB_NUM 6         //订阅标志 001订阅1个，011订阅2个，111订阅3个
#define EEPROM_SUB1_LEN 7        //接受标志1长度
#define EEPROM_SUB2_LEN 8        //接受标志2长度
#define EEPROM_SUB3_LEN 9        //接受标志3长度
#define EEPROM_HEART_TIMER 10    //心跳间隔
#define EEPROM_DEVICENAME_LEN 11 //本设备名称长度（长度上限10个字符）

//数据起始地址-配合各个数据长度长度使用
#define EEPROM_SSID_ADD 32         // wifi ssid起始位置
#define EEPROM_PASS_ADD 64         // wifi password起始位置
#define EEPROM_LOCALSSID_ADD 96    // wifi ssid起始位置
#define EEPROM_LOCALPASS_ADD 128   // wifi password起始位置
#define EEPROM_PUB_ADD 160         // 发布主题起始地址
#define EEPROM_SUB1_ADD 192        //订阅主题1起始地址
#define EEPROM_SUB2_ADD 224        //订阅主题1起始地址
#define EEPROM_SUB3_ADD 256        //订阅主题1起始地址
#define EEPROM_SERVER_IP_ADD 288   //服务器地址
#define EEPROM_SERVER_PORT_ADD 292 //服务器端口(由2位数据组成)
#define EEPROM_DEVICENAME_ADD 294  //本设备名称地址（长度上限10个字符）

#define EEPROM_FIRSTRUN_FLAG 'N' // No不是

//默认数值
#define EEPROM_MQTT_PORT 1883   // 默认端口1883
#define EEPROM_MQTT_HEARTBEAT 2 // 默认心跳 2s

/******************************************************************
 * 此区域内函数对EEPROM操作的基础函数，包括数据读写和EEPROM的初始化
 * 外部函数谨慎调用
 * ****************************************************************/
/**
 * @brief 数据写入eeprom
 *
 * @param lenAdd str长度保存位置
 * @param startP str初始写入位置
 * @param str 待写入的字符串
 */
void setString(int lenAdd, int startP, String str)
{
    EEPROM.write(lenAdd, str.length());
    for (uint8_t i = 0; i < str.length(); i++)
    {
        EEPROM.write(startP + i, str[i]);
    }
    EEPROM.commit();
}

/**
 * @brief 获取eeprom数据
 *
 * @param lenAdd str长度保存位置 （输入存储在eeprom中的长度信息）
 * @param srartP str初始写入位置
 * @return String eeprom内数据
 */
String getString(int lenAdd, int srartP)
{
    String str = "";
    for (int i = 0; i < lenAdd; i++)
    {
        str += char(EEPROM.read(srartP + i));
    }
    return str;
}

/**
 * @brief 对eeprom进行初始化
 *
 * @return true 首次运行
 * @return false 非首次运行
 */
bool initEEPROM()
{
    EEPROM.begin(EEPROM_SIZE); //设置EEPROM空间

    if (EEPROM.read(EEPROM_FIRSTRUN) != EEPROM_FIRSTRUN_FLAG) //首次运行
    {
        clearEEPROM(); //清空内存
        EEPROM.write(EEPROM_FIRSTRUN, EEPROM_FIRSTRUN_FLAG);
        EEPROM.commit();
        return true;
    }
    return false;
}

/******************************************************************
 * 此区域内函数供其他文件内函数调用的功能性函数，其他功能性函数也写在此区域内
 * 用于获取eeprom内wifi_ssid和wifi_password
 * 以及对eeprom内wifi_ssid和wifi_password的更改
 * ****************************************************************/
/**
 * @brief 清除EEPROM数据
 *
 */
void clearEEPROM()
{
    for (uint16_t i = 0; i < EEPROM_SIZE; i++)
        EEPROM.write(i, 0);
    EEPROM.commit();
}

/**
 * @brief 清除wifi连接
 *
 */
void clearWifi()
{
    EEPROM.write(EEPROM_SSID_ADD, 0);
    EEPROM.commit();
}

/**
 * @brief 获取需要连接的 wifi ssid
 *
 * @return String 返回 wifi ssid
 */
String getWifi_ssid()
{
    return getString(EEPROM.read(EEPROM_SSID_LEN), EEPROM_SSID_ADD);
}

/**
 * @brief 获取需要连接的 wifi password
 *
 * @return String 返回wifi password
 */
String getWifi_pass()
{
    return getString(EEPROM.read(EEPROM_PASS_LEN), EEPROM_PASS_ADD);
}

/**
 * @brief  获取本地 wifi ssid
 *
 * @return String 返回wifi ssid
 */
String getWifiLocal_ssid()
{
    return getString(EEPROM.read(EEPROM_LOCALSSID_LEN), EEPROM_LOCALSSID_ADD);
}

/**
 * @brief 获取本地 wifi password
 *
 * @return String 返回wifi password
 */
String getWifiLocal_pass()
{
    return getString(EEPROM.read(EEPROM_LOCALPASS_LEN), EEPROM_LOCALPASS_ADD);
}

/**
 * @brief 设定wifi信息
 *
 * @param wifi_ssid wifi ssid
 * @param wifi_pass wifi密码
 */
void setWifi_Data(String wifi_ssid, String wifi_pass)
{
    Serial.print("-->");
    Serial.print(wifi_ssid);
    Serial.print("<:>");
    Serial.println(wifi_pass);
    setString(EEPROM_SSID_LEN, EEPROM_SSID_ADD, wifi_ssid);
    setString(EEPROM_PASS_LEN, EEPROM_PASS_ADD, wifi_pass);
}

/**
 * @brief 设定本地wifi信息
 *
 * @param wifi_ssid wifi ssid
 * @param wifi_pass wifi密码
 */
void setWifiLocal_Data(String wifi_ssid, String wifi_pass)
{
    setString(EEPROM_LOCALSSID_LEN, EEPROM_LOCALSSID_ADD, wifi_ssid);
    setString(EEPROM_LOCALPASS_LEN, EEPROM_LOCALPASS_ADD, wifi_pass);
}

/******************************************************************
 * 此区域内函数供其他文件内函数调用的功能性函数，其他功能性函数也写在此区域内
 * 用于获取eeprom内服务器信息和发布订阅主题
 * 以及对eeeprom内服务器信息和发布订阅主题的更改
 * ****************************************************************/

/**
 * @brief 获取服务器端口和IP
 *
 * @return String 服务器IP和端口信息
 */
String getServerIp()
{
    uint8_t ipa = EEPROM.read(EEPROM_SERVER_IP_ADD);
    uint8_t ipb = EEPROM.read(EEPROM_SERVER_IP_ADD + 1);
    uint8_t ipc = EEPROM.read(EEPROM_SERVER_IP_ADD + 2);
    uint8_t ipd = EEPROM.read(EEPROM_SERVER_IP_ADD + 3);
    String ip = (String)ipa + '.' + (String)ipb + '.' + (String)ipc + '.' + (String)ipd;
    return ip;
}

/**
 * @brief 设置服务器地址
 *
 * @param ipa ipa
 * @param ipb ipb
 * @param ipc ipc
 * @param ipd ipd
 */
void setServerIp(uint8_t ipa, uint8_t ipb, uint8_t ipc, uint8_t ipd)
{
    EEPROM.write(EEPROM_SERVER_IP_ADD, ipa);
    EEPROM.write(EEPROM_SERVER_IP_ADD + 1, ipb);
    EEPROM.write(EEPROM_SERVER_IP_ADD + 2, ipc);
    EEPROM.write(EEPROM_SERVER_IP_ADD + 3, ipd);
    EEPROM.commit();
}

/**
 * @brief 获取服务器端口
 *
 * @return int
 */
uint16_t getServerPort()
{
    uint8_t portH = EEPROM.read(EEPROM_SERVER_PORT_ADD);
    uint8_t portL = EEPROM.read(EEPROM_SERVER_PORT_ADD + 1);
    uint16_t port = (portH << 8) + portL;
    if (!port) //设定默认端口
    {
        setSeerverPort(EEPROM_MQTT_PORT);
        port = EEPROM_MQTT_PORT;
    }

    return port;
}

/**
 * @brief 设定服务器端口
 *
 * @param port 端口号
 */
void setSeerverPort(uint16_t port)
{
    uint8_t portH = port >> 8 & 0xff;
    uint8_t portL = port & 0xff;
    EEPROM.write(EEPROM_SERVER_PORT_ADD, portH);
    EEPROM.write(EEPROM_SERVER_PORT_ADD + 1, portL);
    EEPROM.commit();
}

/**
 * @brief 获取设备uid
 *
 * @return String 设备uid
 */
String getDeviceName()
{
    return getString(EEPROM.read(EEPROM_DEVICENAME_LEN), EEPROM_DEVICENAME_ADD);
}

/**
 * @brief 设置设备uid
 *
 * @param deviceName
 */
void setDeviceName(String deviceName)
{
    setString(EEPROM_DEVICENAME_LEN, EEPROM_DEVICENAME_ADD, deviceName);
}

/**
 * @brief 设置心跳
 *
 * @param heartbeat 心跳数值 0-255
 */
void setHeart(uint8_t heartbeat)
{
    EEPROM.write(EEPROM_HEART_TIMER, heartbeat);
    EEPROM.commit();
}

/**
 * @brief 获取心跳
 *
 * @return uint8_t 心跳间隔
 */
uint8_t getHeart()
{
    uint8_t heartbeat = EEPROM.read(EEPROM_HEART_TIMER);
    if (heartbeat == 0)
        heartbeat = EEPROM_MQTT_HEARTBEAT;
    return heartbeat;
}

/**
 * @brief 获取发布主题
 *
 * @return String 主题名称
 */
String getPubClient()
{
    return getString(EEPROM.read(EEPROM_PUB_LEN), EEPROM_PUB_ADD);
}

/**
 * @brief 设定发布主题
 *
 * @param pubclient
 */
void setPubClient(String pubclient)
{
    setString(EEPROM_PUB_LEN, EEPROM_PUB_ADD, pubclient);
}

/**
 * @brief 获取订阅主题数量
 *
 * @return uint8_t
 */
uint8_t getSubNum()
{
    return EEPROM.read(EEPROM_SUB_NUM);
}

/**
 * @brief 获取订阅主题1
 *
 * @return String 订阅主题1
 */
String getSubClient1()
{

    return getString(EEPROM.read(EEPROM_SUB1_LEN), EEPROM_SUB1_ADD);
}

/**
 * @brief 获取订阅主题2
 *
 * @return String 订阅主题2
 */
String getSubClient2()
{

    return getString(EEPROM.read(EEPROM_SUB2_LEN), EEPROM_SUB2_ADD);
}

/**
 * @brief 获取订阅主题3
 *
 * @return String 订阅主题3
 */
String getSubClient3()
{
    return getString(EEPROM.read(EEPROM_SUB3_LEN), EEPROM_SUB3_ADD);
}

/**
 * @brief 清除所有订阅
 *
 */
void subClear()
{
    EEPROM.write(EEPROM_SUB_NUM, 0);
    EEPROM.commit();
}

/**
 * @brief 设定订阅主题 000 001 010 011 100 101 110 111
 *
 * @param subClient 主题名称 如果为空将清除此位置
 * @param add 保存位置 0清除 1保存在1 以此类推
 * @return true 保存数据
 * @return false 未能保存数据
 */
bool setSubClient(String subClient, uint8_t add)
{
    if (add == 0 || add > 3) //数据错误
    {
        return false;
    }

    uint8_t subNum = EEPROM.read(EEPROM_SUB_NUM); //获取数据
    if (subClient.length() == 0)                  //用来清空订阅
    {
        subNum &= ~(0x01 << (add - 1));       //第add位置0
        EEPROM.write(EEPROM_SUB_NUM, subNum); //订阅数量写入
        EEPROM.commit();
        return true;
    }

    switch (add) //数据写入eeprom
    {
    case 1:
        setString(EEPROM_SUB1_LEN, EEPROM_SUB1_ADD, subClient);
        break;
    case 2:
        setString(EEPROM_SUB2_LEN, EEPROM_SUB2_ADD, subClient);
        break;
    case 3:
        setString(EEPROM_SUB3_LEN, EEPROM_SUB3_ADD, subClient);
        break;
    default:
        return false;
        break;
    }

    //订阅主题数量写入（三位000）
    subNum |= (0x01 << (add - 1));
    EEPROM.write(EEPROM_SUB_NUM, subNum); //订阅数量写入
    EEPROM.commit();

    return true;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

// user code begin
/******************************************************************
 * 此区域内函数为灯光控制函数，可根据程序的不同进行修改
 * ****************************************************************/
// led相关
/**
 * @brief 设置led颜色数值
 *
 * @param r 红色
 * @param g 绿色
 * @param b 蓝色
 */
void setLedData(uint8_t r, uint8_t g, uint8_t b)
{
    EEPROM.write(EEPROM_LED_MODE, '1');
    EEPROM.write(EEPROM_LED_RED, r);
    EEPROM.write(EEPROM_LED_GREEN, g);
    EEPROM.write(EEPROM_LED_BLUE, b);
    EEPROM.commit();
}

/**
 * @brief 获取灯光信息，判断是否首次运行
 *
 * @return true 灯光首次写入
 * @return false 灯光写入
 */
bool getLedDefDate()
{
    //用来判断是否灯光数据已经写进过eeprom，如果没有就代表着是初次写入
    if (EEPROM.read(EEPROM_LED_MODE) == 0)
    {
        return true;
    }
    return false;
}

/**
 * @brief 设置灯光亮度
 *
 * @param bright
 */
void setLedBright(uint8_t bright)
{
    EEPROM.write(EEPROM_LED_BRIGHT, bright);
    EEPROM.commit();
}

/**
 * @brief 获取灯光亮度
 *
 * @return uint8_t
 */
uint8_t getLedBright()
{
    return EEPROM.read(EEPROM_LED_BRIGHT);
}

uint8_t getLedR()
{
    return EEPROM.read(EEPROM_LED_RED);
}
uint8_t getLedG()
{
    return EEPROM.read(EEPROM_LED_GREEN);
}
uint8_t getLedB()
{
    return EEPROM.read(EEPROM_LED_BLUE);
}

// user code end

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
