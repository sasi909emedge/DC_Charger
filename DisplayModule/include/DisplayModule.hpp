/*
 * DisplayModule.h
 *
 *  Created on: May 28, 2025
 *      Author: kishore
 */

#ifndef INC_DISPLAY_MODULE_H_
#define INC_DISPLAY_MODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include "CCSConnector.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

// C++ specific section begins here
namespace DisplayModule
{
   enum class PacketType : uint8_t
   {
      PAGE,
      COMMAND
   };
   // Function pointer types
   typedef bool (*SendDataFunc)(uint8_t *data, uint16_t length);
   // typedef bool (*ReceiveDataFunc)(uint8_t *data, uint16_t length);

   // C++ interface class with original methods
   class DisplayController
   {
   public:
      const uint8_t dwin_cmd[8] = {0x5A, 0xA5, 0x05, 0x82, 0x10, 0x03, 0x00, 0x00};
      uint8_t dwin_buf[8] = {0x5A, 0xA5, 0x05, 0x82, 0x10, 0x03, 0x00, 0x00};

      const uint8_t dwin_page_cmd[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00};
      uint8_t dwin_page_buf[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00};

      DisplayController(SendDataFunc sendFunction);

      static void displayTask(void *pvParameters);
      // Set function pointers
      void SetSendFunction(SendDataFunc func);
      // void SetReceiveFunction(ReceiveDataFunc func);

      // Call these instead of original methods
      bool SendData(uint8_t *data, uint16_t length);
      bool ReceiveData(uint8_t *data, uint16_t length);

      ~DisplayController();

      void Set_INITIALIZING(void);
      void Set_COMMISSIONING(void);
      void Set_FIRMWARE_UPDATE(void);
      void Set_RFID_TAPPED(void);
      void Set_AUTH_FAILED(void);
      void Set_AUTH_SUCCESS_PLUG_EV(void);
      void Set_AUTH_SUCCESS(void);
      void Set_NETWORKSWITCH(void);

      void Set_RESERVED(uint8_t ConnId);
      void Set_AVAILABLE(uint8_t ConnId);
      void Set_SUSPENDED(uint8_t ConnId);
      void Set_UNAVAILABLE(uint8_t ConnId);
      void Set_EV_PLUGGED_TAP_RFID(uint8_t ConnId);
      void Set_FINISHING(uint8_t ConnId);
      void Set_CHARGING(uint8_t ConnId);
      void Set_FAULT(uint8_t ConnId);

   private:
      SendDataFunc sendFunc;
      // ReceiveDataFunc receiveFunc;

      void SendToDisplay(uint8_t ConnId, PacketType packet);
   };

} // namespace DisplayModule

#endif // __cplusplus
#endif /* INC_DISPLAY_MODULE_H_ */