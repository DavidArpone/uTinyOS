#pragma once

#include "uTaskManager.h"
#include "uWifiManager.h"
#include "uEvents.h"
#include <stdint.h>
#include <stdbool.h>

#define uWIFI_VECT 		0
#define uHTTP_VECT		1
#define uNVS_VECT 		2

#define MODULES			3

typedef struct {
	
	uTaskConfig_t module;
	bool initOnStart;
	
} uModuleItem;

extern uModuleItem uModulesArray[];

void uModuleManagerStartUp();

uEventsType uBLOP_STATE();
uEventsType uBLOPREAD_STATE();
void readBlop(genericDriverNVS blop);
void saveBlop(genericDriverNVS blop);
uEventsType uBLOP_CHANGE_STATE();
uEventsType uBLOPREAD_CHANGE_STATE();
saveCell getBLOP();