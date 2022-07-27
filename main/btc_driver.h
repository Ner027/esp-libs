#ifndef BTC_DRIVER_H
#define BTC_DRIVER_H

#define BT_TAG "Bluetooth"
#define BT_NAME "ESP"

void config_bluetooth(void (*callback)(esp_spp_cb_param_t*),void(*conCallback)(void));
bool send_data(int dataLen,uint8_t* data);

#endif
