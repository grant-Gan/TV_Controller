#include "TV.h"

TV :: TV()
{
  pinMode(IRLED, OUTPUT);
}

///关闭电视电源
void TV :: turnOffTv()
{
  IRsend irsend(IRLED);
  delay(1000);
  irsend.sendNEC(irsend.encodeNEC(0xbf00, 0xd), 32, 0);
}

bool TV :: setNetStatus( bool setStatus )
{
  //如果网络状态与要设置的网络状态相同, 则直接返回值, 避免重复调用
  if( this->netStatus == setStatus )
    return setStatus;
  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  String enable = setStatus ? "0" : "1";
  http.begin(client, "http://192.168.1.1/userRpm/AccessCtrlAccessRulesRpm.htm?enable=" + enable + "&enableId=0&Page=1"); //HTTP
  http.addHeader("Referer", "http://192.168.1.1/userRpm/AccessCtrlAccessRulesRpm.htm");
  http.addHeader("Cookie", "Authorization=Basic%20YWRtaW46MTIzNDU2Nzg5; ChgPwdSubTag=");
  
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header and body
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    //改变TV对象中网络的状态
    this->netStatus = setStatus;
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      http.getString();
      // const String& payload = http.getString();
      // Serial.println("received payload:\n<<");
      // Serial.println(payload);
      // Serial.println(">>");
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

}

///通过ADC检测电视的电源状态
///checkCount : 检查次数定义
bool TV :: getTvPowerState(int checkCount) //检测电视的是否开机
{
  int voltageSample[50] = {0};
  int maxValue = 0, minValue = 0;

  for (int i = 0; i < checkCount; i++)
  {
    voltageSample[i] = analogRead(powerCheckPin);
    delay(1);
  }
  maxValue = getMaxValue(voltageSample);
  minValue = getMinValue(voltageSample);
  //求峰峰值, 大于100则可判定电视开机
  if (maxValue - minValue > this->voltageThreshold){
    powerStatus = true;
    return true;
  } else {
    powerStatus = false;
    return false;
  }
}

///获取数组中的最大值
int TV :: getMaxValue(int sample[])
{
  int maxValue = sample[0];
  for (int i = 1; i < 50; i++)
  {
    if (sample[i] > maxValue)
      maxValue = sample[i];
    else
      continue;
  }
  return maxValue;
}

///获取数组中的最小值
int TV :: getMinValue(int sample[])
{
  int minValue = sample[0];
  for (int i = 1; i < 50; i++)
  {
    if (sample[i] < minValue)
      minValue = sample[i];
    else
      continue;
  }
  return minValue;
}

bool TV :: getPowerStatus()
{
  return this->powerStatus;
}

bool TV :: netPowerStatus()
{
  return this->netStatus;
}