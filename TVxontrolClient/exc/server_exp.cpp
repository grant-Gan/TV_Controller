/**********************************************************************
��Ŀ����/Project          : ���������ѧ��������
��������/Program name     : csd_server
�Ŷ�/Team                : ̫�������Ŷ� / Taichi-Maker (www.taichi-maker.com)
����/Author              : CYNO˷
����/Date��YYYYMMDD��     : 20200228
����Ŀ��/Purpose          : 
��ʾ���ʵ������NodeMCU��ͨ��WiFi����ͨѶ����̨NodeMCUһ���Է�����ģʽ���У�
һ���Կͻ���ģʽ���С�
 
�˴���Ϊ�ͻ��˴��롣�˴�����Ҫ���ܣ�
    - ���տͻ��˷�����http����
    - �����ͻ��������е�������Ϣ
    - ��������������Ϣͨ�����ڼ�������ʾ���û��鿴
    - �������Ŀͻ��˰���״̬��Ϣ���ڿ��ư���LED�ĵ�����Ϩ��
    
-----------------------------------------------------------------------
�޶���ʷ/Revision History  
����/Date    ����/Author      �ο���/Ref    �޶�˵��/Revision Description
20200229      CYNO˷           0.01       ����IP����
 
***********************************************************************/
 
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   // ʹ��WiFiMulti�� 
#include <ESP8266WebServer.h>   // ʹ��WebServer��
 
ESP8266WiFiMulti wifiMulti;     // ����ESP8266WiFiMulti����,���������� 'wifiMulti'
 
ESP8266WebServer server(80);    // ����������������󣬸ö���������ӦHTTP���󡣼����˿ڣ�80��
 
IPAddress local_IP(192, 168, 0, 123); // ����ESP8266-NodeMCU�������IP
IPAddress gateway(192, 168, 0, 1);    // ��������IP��ͨ������IP��WiFI·��IP��
IPAddress subnet(255, 255, 255, 0);   // ������������
IPAddress dns(192,168,0,1);           // ���þ�����DNS��IP��ͨ��������DNS��IP��WiFI·��IP��
 
void setup(void){
  Serial.begin(9600);          // ��������ͨѶ
  Serial.println("");
 
  pinMode(LED_BUILTIN, OUTPUT);   
  digitalWrite(LED_BUILTIN, HIGH);
  
  // ���ÿ��������绷��
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to Config ESP8266 IP"); 
  } 
  
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1"); // ����Ҫ���ӵ�һϵ��WiFi ID��������������
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2"); // ESP8266-NodeMCU���������ɨ�赱ǰ����
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3"); // ���������Ƿ��������г���WiFi ID�������
  Serial.println("Connecting ...");                            // ����ʹ�ô˴��洢������������ӡ�
  
  // ���Խ���wifi���ӡ�
  while (wifiMulti.run() != WL_CONNECTED) { 
    delay(250);
    Serial.print('.');
  }
 
  // WiFi���ӳɹ���ͨ�����ڼ�����������ӳɹ���Ϣ 
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // ͨ�����ڼ�����������ӵ�WiFi����
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // ͨ�����ڼ��������ESP8266-NodeMCU��IP
 
  server.on("/update", handleUpdate);        // ������������º���
  
  server.begin();                            // ������վ����
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();                    // ���http����������
}
 
void handleUpdate(){
  float floatValue = server.arg("float").toFloat();  // ��ȡ�ͻ��˷���HTTP��Ϣ�еĸ�����ֵ
  int intValue = server.arg("int").toInt();        // ��ȡ�ͻ��˷���HTTP��Ϣ�е�������ֵ
  int buttonValue = server.arg("button").toInt();  // ��ȡ�ͻ��˷���HTTP��Ϣ�еİ���������
  
  server.send(200, "text/plain", "Received");                 // ����http��Ӧ
 
  buttonValue == 0 ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);
 
  // ͨ�����ڼ����������ȡ���ı�����ֵ
  Serial.print("floatValue = ");  Serial.println(floatValue);  
  Serial.print("intValue = ");  Serial.println(intValue); 
  Serial.print("buttonValue = ");  Serial.println(buttonValue);   
  Serial.println("=================");    
}