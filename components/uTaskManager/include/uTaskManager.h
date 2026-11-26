#pragma once

#include <stdint.h>

//#include "freertos/projdefs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "sdkconfig.h"

#include "uEvents.h"

typedef enum{
	
	uTASK_OK,
	uTASK_ERROR
	
} uTaskManagerError;

typedef struct{
	
	const char name[32];
	uModulesDefined owner;
	TaskFunction_t entry;
	TaskHandle_t *handle;
	void *parameters;
	uint32_t stackSize;
	UBaseType_t priority;
	UBaseType_t coreID;
	
	QueueHandle_t *uTOSQueue;
	
} uTaskConfig_t;

typedef struct{
	
	char name[32];
	uModulesDefined owner;
	uint32_t stackSize;
	UBaseType_t priority;
	UBaseType_t coreID;
	bool registered;
	bool monitor_stack;
	
} uTaskInfo_t;

typedef struct{
	
	uint32_t minFreeStack;
	uint32_t stackUsagePercent;
	bool stackWarning;
	bool stackCritical;
	
} uTaskStats_t;

typedef struct node{
		
	uTaskInfo_t info;
	struct node *next;
	
} uTaskList_t;


uTaskManagerError uTask_init();
uTaskManagerError uTask_create(uTaskConfig_t taskConfig);
uTaskManagerError uTask_registry(uTaskInfo_t *node);
