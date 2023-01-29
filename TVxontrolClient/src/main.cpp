/*********
 * 本机IP:192.168.1.3
 * 
 * 2021年4月10日增改
 * 增加OTA功能
 * 
 * 
*********/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "main.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Ticker task1_1min;
Ticker task2_10min;

void setup()
{
  Serial.begin(115200);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin("DrawingRoom", "ganzuxiang9558");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // 通过串口监视器输出连接的WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // 通过串口监视器输出ESP8266-NodeMCU的IP
  
  httpUpdater.setup(&esp8266_server);     //httpUpdater绑定到webServer上
  esp8266_server.begin();
  screen_init();
  pinMode(KEY_PIN, INPUT);

  attachInterrupt(VOICE_PIN, voice_inrup_func, RISING);

  task1_1min.attach(60, Task_1min_time_update);
  task2_10min.attach(600, Task_10min_time_update);
  
}

void loop()
{
  esp8266_server.handleClient(); // 处理http服务器访问
  
  if( digitalRead(KEY_PIN) == 0)
  {
    screen_print_time();   
    //screen_print("hello", 2, 0, 0);
  }

  if(time_flag_1min == true)
  {
    time_flag_1min = false;
    check_time_1min();
  }

  if(time_flag_10min == true)
  {
    time_flag_10min = false;
    Task_10min_check_network();
  }
}

//成功返回时间, 失败返回 -1 
int wifi_get_time()
{
  WiFiClient client;          // 建立WiFiClient对象
  
  // 建立客户端请求信息
  String httpRequest =  String("GET /get")  + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";
                        
  // 发送客户端请求
  Serial.println("Sending request: ");
  Serial.print(httpRequest);  

  if (client.connect(host, httpPort)) {  //如果连接失败则串口输出信息告知用户然后返回loop
    Serial.println(" Sucess");
    
    client.print(httpRequest);          // 向服务器发送HTTP请求
    Serial.println("Sending request: ");// 通过串口输出HTTP请求信息内容以便查阅
    Serial.println(httpRequest);         
  } else{
      Serial.println(" failed");
  }
  
  while (client.connected() || client.available())
  {
    if(client.find("剩余时间: ")) 
    {  
      client.stop();    // 停止客户端  
      return client.parseInt(); 
    }
    else
    {
      client.stop();    // 停止客户端  
      return -1;      //失败代码
    }
  }
  return -1;   
}

//成功返回 1, 失败返回 -1
int wifi_update_time()
{
  WiFiClient client;          // 建立WiFiClient对象

  Serial.print("Connecting to "); Serial.print(host);
 
  // 连接服务器
  if (client.connect(host, httpPort))
  {
    Serial.println(" Success!");
    
    // 建立客户端请求信息
    String httpRequest =  String("GET /updateTime") + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n" +
                          "\r\n";
                          
    // 发送客户端请求
    Serial.println("Sending request: ");Serial.print(httpRequest);  
    client.print(httpRequest);
  }

    while (client.connected() || client.available())
    {
      if( client.find("update_successed") )
      {
        client.stop();
        return 1;   //更新时间成功
      }          
      else
      {
        client.stop();
        return -1;   //更新时间失败
      }
    } 
    return -1;     
}

void check_time_1min()
{ 
  if( voice_count >= 2000)
  {
    voice_count = 0;
    //将更新数据发送到服务端
    int retry_count = 3;
    int state = wifi_update_time();
    Serial.println(state);
    while( (retry_count--) && (state == -1) )
    {
      state = wifi_update_time();
      screen_print("Update error", 2, 0, 0);
      Serial.println(state);
      delay(100);
    }
  } else {
    voice_count = 0;
  }
}

void Task_10min_time_update()
{
  time_flag_10min = true;
}

void Task_1min_time_update()
{
  time_flag_1min = true;
}

void Task_10min_check_network()
{
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin();
    delay(1000);
    Serial.print(i++);
    Serial.print(' ');
  }
}

ICACHE_RAM_ATTR void voice_inrup_func()
{
  voice_count++;
}

void screen_print(String str, int char_size = 4, int cursor_x = 0, int cursor_y = 0)
{
  screen.clearDisplay();
  screen.setCursor(cursor_x, cursor_y);
  screen.setTextSize(char_size);
  screen.println(str);
  screen.display();
  delay(5000);
  screen.clearDisplay();
  screen.display();
}

void screen_print_time()
{
  String str = "";
  screen_print("Please wait...", 1, 0, 10);
  int time = wifi_get_time();

  if(time == -1)
  {
    screen_print("Get error", 2, 0, 0);
  }
  else
  {
    unsigned int hour = time / 60;
    unsigned int minute = time % 60;

    str = hour >= 10 ? String(hour) : "0" + String(hour);
    str += ":" + (minute >= 10 ? String(minute) : "0" + String(minute));
    
    //时间显示的默认值: size=4 x=7 y=5 
    screen_print(str, 4, 7, 5);
  }
}

void screen_init()
{
  if (!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      for(;;);
  }
  screen.clearDisplay();
  screen.setTextColor(WHITE);
  screen.display();

}
