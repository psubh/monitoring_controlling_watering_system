/*note:after aploading this code go to google search and search "adafruit io" then login by 
 * user name=psubh
 * passward=@subh9406
 * then you enter into account then click on humidity_sensor
 * now you can see three gauge in screen 1.DHT reading  2.water level sensor
 * and three on-off switch for controlling three solenoid valve according to observing values from this three gauge display
 * or use this url to open this https://io.adafruit.com/psubh/dashboards/humidity-sensor.
 
 */
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include <RtcDS3231.h>
#include <Wire.h>
RtcDS3231<TwoWire> Rtc(Wire);

#define DHTTYPE DHT11

#define WLAN_SSID       "psubh"//put your hotspot name in ""
#define WLAN_PASS       "subhash1998"//put passward as it is in ""


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "psubh"
#define AIO_KEY         "aio_NgKp90J43bVeiEQcaMIyVdAE5kPJ"

int ontime1hour=06;//enter morning hour time when you want to open solenoid valve 
int ontime1minute=30;//enter morning minute time when you want to open solenoid valve
int offtime1hour=07;//enter morning hour time when you want to close solenoid valve 
int offtime1minute=30;//enter morning minute time when you want to close solenoid valve

int ontime2hour=18;//enter evening hour time when you want to open solenoid valve 
int ontime2minute=10;//enter evening minute time when you want to open solenoid valve 
int offtime2hour=19;//enter evening hour time when you want to close solenoid valve 
int offtime2minute=10;//enter evening minute time when you want to close solenoid valve 

WiFiClient client;

int DHTPIN=14; //D5
int waterlevel_sensor =A0;
int solenoidpin1 =12;//D6
int solenoidpin2 =0;//D3
int solenoidpin3 =2;//D4

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish sensor1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish sensor2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/water_level");
Adafruit_MQTT_Publish sensor3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/");

Adafruit_MQTT_Subscribe onoffbutton_1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/solenoid_1");
Adafruit_MQTT_Subscribe onoffbutton_2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/solenoid_2");
Adafruit_MQTT_Subscribe onoffbutton_3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/solenoid_3");

void MQTT_connect();

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
   RtcDateTime compiled = RtcDateTime(2020,2,1,11,16,00);//set data and time when you going to check this code put current time here(yy,mm,d,hr,mn,sec)
   RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&onoffbutton_1);
  mqtt.subscribe(&onoffbutton_2);
  mqtt.subscribe(&onoffbutton_3);
}

void loop() {
  MQTT_connect();
  RtcDateTime now = Rtc.GetDateTime();
  if( now.Hour() == ontime1hour && now.Minute() ==ontime1minute )
  {
   digitalWrite(solenoidpin1,HIGH);
   digitalWrite(solenoidpin2,HIGH);
   digitalWrite(solenoidpin3,HIGH); 
  }
  if( now.Hour() == offtime1hour && now.Minute() ==offtime1minute )
  {
   digitalWrite(solenoidpin1,LOW);
   digitalWrite(solenoidpin2,LOW);
   digitalWrite(solenoidpin3,LOW); 
  }
   if( now.Hour() == ontime2hour && now.Minute() ==ontime2minute )
  {
   digitalWrite(solenoidpin1,HIGH);
   digitalWrite(solenoidpin2,HIGH);
   digitalWrite(solenoidpin3,HIGH); 
  }
  if( now.Hour() == offtime2hour && now.Minute() ==offtime2minute )
  {
   digitalWrite(solenoidpin1,LOW);
   digitalWrite(solenoidpin2,LOW);
   digitalWrite(solenoidpin3,LOW); 
  }
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton_1) {
      Serial.print(F("Got: "));
      String s1=(char *)onoffbutton_1.lastread;
      if(s1=="ON")
      {
      digitalWrite(solenoidpin1,HIGH);
      }
      else
      {
        digitalWrite(solenoidpin1,LOW);
      }
    }
     if (subscription == &onoffbutton_2) {
      Serial.print(F("Got: "));
      String s2=(char *)onoffbutton_2.lastread;
      if(s2=="ON")
      {
      digitalWrite(solenoidpin2,HIGH);
      }
      else
      {
        digitalWrite(solenoidpin2,LOW);
      }
    }
     if (subscription == &onoffbutton_3) {
      Serial.print(F("Got: "));
      String s3=(char *)onoffbutton_3.lastread;
      if(s3=="ON")
      {
      digitalWrite(solenoidpin3,HIGH);
      }
      else
      {
        digitalWrite(solenoidpin3,LOW);
      }
    }
  }
  

  Serial.print(F("\nSending humidity val "));
  float h = dht.readHumidity();
  if (! sensor1.publish(h)) {
    Serial.println(F("Failed"));
  } 
  else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending water level value "));
  float w = analogRead(waterlevel_sensor);
  float water_sensor =map(w, 0, 1024, 0, 100);
  if (! sensor2.publish(water_sensor)) {
    Serial.println(F("Failed"));
  } 
  else {
    Serial.println(F("OK!"));
  }
  delay(1000);
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
