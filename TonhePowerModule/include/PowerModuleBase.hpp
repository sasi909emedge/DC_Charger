/*
 * PowerModule.h
 *
 *  Created on: May 9, 2025
 *      Author: kishore
 */

#ifndef INC_POWERMODULEBASE_H_
#define INC_POWERMODULEBASE_H_

#include <cstdint>
#include <cstdbool>
#include <vector>
#include <array>

#include "ConfigModule.hpp"

// C++ specific section begins here
namespace PowerModule
{
    enum class FaultBits : uint32_t
    {
        NO_FAULT = 0,
        INPUT_UNDER_VOLTAGE = 1,
        INPUT_OVER_VOLTAGE = 2,
        OUTPUT_OVER_VOLTAGE = 3,
        OUTPUT_OVER_CURRENT = 4,
        HIGH_TEMPERATURE = 5,
        FAN_FAULT = 6,
        HARDWARE_FAULT = 7,
        BUS_EXCEPTION = 8,
        SCI_COMM_EXCEPTION = 9,
        DISCHARGE_FAULT = 10,
        PFC_SHUTDOWN_EXCEPTION = 11,
        OUTPUT_UNDER_VOLTAGE_WARNING = 12,
        OUTPUT_OVER_VOLTAGE_WARNING = 13,
        POWER_LIMIT_HIGH_TEMP = 14,
        SHORT_CIRCUIT_FAULT = 15
    };
    enum class ChargingModuleState : uint8_t
    {
        NORMAL_OFF = 0x00,
        ON = 0x01,
        FAULT_OFF = 0x11
    };

    enum class ProfilingType : uint8_t
    {
        INCREASE = 0x00,
        DECREASE = 0x01
    };

    enum class ConnectorType : uint8_t
    {
        DEFAULT = 0x00,
        Connector1 = 0x01,
        Connector2 = 0x02,
        Connector3 = 0x03,
        Connector4 = 0x04,
        Connector5 = 0x05,
        Connector6 = 0x06,
        Connector7 = 0x07,
        Connector8 = 0x08
    };

    struct ModuleStatus
    {
        bool isActive;
        bool isAlive;
        ConnectorType Connector;
        ChargingModuleState state;
        uint32_t moduleAddress;
        float MaxVoltage;
        float MaxCurrent;
        float MinVoltage;
        float MinCurrent;
        float MaxPower;
        float MinPower;
        float MaxTemperature;
        float MinTemperature;
        float PhaseAVoltage;
        float PhaseBVoltage;
        float PhaseCVoltage;
        float temperature;
        float inputVoltage;
        float inputCurrent;
        float outputVoltage;
        float outputCurrent;
        bool isFaultTriggered;
        bool isProfilingOngoing;
        ProfilingType ProfileType;
        std::array<bool, 24> faultBits;

        ModuleStatus() : isActive(false),
                         Connector(ConnectorType::DEFAULT),
                         state(ChargingModuleState::NORMAL_OFF),
                         PhaseAVoltage(0.0f),
                         PhaseBVoltage(0.0f),
                         PhaseCVoltage(0.0f),
                         temperature(0.0f),
                         inputVoltage(0.0f),
                         inputCurrent(0.0f),
                         outputVoltage(0.0f),
                         outputCurrent(0.0f),
                         isFaultTriggered(false),
                         isProfilingOngoing(false),
                         ProfileType(ProfilingType::INCREASE)
        {
            faultBits.fill(false);
        }
    };

    // C++ interface class with original methods
    class PowerModuleBase
    {
    public:
        virtual void Initialize() = 0;

        virtual bool ModuleTimingCommand(void) = 0;
        virtual bool ModuleStart(uint8_t moduleAddress, float voltage, float current) = 0;
        virtual bool BulkModuleStart(uint64_t Modules, float voltage, float current) = 0;

        virtual bool ModuleStop(uint8_t moduleAddress, float voltage, float current) = 0;
        virtual bool BulkModuleStop(uint64_t Modules, float voltage, float current) = 0;

        virtual bool ModuleParameters(uint8_t moduleAddress, float voltage, float current) = 0;
        virtual bool BulkModuleParameters(uint64_t Modules, float voltage, float current) = 0;

        virtual ModuleStatus GetModuleStatus(uint8_t moduleAddress) const = 0;

        virtual ~PowerModuleBase() {}
    };

} // namespace PowerModule

#endif /* INC_POWERMODULEBASE_H_ */