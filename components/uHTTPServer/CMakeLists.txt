idf_component_register(
		SRCS "uHTTPServer.c"
        
        INCLUDE_DIRS 
        	"include"
        	
        REQUIRES
        	uEvents
        	spiffs
        	esp_http_server
         	
)

