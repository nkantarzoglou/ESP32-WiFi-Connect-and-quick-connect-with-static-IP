#include <WiFi.h>


char ssid[32] = "SSID";
char password[63] = "PASS";

int local_IP[4] = { 0, 0, 0, 0 };
int gateway[4] = { 0, 0, 0, 0 };
int subnet[4] = { 0, 0, 0, 0 };
int primaryDNS[4] = { 0, 0, 0, 0 };
int secondaryDNS[4] = { 0, 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("SETUP COMPLETE");
}
void loop() {
  delay(1000);
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
  if (strlen(ssid) == 0 || strlen(password) == 0) {
    Serial.println("Credentials not set.");
    return 0;
  }
  if (compareIpItemsWithDefault()) {
    Serial.println("IP items are default. Doing normal connection.");
    if (normalWifiConnect()) {
      Serial.println("Connected using normalWifiConnect.");
      addIpItems(WiFi.localIP(), WiFi.subnetMask(), WiFi.gatewayIP(), WiFi.dnsIP(0), WiFi.dnsIP(1));
      return 2;
    }
    Serial.println("Connection error using normalWifiConnect.");
    return 1;
  } else {
    Serial.println("IP items are NOT default. Doing quick connection.");
    if (quickWifiConnect()) {
      Serial.println("Connected using quickWifiConnect.");
      return 2;
    } else {
      Serial.println("Failed to connect using quickWifiConnect. Will attempt normalWifiConnect.");
      resetIpItems();
      if (normalWifiConnect()) {
        Serial.println("Connected using normalWifiConnect.");
        addIpItems(WiFi.localIP(), WiFi.subnetMask(), WiFi.gatewayIP(), WiFi.dnsIP(0), WiFi.dnsIP(1));
        return 2;
      }
      Serial.println("Connection error using normalWifiConnect.");
      return 1;
    }
  }
}

boolean quickWifiConnect() {
  int counter = 10;  //10 will be 5 seconds
  WiFi.config(convertAddress(local_IP), convertAddress(gateway), convertAddress(subnet), convertAddress(primaryDNS), convertAddress(secondaryDNS));
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP32");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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
  WiFi.config(convertAddress(local_IP), convertAddress(gateway), convertAddress(subnet), convertAddress(primaryDNS), convertAddress(secondaryDNS));
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP32");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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

void addIpItems(IPAddress lip, IPAddress sm, IPAddress gip, IPAddress dsnp, IPAddress dnss) {
  for (int i = 0; i < 4; i++) {
    local_IP[i] = lip[i];
    gateway[i] = gip[i];
    subnet[i] = sm[i];
    primaryDNS[i] = dsnp[i];
    secondaryDNS[i] = dnss[i];
  }
}
void resetIpItems() {
  for (int i = 0; i < 4; i++) {
    local_IP[i] = 0;
    gateway[i] = 0;
    subnet[i] = 0;
    primaryDNS[i] = 0;
    secondaryDNS[i] = 0;
  }
}

boolean compareIpItemsWithDefault() {
  for (int i = 0; i < 4; i++) {
    if (local_IP[i] != 0 || gateway[i] != 0 || subnet[i] != 0 || primaryDNS[i] != 0 || secondaryDNS[i] != 0) {
      return false;
    }
    return true;
  }
}

IPAddress convertAddress(int array[]) {
  IPAddress address(array[0], array[1], array[2], array[3]);
  return address;
}