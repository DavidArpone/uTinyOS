#include "uHTTPServer.h"
#include "esp_http_server.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "uEvents.h"

static esp_err_t setWifi_handler(httpd_req_t *);
static esp_err_t setWifi_handler_css(httpd_req_t *);
static esp_err_t setWifi_post_handler(httpd_req_t *);
//static esp_err_t http_wifi_scan_start(httpd_req_t *);
//static esp_err_t http_wifi_scan_get(httpd_req_t *);

static httpd_handle_t server;

static const httpd_uri_t url_setWifiGet = {
	
			.uri = "/setWifi",
			.method = HTTP_GET,
			.handler = setWifi_handler,
			.user_ctx = NULL
			
};

static const httpd_uri_t url_setWifiGetCss = {
	
			.uri = "/styles.css",
			.method = HTTP_GET,
			.handler = setWifi_handler_css,
			.user_ctx = NULL
			
};

static const httpd_uri_t url_setWifiPost = {
	
			.uri = "/",
			.method = HTTP_POST,
			.handler = setWifi_post_handler,
			.user_ctx = NULL
	
};

void start_webserver(void * pvParameters){
	
	uWS2uT = xQueueCreate(10, sizeof(genericQueueStruct)); 
	
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	
	server = NULL;
	
	if(httpd_start(&server, &config) == ESP_OK){
		
		//Server mount
		
		if(httpd_register_uri_handler(server, &url_setWifiGet) == ESP_OK){
			
			//URL handler get is ok
			
		}
		
		if(httpd_register_uri_handler(server, &url_setWifiGetCss) == ESP_OK){
					
			//URL handler get is ok
				
		}
		
		if(httpd_register_uri_handler(server, &url_setWifiPost) == ESP_OK){
					
			//URL hanlder post is ok
					
		}
		
	}
	
	vTaskDelete(NULL);
	
}


esp_err_t setWifi_handler(httpd_req_t *req){
	
	//setWifi GET
	
	FILE *f = fopen("/spiffs/setWifi.html", "r");
	
	
	if(f == NULL){
		
		//setWifi.html not found
		httpd_resp_send_404(req);
		return ESP_FAIL;
		
	}
	
	httpd_resp_set_type(req, "text/html");
	
	char buffer[128];
	
	while(fgets(buffer, sizeof(buffer), f)){
		
		httpd_resp_send_chunk(req, buffer, strlen(buffer));
		
	}
	
	fclose(f);
	
	httpd_resp_send_chunk(req, NULL, 0);
	
	return ESP_OK;
}

esp_err_t setWifi_handler_css(httpd_req_t *req){
	
	//setWifi GET
	
	FILE *f = fopen("/spiffs/styles.css", "r");
	
	
	if(f == NULL){
		
		//setWifi.html not found
		httpd_resp_send_404(req);
		return ESP_FAIL;
		
	}
	
	httpd_resp_set_type(req, "text/css");
	
	char buffer[128];
	
	while(fgets(buffer, sizeof(buffer), f)){
		
		httpd_resp_send_chunk(req, buffer, strlen(buffer));
		
	}
	
	fclose(f);
	
	httpd_resp_send_chunk(req, NULL, 0);
	
	return ESP_OK;
}

esp_err_t setWifi_post_handler(httpd_req_t *req){

    char *buffer = malloc(req->content_len + 1);

    if(buffer == NULL){
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    int received = 0;

    while(received < req->content_len){

        int ret = httpd_req_recv(
            req,
            buffer + received,
            req->content_len - received
        );

        if(ret <= 0){
            free(buffer);
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }

        received += ret;
    }

    buffer[received] = '\0';

    char ssid[32] = {0};
    char psw[64] = {0};

    if(httpd_query_key_value(buffer, "ssid", ssid, sizeof(ssid)) != ESP_OK ||
       httpd_query_key_value(buffer, "psw", psw, sizeof(psw)) != ESP_OK){

        free(buffer);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid credentials");
        return ESP_FAIL;
    }

    credentials c = {0};

    strlcpy(c.ssid, ssid, sizeof(c.ssid));
    strlcpy(c.psw, psw, sizeof(c.psw));

    genericQueueStruct msg = {
        .event = uHTTP_POSTCRED,
        .sender = uHTTPSERVER,
        .reciver = uTINYOS,
        .config.wifiCred = c
    };

    BaseType_t sent = xQueueSend(uM2uT, &msg, 0);

    free(buffer);

    if(sent != pdPASS){
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_sendstr(req, "Credentials received");

    return ESP_OK;
}
