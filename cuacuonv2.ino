#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ESP8266HTTPClient.h>
#include "html.h"
#include "tiny-json.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define UPDATE_PATH                 "/update"
#define UPDATE_USER                 "DamDat"
#define UPDATE_PASS                 "04010098"
#define LOCAL_CONTROL_PASS          "04010098"

///network config fors soft ap mode
#define SOFT_AP_SSID                "CuaCuonVanTam"
#define SOFT_AP_PASS                "04010098"
#define SOFT_AP_CONTROL_PAGE_PASS   "04010098"
IPAddress softAPIP(192,168,11,2);
IPAddress softAPGateway(192,168,11,1);
IPAddress softAPSubnet(255,255,255,0);
//network wifi config
#define NETWORK_WIFI_SSID            "VanTam3"
#define NETWORK_WIFI_PASS            "11111972"

HTTPClient httpClient;
WiFiClient wifiClient;
AsyncWebServer w(80);

//PIN DEFINE
#define UP_PIN          13
#define DOWN_PIN        14
#define STOP_PIN        12
static bool stopFlag    =   false;
static bool upFlag      =   false;
static bool downFlag    =   false;

//wireless config status
bool networkWiFiBegin   =   false;
bool firstTimeConnected =   false;
String localNetworkIP   =   "Unknow";

//Blynk 
#define BLYNK_TEMPLATE_ID "TMPLFe0_Vs08"
#define BLYNK_DEVICE_NAME "CuaCuon"
#define BLYNK_AUTH_TOKEN "GdE5CpEBs-_Yyxzd4N0K7GRCzgpqDZdU"
String urlGetMulti = "http://blynk.cloud/external/api/get?";
String deviceToken = "token=GdE5CpEBs-_Yyxzd4N0K7GRCzgpqDZdU&v0&v1&v2";
bool firstTimeGetData = false;
int v0Val;
int v1Val;
int v2Val;
int v0Valp;
int v1Valp;
int v2Valp;


//timing 
unsigned long now,preHttp,preW;


//function
void http();

void setup() {
  // pin init:
  pinMode(14,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(12,HIGH);
  digitalWrite(13,HIGH);
  digitalWrite(14,HIGH);


  Serial.begin(9600);
  Serial.println("check");
  WiFi.disconnect();
  delay(200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(softAPIP,softAPGateway,softAPSubnet);
  WiFi.softAP(SOFT_AP_SSID,SOFT_AP_PASS);
  Serial.println("Soft begin");
  AsyncElegantOTA.begin(&w,UPDATE_USER,UPDATE_PASS);
  Serial.println("OTA Server begin");
  w.on("/",HTTP_GET,[](AsyncWebServerRequest *request){

    int paramsNr = request->params();
    if (paramsNr > 0)
    {
      for(uint8_t i = 0 ; i< paramsNr ; i++)
      {
        AsyncWebParameter* p = request->getParam(i);
        String hArgName = p->name();
        if(hArgName == "func")
        {
          String cmd = p->value();
          request->send(200,"text/plain","c:"+cmd+" "+localNetworkIP);
          if(cmd=="1")
          {
            upFlag = true;
          }
          else if(cmd == "2")
          {
            stopFlag = true;
          }
          else if(cmd == "3")
          {
            downFlag = true;
          }
        }
      }
    }
    else
    {
      request->send(200,"text/html",controlPage);
    }
  });
  w.begin();
  Serial.println("local control server assigned");
  WiFi.begin(NETWORK_WIFI_SSID,NETWORK_WIFI_PASS);
  Serial.println("WiFi begin");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  localNetworkIP = WiFi.localIP().toString();

  //blynk setup
  Blynk.begin(BLYNK_AUTH_TOKEN,NETWORK_WIFI_SSID,NETWORK_WIFI_PASS);
  // timing 
  now = preHttp = millis();
}

bool requested=false;
int httpCode;


void loop() {
  Blynk.run();
  now = millis();
  //pin control
  if(WiFi.status()==WL_CONNECTED)
  {
    if(now - preHttp > 500) 
    {
      preHttp = now;
      http();    
    } 
  }
  else
  {
    firstTimeGetData = false;
  }

  if(firstTimeGetData)
  {
    if(v0Val != v0Valp)
    {
      v0Valp = v0Val;
      upFlag = true;
    }
    if(v1Val != v1Valp)
    {
      v1Valp = v1Val;
      stopFlag = true;
    }
    if(v2Val != v2Valp)
    {
      v2Valp = v2Val;
      downFlag = true;
    }
  }

  if(upFlag||downFlag||stopFlag)
  {
    if(upFlag)
    {
      upFlag=false;
      digitalWrite(UP_PIN,HIGH);
      delay(20);
      digitalWrite(UP_PIN,LOW);
      Serial.println(1);
    }
    if(downFlag)
    {
      downFlag=false;
      digitalWrite(DOWN_PIN,HIGH);
      delay(20);
      digitalWrite(DOWN_PIN,LOW);
      Serial.println(3);
    }
    if(stopFlag)
    {
      stopFlag=false;
      digitalWrite(STOP_PIN,HIGH);
      delay(20);
      digitalWrite(STOP_PIN,LOW);
      Serial.println(2);
    }
  }
}

BLYNK_WRITE(V1)
{
  // Read the value of the virtual pin
  int pinValue = param.asInt();

  // Set the value of the physical pin to the value of the virtual pin
  digitalWrite(pin1, pinValue);
}


void http()
{
  if(httpClient.begin(wifiClient,urlGetMulti+deviceToken))
  {
    httpCode = httpClient.GET();
    if(httpCode>0)
    {
      if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = httpClient.getString();
        Serial.println(payload);
        char buf[100];
        payload.toCharArray(buf,payload.length()+1);
        json_t mem[16];
        json_t const * json = json_create(buf,mem,sizeof mem / sizeof *mem );
        if(!json)
        {
          firstTimeGetData = false;
          Serial.println("Error json");
        }
        else
        {
          json_t const * v0 = json_getProperty(json,"v0");
          json_t const * v1 = json_getProperty(json,"v1");
          json_t const * v2 = json_getProperty(json,"v2");
          if(!v0||!v1||!v2||JSON_INTEGER != json_getType(v0)||JSON_INTEGER != json_getType(v1)||JSON_INTEGER != json_getType(v2))
          {
            Serial.println("error");
            firstTimeGetData = false;
          }
          else
          {
            v0Val = json_getInteger(v0);
            v1Val = json_getInteger(v1);
            v2Val = json_getInteger(v2);
            Serial.println("V0="+String(v0Val)+"   "+"v1="+String(v1Val)+"   "+"v2="+String(v2Val));
            if(!firstTimeGetData)
            {
              v0Valp = v0Val;
              v1Valp = v1Val;
              v2Valp = v2Val;
            }
            firstTimeGetData = true;
          }
        }
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
        firstTimeGetData = false;
      }
      httpClient.end();
    }
    else
    {
      firstTimeGetData = false;
      Serial.printf("[HTTP} Unable to connect tt\n");
    }
  }
  else
  {
    firstTimeGetData = false;
    Serial.println("error http begin");
  }
} 
