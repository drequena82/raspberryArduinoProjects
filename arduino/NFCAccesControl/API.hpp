String processResponse(int httpCode, HTTPClient& http){
	String responseData = "{}";
	if (httpCode > 0) {
		Serial.printf("Response code: %d\t", httpCode);

		if (httpCode == HTTP_CODE_OK) {
			responseData = http.getString();
		}
	} else {
		Serial.printf("Request failed, error: %s\n", http.errorToString(httpCode).c_str());
	}
	http.end();

	return responseData;
}

String getToken(String username, String password){
	HTTPClient http;
	WiFiClient client;
	if(http.begin(client, apiHost + "/login")){
		// Build a json with credentials
		String message = "";
		JsonDocument jsonDoc;
		jsonDoc["username"] = username;
		jsonDoc["password"] = password;
		serializeJson(jsonDoc, message);
		Serial.println(message);
		
		int httpCode = http.POST(message);
		if(httpCode >= 300 || httpCode < 200){
			return "error";
		}
		
		// Process response
		String data = processResponse(httpCode, http);	
		Serial.println(message);
		JsonDocument doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) { 
			return "error"; 
		}

		return doc["token"];
	}
	return "error";
}

JsonDocument getCardCredentials(char* keyId){
	HTTPClient http;
	WiFiClient client;
	JsonDocument doc;
	String strKeyId = String(keyId);
	if(http.begin(client, apiHost + "/keyCard/" + strKeyId)){
		http.addHeader("Content-Type", "application/json");
		String token = getToken(username,userpass);
		if(token == "error"){
			return doc;
		}
		http.addHeader("Authorization", token);
		int httpCode = http.GET();
		String response = processResponse(httpCode, http);
		JsonDocument doc;
		DeserializationError error = deserializeJson(doc, response);
		if (error) { 
			return doc; 
		}
		// Level of the key card (1, 2 , 3). 1 minimum level, 3 maximum level.
		// Gate level defined by config. 
	}
	return doc;
}

JsonDocument newCard(String keyId, String username, uint level){
	HTTPClient http;
	WiFiClient client;
	JsonDocument doc;

	if(http.begin(client, apiHost + "/keyCard/new")){
		http.addHeader("Content-Type", "application/json");
		String token = getToken(username,userpass);
		if(token == "error"){
			return doc;
		}

		// Build a json with credentials
		String message = "";
		JsonDocument jsonDoc;
		jsonDoc["username"] = username;
		jsonDoc["keyId"] = keyId;
		jsonDoc["level"] = level;
		serializeJson(jsonDoc, message);
		Serial.println(message);
		
		int httpCode = http.POST(message);
		if(httpCode >= 300 || httpCode < 200){
			return doc;
		}
		
		// Process response
		String data = processResponse(httpCode, http);	
		Serial.println(message);
		
		DeserializationError error = deserializeJson(doc, data);
		if (error) { 
			return doc; 
		}
	}
	return doc;
}



