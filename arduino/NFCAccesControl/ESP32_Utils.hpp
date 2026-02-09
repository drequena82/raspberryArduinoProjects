void connectWiFi_STA(bool useStaticIP = false){
   Serial.println("Conecting");
   WiFi.mode(WIFI_STA);
   WiFi.begin(SSID, WLAN_PASSWORD);
   if(useStaticIP) {
    WiFi.config(ip, gateway, subnet);
   }
   while (WiFi.status() != WL_CONNECTED) { 
     delay(100);  
     Serial.print('.'); 
   }
 
   Serial.println("");
   Serial.print("Init STA:\t");
   Serial.println(SSID);
   Serial.print("IP address:\t");
   Serial.println(WiFi.localIP());
}

void connectWiFi_AP(bool useStaticIP = false){ 
   Serial.println("Conecting");
   WiFi.mode(WIFI_AP);
   while(!WiFi.softAP(SSID, WLAN_PASSWORD)){
     Serial.println(".");
     delay(100);
   }
   if(useStaticIP) {
    WiFi.softAPConfig(ip, gateway, subnet);
   }

   Serial.println("");
   Serial.print("Init AP:\t");
   Serial.println(SSID);
   Serial.print("IP address:\t");
   Serial.println(WiFi.softAPIP());
}
