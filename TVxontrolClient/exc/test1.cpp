
/**********************************************************************
项目名称/Project          : 零基础入门学用物联网
程序名称/Program name     : cgd_client
团队/Team                : 太极创客团队 / Taichi-Maker (www.taichi-maker.com)
作者/Author              : CYNO朔
日期/Date（YYYYMMDD）     : 20200228
程序目的/Purpose          : 
演示如何实现两个NodeMCU间通过WiFi进行通讯。两台NodeMCU一个以服务器模式运行，
一个以客户端模式运行。
 
此代码为客户端代码。此代码主要功能：
    - 定时向服务器发送请求信息，从而获取服务器开发板上按钮状态
    - 将服务器端发来的按钮引脚电平状态解析为数字量，并且用该数字量控制板上LED点亮和熄灭
***********************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
 
ESP8266WiFiMulti wifiMulti;           // 建立ESP8266WiFiMulti对象
 
const char* host = "192.168.0.123";   // 网络服务器IP
const int httpPort = 80;              // http端口80
 
void setup(){
  Serial.begin(9600);          
  Serial.println("");
 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1"); // 将需要连接的一系列WiFi ID和密码输入这里
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2"); // ESP8266-NodeMCU再启动后会扫描当前网络
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3"); // 环境查找是否有这里列出的WiFi ID。如果有
  Serial.println("Connecting ..."); 
 
  int i = 0;  
  while (wifiMulti.run() != WL_CONNECTED) { // 尝试进行wifi连接。
    delay(1000);
    Serial.print(i++); Serial.print(' ');
  }
  
  // WiFi连接成功后将通过串口监视器输出连接成功信息 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // IP
}
 
void loop(){
  wifiClientRequest();  
  delay(3000);
}
 
void wifiClientRequest(){
  WiFiClient client;          // 建立WiFiClient对象
 
  bool buttonState;     // 储存服务器按钮状态变量  
         
  Serial.print("Connecting to "); Serial.print(host);
 
  // 连接服务器
  if (client.connect(host, httpPort)){
    Serial.println(" Success!");
    
    // 建立客户端请求信息
    String httpRequest =  String("GET /update") + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n" +
                          "\r\n";
                          
    // 发送客户端请求
    Serial.println("Sending request: ");Serial.print(httpRequest);  
    client.print(httpRequest);
 
    // 获取服务器响应信息中的按钮状态信息
    while (client.connected() || client.available()){
      if(client.find("buttonState:")){      
        buttonState = client.parseInt(); 
        Serial.print("buttonState: " ); 
        Serial.println(buttonState); 
      }
    }
  } else{
    Serial.println(" failed!");
  } 
  
  Serial.println("===============");
  client.stop();    // 停止客户端  
   
  // 根据服务器按键状态点亮或熄灭LED
  buttonState == 0 ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);
}