/*
 * SerialModule.h
 *
 *  Created on: May 13, 2025
 *      Author: kishore
 */

#ifndef INC_SERIALMODULE_H_
#define INC_SERIALMODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include "TimeModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

// C++ specific section begins here
namespace SerialModule
{
   enum class PacketId : uint16_t
   {
      // SystemHealth ID's
      HeartBeat = 10,

      // CAN1 ID's
      CAN1 = 100,

      // CAN2 ID's
      CAN2 = 200,

      // Modbus ID's
      ENERGY = 300,

      // Display ID's
      DISPLAY = 400,
      // Relay ID's
      RELAY = 500,

      // RFID ID's
      RFID = 600,

      // ALPR
      ALPR = 700

   };

   // Function pointer types
   // typedef bool (*SendDataFunc)(const uint32_t id, const uint64_t data);
   typedef bool (*ReceiveDataFunc)(const PacketId id, uint8_t *data, uint16_t length);

   // C++ interface class with original methods
   class SerialController
   {
   public:
      uint32_t HeartBeatCount = 0;
      SerialController(ReceiveDataFunc receiveFunction);
      static void serialTask(void *pvParameters);
      void SetReceiveFunction(ReceiveDataFunc func);
      bool SendHeartBeat(void);
      bool SendData(const PacketId id, uint8_t *data, uint16_t length);
      bool ReceiveData(const PacketId id, uint8_t *data, uint16_t length);

      ~SerialController();

   private:
      bool isActive = false;
      ReceiveDataFunc receiveFunc;
   };

} // namespace SerialModule
extern SerialModule::SerialController *serial;

#endif // __cplusplus
#endif /* INC_SERIALMODULE_H_ */