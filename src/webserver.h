#include <ArduinoJson.h> // 6.9.0 or later
#include "spiffs.h"

const char* www_realm = "Custom Auth Realm";
String authFailResponse = "<meta http-equiv='refresh' content='0; url=/' /> Authentication Failed! <p><a href='/'>Redirect</a></p>";
  
void handle_root() {
  String message = FPSTR(WEB_PAGE_HEADER);
    	message.replace("{Language}", (TEXT_LANG));
    	message.replace("{CurrentPageTitle}", (TEXT_INDEX_PAGE));
    	message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
    	message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
    	message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));
	
	message += FPSTR(WEB_ROOT_PAGE_MEASUREMENTS);
	
	if (!AUTOUPDATE_ON) {
		if (need_update) {
			message.replace("{WEB_UPDATE_INFO_WARNING}", FPSTR(WEB_UPDATE_INFO_WARNING));
			message.replace("{TEXT_FWUPDATEAVALIBLE}", (TEXT_FWUPDATEAVALIBLE));
			message.replace("{MANUALUPDATEBUTTON}", FPSTR(WEB_UPDATE_BUTTON_MANUALUPDATE));
			message.replace("{TEXT_MANUALUPDATEBUTTON}", (TEXT_MANUALUPDATEBUTTON));
			message.replace("{FWUPDATEBUTTON}", FPSTR(WEB_UPDATE_BUTTON_FWUPDATE));
			message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
			message.replace("{AUTOUPDATEONBUTTON}", FPSTR(WEB_UPDATE_BUTTON_AUTOUPDATEON));
			message.replace("{TEXT_AUTOUPDATEONBUTTON}", (TEXT_AUTOUPDATEONBUTTON));
			message.replace("{TEXT_AUTOUPDATEWARNING}", (TEXT_AUTOUPDATEWARNING));
			message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
		}
		message.replace("{WEB_UPDATE_INFO_WARNING}", "");
	} else {
		message.replace("{WEB_UPDATE_INFO_WARNING}", "");
	}
	
	if (!strcmp(THP_MODEL, "Non")) {
	    	message.replace("{TEXT_WEATHER}:", "");
			message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
			message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
			message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
			message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
	    } else {
	    	message.replace("{TEXT_WEATHER}", (TEXT_WEATHER));
			message.replace("{TEXT_TEMPERATURE}", (TEXT_TEMPERATURE));
			message.replace("{TEXT_HUMIDITY}", (TEXT_HUMIDITY));
			message.replace("{TEXT_PRESSURE}", (TEXT_PRESSURE));
			message.replace("{TEXT_DEWPOINT}", (TEXT_DEWPOINT));
	    }
    if (!strcmp(THP_MODEL, "BME280")) {
      if (checkBmeStatus()) {
      	message.replace("{Temperature}", String(BMESensor.temperature));
      	message.replace("{Pressure}", String(BMESensor.seaLevelForAltitude(MYALTITUDE)));
      	message.replace("{Humidity}", String(BMESensor.humidity));
      	message.replace("{Dewpoint}", String(float(pow((BMESensor.humidity) / 100, 0.125) * (112 + 0.9 * (BMESensor.temperature)) + 0.1 * (BMESensor.temperature) - 112)));
	  }
    } else if (!strcmp(THP_MODEL, "HTU21")) {
      if (checkHTU21DStatus()) {
        message.replace("{Temperature}", String(myHTU21D.readTemperature()));
		message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
        message.replace("{Humidity}", String(myHTU21D.readCompensatedHumidity()));
        message.replace("{Dewpoint}", String(float(pow((myHTU21D.readCompensatedHumidity()) / 100, 0.125) * (112 + 0.9 * (myHTU21D.readTemperature())) + 0.1 * (myHTU21D.readTemperature()) - 112)));
      }
    } else if (!strcmp(THP_MODEL, "DHT22")) {
      if (checkDHT22Status()) {
        message.replace("{Temperature}", String(dht.readTemperature()));
  		message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
        message.replace("{Humidity}", String(dht.readHumidity()));
        message.replace("{Dewpoint}", String(float(pow((dht.readHumidity()) / 100, 0.125) * (112 + 0.9 * (dht.readTemperature())) + 0.1 * (dht.readTemperature()) - 112)));
      }
    } else if (!strcmp(THP_MODEL, "BMP280")) {
      if (checkBmpStatus()) {
        message.replace("{Temperature}", String(bmp.readTemperature()));
        message.replace("{Pressure}", String((bmp.readPressure()) / 100));
		message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
		message.replace("{TEXT_DEWPOINT}: {Pressure} °C", "");
      }
    } else if (!strcmp(THP_MODEL, "SHT1x")) {
      if (checkSHT1xStatus()) {
        message.replace("{Temperature}", String(sht1x.readTemperatureC()));
    	message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
        message.replace("{Humidity}", String(float(sht1x.readHumidity())));
        message.replace("{Dewpoint}", String(float(pow((sht1x.readHumidity()) / 100, 0.125) * (112 + 0.9 * (sht1x.readTemperatureC())) + 0.1 * (sht1x.readTemperatureC()) - 112)));
      }
    }

  if (strcmp(DUST_MODEL, "Non")) {
	message.replace("{TEXT_AIRPOLLUTION}", (TEXT_AIRPOLLUTION));
	
    if (DISPLAY_PM1) {
		message.replace("{averagePM1}", String(averagePM1));
    } else {
    	message.replace("PM1: {averagePM1} µg/m³", "");
    }
	
    if (averagePM25 <= 10) {
		message.replace("{colorAveragePM25}", "<font color='#61EEE4'>");
    } else if (averagePM25 > 10 && averagePM25 <= 20) {
		message.replace("{colorAveragePM25}", "<font color='#5BCAAA'>");
    } else if (averagePM25 > 20 && averagePM25 <= 25) {
		message.replace("{colorAveragePM25}", "<font color='#EEE25D'>");
    } else if (averagePM25 > 25 && averagePM25 <= 50) {
		message.replace("{colorAveragePM25}", "<font color='#F95459'>");
    } else if (averagePM25 > 50) {
		message.replace("{colorAveragePM25}", "<font color='#920736'>");
    } else {
		message.replace("{colorAveragePM25}", "<font>");
    }
	
	message.replace("{averagePM25}", String(averagePM25) + "</font>");

    if (averagePM10 <= 20) {
		message.replace("{colorAveragePM10}", "<font color='#61EEE4'>");
    } else if (averagePM10 > 20 && averagePM10 <= 35) {
      	message.replace("{colorAveragePM10}", "<font color='#5BCAAA'>");
    } else if (averagePM10 > 35 && averagePM10 <= 50) {
      	message.replace("{colorAveragePM10}", "<font color='#EEE25D'>");
    } else if (averagePM10 > 50 && averagePM10 <= 100) {
      	message.replace("{colorAveragePM10}", "<font color='#F95459'>");
    } else if (averagePM10 > 100) {
      	message.replace("{colorAveragePM10}", "<font color='#920736'>");
    } else {
      	message.replace("{colorAveragePM10}", "<font>");
    }
	message.replace("{averagePM10}", String(averagePM10) + "</font>");
  } else {
  	message.replace("{TEXT_AIRPOLLUTION}:", "");
	message.replace("PM1: {averagePM1} µg/m³", "");
	message.replace("PM2.5: {colorAveragePM25} {averagePM25} µg/m³", "");
	message.replace("PM10: {colorAveragePM10} {averagePM10} µg/m³", "");
  }

  if (AIRMONITOR_GRAPH_ON) {
	message += FPSTR(WEB_ROOT_PAGE_AIRMONITOR_GRAPH);
    message.replace("{LATITUDE}", String(LATITUDE, 6));
    message.replace("{LONGITUDE}", String(LONGITUDE, 6));
  }
  
  if (THINGSPEAK_GRAPH_ON) {
	  message += FPSTR(WEB_ROOT_PAGE_THINGSPEAK_GRAPH);
	  message.replace("{THINGSPEAK_CHANNEL_ID}", String(THINGSPEAK_CHANNEL_ID));
  }
  
  message += FPSTR(WEB_PAGE_FOOTER);
  WebServer.send(200, "text/html", message);
}

