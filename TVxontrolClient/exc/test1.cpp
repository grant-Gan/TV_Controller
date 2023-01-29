
/**********************************************************************
��Ŀ����/Project          : ���������ѧ��������
��������/Program name     : cgd_client
�Ŷ�/Team                : ̫�������Ŷ� / Taichi-Maker (www.taichi-maker.com)
����/Author              : CYNO˷
����/Date��YYYYMMDD��     : 20200228
����Ŀ��/Purpose          : 
��ʾ���ʵ������NodeMCU��ͨ��WiFi����ͨѶ����̨NodeMCUһ���Է�����ģʽ���У�
һ���Կͻ���ģʽ���С�
 
�˴���Ϊ�ͻ��˴��롣�˴�����Ҫ���ܣ�
    - ��ʱ�����������������Ϣ���Ӷ���ȡ�������������ϰ�ť״̬
    - ���������˷����İ�ť���ŵ�ƽ״̬����Ϊ�������������ø����������ư���LED������Ϩ��
***********************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
 
ESP8266WiFiMulti wifiMulti;           // ����ESP8266WiFiMulti����
 
const char* host = "192.168.0.123";   // ���������IP
const int httpPort = 80;              // http�˿�80
 
void setup(){
  Serial.begin(9600);          
  Serial.println("");
 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1"); // ����Ҫ���ӵ�һϵ��WiFi ID��������������
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2"); // ESP8266-NodeMCU���������ɨ�赱ǰ����
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3"); // ���������Ƿ��������г���WiFi ID�������
  Serial.println("Connecting ..."); 
 
  int i = 0;  
  while (wifiMulti.run() != WL_CONNECTED) { // ���Խ���wifi���ӡ�
    delay(1000);
    Serial.print(i++); Serial.print(' ');
  }
  
  // WiFi���ӳɹ���ͨ�����ڼ�����������ӳɹ���Ϣ 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // WiFi����
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // IP
}
 
void loop(){
  wifiClientRequest();  
  delay(3000);
}
 
void wifiClientRequest(){
  WiFiClient client;          // ����WiFiClient����
 
  bool buttonState;     // �����������ť״̬����  
         
  Serial.print("Connecting to "); Serial.print(host);
 
  // ���ӷ�����
  if (client.connect(host, httpPort)){
    Serial.println(" Success!");
    
    // �����ͻ���������Ϣ
    String httpRequest =  String("GET /update") + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n" +
                          "\r\n";
                          
    // ���Ϳͻ�������
    Serial.println("Sending request: ");Serial.print(httpRequest);  
    client.print(httpRequest);
 
    // ��ȡ��������Ӧ��Ϣ�еİ�ť״̬��Ϣ
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
  client.stop();    // ֹͣ�ͻ���  
   
  // ���ݷ���������״̬������Ϩ��LED
  buttonState == 0 ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);
}