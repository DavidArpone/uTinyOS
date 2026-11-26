#include "uSpiffs.h"
#include "esp_spiffs.h"
#include <stddef.h>

size_t spiffsTotal, spiffsUsed;

fsServerStatus uSpiffs_mount(){
	
	esp_vfs_spiffs_conf_t conf = {
		
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 5,
		.format_if_mount_failed = true
		
	};
	
	esp_err_t fsret = esp_vfs_spiffs_register(&conf);
	
	if(fsret != ESP_OK){
		
		return uSPIFFS_ERROR;
		
	}
	
	esp_spiffs_info(NULL, &spiffsTotal, &spiffsUsed);
	
	return uSPIFFS_MOUNT;	
	
}