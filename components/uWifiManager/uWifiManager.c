#include "uWifiManager.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_netif_types.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"
#include "uEvents.h"
#include "uTaskManager.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"

static QueueHandle_t queueEvent;
QueueHandle_t uT2uWM;

TaskHandle_t uWifiHandlers[2] = {NULL, NULL};
static credentials c;

static void wifi_event_task(void *pvParameters);
static void wifi_driver_task(void *pvParameters);
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_retry(int retry);
static void wifiScan();
static void wifi_connect(credentials wifi);
static void wifi_create();

wifiStatus wifi_status_flag = WIFI_INIT;


void uWifi_init(void *pvParameters){
	
	
	uT2uWM = xQueueCreate(10, sizeof(genericQueueStruct)); 
	
	queueEvent = xQueueCreate(10, sizeof(wifiQueue));
	
	uTaskConfig_t task_event = {
		
		"Wifi Driver",
		uWIFI,
		wifi_event_task,
		&uWifiHandlers[0],
		NULL,
		2048,
		23,
		0,
		NULL
		
	};
	
	uTask_create(task_event);
	
	uTaskConfig_t task_manager = {
		
		"Wifi Manager Task",
		uWIFI,
		wifi_driver_task,
		&uWifiHandlers[1],
		NULL,
		2048,
		23,
		0,
		NULL		
		
	};
	
	uTask_create(task_manager);
	
	esp_netif_init();
	
	esp_event_loop_create_default();
	
	esp_netif_create_default_wifi_sta();
	esp_netif_create_default_wifi_ap();
	
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	
	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_any_ip;
	
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL, &instance_any_ip);
	
	genericQueueStruct msg = {
		
		.event = uWifi_START,
		.sender = uWIFI,
		.reciver = uTINYOS,
		.config.trash = true
		
	};
	
	xQueueSend(uM2uT, &msg, 0);
	
	vTaskDelete(NULL);

	return;
	
}

static void wifi_connect(credentials wifi){
	
	wifi_config_t wifi_config = {0};
	
	strlcpy((char*)wifi_config.sta.ssid, wifi.ssid, sizeof(wifi_config.sta.ssid));
	strlcpy((char*)wifi_config.sta.password, wifi.psw, sizeof(wifi_config.sta.password));
		
	esp_wifi_set_mode(WIFI_MODE_APSTA);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
		
	esp_wifi_start();
	
	return;
}

static void wifi_create(){
		
	wifi_config_t ap_config = {
		
		.ap = {
			
			.ssid = SSID_BASE,
			.ssid_len = 0,
			.channel = 1,
			.password = PASS_BASE,
			.max_connection = 4,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK
			
		}
		
	};
	
	esp_wifi_set_mode(WIFI_MODE_AP);
	esp_wifi_set_config(WIFI_IF_AP, &ap_config);
	esp_wifi_start();
	
}

static void wifi_retry(int retry){
	
	if(retry != 0){
		
		vTaskDelay(2000/portTICK_PERIOD_MS);
		esp_wifi_connect();
		
	}else{
		
		esp_wifi_stop();
		
	}
	
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	//The event handler send through the queue the event to the wifi Task
	wifiQueue msg = {
		
		.eventBase = event_base,
		.eventId = event_id
			
	};
	
	switch (event_id){
		
		case WIFI_EVENT_STA_DISCONNECTED:
		
			memcpy(&msg.data.sta_disconnected, event_data, sizeof(wifi_event_sta_disconnected_t));
			break;
		
		case WIFI_EVENT_AP_STACONNECTED:
			
			memcpy(&msg.data.ap_connected, event_data, sizeof(wifi_event_ap_staconnected_t));
			break;
		
	}

	
	switch (event_id) {
	
		case WIFI_EVENT_STA_START:
		case WIFI_EVENT_STA_CONNECTED:
		case WIFI_EVENT_STA_DISCONNECTED:
		case WIFI_EVENT_STA_STOP:
		case WIFI_EVENT_AP_START:
				
			xQueueSend(queueEvent, &msg, 0);
			break;
		
		default:
			break;
		
	}	
	
}

