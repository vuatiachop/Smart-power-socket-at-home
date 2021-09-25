#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

//#define FIREBASE_HOST "https://smart-power-socket-at-home-default-rtdb.asia-southeast1.firebasedatabase.app/"                   //Thay bằng địa chỉ firebase của bạn
//#define FIREBASE_AUTH "uQGKhlzeWpuzUni8ofwgTvqOTJOmVauTua0AMeB9"                                                                //Xác thực
//#define WIFI_SSID "BaoHan"
//#define WIFI_PASSWORD "06042004"

const String FIREBASE_HOST = "https://smart-power-socket-at-home-default-rtdb.asia-southeast1.firebasedatabase.app/";
const String FIREBASE_AUTH = "uQGKhlzeWpuzUni8ofwgTvqOTJOmVauTua0AMeB9";
const String WIFI_SSID = "BaoHan";
const String WIFI_PASSWORD = "06042004";

const long utcOffsetInSeconds = 25200;                                                                                        //UTC time offsets for UTC +7.00:7*60*60:25200
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "1.asia.pool.ntp.org", utcOffsetInSeconds);                                                        // Worldwide  --  pool.ntp.org; Asia  --  asia.pool.ntp.org

FirebaseData firebaseData;

int gio_NTP, phut_NTP;

int gio_relay_1_on, phut_relay_1_on;
int gio_relay_2_on, phut_relay_2_on;

int gio_relay_1_off, phut_relay_1_off;
int gio_relay_2_off, phut_relay_2_off;

int relay_1 = 5;
int relay_2 = 4;

int control_relay_1;
int control_relay_2;

String gio_relay_1_on_string, phut_relay_1_on_string;
String gio_relay_2_on_string, phut_relay_2_on_string;

String gio_relay_1_off_string, phut_relay_1_off_string;
String gio_relay_2_off_string, phut_relay_2_off_string;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Ket noi den wifi ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi da duoc ket noi");
  Serial.println("dia chi IP: "); 
  Serial.println(WiFi.localIP());
  
  timeClient.begin();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
}

void Getting_online_time(){
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());
  gio_NTP = timeClient.getHours();
  phut_NTP = timeClient.getMinutes();
} 

void Control_relay_by_set_time(){
  if( (gio_relay_1_on == gio_NTP) && (phut_relay_1_on == phut_NTP) ){
    digitalWrite(relay_1,HIGH);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_1/Trạng thái", "Đang Mở");
  }
  if( (gio_relay_1_off == gio_NTP) && (phut_relay_1_off == phut_NTP) ){
    digitalWrite(relay_1,LOW);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_1/Trạng thái", "Đang Tắt");
  }
  
  if( (gio_relay_2_on == gio_NTP) && (phut_relay_2_on == phut_NTP) ){
    digitalWrite(relay_2,HIGH);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_2/Trạng thái", "Đang Mở");
  }
  if( (gio_relay_2_off == gio_NTP) && (phut_relay_2_off == phut_NTP) ){
    digitalWrite(relay_2,LOW);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_2/Trạng thái", "Đang Tắt");
  }
}

void Get_data_firebase(){
  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_1/gio_on");
  gio_relay_1_on_string=firebaseData.stringData();
  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_2/gio_on");
  gio_relay_2_on_string=firebaseData.stringData();

  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_1/gio_off");
  gio_relay_1_off_string=firebaseData.stringData();
  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_2/gio_off");
  gio_relay_2_off_string=firebaseData.stringData();

  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_1/phut_on");
  phut_relay_1_on_string=firebaseData.stringData();
  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_2/phut_on");
  phut_relay_2_on_string=firebaseData.stringData();
  
  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_1/phut_off");
  phut_relay_1_off_string=firebaseData.stringData();
  Firebase.getString(firebaseData,"Nghi'sHouse/Relay_2/phut_off");
  phut_relay_2_off_string=firebaseData.stringData();
  
  Firebase.getInt(firebaseData,"Control_relay_1");
  control_relay_1 = firebaseData.intData();
  Firebase.getInt(firebaseData,"Control_relay_2");
  control_relay_2 = firebaseData.intData();
  
                                                                        
  gio_relay_1_on = gio_relay_1_on_string.toInt();
  gio_relay_2_on = gio_relay_2_on_string.toInt();
  gio_relay_1_off = gio_relay_1_off_string.toInt();
  gio_relay_2_off = gio_relay_2_off_string.toInt();
  
  phut_relay_1_on = phut_relay_1_on_string.toInt();
  phut_relay_2_on = phut_relay_2_on_string.toInt();
  phut_relay_1_off = phut_relay_1_off_string.toInt();
  phut_relay_2_off = phut_relay_2_off_string.toInt();
}

void Control_relay_manual(){
  if ( control_relay_1 == 1){
    digitalWrite(relay_1,HIGH);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_1/Trạng thái", "Đang Mở");
  }
  if ( control_relay_1 == 0){
    digitalWrite(relay_1,LOW);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_1/Trạng thái", "Đang Tắt");
  }

    if ( control_relay_2 == 1){
    digitalWrite(relay_2,HIGH);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_2/Trạng thái", "Đang Mở");
  }
  if ( control_relay_2 == 0){
    digitalWrite(relay_2,LOW);
    Firebase.setString(firebaseData,"Nghi'sHouse/Relay_2/Trạng thái", "Đang Tắt");
  }
}

void loop() {
Getting_online_time();
Control_relay_by_set_time();
Control_relay_manual();
Get_data_firebase();
}
