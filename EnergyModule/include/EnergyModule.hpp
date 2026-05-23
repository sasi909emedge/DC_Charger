/*
 * SerialModule.h
 *
 *  Created on: May 13, 2025
 *      Author: kishore
 */

#ifndef INC_ENERGY_MODULE_H_
#define INC_ENERGY_MODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include "ConfigModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

// C++ specific section begins here
namespace EnergyModule
{

   enum class EnergyPacket : uint8_t
   {
      AC,
      DC
   };

   enum class MeterEnable : uint8_t
   {
      DISABLE,
      ENABLE
   };

#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((__packed__))
#elif defined(_MSC_VER)
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED
#endif
   struct PACKED MeterValueDC
   {
      float voltage;
      float current;
      float power;
      float temperature;
   };

   struct PACKED MeterValueAC
   {
      float voltage[4];
      float current[4];
      float power[4];
      float temperature;
      float powerFactor;
   };

#if defined(_MSC_VER)
#pragma pack(pop)
#endif
#undef PACKED

   // Function pointer types
   typedef bool (*SendDataFunc)(uint8_t *data, uint16_t length);
   // typedef bool (*ReceiveDataFunc)(uint8_t *data, uint16_t length);

   // C++ interface class with original methods
   class EnergyController
   {
   public:
      EnergyController(SendDataFunc sendFunction);
      bool SendEnergyConfig(uint8_t ConnID, MeterEnable Enable, EnergyPacket mode, uint16_t sampletimeinMillis);
      MeterValueAC GetACEnergyMeterValue(uint8_t ConnID);
      MeterValueDC GetDCEnergyMeterValue(uint8_t ConnID);

      static void energyTask(void *pvParameters);
      // Set function pointers
      void SetSendFunction(SendDataFunc func);
      // void SetReceiveFunction(ReceiveDataFunc func);

      // Call these instead of original methods
      bool SendData(uint8_t *data, uint16_t length);
      bool ReceiveData(uint8_t *data, uint16_t length);

      ~EnergyController();

   private:
      SendDataFunc sendFunc;
      // ReceiveDataFunc receiveFunc;
      MeterValueAC ACMeterValues[NUM_OF_CONNECTORS];
      MeterValueAC ACMeterFaultValues[NUM_OF_CONNECTORS];
      MeterValueDC DCMeterValues[NUM_OF_CONNECTORS];
      MeterValueDC DCMeterFaultValues[NUM_OF_CONNECTORS];
   };

} // namespace EnergyModule

extern EnergyModule::EnergyController *energy;

#endif // __cplusplus
#endif /* INC_ENERGY_MODULE_H_ */