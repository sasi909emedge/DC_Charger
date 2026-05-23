/*
 * PowerMuxModule.h
 *
 *  Created on: May 13, 2025
 *      Author: kishore
 */

#ifndef _INC_POWER_MUX_MODULE_H_
#define _INC_POWER_MUX_MODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>

#include "CCSConnector.hpp"
#include "PLCModule.hpp"
#include "OCPPModule.hpp"
#include "ConfigModule.hpp"
#include "TimeModule.hpp"
#include "TonhePowerModule.hpp"
#include "GpioModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

// C++ specific section begins here
namespace PowerMuxModule
{

    enum class RelayPowerFlowDirection : uint8_t
    {
        Unknown,
        DrawingPowerFromAdjuscent1Relay,
        DrawingPowerFromAdjuscent2Relays,
        SourcingPowerToAdjuscentRelays,
        RelayIsNotConnectedToAdjuscentRelay
    };

    typedef struct
    {
        uint16_t RelayNum;
        uint16_t SuperSetInternalMux1Num;
        uint16_t SuperSetInternalMux2Num;
        uint16_t StackMux1Num;
        uint16_t StackMux2Num;
        GpioModule::GpioState RelayNumStatus;
        GpioModule::GpioState SuperSetInternalMux1NumStatus;
        GpioModule::GpioState SuperSetInternalMux2NumStatus;
        GpioModule::GpioState StackMux1NumStatus;
        GpioModule::GpioState StackMux2NumStatus;
    } ConnectorMuxesRelays;

    typedef struct
    {
        uint8_t PmId;
        uint8_t subSetNum;
        uint16_t leftRelayNum;
        uint16_t rightRelayNum;
        uint16_t subSetAcContactorNum;
    } PowerModuleConnections;

    typedef struct
    {
        uint8_t Connector;
        uint8_t PmId[8]; // Max PMs For Connector: 30KW x 20 = 600KW Max
        uint8_t PmCount;
        uint8_t EVPower;    // Target Power
        uint8_t EVSEPower;  // Actual Power from EVSE
        uint32_t StartTime; // For Priority - connected time or charge start time changed to uint
    } ConnectorPowerModules;

    struct ModuleStatus
    {
        bool isActive;

        // Data from PowerMuxModule to ConnectorModule
        bool isParameterStateFinished;
        float EVSEMaxCurrent;
        float EVSEMaxVoltage;

        float EVSEMinCurrent;
        float EVSEMinVoltage;

        float EVSEPresentCurrent;
        float EVSEPresentVoltage;

        float EVSEMaxPower;

        // Data from ConnectorModule to PowerMuxModule
        PLCModule::ControlPilotState controlPilotState;
        PLCModule::StateMachineState stateMachineState;

        float EVMaxCurrent;
        float EVMaxVoltage;

        float EVTargetCurrent;
        float EVTargetVoltage;

        float EVMaxPower;
    };
    // Function pointer types
    typedef bool (*SendDataFunc)(const uint32_t id, const uint64_t data);
    // typedef bool (*ReceiveDataFunc)(const uint32_t id, const uint64_t data);

    // C++ interface class with original methods
    class PowerMuxController
    {
    public:
        PowerMuxController(SendDataFunc SendFunction);
        static void PowerMuxControllerTask(void *pvParameters);

        PLCModule::ControlPilotState controlPilotState_old[PLCModule::Constants::MAX_MODULES];
        PLCModule::StateMachineState stateMachineState_old[PLCModule::Constants::MAX_MODULES];

        bool StartPowerModule(uint8_t PmId);
        bool StopPowerModule(uint8_t PmId);
        bool StopDefaultPowerModules(uint8_t connId);
        bool DcMergerOff(uint8_t mergerId);
        bool DcMergerOn(uint8_t mergerId);

        ConnectorMuxesRelays GetConnectorMuxesRelays(uint8_t connId);
        PowerModule::ModuleStatus GetDefaultPowerModule(uint8_t connId, PowerModuleConnections *PmConnections);
        RelayPowerFlowDirection GetRelayPowerFlowDirection(uint8_t connId,
                                                           ConnectorMuxesRelays *ConnMuxRelay,
                                                           PowerModuleConnections *PmConnections,
                                                           PowerModule::ModuleStatus *PmStatus);
        uint8_t GetDestinationConnectorIdFromSuperSetInternalMuxId(uint8_t connId, uint16_t MuxId);
        uint8_t GetDestinationConnectorIdFromStackMuxId(uint8_t connId, uint16_t MuxId);
        bool isolateConnectorAndAssignDefaultPowerModule(uint8_t connId);
        bool isolateConnector(uint8_t SourceConnId, uint8_t DestinationConnId);
        void AssignPowerModulesAndRelays(void);
        void AssignExtraPowerModules(uint8_t connId);

        bool SubSetRelay[NUM_OF_SUBSETS][3];
        bool SuperSetMux[NUM_OF_SUPERSETS][4];
        bool StackMux[4];

        std::array<ModuleStatus, PLCModule::Constants::MAX_MODULES> moduleStatus;
        // Set function pointers
        void SetSendFunction(SendDataFunc func);
        // void SetReceiveFunction(ReceiveDataFunc func);

        bool SendEVSEMaxData(uint8_t ConnId);

        // Call these instead of original methods
        bool SendData(const uint32_t id, const uint64_t data);
        bool ReceiveData(const uint32_t id, const uint64_t data);

        bool IsConnectorIsolated(uint8_t connId);
        bool IsPowerModuleIsolated(uint8_t pmId);
        bool GetConnectorRelayPMs(uint8_t connId, uint16_t relayId, uint8_t pmId[2]);
        void UpdateConnectorPowerModules();
        bool sufficientPower(uint8_t connId);
        float AssignedPower(uint8_t connId);
        void optimizePowerModules();
        uint8_t PriorityConnector(uint8_t connId1, uint8_t connId2);
        bool AddPowerModulesToConnector(uint8_t connId);
        bool RemovePowerModulesFromConnector(uint8_t connId);

        // 2gun standalone
        bool AssignDefaultPowerModules(uint8_t connId);
        void AssignAvailablePowerModules(uint8_t connId);
        bool IsolatePowerModules(uint8_t pmId);
        bool IsolateDefaultPowerModules(uint8_t connId);
        void OptimizePowerModulesForStandAlone();
        uint8_t GetMergerId(uint8_t pmId, uint8_t connId);
        void GetConnections(uint8_t pmId, uint8_t &connId, uint8_t &leftMerger, uint8_t &rightMerger);

        ~PowerMuxController();

    private:
        SendDataFunc sendFunc;
        PowerModuleConnections powerModuleConnections[NUM_OF_POWER_MODULES];
        // ReceiveDataFunc receiveFunc;
    };

} // namespace PowerMuxModule

extern PowerMuxModule::PowerMuxController *pmm;
#endif // __cplusplus
#endif /* _INC_POWER_MUX_MODULE_H_ */