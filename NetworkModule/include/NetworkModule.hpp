/*
 * NetworkModule.h
 *
 *  Created on: May 26, 2025
 *      Author: kishore
 */

#ifndef INC_NETWORK_MODULE_H_
#define INC_NETWORK_MODULE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_defaults.h"
#include "esp_eth.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include "esp_crt_bundle.h"
#include "ConfigModule.hpp"
#include "esp_websocket_client.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

// C++ specific section begins here
namespace NetworkModule
{

   enum class NETIF_ID : uint8_t
   {
      NOT_CONNECTED,
      ConnectedToWIFI,
      ConnectedToMODEM,
      ConnectedToETHERNET
   };
   // Function pointer types
   // typedef bool (*SendDataFunc)(std::string jsonData);
   typedef bool (*ReceiveDataFunc)(std::string jsonData);

   // C++ interface class with original methods
   class NetworkController
   {
   public:
      NetworkController(ReceiveDataFunc receiveFunction);

      bool isWebsocketConnected;
      bool isDiagnosticServerConnected;
      bool isApConnected;
      bool isWifiConnected;
      bool isGsmConnected;
      bool isEthernetConnected;
      bool isWifiEnabled;
      bool isModemEnabled;
      bool isEthernetEnabled;
      size_t WebSocketPingInterval;

      uint8_t HighPriorityNetwork;
      bool wifiSignalStrengthReceived;
      bool wifiWeakSignal;
      int wifiSignalStrength;
      int wifi_rssi;

      bool gsmSignalStrengthReceived;
      bool gsmWeakSignal;
      int gsmSignalStrength;
      int gsm_rssi;
      int gsm_ber;
      char gsm_imei[20];
      char gsm_imsi[20];

      static void networkTask(void *pvParameters);
      void SetReceiveFunction(ReceiveDataFunc func);
      bool SendData(std::string jsonData);
      bool ReceiveData(std::string jsonData);

      bool modemDiscoonectedFromPPPServer;
      esp_netif_t *netif[3];
      esp_websocket_client_handle_t client;

      ~NetworkController();

   private:
      ReceiveDataFunc receiveFunc;
   };

   void wifi_initialise(int priority);
   void ethernet_initialise(int priority);
   void modem_initialise(int priority);
   void restartEthernet(void);
} // namespace NetworkModule

extern NetworkModule::NetworkController *network;

#endif // __cplusplus
#endif /* INC_NETWORK_MODULE_H_ */