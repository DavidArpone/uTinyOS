idf_component_register(
		SRCS "uSpiffs.c"
        
        INCLUDE_DIRS
        	"include"
        
        REQUIRES
        	spiffs
        	esp_http_server
)