String _addOption(const String &value, const String &label, const String &srslyValue) {
	String option = FPSTR(WEB_CONFIG_PAGE_ADDOPTION);
	option.replace("{value}", value);
  if (value == srslyValue) {
	  option.replace("{srslyValue}", "' selected>");
  } else {
	  option.replace("{srslyValue}", "'>");
  }
  option.replace("{label}", label);
  return option;
}

String _addBoolSelect(const String &key, const bool &value) {
  String selectValue = value ? "yes" : "no";
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("yes", (TEXT_YES), selectValue);
  input += _addOption("no", (TEXT_NO), selectValue);  
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addModelSelect(const String &key, const String &value) {
    String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
    input.replace("{key}", key);
    input += _addOption("red", (TEXT_WITHOUTCALIBRATION), value);
    if (!strcmp(THP_MODEL, "BME280")) {
      input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
    }
    if (!strcmp(THP_MODEL, "HTU21")) {
      input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
    }
    if (!strcmp(THP_MODEL, "DHT22")) {
      input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
    }
    if (!strcmp(THP_MODEL, "SHT1x")) {
      input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
    }
  	input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addTHP_MODELSelect(const String &key, const String &value) {
    String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
    input.replace("{key}", key);
    input += _addOption("BME280", "BME280", value);
    input += _addOption("SHT1x", "SHT1x", value);
    input += _addOption("HTU21", "SHT21/HTU21D", value);
    input += _addOption("DHT22", "DHT22", value);
    input += _addOption("BMP280", "BMP280", value);

    input += _addOption("Non", (TEXT_WITHOUTSENSOR), value);
  	input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addDUST_MODELSelect(const String &key, const String &value) {
    String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
    input.replace("{key}", key);
	input += _addOption("PMS7003", "PMS5003/7003", value);
	
    input += _addOption("Non", (TEXT_WITHOUTSENSOR), value);
  	input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addLanguageSelect(const String &key, const String &value) {
    String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
    input.replace("{key}", key);
    input += _addOption("polish", (TEXT_INTL_PL), value);
    input += _addOption("english", (TEXT_INTL_EN), value);
  	input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _escapeString (const String &value) {
  String trimmed = value;
  trimmed.trim();
  trimmed.replace("'", "&#39;");
  return trimmed;
}

String _addTextInput(const String &key, const String &value, const String &postfix = "") {
	String input = FPSTR(WEB_CONFIG_PAGE_TEXTIMPUT);
	input.replace("{key}", key);
	input.replace("{value}", _escapeString(value));
	input.replace("{postfix}", postfix);
  return input;
}

String _addPasswdInput(const String &key, const String &value, const String &postfix = "") {
	String input = FPSTR(WEB_CONFIG_PAGE_PASSWDINPUT);
	input.replace("{key}", key);
	input.replace("{value}", _escapeString(value));
	input.replace("{postfix}", postfix);
  return input;
}

String _addIntInput(const String &key, const int &value, const String &postfix = "") {
	String input = FPSTR(WEB_CONFIG_PAGE_INTINPUT);
	input.replace("{key}", key);
	input.replace("{value}", String(value));
	input.replace("{postfix}", postfix);
  return input;
}

String _addFloatInput(const String &key, const double &value, const int &precision = 6, const String &postfix = "") {
	String input = FPSTR(WEB_CONFIG_PAGE_FLOATINPUT);
	input.replace("{key}", key);
	input.replace("{value}", String(value, precision));
	input.replace("{postfix}", postfix);
  return input;
}

String _addSubmit() {
	String submit = FPSTR(WEB_CONFIG_PAGE_SUBMIT_BUTTON);
	submit.replace("{TEXT_SAVE}", (TEXT_SAVE));
	return submit;
}

String _addWiFiErase() {
	String WiFiErase = FPSTR(WEB_CONFIG_PAGE_WIFIERASE);
	WiFiErase.replace("{TEXT_ERASEWIFICONFIG}", (TEXT_ERASEWIFICONFIG));
    return WiFiErase;
}

String _addRestoreConfig() {
	String RestoreConfig = FPSTR(WEB_CONFIG_PAGE_RESTORECONFIG);
	RestoreConfig.replace("{TEXT_RESTORESETTINGS}", (TEXT_RESTORESETTINGS));
    return RestoreConfig;
}

void _handle_config(bool is_success) {
	
  if (CONFIG_AUTH == true) {
    if (!WebServer.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
      //return WebServer.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      return WebServer.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }
  }
  String message = FPSTR(WEB_PAGE_HEADER);
    message.replace("{Language}", (TEXT_LANG));
    message.replace("{CurrentPageTitle}", (TEXT_CONFIG_PAGE));
    message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
    message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
    message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

    message += FPSTR(WEB_CONFIG_PAGE_TOP);
	message.replace("{TEXT_CONFIG_PAGE}", (TEXT_CONFIG_PAGE));
	
    if (!is_success) {
		message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");
    } else {
    	message.replace("{TEXT_SAVED}", (TEXT_SAVED));
		message.replace("{TEXT_POSTCONFIG_INFO}", (TEXT_POSTCONFIG_INFO));
    }
	
	message.replace("{TEXT_INSTRUCIONSLINK}", (TEXT_INSTRUCIONSLINK));
	message.replace("{GITHUB_LINK}", (GITHUB_LINK));
	message.replace("{TEXT_HERE}", (TEXT_HERE));
	
	message += FPSTR(WEB_CONFIG_PAGE_CONFIG);
	
	message.replace("{TEXT_DEVICENAME}", (TEXT_DEVICENAME));
    if (DEVICENAME_AUTO) {
		message.replace("{device_name}", (device_name));
    } else {
		message.replace("{device_name}", _addTextInput("DEVICENAME", DEVICENAME));
    }
	message.replace("{TEXT_DEVICENAMEAUTO}", (TEXT_DEVICENAMEAUTO));	
	message.replace("{DEVICENAME_AUTO}", _addBoolSelect("DEVICENAME_AUTO", DEVICENAME_AUTO));
	message.replace("{TEXT_SELECTEDLANGUAGE}", (TEXT_SELECTEDLANGUAGE));
	message.replace("{LanguageSelect}", _addLanguageSelect("LANGUAGE", LANGUAGE));
	message.replace("{TEXT_TEMPHUMIPRESSSENSOR}", (TEXT_TEMPHUMIPRESSSENSOR));
	message.replace("{THP_MODELSelect}", _addTHP_MODELSelect("THP_MODEL", THP_MODEL));
	message.replace("{TEXT_PMSENSOR}", (TEXT_PMSENSOR));
	message.replace("{DUST_MODELSelect}", _addDUST_MODELSelect("DUST_MODEL", DUST_MODEL));
	
	message.replace("{TEXT_FREQUENTMEASUREMENTONOFF}", (TEXT_FREQUENTMEASUREMENTONOFF));
	message.replace("{TEXT_FREQUENTMEASUREMENTINFO}", (TEXT_FREQUENTMEASUREMENTINFO));
	message.replace("{TEXT_MEASUREMENTFREQUENCY}", (TEXT_MEASUREMENTFREQUENCY));
	message.replace("{TEXT_SENDINGINTERVAL}", (TEXT_SENDINGINTERVAL));
	message.replace("{FREQUENTMEASUREMENT_Select}", _addBoolSelect("FREQUENTMEASUREMENT", FREQUENTMEASUREMENT));
	
    if (FREQUENTMEASUREMENT == true) {
		message.replace("{FREQUENTMEASUREMENT_time}", _addIntInput("DUST_TIME", DUST_TIME, "seconds"));
    } else {
		message.replace("{FREQUENTMEASUREMENT_time}", _addIntInput("DUST_TIME", DUST_TIME, "minutes"));
    }
	
	message.replace("{TEXT_AVERAGELASTRESULT}", (TEXT_AVERAGELASTRESULT));
	message.replace("{NUMBEROFMEASUREMENTS}", _addIntInput("NUMBEROFMEASUREMENTS", NUMBEROFMEASUREMENTS, "{TEXT_PMMEASUREMENTS}"));
	message.replace("{TEXT_PMMEASUREMENTS}", (TEXT_PMMEASUREMENTS));
	
    if (FREQUENTMEASUREMENT == true) {
		message.replace("{SENDING_FREQUENCY}", _addIntInput("SENDING_FREQUENCY", SENDING_FREQUENCY, "{TEXT_SECONDS}"));
		message.replace("{TEXT_SECONDS}", (TEXT_SECONDS));
		
		message.replace("<hr>DeepSleep</b> - {TEXT_DEEPSLEEPINFO}:", ""); 
		message.replace("{DEEPSLEEP_ON}<hr>", "");
    } else {
		message.replace("{SENDING_FREQUENCY}", _addIntInput("SENDING_FREQUENCY", SENDING_FREQUENCY, "{TEXT_MINUTES}"));
		message.replace("{TEXT_MINUTES}", (TEXT_MINUTES));
		message.replace("{TEXT_DEEPSLEEPINFO}", TEXT_DEEPSLEEPINFO);
		message.replace("{INTERFACEWWWONTIME}", String(int(NUMBEROFMEASUREMENTS)*2+6)); 
		message.replace("{SENDING_FREQUENCY}", String(SENDING_FREQUENCY)); 
		message.replace("{DEEPSLEEP_ON}", _addBoolSelect("DEEPSLEEP_ON", DEEPSLEEP_ON));
    }
	
    if (!strcmp(DUST_MODEL, "PMS7003")) {
		message.replace("{DISPLAY_PM1}", _addBoolSelect("DISPLAY_PM1", DISPLAY_PM1));
		message.replace("{TEXT_DISPLAYPM1}", (TEXT_DISPLAYPM1));
    } else {
    	message.replace("<b>{TEXT_DISPLAYPM1}: </b> {DISPLAY_PM1}", "");
    }
	message.replace("{TEXT_ALTITUDEINFO}", (TEXT_ALTITUDEINFO));
	message.replace("{WSPOLRZEDNE_GPS_LINK}", (WSPOLRZEDNE_GPS_LINK));
	message.replace("{TEXT_HERE}", (TEXT_HERE));
	message.replace("{MYALTITUDE}", _addIntInput("MYALTITUDE", MYALTITUDE, "m.n.p.m"));
	
	message.replace("{TEXT_SECURECONFIGUPDATEPAGE}", (TEXT_SECURECONFIGUPDATEPAGE));
	message.replace("{CONFIG_AUTH}", _addBoolSelect("CONFIG_AUTH", CONFIG_AUTH));
	message.replace("{TEXT_SECURELOGIN}", (TEXT_SECURELOGIN));
	message.replace("{CONFIG_USERNAME}", _addTextInput("CONFIG_USERNAME", CONFIG_USERNAME));
	message.replace("{TEXT_SECUREPASSWD}", (TEXT_SECUREPASSWD));
	message.replace("{CONFIG_PASSWORD}", _addPasswdInput("CONFIG_PASSWORD", CONFIG_PASSWORD));

    if (!CONFIG_AUTH) {
		message.replace("{TEXT_SECURELOGOUTINFO}", "");
    } else {
    	message.replace("{TEXT_SECURELOGOUTINFO}", (TEXT_SECURELOGOUTINFO));
    }
	
	message.replace("{TEXT_SMOGLISTSENDING}", (TEXT_SMOGLISTSENDING));
	message.replace("{SMOGLIST_ON}", _addBoolSelect("SMOGLIST_ON", SMOGLIST_ON));	
	
	message.replace("{TEXT_LUFTDATENSENDING}", (TEXT_LUFTDATENSENDING));
	message.replace("{LUFTDATEN_LINK}", (LUFTDATEN_LINK));
	message.replace("{LUFTDATENFORM_LINK}", (LUFTDATENFORM_LINK));
	message.replace("{TEXT_THEFORM}", (TEXT_THEFORM));
		
	message.replace("{LUFTDATEN_ON}", _addBoolSelect("LUFTDATEN_ON", LUFTDATEN_ON));
	message.replace("{ChipID}", "smogomierz-" + String(ESP.getChipId()));
	
	message.replace("{TEXT_AIRMONITORSENDING}", (TEXT_AIRMONITORSENDING));
	message.replace("{AIRMONITOR_LINK}", (AIRMONITOR_LINK));
	message.replace("{AIRMONITORFORM_LINK}", (AIRMONITORFORM_LINK));
	message.replace("{TEXT_THEFORM}", (TEXT_THEFORM));
	message.replace("{TEXT_AIRMONITORCHARTS}", (TEXT_AIRMONITORCHARTS));
	message.replace("{AIRMONITOR_ON}", _addBoolSelect("AIRMONITOR_ON", AIRMONITOR_ON));
	message.replace("{TEXT_AIRMONITORCOORDINATESINFO}", (TEXT_AIRMONITORCOORDINATESINFO));
	message.replace("{LATLONG_LINK}", (LATLONG_LINK));
	message.replace("{TEXT_HERE}", (TEXT_HERE));
	message.replace("{AIRMONITOR_GRAPH_ON}", _addBoolSelect("AIRMONITOR_GRAPH_ON", AIRMONITOR_GRAPH_ON));
	message.replace("{TEXT_AIRMONITORLATITUDE}", (TEXT_AIRMONITORLATITUDE));
	message.replace("{LATITUDE}", _addFloatInput("LATITUDE", LATITUDE, 6, "°"));
	message.replace("{TEXT_AIRMONITORLONGITUDE}", (TEXT_AIRMONITORLONGITUDE));
	message.replace("{LONGITUDE}", _addFloatInput("LONGITUDE", LONGITUDE, 6, "°"));
	
	message.replace("{TEXT_THINGSPEAKSENDING}", (TEXT_THINGSPEAKSENDING));
	message.replace("{THINGSPEAK_LINK}", (THINGSPEAK_LINK));
	message.replace("{THINGSPEAK_ON}", _addBoolSelect("THINGSPEAK_ON", THINGSPEAK_ON));
	message.replace("{TEXT_THINGSPEAKCHARTS}", (TEXT_THINGSPEAKCHARTS));
	message.replace("{THINGSPEAK_GRAPH_ON}", _addBoolSelect("THINGSPEAK_GRAPH_ON", THINGSPEAK_GRAPH_ON));
	message.replace("{TEXT_THINGSPEAKAPIKEY}", (TEXT_THINGSPEAKAPIKEY));
	message.replace("{THINGSPEAK_API_KEY}", _addTextInput("THINGSPEAK_API_KEY", THINGSPEAK_API_KEY));
	message.replace("{TEXT_THINGSPEAKCHANNELID}", (TEXT_THINGSPEAKCHANNELID));
	message.replace("{THINGSPEAK_CHANNEL_ID}", _addIntInput("THINGSPEAK_CHANNEL_ID", THINGSPEAK_CHANNEL_ID));
	
	message.replace("{TEXT_INFLUXDBSENDING}", (TEXT_INFLUXDBSENDING));
	message.replace("{INFLUXDB_ON}", _addBoolSelect("INFLUXDB_ON", INFLUXDB_ON));
	message.replace("{TEXT_INFLUXDBSERVER}", (TEXT_INFLUXDBSERVER));
	message.replace("{INFLUXDB_HOST}", _addTextInput("INFLUXDB_HOST", INFLUXDB_HOST));
	message.replace("{TEXT_INFLUXDBPORT}", (TEXT_INFLUXDBPORT));
	message.replace("{INFLUXDB_PORT}", _addIntInput("INFLUXDB_PORT", INFLUXDB_PORT));
	message.replace("{TEXT_INFLUXDBNAME}", (TEXT_INFLUXDBNAME));
	message.replace("{INFLUXDB_DATABASE}", _addTextInput("INFLUXDB_DATABASE", INFLUXDB_DATABASE));
	message.replace("{TEXT_INFLUXDBUSER}", (TEXT_INFLUXDBUSER));
	message.replace("{DB_USER}", _addTextInput("DB_USER", DB_USER));
	message.replace("{TEXT_INFLUXDBPASSWD}", (TEXT_INFLUXDBPASSWD));
	message.replace("{DB_PASSWORD}", _addPasswdInput("DB_PASSWORD", DB_PASSWORD));
	
	message.replace("{TEXT_MQTTSENDING}", (TEXT_MQTTSENDING));
	message.replace("{MQTT_ON}", _addBoolSelect("MQTT_ON", MQTT_ON));
	message.replace("{TEXT_MQTTSERVER}", (TEXT_MQTTSERVER));
	message.replace("{MQTT_HOST}", _addTextInput("MQTT_HOST", MQTT_HOST));
	message.replace("{TEXT_MQTTPORT}", (TEXT_MQTTPORT));
	message.replace("{MQTT_PORT}", _addIntInput("MQTT_PORT", MQTT_PORT));
	message.replace("{TEXT_MQTTUSER}", (TEXT_MQTTUSER));
	message.replace("{MQTT_USER}", _addTextInput("MQTT_USER", MQTT_USER));
	message.replace("{TEXT_MQTTPASSWD}", (TEXT_MQTTPASSWD));
	message.replace("{MQTT_PASSWORD}", _addPasswdInput("MQTT_PASSWORD", MQTT_PASSWORD));
	
	message.replace("{DEBUG}", _addBoolSelect("DEBUG", DEBUG));
	message.replace("{TEXT_CALIBMETHOD}", (TEXT_CALIBMETHOD));
	message.replace("{CalibrationModelSelect}", _addModelSelect("MODEL", MODEL));
	message.replace("{TEXT_CALIB1}", (TEXT_CALIB1));
	message.replace("{calib1}", String(calib1));
	message.replace("{TEXT_CALIB2}", (TEXT_CALIB2));
	message.replace("{calib2}", String(calib2));
	
	message.replace("{TEXT_SOFTWATEVERSION}", (TEXT_SOFTWATEVERSION));
	message.replace("{SOFTWAREVERSION}", SOFTWAREVERSION);
	
	message.replace("{TEXT_AUTOUPDATEON}", TEXT_AUTOUPDATEON);
	message.replace("{AUTOUPDATEON}", _addBoolSelect("AUTOUPDATE_ON", AUTOUPDATE_ON));
	message.replace("{TEXT_UPDATEPAGEAUTOUPDATEWARNING}", TEXT_UPDATEPAGEAUTOUPDATEWARNING);
	
	message.replace("{WiFiEraseButton}", _addWiFiErase());
	message.replace("{RestoreConfigButton}", _addRestoreConfig());
	message.replace("{SubmitButton}", _addSubmit());
  	message += FPSTR(WEB_PAGE_FOOTER);
	
  WebServer.send(200, "text/html", message);
}

bool _parseAsBool(String value) {
  return value == "yes";
}

void _set_calib1_and_calib2() {
  if (!strcmp(MODEL, "black")) {
    calib1 = 1.6;
    calib2 = 0.55;
  } else if (!strcmp(MODEL, "red")) {
    calib1 = 1.0;
    calib2 = 1.0;
  } else if (!strcmp(MODEL, "white")) {
    if (!strcmp(THP_MODEL, "BME280")) {
      if (checkBmeStatus() == true) {
        calib1 = float((200 - (BMESensor.humidity)) / 150);
      }
    }
    if (!strcmp(THP_MODEL, "HTU21D")) {
      if (checkHTU21DStatus() == true) {
        calib1 = float((200 - (myHTU21D.readCompensatedHumidity())) / 150);
      }
    }
    if (!strcmp(THP_MODEL, "DHT22")) {
      if (checkDHT22Status() == true) {
        calib1 = float((200 - (dht.readHumidity())) / 150);
      }
    }
    if (!strcmp(THP_MODEL, "SHT1x")) {
      if (checkSHT1xStatus() == true) {
        calib1 = float((200 - (sht1x.readHumidity())) / 150);
      }
    }
    calib1 = calib1;
    calib2 = calib1 / 2;
  } else {
    calib1 = 1.0;
    calib2 = 1.0;
  }
}

void _set_language() {
  if (!strcmp(LANGUAGE, "english")) {
    SELECTED_LANGUAGE = 1;
  } else if (!strcmp(LANGUAGE, "polish")) {
    SELECTED_LANGUAGE = 2;
  } else {
    SELECTED_LANGUAGE = 1;
  }
}

void _parseAsCString(char* dest, String value) {
  strncpy(dest, value.c_str(), 255);
}

void handle_config() {
  _handle_config(false);
}

void handle_config_post() {

  if (DEBUG) {
    Serial.println("POST CONFIG START!!");
    int argsLen = WebServer.args();
    for (int i = 0; i < argsLen; i++) {
      String argName = WebServer.argName(i);
      String arg = WebServer.arg(i);
      String ss = "** ";
      ss += argName;
      ss += " = ";
      ss += arg;
      Serial.println(ss);
    }
  }

  // REMEMBER TO ADD/EDIT KEYS IN config.h AND spiffs.cpp!!

  DEVICENAME_AUTO = _parseAsBool(WebServer.arg("DEVICENAME_AUTO"));
  if (!DEVICENAME_AUTO) {
    _parseAsCString(DEVICENAME, WebServer.arg("DEVICENAME"));
  }
  DISPLAY_PM1 = _parseAsBool(WebServer.arg("DISPLAY_PM1"));
  _parseAsCString(LANGUAGE, WebServer.arg("LANGUAGE"));
  _set_language();
  _parseAsCString(THP_MODEL, WebServer.arg("THP_MODEL"));
  _parseAsCString(DUST_MODEL, WebServer.arg("DUST_MODEL"));
  FREQUENTMEASUREMENT = _parseAsBool(WebServer.arg("FREQUENTMEASUREMENT"));

  DUST_TIME = WebServer.arg("DUST_TIME").toInt();
  NUMBEROFMEASUREMENTS = WebServer.arg("NUMBEROFMEASUREMENTS").toInt();

  LUFTDATEN_ON = _parseAsBool(WebServer.arg("LUFTDATEN_ON"));
  SMOGLIST_ON = _parseAsBool(WebServer.arg("SMOGLIST_ON"));

  AIRMONITOR_ON = _parseAsBool(WebServer.arg("AIRMONITOR_ON"));
  AIRMONITOR_GRAPH_ON = _parseAsBool(WebServer.arg("AIRMONITOR_GRAPH_ON"));
  LATITUDE = WebServer.arg("LATITUDE").toFloat();
  LONGITUDE = WebServer.arg("LONGITUDE").toFloat();
  MYALTITUDE = WebServer.arg("MYALTITUDE").toInt();

  THINGSPEAK_ON = _parseAsBool(WebServer.arg("THINGSPEAK_ON"));
  THINGSPEAK_GRAPH_ON = _parseAsBool(WebServer.arg("THINGSPEAK_GRAPH_ON"));
  _parseAsCString(THINGSPEAK_API_KEY, WebServer.arg("THINGSPEAK_API_KEY"));
  THINGSPEAK_CHANNEL_ID = WebServer.arg("THINGSPEAK_CHANNEL_ID").toInt();

  INFLUXDB_ON = _parseAsBool(WebServer.arg("INFLUXDB_ON"));
  _parseAsCString(INFLUXDB_HOST, WebServer.arg("INFLUXDB_HOST"));
  INFLUXDB_PORT = WebServer.arg("INFLUXDB_PORT").toInt();
  _parseAsCString(INFLUXDB_DATABASE, WebServer.arg("INFLUXDB_DATABASE"));
  _parseAsCString(DB_USER, WebServer.arg("DB_USER"));
  _parseAsCString(DB_PASSWORD, WebServer.arg("DB_PASSWORD"));

  MQTT_ON = _parseAsBool(WebServer.arg("MQTT_ON"));
  _parseAsCString(MQTT_HOST, WebServer.arg("MQTT_HOST"));
  MQTT_PORT = WebServer.arg("MQTT_PORT").toInt();
  _parseAsCString(MQTT_USER, WebServer.arg("MQTT_USER"));
  _parseAsCString(MQTT_PASSWORD, WebServer.arg("MQTT_PASSWORD"));

  SENDING_FREQUENCY = WebServer.arg("SENDING_FREQUENCY").toInt();
  DEEPSLEEP_ON = _parseAsBool(WebServer.arg("DEEPSLEEP_ON"));

  DEBUG = _parseAsBool(WebServer.arg("DEBUG"));
  AUTOUPDATE_ON = _parseAsBool(WebServer.arg("AUTOUPDATE_ON"));

  CONFIG_AUTH = _parseAsBool(WebServer.arg("CONFIG_AUTH"));
  _parseAsCString(CONFIG_USERNAME, WebServer.arg("CONFIG_USERNAME"));
  _parseAsCString(CONFIG_PASSWORD, WebServer.arg("CONFIG_PASSWORD"));

  _parseAsCString(MODEL, WebServer.arg("MODEL"));
  _set_calib1_and_calib2();

  if (DEBUG) {
    Serial.println("POST CONFIG END!!");
  }

  saveConfig();
  _handle_config(true);
  // https://github.com/esp8266/Arduino/issues/1722
  //ESP.reset();
  delay(300);
  ESP.restart();
}

void handle_update() {            //Handler for the handle_update
  if (CONFIG_AUTH == true) {
    if (!WebServer.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
      //return WebServer.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      return WebServer.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }
  }
  String message = FPSTR(WEB_PAGE_HEADER);
    message.replace("{Language}", (TEXT_LANG));
    message.replace("{CurrentPageTitle}", (TEXT_UPDATE_PAGE));
    message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
    message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
    message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));
	
	message += FPSTR(WEB_UPDATE_PAGE_UPDATE);
	
	if (!AUTOUPDATE_ON) {
		if (need_update) {
			message.replace("{WEB_UPDATE_INFO_WARNING}", FPSTR(WEB_UPDATE_INFO_WARNING));
			message.replace("{TEXT_FWUPDATEAVALIBLE}", (TEXT_FWUPDATEAVALIBLE));
			message.replace("{MANUALUPDATEBUTTON}", "");
			message.replace("{FWUPDATEBUTTON}", FPSTR(WEB_UPDATE_BUTTON_FWUPDATE));
			message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
			message.replace("{AUTOUPDATEONBUTTON}", FPSTR(WEB_UPDATE_BUTTON_AUTOUPDATEON));
			message.replace("{TEXT_AUTOUPDATEONBUTTON}", (TEXT_AUTOUPDATEONBUTTON));
			message.replace("{TEXT_AUTOUPDATEWARNING}", (TEXT_AUTOUPDATEWARNING));
			message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
		}
		message.replace("{WEB_UPDATE_INFO_WARNING}", "");
	} else {
		message.replace("{WEB_UPDATE_INFO_WARNING}", "");
	}
	
	message.replace("{TEXT_UPDATE_PAGE}", (TEXT_UPDATE_PAGE));
	message.replace("{TEXT_SELECTUPDATEFILE}", (TEXT_SELECTUPDATEFILE));
	message.replace("{TEXT_SUBMITUPDATE}", (TEXT_SUBMITUPDATE));
	
	message.replace("{TEXT_AUTOUPDATEON}", (TEXT_AUTOUPDATEON));
	if (AUTOUPDATE_ON) {
		message.replace("{AUTOUPDATEONSTATUS}", (TEXT_YES));
	} else {
		message.replace("{AUTOUPDATEONSTATUS}", (TEXT_NO));
	}
		
	message.replace("{TEXT_CURRENTSOFTVERSION}", (TEXT_CURRENTSOFTVERSION));
	message.replace("{SOFTWAREVERSION}", String(CURRENTSOFTWAREVERSION) + " " + String(PMSENSORVERSION));
	
	message.replace("{TEXT_SERVERSOFTWAREVERSION}", (TEXT_SERVERSOFTWAREVERSION));
	message.replace("{SERVERSOFTWAREVERSION}", String(SERVERSOFTWAREVERSION) + " " + String(PMSENSORVERSION));
	
	message.replace("{TEXT_LATESTAVAILABLESOFT}", TEXT_LATESTAVAILABLESOFT);
	message.replace("{SMOGOMIERZRELEASES_LINK}", (SMOGOMIERZRELEASES_LINK));
	message.replace("{TEXT_HERE}", (TEXT_HERE));
	
  	message += FPSTR(WEB_PAGE_FOOTER);
  WebServer.send(200, "text/html", message);
}

void erase_wifi() {
  Serial.println("Erasing Config...");
  ESP.eraseConfig();
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

void restore_config() {
  Serial.println("Restoring default settings...");
  deleteConfig();
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

void fwupdate() {
  doUpdate();
  delay(1000);
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  delay(1000);
}

void autoupdateon() {
  AUTOUPDATE_ON = true;
  saveConfig();
  delay(300);
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

void handle_api() {
  String message;
  StaticJsonDocument<800> jsonBuffer;
  JsonObject json = jsonBuffer.to<JsonObject>();

  json["device_name"] = device_name;
  if (strcmp(DUST_MODEL, "Non")) {
    json["pm1"] = averagePM1;
    json["pm25"] = averagePM25;
    json["pm10"] = averagePM10;
  }

  if (!strcmp(THP_MODEL, "BME280")) {
    if (checkBmeStatus()) {
      json["temperature"] = float(BMESensor.temperature);
      json["pressure"] = int(BMESensor.seaLevelForAltitude(MYALTITUDE));
      json["humidity"] = int(BMESensor.humidity);
      json["dewpoint"] = float(pow((BMESensor.humidity) / 100, 0.125) * (112 + 0.9 * (BMESensor.temperature)) + 0.1 * (BMESensor.temperature) - 112);
    }
  }
  if (!strcmp(THP_MODEL, "BMP280")) {
    if (checkBmpStatus()) {
      json["temperature"] = float(bmp.readTemperature());
      json["pressure"] = int((bmp.readPressure()) / 100);
    }
  }
  if (!strcmp(THP_MODEL, "HTU21")) {
    if (checkHTU21DStatus()) {
      json["temperature"] = float(myHTU21D.readTemperature());
      json["humidity"] = int(myHTU21D.readCompensatedHumidity());
      json["dewpoint"] = float(pow((myHTU21D.readCompensatedHumidity()) / 100, 0.125) * (112 + 0.9 * (myHTU21D.readTemperature())) + 0.1 * (myHTU21D.readTemperature()) - 112);
    }
  }
  if (!strcmp(THP_MODEL, "DHT22")) {
    if (checkDHT22Status()) {
      json["temperature"] = float(dht.readTemperature());
      json["humidity"] = int(dht.readHumidity());
      json["dewpoint"] = float(pow((dht.readHumidity()) / 100, 0.125) * (112 + 0.9 * (dht.readTemperature())) + 0.1 * (dht.readTemperature()) - 112);
    }
  }
  if (!strcmp(THP_MODEL, "SHT1x")) {
    if (checkSHT1xStatus()) {
      json["temperature"] = float(sht1x.readTemperatureC());
      json["humidity"] = int(sht1x.readHumidity());
      json["dewpoint"] = float(pow((sht1x.readHumidity()) / 100, 0.125) * (112 + 0.9 * (sht1x.readTemperatureC())) + 0.1 * (sht1x.readTemperatureC()) - 112);
    }
  }

  serializeJsonPretty(json, message);
  WebServer.send(200, "text/json", message);
}