#pragma once

#include "esp_spiffs.h"

typedef enum{
	
	uSPIFFS_MOUNT,
	uSPIFFS_ERROR
	
} fsServerStatus;

fsServerStatus uSpiffs_mount();

