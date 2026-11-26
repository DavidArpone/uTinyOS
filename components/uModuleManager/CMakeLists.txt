idf_component_register(
		SRCS "uModuleManager.c"
		
		INCLUDE_DIRS 
			"include"
		
		REQUIRES
			uEvents
			uTaskManager
			uWifiManager
			uHTTPServer
			uTinyOS
			uDriverNVS
)

