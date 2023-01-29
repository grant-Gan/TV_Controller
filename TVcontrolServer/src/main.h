#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>         
#include <ESP8266WebServer.h>   
#include <ESP8266HTTPUpdateServer.h>
#include <Ticker.h>
#include <FS.h>
#include "TV.h"

bool SYS_ENABLE = true;
bool time_flag_30s = false;
bool time_flag_5min = false;
bool time_flag_10min = false;
int update_Rate = 1;
uint16_t global_time = 60;

String file_name = "/text/timeBackup.txt"; //被读取的文件位置和名称
String ssid = "DrawingRoom";
String password = "ganzuxiang9558";

ESP8266WebServer esp8266_server(80); // 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
ESP8266HTTPUpdateServer httpUpdater;

IPAddress local_IP(192, 168, 1, 2); // 设置ESP8266-NodeMCU联网后的IP
IPAddress gateway(192, 168, 1, 1);  // 设置网关IP（通常网关IP是WiFI路由IP）
IPAddress subnet(255, 255, 255, 0);   // 设置子网掩码
IPAddress dns(192, 168, 1, 1);      // 设置局域网DNS的IP（通常局域网DNS的IP是WiFI路由IP）

Ticker task1_1min;
Ticker task2_5min;
Ticker task3_10min;

//ticker处理任务
void Task_30s_update();
void Task_5min_update();
void Task_10min_update();

void Task_5min_backup_time();
void Task_30s_update_tv_state();
void Task_10min_check_network();

//初始化
void wifi_init();
void file_init();

//网络处理回调
void handle_update();
void handle_get();
void handle_set();
void handle_set_power();
void handle_set_time();

//工具类
void file_write(int time);