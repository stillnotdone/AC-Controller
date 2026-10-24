#ifndef AC_CONFIG
#define AC_CONFIG

#define AC_COMMAND_QUEUE_SIZE 25
#define AC_COMMAND_UPDATE_FREQUENCY 400
#define AC_IR_LED_PIN 4 

// for longer distances some frames might get dropped, comment this out to only send it once
#define AC_DOUBLE_SEND_PAYLOAD_DELAY 100

#define AC_MIN_TEMP (uint8_t)62u
#define AC_MAX_TEMP (uint8_t)86u

#define AC_DEFAULT_TEMP (AC_MIN_TEMP + ((AC_MAX_TEMP - AC_MIN_TEMP) / 2))
#define AC_DEFAULT_SPEED AC_SPEED_LOW
#define AC_DEFAULT_MODE AC_MODE_COOL

#define AC_DEFAULT_SPEED_MODE AC_MODE_FAN
#define AC_DEFAULT_TEMP_MODE AC_MODE_COOL

#define WEB_API_KEY_HEADER "X-Api-Key"

#define WEB_UPDATE_FREQUENCY 100
#define WEB_WIFI_STOP_RETRY_AFTER 60000

#endif