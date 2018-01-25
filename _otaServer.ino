/******************************************************************************************
 *
 * Description: Source code for single-channel LoRaWAN Gateway based on ESP8266 and SX1276
 * Version    : 0.8.1
 * Date       : 2018-01-25
 * Software   : https://github.com/SandboxElectronics/LoRaGoDOCK-Gateway
 * Hardware   : LoRaGo DOCK – http://sandboxelectronics.com/?product=lorago-dock-single-channel-lorawan-gateway
 * 
 * Copyright (c) 2016, 2017 Maarten Westenberg
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the MIT License
 * which accompanies this distribution, and is available at
 * https://opensource.org/licenses/mit-license.php
 *
 *****************************************************************************************/
 
// This file contains the otaa code for the ESP Single Channel Gateway.

// Provide OTAA server funcionality so the 1ch gateway can be updated 
// over the air.
// This code uses the ESPhttpServer functions to update the gateway.

#if A_OTA==1

//extern ArduinoOTAClass ArduinoOTA;

// Make sure that webserver is running before continuing

// ----------------------------------------------------------------------------
// setupOta
// Function to run in the setup() function to initialise the update function
// ----------------------------------------------------------------------------
void setupOta(char *hostname) {
	ArduinoOTA.begin();
	Serial.println(F("OTA Started"));
	
	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname(hostname);
	
	ArduinoOTA.onStart([]() {
		String type;
		// XXX version mispatch of platform.io and ArduinoOtaa
		// see https://github.com/esp8266/Arduino/issues/3020
		//if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		//else // U_SPIFFS
		//	type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	
	// Only if the Webserver is active also
#if A_SERVER==2										// Displaed for the moment
	ESPhttpUpdate.rebootOnUpdate(false);
   
	server.on("/esp", HTTP_POST, [&](){
   
      HTTPUpdateResult ret = ESPhttpUpdate.update(server.arg("firmware"), "1.0.0");
	  
      switch(ret) {
        case HTTP_UPDATE_FAILED:
            //PREi::sendJSON(500, "Update failed.");
			Serial.println(F("Update failed"));
            break;
        case HTTP_UPDATE_NO_UPDATES:
            //PREi::sendJSON(304, "Update not necessary.");
			Serial.println(F("Update not necessary"));
            break;
        case HTTP_UPDATE_OK:
            //PREi::sendJSON(200, "Update started.");
			Serial.println(F("Update started"));
            ESP.restart();
            break;
		default:
			Serial.println(F("setupOta:: Unknown ret="));
      }
	});
#endif
}


// ----------------------------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
void updateOtaa() {

	String response="";
	printIP((IPAddress)WiFi.localIP(),'.',response);
	
	ESPhttpUpdate.update(response, 80, "/arduino.bin");

}


#endif
