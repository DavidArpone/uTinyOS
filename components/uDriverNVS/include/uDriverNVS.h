#pragma once
#include "freertos/idf_additions.h"

typedef enum{
	
	uNVS_WIFI_LABEL_NOT_READ,
	uNVS_WIFI_LABEL_NOT_WRITE,
	uNVS_SSID_FIELD_ERR,
	uNVS_PSW_FIELD_ERR,
	uNVS_ERROR,
	uNVS_SUCC
	
} nvsResponse;
 

void task_uDriverNVS(void * pvParameters);