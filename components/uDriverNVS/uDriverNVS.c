#include "uDriverNVS.h"

#include "uEvents.h"

#include "freertos/idf_additions.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"

static void nvs_read(char * namespace, saveCell *data);
static void nvs_save(char * namespace, saveCell *data);
static uEventsType uDriverNvs(uEventsType event);


void task_uDriverNVS(void * pvParameters){
	
	genericDriverNVS *argument = (genericDriverNVS *) pvParameters;
	
	switch (argument->event) {
		
		case uNVS_START:
	
			argument->event = uDriverNvs(argument->saveCell.event);
			break;
	
		case uNVS_READ:
			
			nvs_read(argument->label, &argument->saveCell);
		    argument->event = argument->saveCell.event;
			break;
		
		case uNVS_SAVE:
		
			nvs_save(argument->label, &argument->saveCell);
		    argument->event = argument->saveCell.event;
			break;
		
		default:
			break;
	}
	
	vTaskDelete(NULL);
	
}


uEventsType uDriverNvs(uEventsType e){
	
	esp_err_t ret = nvs_flash_init();
		
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		
		nvs_flash_erase();
		nvs_flash_init();
		
	}
	
	e = uNVS_SUCCESS;
	
	return e;
}

static void nvs_read(char *namespace, saveCell *data){

    nvs_handle_t handle;

    esp_err_t ret = nvs_open(namespace, NVS_READONLY, &handle);

    if(ret != ESP_OK){
        data->event = uNVS_ERR_LABEL;
        return;
    }

    size_t blob_len = sizeof(data->body);

    ret = nvs_get_blob(handle, data->key, &data->body, &blob_len);

    nvs_close(handle);

    if(ret != ESP_OK){
        data->event = uNVS_ERR_BLOB;
        return;
    }

    data->event = uNVS_SUCCESS;
}

static void nvs_save(char *namespace, saveCell *data){

    nvs_handle_t handle;

    esp_err_t ret = nvs_open(namespace, NVS_READWRITE, &handle);

    if(ret != ESP_OK){
        data->event = uNVS_ERR_LABEL;
        return;
    }

    ret = nvs_set_blob(handle, data->key, &data->body, sizeof(data->body));

    if(ret == ESP_OK){
        ret = nvs_commit(handle);
    }

    nvs_close(handle);

    if(ret != ESP_OK){
        data->event = uNVS_ERR;
        return;
    }

    data->event = uNVS_SUCCESS;
}
