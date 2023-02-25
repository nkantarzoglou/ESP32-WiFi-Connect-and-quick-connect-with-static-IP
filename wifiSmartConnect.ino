#include <Preferences.h>
#include "WiFi.h"

struct wifiData {
  char ssid[32] = "SSID";
  char password[63] = "PASS";
  int local_IP[4];
  int gateway[4];
  int subnet[4];
  int primaryDNS[4];
  int secondaryDNS[4];
};

Preferences preferences;
wifiData connectionData;

void setup() {
  Serial.begin(115200);
  delay(500);
  initDataFromNVS();
  Serial.println("SETUP COMPLETE");
}
void loop() {
  int res = wifiConnectV2();
  Serial.println("Result is: " + String(res));
  Serial.println("Delaying 10 seconds");
  delay(10000);
}
/*
wifi status:
0 -> ssid and/or password not set
1 -> ssid and password set but unsuccesful connection with quickWifi and without quickWifi
2 -> ssid and password set correctly and succesful connection 
*/
int wifiConnectV2() {
  if (strlen(connectionData.ssid) == 0 || strlen(connectionData.password) == 0) {
    Serial.println("Credentials not set.");
    return 0;
  }
  if (compareIpItemsWithDefault()) {
    Serial.println("IP items are default. Doing normal connection.");
    if (normalWifiConnect()) {
      Serial.println("Connected using normalWifiConnect.");
      changeIpItems(WiFi.localIP(), WiFi.subnetMask(), WiFi.gatewayIP(), WiFi.dnsIP(0), WiFi.dnsIP(1));
      return 2;
    }
    Serial.println("Connection error using normalWifiConnect.");
    return 1;

  } else {
    Serial.println("IP items are NOT default. Doing quick connection.");
    if (quickWifiConnect()) {
      Serial.println("Failed to connect using quickWifiConnect. Will attempt normalWifiConnect.");
      return 2;
    } else {
      Serial.println("Failed to connect using quickWifiConnect. Will attempt normalWifiConnect.");
      int resetArray[4] = { 0, 0, 0, 0 };
      changeIpItems(convertAddress(resetArray), convertAddress(resetArray), convertAddress(resetArray), convertAddress(resetArray), convertAddress(resetArray));
      if (normalWifiConnect()) {
        Serial.println("Connected using normalWifiConnect.");
        changeIpItems(WiFi.localIP(), WiFi.subnetMask(), WiFi.gatewayIP(), WiFi.dnsIP(0), WiFi.dnsIP(1));
        return 2;
      }
      Serial.println("Connection error using normalWifiConnect.");
      return 1;
    }
  }
}
boolean quickWifiConnect() {
  int counter = 10;  //10 will be 5 seconds
  WiFi.config(convertAddress(connectionData.local_IP), convertAddress(connectionData.gateway), convertAddress(connectionData.subnet), convertAddress(connectionData.primaryDNS), convertAddress(connectionData.secondaryDNS));
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP32");
  Serial.println("Connecting to ");
  Serial.println(connectionData.ssid);
  WiFi.begin(connectionData.ssid, connectionData.password);
  while (WiFi.status() != WL_CONNECTED && counter > 0) {
    Serial.print(".");
    counter--;
    delay(500);
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  return false;
}
boolean normalWifiConnect() {
  int counter = 30;  //10 will be 5 seconds
  WiFi.config(convertAddress(connectionData.local_IP), convertAddress(connectionData.gateway), convertAddress(connectionData.subnet), convertAddress(connectionData.primaryDNS), convertAddress(connectionData.secondaryDNS));
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP32");
  Serial.println("Connecting to ");
  Serial.println(connectionData.ssid);
  WiFi.begin(connectionData.ssid, connectionData.password);
  while (WiFi.status() != WL_CONNECTED && counter > 0) {
    Serial.print(".");
    counter--;
    delay(500);
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  return false;
}
void changeIpItems(IPAddress lip, IPAddress sm, IPAddress gip, IPAddress dsnp, IPAddress dnss) {
  for (int i = 0; i < 4; i++) {
    connectionData.local_IP[i] = lip[i];
    connectionData.gateway[i] = gip[i];
    connectionData.subnet[i] = sm[i];
    connectionData.primaryDNS[i] = dsnp[i];
    connectionData.secondaryDNS[i] = dnss[i];
  }
  preferences.begin("wifiData", false);
  preferences.putBytes("local_IP", connectionData.local_IP, sizeof(int[4]));
  preferences.putBytes("gateway", connectionData.gateway, sizeof(int[4]));
  preferences.putBytes("subnet", connectionData.subnet, sizeof(int[4]));
  preferences.putBytes("primaryDNS", connectionData.primaryDNS, sizeof(int[4]));
  preferences.putBytes("secondaryDNS", connectionData.secondaryDNS, sizeof(int[4]));
  preferences.end();
}
boolean compareIpItemsWithDefault() {
  for (int i = 0; i < 4; i++) {
    if (connectionData.local_IP[i] != 0 || connectionData.gateway[i] != 0 || connectionData.subnet[i] != 0 || connectionData.primaryDNS[i] != 0 || connectionData.secondaryDNS[i] != 0) {
      return false;
    }
    return true;
  }
}
IPAddress convertAddress(int array[]) {
  IPAddress address(array[0], array[1], array[2], array[3]);
  return address;
}
void initDataFromNVS() {
  //we want to read only
  preferences.begin("wifiData", true);
  preferences.getBytes("ssid", connectionData.ssid, sizeof(char[32]));
  preferences.getBytes("password", connectionData.password, sizeof(char[63]));
  preferences.getBytes("local_IP", connectionData.local_IP, sizeof(int[4]));
  preferences.getBytes("gateway", connectionData.gateway, sizeof(int[4]));
  preferences.getBytes("subnet", connectionData.subnet, sizeof(int[4]));
  preferences.getBytes("primaryDNS", connectionData.primaryDNS, sizeof(int[4]));
  preferences.getBytes("secondaryDNS", connectionData.secondaryDNS, sizeof(int[4]));
  preferences.end();
}
