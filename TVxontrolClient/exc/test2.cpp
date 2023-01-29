
/**********************************************************************
��Ŀ����/Project          : ���������ѧ��������
��������/Program name     : csd_client
�Ŷ�/Team                : ̫�������Ŷ� / Taichi-Maker (www.taichi-maker.com)
����/Author              : CYNO˷
����/Date��YYYYMMDD��     : 20200228
����Ŀ��/Purpose          : 
��ʾ���ʵ������NodeMCU��ͨ��WiFi����ͨѶ����̨NodeMCUһ���Է�����ģʽ���У�
һ���Կͻ���ģʽ���С�
 
�˴���Ϊ�ͻ��˴��롣�˴�����Ҫ���ܣ�
    - ͨ��HTTPЭ�������������HTTP����
    - HTTP�����а����ͻ��˰�����������״̬�ͳ����������
-----------------------------------------------------------------------
�޶���ʷ/Revision History  
����/Date    ����/Author      �ο���/Ref    �޶�˵��/Revision Description
-----------------------------------------------------------------------
http��Ϣ����˵��
192.168.0.123/update?float=1.5&int=2&button=0
***********************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   // ʹ��WiFiMulti�� 
 
#define buttonPin D3            // ��ť����D3
 
ESP8266WiFiMulti wifiMulti;     // ����ESP8266WiFiMulti����,���������� 'wifiMulti'
 
bool buttonState;       //�洢�ͻ��˰�����������
float clientFloatValue; //�洢�ͻ��˷��͵ĸ����Ͳ�������
int clientIntValue;     //�洢�ͻ��˷��͵������Ͳ�������
 
const char* host = "192.168.0.123";    // �������ӷ�������ַ/IP
const int httpPort = 80;               // �������ӷ������˿�
 
void setup(void){
  Serial.begin(9600);                  // ��������ͨѶ
  Serial.println("");
 
  pinMode(buttonPin, INPUT_PULLUP);    // ��������������Ϊ��������ģʽ
  
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1"); // ����Ҫ���ӵ�һϵ��WiFi ID��������������
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2"); // ESP8266-NodeMCU���������ɨ�赱ǰ����
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3"); // ���������Ƿ��������г���WiFi ID�������
  Serial.println("Connecting ...");                            // ����ʹ�ô˴��洢������������ӡ�
  
  while (wifiMulti.run() != WL_CONNECTED) { // ���Խ���wifi���ӡ�
    delay(250);
    Serial.print('.');
  }
 
  // WiFi���ӳɹ���ͨ�����ڼ�����������ӳɹ���Ϣ 
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // ͨ�����ڼ�����������ӵ�WiFi����
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // ͨ�����ڼ��������ESP8266-NodeMCU��IP
}
 
void loop(void){
 // ��ȡ��������״̬
 buttonState = digitalRead(buttonPin); 
 
 // �ı�����ñ�����ֵ���ڷ������˽������ݼ��
 clientFloatValue += 1.5;
 clientIntValue += 2;
 
 // ��������
 wifiClientRequest();
 delay(1000);
}
 
void wifiClientRequest(){
  WiFiClient client;  
 
  // ����Ҫ���͵�������Ϣ����ͻ�������
  String url = "/update?float=" + String(clientFloatValue) + 
               "&int=" + String(clientIntValue) +
               "&button=" + String(buttonState);
                         
  // �����ַ���������HTTP����
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";
                        
  Serial.print("Connecting to "); 
  Serial.print(host); 
  
  if (client.connect(host, httpPort)) {  //�������ʧ���򴮿������Ϣ��֪�û�Ȼ�󷵻�loop
    Serial.println(" Sucess");
    
    client.print(httpRequest);          // �����������HTTP����
    Serial.println("Sending request: ");// ͨ���������HTTP������Ϣ�����Ա����
    Serial.println(httpRequest);        
  } else{
    Serial.println(" failed");
  }
  
  client.stop();                         
}