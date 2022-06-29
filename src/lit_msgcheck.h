/**
 * @file lit_msgcheck.h
 * @author isZeroX@outlook.com
 * @brief
 * @version 0.1
 * @date 2022-06-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LIT_MSGCHECK_H
#define LIT_MSGCHECK_H

//固定指令
#define CALL_ESP "eo"   // esp ok
#define CALL_PHONE "po" // phone ok
#define CALL_TCP "to"   // phone ok

#define MSG_MQTT 1   // mqtt
#define MSG_TCP 2    // tcp
#define MSG_SERIAL 3 //串口

void message_check(String msg, uint8_t source); //消息检测
void chkSerialMsg();                             //串口检测

#endif // !LIT_MSGCHECK
