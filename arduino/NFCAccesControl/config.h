#include "c_types.h"
const char* SSID     = "HOMEFIBER2";
const char* WLAN_PASSWORD = "1A2B3C4D5E6F";
const char* HOSTNAME = "NFCACCESSCONTROL";
const String API_HOST = "http://192.168.1.1:8080/keyCardAccess";
const String USERNAME = "";
const String USERPASS = "";
const uint8 DEVICE_LEVEL = 1;
const bool TEST_MODE = true;
const uint8_t KEY[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
