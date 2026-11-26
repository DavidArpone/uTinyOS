idf_component_register(
    	SRCS "main.c"         
    	
    	INCLUDE_DIRS
    		"include"
    	
    	REQUIRES
    		uTinyOS
    		esp_driver_gpio
    		uWifiManager
    		
    	
)
