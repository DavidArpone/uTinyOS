#include "uTinyOS.h"

#include "freertos/idf_additions.h"
#include "portmacro.h"

#include "uEvents.h"
#include "uTaskManager.h"
#include "uModuleManager.h"
#include "uSpiffs.h"
#include "esp_log.h"

static void uTinyOS(void * pvParameters);
static void handler_uWIFI(void * pvParameters);
static void handler_uHTTP(void * pvParameters);



void start_uTinyOS(){	
	
	ESP_LOGI("START", "1 - Antes de uTask_init");
	uTask_init();

	ESP_LOGI("START", "2 - Antes de SPIFFS");
	uSpiffs_mount();
	
	FILE *f = fopen("/spiffs/setWifi.html", "r");

	if(f == NULL){
	    ESP_LOGI("SPIFFS", "No se pudo abrir setWifi.html");
	} else {
	    ESP_LOGI("SPIFFS", "setWifi.html encontrado");
	    fclose(f);
	}

	ESP_LOGI("START", "3 - Antes de crear cola");
	uM2uT = xQueueCreate(50, sizeof(genericQueueStruct));

	uTaskConfig_t uTinyTask = { "uTinyOS", 
		 uTINYOS, 
	 	uTinyOS, 
		 NULL, 
		 NULL, 
		 4069, 
		 24, 
		 1, 
		 NULL
	};

	
	ESP_LOGI("START", "4 - Antes de crear uTinyOS");
	uTask_create(uTinyTask);

	ESP_LOGI("START", "5 - Antes de ModuleManagerStartUp");
	uModuleManagerStartUp();

	ESP_LOGI("START", "6 - ModuleManagerStartUp finalizado");
	
}


void uTinyOS(void *pvParameters){
	
	genericQueueStruct argument;
	
	genericQueueStruct *argumentTask = malloc(sizeof(genericQueueStruct));
	
	while(true){
		
		xQueueReceive(uM2uT, &argument, portMAX_DELAY);
		
		*argumentTask = argument;
		
		switch (argument.sender) {
		
			case uHTTPSERVER:
				
				uTaskConfig_t httpServer = {

					"handler_uHTTP",
					uTINYOS,
					handler_uHTTP,
					NULL,
					argumentTask,
					2048,
					23,
					1,
					NULL,
					
				};
				
				uTask_create(httpServer);
			
				break;
			
			case uWIFI:
				
				uTaskConfig_t wifi = {

					"handler_uWIFI",
					uTINYOS,
					handler_uWIFI,
					NULL,
					argumentTask,
					2048,
					23,
					1,
					NULL,
					
				};
				
				uTask_create(wifi);
			
				break;
				
			default:
				break;
			
		}
		
		
	}
	
	
}

void handler_uHTTP(void * pvParameters){
	
	genericQueueStruct *argument =  pvParameters;
	
	switch (argument->event) {
	
		case uHTTP_POSTCRED:
			
			credentials c = argument->config.wifiCred;
			
			genericQueueStruct msg = {
				
				.event = uWifi_CONNECT,
				.config.wifiCred = c
				
			};
			
			ESP_LOGI("RECIBIDO", "%s",c.ssid);
			ESP_LOGI("RECIBIDO", "%s",c.psw);
			
			xQueueSend(*(uModulesArray[uWIFI_VECT].module.uTOSQueue), &msg, 0); 
		
			break;
		
		default:
			break;
		
	}
	
	vTaskDelete(NULL);
	
}

void handler_uWIFI(void * pvParameters){
	
	genericQueueStruct *argument =  pvParameters;
		
	switch (argument->event) {
	
		case uWifi_SUCCESCREDENTIALS:
			
			credentials c = argument->config.wifiCred;
			
			genericDriverNVS blop = {
							
				.event = uNVS_SAVE,
				.label = "WiFi_Cred",
				.saveCell.key = "Cred",
				.saveCell.body.credentials = c
				
			};
			
			while(uBLOP_STATE() != uNVS_SUCCESS){
				
				vTaskDelay(1000/portTICK_PERIOD_MS);
				
			}
			
			uBLOP_CHANGE_STATE();
			
			saveBlop(blop);
		
			break;
			
		case uWifi_START:
			
			genericDriverNVS blopREAD = {
				
				.event = uNVS_READ,
				.label = "WiFi_Cred",
				.saveCell.key = "Cred"
				
			};
			
			while(uBLOPREAD_STATE() != uNVS_SUCCESS){
							
				vTaskDelay(1000/portTICK_PERIOD_MS);
				
			}
			
			uBLOPREAD_CHANGE_STATE();
			
			readBlop(blopREAD);
			
			while(uBLOPREAD_STATE() == uNVS_READ){
										
				vTaskDelay(1000/portTICK_PERIOD_MS);
				
			}
			
			if(uBLOPREAD_STATE() == uNVS_SUCCESS){
				
				credentials c = getBLOP().body.credentials;
				
				genericQueueStruct wifiConnect = {
					
					.event = uWifi_CONNECT,
					.sender = uTINYOS,
					.reciver = uWIFI,
					.config.wifiCred = c
					
				};
				
				xQueueSend(*(uModulesArray[uWIFI_VECT].module.uTOSQueue), &wifiConnect, 0);
				
			}else{
				
				genericQueueStruct wifiAP = {
					
					.event = uWifi_AP,
					.sender = uTINYOS,
					.reciver = uWIFI
					
				};
				
				xQueueSend(*(uModulesArray[uWIFI_VECT].module.uTOSQueue), &wifiAP, 0);
	
			
			}
			
		
			break;
		
		default:
			break;
		
	}
	
	vTaskDelete(NULL);
	
}

