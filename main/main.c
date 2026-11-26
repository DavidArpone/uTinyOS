#include "app.h"
#include "freertos/idf_additions.h"
#include "uTinyOS.h"

#include "driver/gpio.h"
#include "uWifiManager.h"


#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define GREEN 0
#define RED 46
#define BLUE 45

void app_main(void)
{
	
	gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);
	gpio_set_direction(RED, GPIO_MODE_OUTPUT);
	gpio_set_direction(BLUE, GPIO_MODE_OUTPUT);
	
	gpio_set_level(GREEN, 1);
	gpio_set_level(RED, 0);
	gpio_set_level(BLUE, 1);
	
	start_uTinyOS();
	
	gpio_set_level(RED, 1);
	
    while (true) {
        
		if(wifi_status_flag == WIFI_CONNECT){
					
			gpio_set_level(GREEN, 0);
			gpio_set_level(BLUE, 1);
			gpio_set_level(RED, 1);
			
			
		
		}else{
		
			gpio_set_level(RED, 1);
			vTaskDelay(100/portTICK_PERIOD_MS);
			
			gpio_set_level(RED,0);
			vTaskDelay(100/portTICK_PERIOD_MS);
			
		}
		
		vTaskDelay(1000/portTICK_PERIOD_MS);
		
		
    }
}
