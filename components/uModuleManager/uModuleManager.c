#include "uModuleManager.h"

#include "freertos/idf_additions.h"
#include <stdio.h>
#include <string.h>


#include "uEvents.h"
#include "uWifiManager.h"
#include "uHTTPServer.h"
#include "uTaskManager.h"
#include "uDriverNVS.h"

/*
typedef struct{
	
	const char name[32];
	const char owner[16];
	TaskFunction_t entry;
	TaskHandle_t *handle;
	void *parameters;
	uint32_t stackSize;
	UBaseType_t priority;
	UBaseType_t coreID;
	S
	genericQueueStruct *uTOSQueue;
	
} uTaskConfig_t;
*/

#define uWIFI_STARTUP	true
#define uHTTP_STARTUP	true
#define uNVS_STARTUP	true

static genericDriverNVS uBLOP = {
	
	.event = uNVS_SUCCESS
	
};

static genericDriverNVS uBLOPREAD = {
	
	.event = uNVS_START
	
};



TaskHandle_t handlers[25] = {
	NULL
};

uModuleItem uModulesArray[] = {
	
	{ {
		 "uWifi", 
		 uWIFI, 
		 uWifi_init, 
		 &handlers[uWIFI_VECT], 
		 NULL, 
		 4096, 
		 24, 
		 0, 
		 &uT2uWM 
	}, 
	 uWIFI_STARTUP
 
 	}, 
	{ { 
		"uHTTPServer",
		uHTTPSERVER,
		start_webserver,
		&handlers[uHTTP_VECT],
		NULL,
		2048,
		24,
		tskNO_AFFINITY,
		NULL
	},
	uHTTP_STARTUP
 	}, 
	{ { 
		"uDriverNVS",
		uDriverNVS,
		task_uDriverNVS,
		&handlers[uNVS_VECT],
		&uBLOPREAD,
		2048,
		24,
		tskNO_AFFINITY,
		NULL
	},
	uNVS_STARTUP
 	}
	
};

uEventsType uBLOP_STATE(){
	
	return uBLOP.event;
	
}

uEventsType uBLOPREAD_STATE(){
	
	return uBLOPREAD.event;
	
}

uEventsType uBLOP_CHANGE_STATE(){
	
	uBLOP.event = uNVS_SAVE;
	
	return uBLOP.event;
}

uEventsType uBLOPREAD_CHANGE_STATE(){
		
	uBLOPREAD.event = uNVS_READ;
	
	return uBLOP.event;
	
}

void saveBlop(genericDriverNVS blop){
	
	uBLOP = blop;	
	
	uTaskConfig_t nvs = {

		"uModuleManager",
		uModuleManger,
		task_uDriverNVS,
		NULL,
		&uBLOP,
		2048,
		23,
		tskNO_AFFINITY,
		NULL,
		
	};
	
	uTask_create(nvs);
	
}

void readBlop(genericDriverNVS blop){
	
	uBLOPREAD = blop;
	
	uTaskConfig_t nvs = {

		"uModuleManager",
		uModuleManger,
		task_uDriverNVS,
		NULL,
		&uBLOPREAD,
		2048,
		23,
		tskNO_AFFINITY,
		NULL,
			
	};
		
	uTask_create(nvs);
	
	
}

saveCell getBLOP(){
	
	return uBLOPREAD.saveCell;
	
}

void uModuleManagerStartUp(){
	
	for(int i = 0; i < MODULES; i++){
		
		if(uModulesArray[i].initOnStart == true){
			
			
			uTask_create(uModulesArray[i].module);
			
			
		}
		
	}
	
}

