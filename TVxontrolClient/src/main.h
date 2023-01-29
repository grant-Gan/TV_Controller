#pragma once

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WebServer.h>   
#include <ESP8266HTTPUpdateServer.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define KEY_PIN D5
#define VOICE_PIN D6

unsigned long voice_count = 0;         //声音上升沿计数
const char* host = "192.168.1.2";    // 即将连接服务器网址/IP
const int httpPort = 80;               // 即将连接服务器端口

IPAddress local_IP(192, 168, 1, 3); // 设置ESP8266-NodeMCU联网后的IP
IPAddress gateway(192, 168, 1, 1);  // 设置网关IP（通常网关IP是WiFI路由IP）
IPAddress subnet(255, 255, 255, 0);   // 设置子网掩码
IPAddress dns(192, 168, 1, 1);      // 设置局域网DNS的IP（通常局域网DNS的IP是WiFI路由IP）

ESP8266WebServer esp8266_server(80); // 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
ESP8266HTTPUpdateServer httpUpdater;

bool time_flag_1min = false;
bool time_flag_10min = false;

void screen_init( void );
void screen_display( void );
void screen_print_time();
void screen_print(String, int, int, int);
int wifi_update_time();
int wifi_get_time();

void Task_10min_check_network();
void check_time_1min();
ICACHE_RAM_ATTR void voice_inrup_func();

void Task_10min_time_update();
void Task_1min_time_update();