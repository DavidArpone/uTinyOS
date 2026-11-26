idf_component_register(
		SRCS "uDriverNVS.c"
        
        INCLUDE_DIRS
        	"include"
        	
        REQUIRES
        	uEvents
        	nvs_flash

)

