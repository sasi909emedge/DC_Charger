/*
 * PowerModule.h
 *
 *  Created on: May 9, 2025
 *      Author: kishore
 */

#ifndef INC_POWERMODULE_H_
#define INC_POWERMODULE_H_

#include <cstdint>
#include <cstdbool>
#include <vector>
#include <array>
#include <cstring>
#include <stdexcept>

#include "PowerModuleBase.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

// C++ specific section begins here
namespace PowerModule
{
   // Constants and scaling factors
   namespace Constants
   {
      constexpr double VOLTAGE_FACTOR = 10.0;
      constexpr double CURRENT_FACTOR = 100.0;
      constexpr double TEMPERATURE_FACTOR = 1.0;
      constexpr uint8_t MAX_MODULES = NUM_OF_POWER_MODULES + 1;
   }

   enum class ConfirmationStatus : uint8_t
   {
      NOT_RECEIVED = 0x00,
      RECEIVED = 0x01
   };

   enum class InputSourceState : uint8_t
   {
      AC = 0,
      DC = 1
   };

   enum class InputMode : uint8_t
   {
      DC = 0,
      AC = 1
   };

   enum class E2FaultState : uint8_t
   {
      ENABLED = 0,
      DISABLED = 1
   };

   enum class HotPlugState : uint8_t
   {
      DISABLED = 0,
      ENABLED = 1
   };

   enum class ModuleStartStopCommand : uint8_t
   {
      START = 0xAA,
      STOP = 0x55
   };

   enum class ModuleProcessState : uint8_t
   {
      NO = 0,
      YES = 1
   };

   enum class AddressSettingMode : uint8_t
   {
      AUTOMATIC = 0,
      MANUAL = 1
   };

   enum class ModuleAddress : uint8_t
   {
      MASTER = 0xA0,
      BROADCAST = 0xFF
   };

   enum class ModuleGroupNumber : uint8_t
   {
      GROUP0 = 0,
      GROUP1 = 1
   };

   enum class AddressMultiple : uint8_t
   {
      MULTIPLE0 = 0,
      MULTIPLE1 = 1
   };

   enum class Priority : uint8_t
   {
      CHARGING_MODULE_STATE = 6,
      MODULE_START_STOP_CONFIRMATION = 2,
      AC_INFORMATION_UPLOAD = 6,
      EXTENDED_STATE_FAULT_INFO = 7,
      MODULE_START_STOP = 2,
      MODULE_PARAMETER_SETTING = 4,
      MODULE_TIMING_COMMAND = 6,
      MODULE_INPUT_MODE_CONFIG = 6,
      MODULE_ADDRESS_SETTING_SELECTION = 7,
      MODULE_ADDRESS_SETTING = 6,
      SPECIFIC_MODULE_START_STOP = 2
   };

   enum class PGN : uint8_t
   {
      CHARGING_MODULE_STATE = 0x01,
      MODULE_START_STOP_CONFIRMATION = 0x02,
      MODULE_START_STOP = 0x03,
      MODULE_PARAMETER_SETTING = 0x04,
      MODULE_TIMING_COMMAND = 0x05,
      SPECIFIC_MODULE_START_STOP = 0x06,
      MODULE_ADDRESS_SETTING = 0x09,
      AC_INFORMATION_UPLOAD = 0x0B,
      MODULE_ADDRESS_SETTING_SELECTION = 0x90,
      EXTENDED_STATE_FAULT_INFO = 0x91,
      MODULE_INPUT_MODE_CONFIG = 0xAA
   };

   // Define the packed structure with proper C++ alignment control
#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((__packed__))
#elif defined(_MSC_VER)
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED
#endif

   struct PACKED TonhePMData
   {
      struct PACKED ChargingModuleState
      {
         uint64_t state : 8;
         uint64_t outputVoltage : 16;
         uint64_t outputCurrent : 16;

         uint64_t inputUnderVoltage : 1;
         uint64_t inputPhaseLoss : 1;
         uint64_t inputOverVoltage : 1;
         uint64_t outputOverVoltage : 1;
         uint64_t outputOverCurrent : 1;
         uint64_t highTemperature : 1;
         uint64_t fanFault : 1;
         uint64_t hardwareFault : 1;
         uint64_t busException : 1;
         uint64_t sciCommException : 1;
         uint64_t dischargeFault : 1;
         uint64_t pfcShutdownException : 1;
         uint64_t outputUnderVoltageWarning : 1;
         uint64_t outputOverVoltageWarning : 1;
         uint64_t powerLimitHighTemp : 1;
         uint64_t shortCircuitFault : 1;

