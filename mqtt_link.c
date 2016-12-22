#define MODULE_NAME MQTT_LINK
#include "trace.h"
#include "mqtt.h"
#include <string.h>
#include "lwip/err.h"
#include "blf.h"

static mqtt_client_t mqtt_client;

static void sub_callback(void *ctx, err_t result)
{
  TTRACE(TTRACE_INFO, "Subscribe callback result: \n", result);
}

static void publish_callback(void *ctx, err_t result)
{
  TTRACE(TTRACE_INFO, "Publish callback result: \n", result);
}

/*
static void connect_info_callback(mqtt_client_t *client, void *ctx,  mqtt_connect_info_t *ci, mqtt_set_connect_info_cb cb)

  else if(status == MQTT_CONNECT_STATUS_GET_INFO)
  {
     //mqtt_connect_info_t ci;
     //mqtt_client_init_connect_info(&ci);
     ci.client_id = "RCLink"
     ci.user_id = "name"
     ci.pass = "password"
     mqtt_client_set_connect_info(client, &ci);
     cb(client, ci);
  }
*/
static BLFCbTimer publishTimer;
/*
static int testseq = 0;
static void publish_timer_cb(BLFCbTimer *timer, void *ctx)
{
  char data[32];
  sprintf(data, "Test seq %d", testseq);
  testseq++;
  mqtt_publish(&mqtt_client, "rclink/rx433", data, strlen(data), 0, 0, publish_callback, 0);
  BLF_startCbTimer(&publishTimer, publish_timer_cb, NULL, 1000);
}
*/
static void connect_callback(mqtt_client_t *client, void *ctx, mqtt_connection_status_t status)
{
  if(status == MQTT_CONNECT_ACCEPTED) {
    mqtt_subscribe(client, "rclink/tx433", 0, sub_callback, 0);
    mqtt_subscribe(client, "rclink/cmd", 0, sub_callback, 0);
    // BLF_startCbTimer(&publishTimer, publish_timer_cb, NULL, 1000);

  } else {
    TTRACE(TTRACE_WARN, "Disconnect result: %d\n", status);
  }
}

void mqtt_link_publish(void)
{
  if(mqtt_client_is_connected(&mqtt_client)) {
    const char data[] = {"rx_data"};
    err_t err = mqtt_publish(&mqtt_client, "rclink/rx433", data, strlen(data), 0, 0, publish_callback, 0);
    if(err != ERR_OK) {
      TTRACE(TTRACE_WARN, "Publish result: \n", err);
    }
  }
}

void mqtt_link_publish_rcrx(const char *s)
{
  if(mqtt_client_is_connected(&mqtt_client)) {
    err_t err = mqtt_publish(&mqtt_client, "rclink/rx433", s, strlen(s), 0, 0, publish_callback, 0);
    if(err != ERR_OK) {
      TTRACE(TTRACE_WARN, "Publish result: \n", err);
    }

  }
}


void mqtt_link_init(void)
{
  BLF_initCbTimer(&publishTimer);
  struct mqtt_connect_client_info_t client_info;
  memset(&client_info, 0, sizeof(client_info));
  client_info.client_id = "rclink";
  client_info.keep_alive = 120;
  mqtt_client_connect(&mqtt_client, "192.168.2.1", connect_callback, 0, &client_info);
  //mqtt_connect(&mqtt_client, "192.168.4.1", connect_callback, 0);
}

