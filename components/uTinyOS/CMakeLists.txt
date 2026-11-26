idf_component_register(
		SRCS "uTinyOS.c"

        INCLUDE_DIRS 
        	"include"
        	
        REQUIRES
        	uEvents
        	uTaskManager
        	uWifiManager
        	uHTTPServer
        	uModuleManager
        	uSpiffs
        	log
)

