/*
 * SerialModule.h
 *
 *  Created on: May 13, 2025
 *      Author: kishore
 */

#ifndef INC_GPIO_MODULE_H_
#define INC_GPIO_MODULE_H_

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
namespace GpioModule
{

   enum class GpioState : uint8_t
   {
      OFF,
      ON,
      WELD,
      INVALID
   };

   typedef struct
   {
      bool isAvailable;
      bool isInput;
      bool isDirectGpio;
      bool isActiveLow;
      GpioState state;
      uint16_t gpioNum;
      uint8_t gpioPort;
      char name[50];
   } ChargerGpio_t;

   // Function pointer types
   typedef bool (*SendDataFunc)(uint8_t port, uint16_t gpio, uint8_t state);
   // typedef bool (*ReceiveDataFunc)(uint8_t *data, uint16_t length);

   // C++ interface class with original methods
   class GpioController
   {
   public:
      GpioController(SendDataFunc sendFunction);

      ChargerGpio_t StmReset;
      ChargerGpio_t ModemReset;

      ChargerGpio_t DcGun[NUM_OF_CONNECTORS + 1];
      ChargerGpio_t DcMerger[4];
      ChargerGpio_t AcPm[5];
      ChargerGpio_t V12Control;
      ChargerGpio_t BatteryControl;
      ChargerGpio_t VentilationFan[3];

      ChargerGpio_t EmergencyBtn;
      ChargerGpio_t EarthFault;
      ChargerGpio_t Door;
      ChargerGpio_t SpdFault;
      ChargerGpio_t SmokeFault;
      ChargerGpio_t DcGunNegFb[NUM_OF_CONNECTORS + 1];
      ChargerGpio_t DcGunPosFb[NUM_OF_CONNECTORS + 1];
      ChargerGpio_t DcGunIsoFb[NUM_OF_CONNECTORS + 1];
      ChargerGpio_t AcPmFb[5];
      ChargerGpio_t DcMergerPosFb[4];
      ChargerGpio_t DcMergerNegFb[4];
      ChargerGpio_t DcGunFuseFb[NUM_OF_CONNECTORS + 1];

      ChargerGpio_t *gpioList[50];
      GpioState GpioStateGet(ChargerGpio_t gpio);
      bool GpioStateSet(ChargerGpio_t gpio, GpioState state);
      const char *GpioStateStringify(GpioState state);

      static void GpioTask(void *pvParameters);
      // Set function pointers
      void SetSendFunction(SendDataFunc func);
      // void SetReceiveFunction(ReceiveDataFunc func);

      // Call these instead of original methods
      bool SendData(uint8_t port, uint16_t gpio, uint8_t state);
      bool ReceiveData(uint8_t port, uint16_t gpio, uint8_t state);

      ~GpioController();

   private:
      SendDataFunc sendFunc;
      // ReceiveDataFunc receiveFunc;
   };

} // namespace GpioModule
extern GpioModule::GpioController *gpio;

#endif // __cplusplus
#endif /* INC_GPIO_MODULE_H_ */