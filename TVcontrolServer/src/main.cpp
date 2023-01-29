/*
参数: 本机地址: 192.168.1.2

2021年4月7日增改:
  更改wifi_sta模式
  增加停用路由器功能
2021年4月10日增改:
  增加OTA功能
  修改 "sizeof" 及 "uint_t" bug

*/
    
#include <Ticker.h>
#include <FS.h>
#include "main.h"
#include "TV.h"

TV tv;

void setup()
{
  
  Serial.begin(115200);
  // 设置开发板网络环境
  wifi_init();
  file_init();

  task1_1min.attach(30, Task_30s_update);
  task2_5min.attach(300, Task_5min_update);
  task3_10min.attach(600, Task_10min_update);

}

void loop()
{

  esp8266_server.handleClient(); // 处理http服务器访问

  if (time_flag_30s == true)
  {
    time_flag_30s = false;
    Task_30s_update_tv_state();
    
  }

  if (time_flag_5min == true)
  {
    time_flag_5min = false;
    Task_5min_backup_time();
    
  }

  if (time_flag_10min == true)
  {
    time_flag_10min = false;
    Task_10min_check_network();
  }

}

/*
*******************************************
网络响应回调函数
*/
void handle_shutdown_tv()
{
  tv.turnOffTv();
  tv.setNetStatus(false);
  esp8266_server.send(200, "text/html", "shutdown_successed");
}

void handle_update()
{

  global_time += update_Rate;
  esp8266_server.send(200, "text/html", "update_successed");
}

void handle_get()
{

  String str = SYS_ENABLE ? "开启" : "关闭";
  String tv_state = tv.getTvPowerState(50) ? "开启" : "关闭";
  String htmlCode = "<div style=\"font-size:30px\">";
  htmlCode+="<p>系统状态: " + str + "</p>";
  htmlCode+="<p>电视状态: " + tv_state + "</p>";
  htmlCode+="<p>剩余时间: " + String(global_time) + "</p>";
  htmlCode+="<p>倍率: " + String(update_Rate) + "</p>";
  htmlCode+= "</div>";
  esp8266_server.send(200, "text/html", htmlCode);

}

void handle_set()
{
  int add_time = esp8266_server.arg("add_time").toInt();
  int rate = esp8266_server.arg("rate").toInt();
  String str_bool = esp8266_server.arg("enable");

  SYS_ENABLE = str_bool == "true" ? true : false;
  global_time = add_time;
  update_Rate = rate;
  esp8266_server.send(200, "text/plain", "Change successed"); // 发送http响应
}

void handle_set_power()
{
  String str_bool = esp8266_server.arg("enable");
  SYS_ENABLE = str_bool == "true" ? true : false;
  esp8266_server.send(200, "text/plain", "Change successed"); // 发送http响应
}

void handle_set_time()
{
  global_time = esp8266_server.arg("add_time").toInt();
  esp8266_server.send(200, "text/plain", "Change successed"); // 发送http响应
}

/*
*****************************************
初始化函数
*/
void wifi_init()
{
  
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.print(WiFi.localIP());
  // 设置网络服务器
  esp8266_server.on("/updateTime", handle_update);
  esp8266_server.on("/get", handle_get);
  esp8266_server.on("/set", handle_set);
  esp8266_server.on("/set_power", handle_set_power);
  esp8266_server.on("/set_time", handle_set_time);
  esp8266_server.on("/shutdown_tv", handle_shutdown_tv);

  httpUpdater.setup(&esp8266_server);     //httpUpdater绑定到webServer上
  esp8266_server.begin();
  Serial.println("HTTP esp8266_server started");
}

void file_init()
{

  //Serial.println("SPIFFS format start");
  //SPIFFS.format();    // 格式化SPIFFS
  //Serial.println("SPIFFS format finish");

  if (SPIFFS.begin())
  { // 启动SPIFFS
    Serial.println("SPIFFS Started.");
  }
  else
  {
    Serial.println("SPIFFS Failed to Start.");
  }

  if (SPIFFS.exists(file_name))
  {
    //  Serial.print(file_name);
    //  Serial.println(" FOUND.");

    File dataFile = SPIFFS.open(file_name, "r");
    String str = dataFile.readString();
    global_time = str.toInt();
    dataFile.close();
  }
  else
  {
    //  Serial.print(file_name);
    //  Serial.print(" NOT FOUND.");

    File dataFile = SPIFFS.open(file_name, "w");
    dataFile.print(String(global_time));
    dataFile.close();
  }
}

/*
*******************************************
工具类函数
*/
void file_write(int time)
{
  File dataFile = SPIFFS.open(file_name, "w");
  dataFile.print(String(time));
  dataFile.close();
}

/*
*********************************
多任务处理函数
*/
void Task_5min_backup_time()
{
  static int tmp_time = 60;
  if ((global_time - tmp_time) != 0) //global_time数值变动
  {
    tmp_time = global_time;
    file_write(global_time);
  }
}

void Task_30s_update_tv_state()
{
  static unsigned char timeCount = 0;
  if (SYS_ENABLE && tv.getTvPowerState(50)) //如果系统使能打开并且电视打开
  {
    if (global_time <= 0) {
      global_time = 0;
      tv.turnOffTv();
      tv.setNetStatus(false);
    } else if(timeCount == 1) { 
      timeCount = 0;     
      global_time -= 1;
      tv.setNetStatus(true);
    } else {
      timeCount++;
    }
  }
}

void Task_10min_check_network()
{
  int i = 0;
  while ( WiFi.status() != WL_CONNECTED )
  {
    WiFi.begin();
    delay(1000);
    Serial.print(i++);
    Serial.print(' ');
  }
}

void Task_30s_update()
{
  time_flag_30s = true;
}
void Task_5min_update()
{
  time_flag_5min = true;
}
void Task_10min_update()
{
  time_flag_10min = true;
}
