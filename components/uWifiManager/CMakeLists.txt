idf_component_register(
		SRCS "uWifiManager.c"

		INCLUDE_DIRS 
			"include"
		
		REQUIRES
			esp_wifi
			uEvents
			uTaskManager
			log
)