         uint64_t inputOverCurrent : 1;
         uint64_t mainsFrequency : 1;
         uint64_t mainsImbalance : 1;
         uint64_t dcTzFault : 1;
         uint64_t addressConflict : 1;
         uint64_t busBias : 1;
         uint64_t phaseException : 1;
         uint64_t busOverVoltage : 1;
      } chargingModuleState;

      struct PACKED StartStopConfirmation
      {
         uint64_t confirmationStatus : 8;
         uint64_t spare : 56;
      } moduleStartStopConfirmation;

      struct PACKED AcInformation
      {
         uint64_t PhaseAVoltage : 16;
         uint64_t PhaseBVoltage : 16;
         uint64_t PhaseCVoltage : 16;
         uint64_t temperature : 32;
      } acInformationUpload;

      struct PACKED ExtendedStateFault
      {
         uint64_t currentEqualization : 1;
         uint64_t mute : 1;
         uint64_t e2FaultOverflow : 1;
         uint64_t inputSource : 1;
         uint64_t e2FaultEnable : 1;
         uint64_t hotPlugStatus : 1;
         uint64_t spare1 : 10;

         uint64_t preLevelWaveStop : 1;
         uint64_t hotPlugFault : 1;
         uint64_t canCommunicationTimeout : 1;
         uint64_t reserved3 : 1;
         uint64_t relayOperationFault : 1;
         uint64_t reserved5 : 1;
         uint64_t internalElementOverTemperature : 1;
         uint64_t airInletOverTemperature : 1;
         uint64_t powerLimitOverTemperature : 1;
         uint64_t dischargeChangeoverAbnormal : 1;
         uint64_t spare2 : 38;
      } extendedStateFaultInfo;

      struct PACKED ModuleStartStop
      {
         uint64_t module8 : 1;
         uint64_t module7 : 1;
         uint64_t module6 : 1;
         uint64_t module5 : 1;
         uint64_t module4 : 1;
         uint64_t module3 : 1;
         uint64_t module2 : 1;
         uint64_t module1 : 1;
         uint64_t module16 : 1;
         uint64_t module15 : 1;
         uint64_t module14 : 1;
         uint64_t module13 : 1;
         uint64_t module12 : 1;
         uint64_t module11 : 1;
         uint64_t module10 : 1;
         uint64_t module9 : 1;
         uint64_t module24 : 1;
         uint64_t module23 : 1;
         uint64_t module22 : 1;
         uint64_t module21 : 1;
         uint64_t module20 : 1;
         uint64_t module19 : 1;
         uint64_t module18 : 1;
         uint64_t module17 : 1;

         uint64_t moduleStartStop : 8;

         uint64_t moduleGroupNumber : 4;
         uint64_t addressMultiple : 4;

         uint64_t spare : 24;
      } moduleStartStop;

      struct PACKED ParameterSetting
      {
         uint64_t module8 : 1;
         uint64_t module7 : 1;
         uint64_t module6 : 1;
         uint64_t module5 : 1;
         uint64_t module4 : 1;
         uint64_t module3 : 1;
         uint64_t module2 : 1;
         uint64_t module1 : 1;
         uint64_t module16 : 1;
         uint64_t module15 : 1;
         uint64_t module14 : 1;
         uint64_t module13 : 1;
         uint64_t module12 : 1;
         uint64_t module11 : 1;
         uint64_t module10 : 1;
         uint64_t module9 : 1;
         uint64_t module24 : 1;
         uint64_t module23 : 1;
         uint64_t module22 : 1;
         uint64_t module21 : 1;
         uint64_t module20 : 1;
         uint64_t module19 : 1;
         uint64_t module18 : 1;
         uint64_t module17 : 1;
         uint64_t moduleGroupNumber : 4;
         uint64_t addressMultiple : 4;
         uint64_t Voltage : 16;
         uint64_t Current : 16;
      } moduleParameterSetting;

      struct PACKED TimingCommand
      {
         uint64_t spare;
      } moduleTimingCommand;