void wifi_event_task(void *pvParameters){
	
	//this task receive the wi-fi event from the queue and take an action
	
	static int retry = 18;
	wifiQueue taskQueue;
	
	while(1){
		
		xQueueReceive(queueEvent, &taskQueue, portMAX_DELAY);
		
		switch (taskQueue.eventId) {
						
			case WIFI_EVENT_STA_START:
			
				esp_wifi_connect();
				retry = 18;
				break;		
				
			case WIFI_EVENT_STA_CONNECTED:
				
				wifi_status_flag = WIFI_STA;
			
				break;
				
			case WIFI_EVENT_STA_DISCONNECTED:
				
				switch (taskQueue.data.sta_disconnected.reason) {
											
					case WIFI_REASON_NO_AP_FOUND:
					
						//AP no found
						wifi_retry(retry--);	
						break;
					
					case WIFI_REASON_HANDSHAKE_TIMEOUT:
												
						//handshake fail
						wifi_retry(retry--);	
						break;
						
					case WIFI_REASON_BEACON_TIMEOUT:
						
						//Router no listen
						wifi_retry(retry--);	
						break;
					
					case WIFI_REASON_ASSOC_FAIL:
						
						//Assoc fail
						wifi_retry(retry--);	
						break;
					
					case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
						
						//Auth fail
						esp_wifi_stop();
						break;
						
					case WIFI_REASON_AUTH_FAIL:
											
						//Password fail
						esp_wifi_stop();
						break;
						
					case WIFI_REASON_AUTH_EXPIRE:
						
						//Auth expire
						esp_wifi_stop();
						break;
						
					default:
						break;
						
					
				}
		
				break;
				
			case WIFI_EVENT_STA_STOP:
			
				genericQueueStruct data = {
			        .event = uWifi_AP,
			        .sender = uWIFI,
			        .reciver = uTINYOS
			    };
				xQueueSend(uT2uWM, &data, 0);
				break;
				
			case WIFI_EVENT_AP_START:
				
				wifi_status_flag = WIFI_AP;
				break;
				
				
			case WIFI_EVENT_AP_STACONNECTED:
			
				//A new device is connected in the AP
				break;
				
			case WIFI_EVENT_AP_STADISCONNECTED:
			
				break;
				
			case WIFI_EVENT_AP_STOP:
			
				break;
				
			case WIFI_EVENT_SCAN_DONE:
				//wifiScan();
				break;
				
			default:
			
				//Default
				break;
			
		}
	}
}

void wifi_driver_task(void *pvParameters){
	
	//This task receive from the uTinyOS the commands for the module wi-fi
	
	genericQueueStruct wifiDriverQueue;
	 
	 while(1){
		
		xQueueReceive(uT2uWM, &wifiDriverQueue, portMAX_DELAY);
		
		switch (wifiDriverQueue.event){
			
			case uWifi_CONNECT:
				
				c = wifiDriverQueue.config.wifiCred;
				ESP_LOGI("RECIBIDO", "%s",c.ssid);
				ESP_LOGI("RECIBIDO", "%s",c.psw);
				wifi_connect(c);
				break;
			
			case uWifi_AP:
			
				wifi_create();
				break;
				
				
			case uEVENTS_DEFAULT:
				esp_wifi_stop();
				break;
				
			default:
				break;		
		
		
		}
		
	 }
	
}

static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	
	switch (event_id) {
		
		case IP_EVENT_STA_GOT_IP:
			//ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
			//Now its connected
		
			wifi_status_flag = WIFI_CONNECT;
			genericQueueStruct msg = { 
				.event = uWifi_SUCCESCREDENTIALS,
				.sender = uWIFI,
				.reciver = uTINYOS
			};
			
			xQueueSend(uM2uT, &msg, 0);			
			break;
			
		case IP_EVENT_STA_LOST_IP:
			break;
		default:
			break;
	
	}
	
}
