#pragma once

#include "esp_event_base.h"
#include "freertos/idf_additions.h"


typedef enum{

	uWifi_START,
	uWifi_SUCCESCREDENTIALS,
	uWifi_CONNECT,
	uWifi_AP,
	
	uHTTP_POSTCRED,
	
	uNVS_START,
	uNVS_READ,
	uNVS_SAVE,
	uNVS_ERR,
	uNVS_ERR_LABEL,
	uNVS_ERR_BLOB,
	uNVS_SUCCESS_INIT,
	uNVS_SUCCESS,

	uEVENTS_DEFAULT
	
} uEventsType;

typedef enum{

	uWIFI,
	uHTTPSERVER,
	uTINYOS,
	uModuleManger,
	uTaskManager,
	uDriverNVS,
	
	uModules_DEFAULT
	
} uModulesDefined;



typedef struct{
	char ssid[32];
	char psw[64];
} credentials;

typedef struct{
	
	uEventsType event;
	uModulesDefined sender;
	uModulesDefined reciver;
	
	union{
			
		bool trash;
		credentials wifiCred;
		
	} config;
	
} genericQueueStruct;

typedef struct{
	
	uEventsType event;
	char key[25];
	
	
	union {	
		char value[25];
		credentials credentials;	
	} body;

} saveCell;

typedef struct{
	
	uEventsType event;
	char label[12];
	saveCell saveCell;
	
} genericDriverNVS;

extern QueueHandle_t uM2uT; //uModule to uTinyOS -> All meesages to uTinyOS is by this queue
extern QueueHandle_t uWS2uT;

void func(void);