      struct PACKED SpecificModuleCommand
      {
         uint64_t moduleStartStop : 8;
         uint64_t spare1 : 8;
         uint64_t chargingVoltage : 16;
         uint64_t chargingCurrent : 16;
         uint64_t spare2 : 16;
      } specificModuleStartStop;

      struct PACKED InputModeConfig
      {
         uint64_t inputMode : 8; // 0: DC, 1: AC
         uint64_t spare : 56;
      } inputModeConfiguration;

      struct PACKED AddressSettingSelection
      {
         uint64_t addressSetting : 8; // 0: Automatic, 1: Manual
         uint64_t spare : 56;
      } addressSettingSelection;

      struct PACKED AddressSetting
      {
         uint64_t moduleAddress : 8;
         uint64_t spare : 56;
      } moduleAddressSetting;

      TonhePMData()
      {
         std::memset(this, 0, sizeof(TonhePMData));
      }
   };

   struct PACKED TonhePMCanId
   {
      uint32_t SA : 8;
      uint32_t DA : 8;
      uint32_t PGN : 8;
      uint32_t Spare1 : 2;
      uint32_t Priority : 3;
      uint32_t Spare2 : 3;

      TonhePMCanId() : SA(0), DA(0), PGN(0), Spare1(0), Priority(0), Spare2(0) {}
   };

#if defined(_MSC_VER)
#pragma pack(pop)
#endif
#undef PACKED

   // Function pointer types
   typedef bool (*SendDataFunc)(const uint32_t id, const uint64_t data);
   // typedef bool (*ReceiveDataFunc)(const uint32_t id, const uint64_t data);

   // C++ interface class with original methods
   class TonhePMController : public PowerModuleBase
   {
   public:
      TonhePMController(SendDataFunc sendFunction);

      void Initialize() override;
      bool ModuleStart(uint8_t moduleAddress, float voltage, float current) override;
      bool BulkModuleStart(uint64_t Modules, float voltage, float current) override;

      bool ModuleStop(uint8_t moduleAddress, float voltage, float current) override;
      bool BulkModuleStop(uint64_t Modules, float voltage, float current) override;

      bool ModuleParameters(uint8_t moduleAddress, float voltage, float current) override;
      bool BulkModuleParameters(uint64_t Modules, float voltage, float current) override;

      PowerModule::ModuleStatus GetModuleStatus(uint8_t moduleAddress) const override;

      bool ModuleTimingCommand(void) override;

      // Set function pointers
      void SetSendFunction(SendDataFunc func);
      // void SetReceiveFunction(ReceiveDataFunc func);

      // Call these instead of original methods
      bool SendData(const uint32_t id, const uint64_t data);
      bool ReceiveData(const uint32_t id, const uint64_t data);

      ~TonhePMController();

      std::array<PowerModule::ModuleStatus, PowerModule::Constants::MAX_MODULES> moduleStatus;

   private:
      SendDataFunc sendFunc;
      // ReceiveDataFunc receiveFunc;

      // Original C functions wrapped in C++ class
      TonhePMCanId getCanExtId(uint8_t Priority, uint8_t PGN, uint8_t DestinationAddress, uint8_t SourceAddress);
      bool SpecificModuleStartStop(uint8_t ModuleAddress, uint8_t StartStop, uint16_t Voltage, uint16_t Current);
      bool ModuleStartStop(uint32_t Modules, uint8_t StartStop, uint8_t ModuleGroupNumber, uint8_t AddressMultiple);
      bool ModuleParameterSetting(uint32_t Modules, uint8_t ModuleGroupNumber, uint8_t AddressMultiple, uint16_t ChargingVoltage, uint16_t ChargingCurrent);
      bool ModuleAddressSetting(uint8_t Address);
      bool ModuleAddressSettingSelection(uint8_t SelectionType);
      bool ModuleInputModeConfiguration(uint8_t Mode);
      TonhePMCanId DecodePowerModuleCanExtId(uint32_t RxCanExtId);
      void ChargingModuleState(const uint32_t id, const uint64_t Candata);
      void SpecificModuleStartStopConfirmation(const uint32_t id, const uint64_t Candata);
      void AcInformationUpload(const uint32_t id, const uint64_t Candata);
      void ExtendedStateFaultInfoUpload(const uint32_t id, const uint64_t Candata);
   };

} // namespace PowerModule

extern PowerModule::TonhePMController *pmc;
#endif // __cplusplus
#endif /* INC_POWERMODULE_H_ */