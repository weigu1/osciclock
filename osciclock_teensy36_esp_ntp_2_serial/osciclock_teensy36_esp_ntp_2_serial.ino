/*
  osciclock_teensy36_esp_ntp_2_serial.ino
  ESP8266: Sending NTP time over hardware Serial (Serial1 or Serial2 (ESP32))
  weigu.lu
  for more infos look here:
  http://weigu.lu/microcontroller/tips_tricks/esp_NTP_tips_tricks/index.html
*/

#ifdef ESP8266   // all includes are from Arduino Core, no external lib
  #include <ESP8266WiFi.h>       
  // <time.h> and <WiFiUdp.h> not needed. already included by core.         
#else
  #include <WiFi.h>         
  #include <time.h>
#endif

// The file "secrets.h" has to be placed in the sketchbook libraries folder
// in a folder named "Secrets" and must contain your secrets e.g.:
// const char *MY_WIFI_SSID = "mySSID"; const char *MY_WIFI_PASSWORD = "myPASS";
#define USE_SECRETS
#define DEBUG
#define HSERIAL Serial1 

/****** WiFi and network settings ******/
#ifdef USE_SECRETS
  #include <secrets.h>
  const char *WIFI_SSID = MY_WIFI_SSID;             
  const char *WIFI_PASSWORD = MY_WIFI_PASSWORD;     // password
#else
  const char* *WIFI_SSID = mySSID;         // if no secrets file, add your SSID here
  const char* *WIFI_PASSWORD = myPASSWORD; // if no secrets file, add your PASS here
#endif

/****** NTP settings ******/
const char *NTP_SERVER = "lu.pool.ntp.org";
// your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
const char *TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";
const char *TZ_INFO_UTC  = "CET0CEST0";

time_t now, now2;
tm timeinfo;                      // time structure

void setup() {
#ifdef   DEBUG
  Serial.begin(115200);  
  delay(1000);
  Serial.println("\nHello");
#endif    
  HSERIAL.begin(115200);
  init_ntp_time();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  #ifdef   DEBUG
    Serial.print ( "." );
  #endif  
  }
  time(&now);                     // this function calls the NTP server only every hour
  
  #ifdef   DEBUG  
    Serial.println("\nConnected to SSID " + WiFi.SSID() + " with IP " + 
                    WiFi.localIP().toString() + "\nSignal strength is " +
                    WiFi.RSSI() + " dBm\n");
    Serial.println("Epoch time (UTC): " + String(now));
    Serial.println("Setup done!");
  #endif  
}

void loop() {    
  time(&now);                   // this function calls the NTP server only every hour
  setenv("TZ", TZ_INFO, 1);     // set environment variable with your time zone
  tzset();
  localtime_r(&now, &timeinfo);
  setenv("TZ", TZ_INFO_UTC, 1); // set environment variable UTC:
  tzset();                      // trick to force mktime to deliver what we need
  now2 = mktime(&timeinfo);  
  
#ifdef   DEBUG    
  Serial.println('T' + String(now2));    
#endif  
  HSERIAL.println('T' + String(now2));
  delay(3600000);
}

// init NTP time: call this before the WiFi connect!
void init_ntp_time() { 
  #ifdef ESP8266
    configTime(TZ_INFO, NTP_SERVER);    
  #else  
    configTime(0, 0, NTP_SERVER); // 0, 0 because we will use TZ in the next line
    setenv("TZ", TZ_INFO, 1);     // set environment variable with your time zone
    tzset();
  #endif
}
