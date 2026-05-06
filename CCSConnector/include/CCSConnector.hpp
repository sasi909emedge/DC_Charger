/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef INC_CCS_CONNECTOR_H_
#define INC_CCS_CONNECTOR_H_

#include <cstdint>
#include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include "freertos/semphr.h"

#include "PLCModule.hpp"
#include "OCPPModule.hpp"
#include "ConfigModule.hpp"
#include "TimeModule.hpp"
#include "EnergyModule.hpp"
#include "GpioModule.hpp"
#include "NetworkModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

#define WEAK_SIGNAL_TRIGGER 1
#define READER_FAILURE_TRIGGER 2

namespace CCSConnector
{
    namespace Constants
    {
        constexpr uint16_t TASK_DELAY_TIME = 100; // in milliseconds
        constexpr uint8_t MAX_MODULES = NUM_OF_CONNECTORS;
    }

    enum class ConnectorMode : uint8_t
    {
        AC = 0,
        DC = 1
    };

    enum class PowerMuxCanId : uint32_t
    {
        EVSEMAX = 0x00100000,
        EVMAX = 0x00200000,
        EVTARTGET = 0x00300000
    };

    struct EVSEMAX
    {
        uint64_t ConnectorId : 8;
        uint64_t EVSEMaxCurrent : 16;
        uint64_t EVSEMaxVoltage : 16;
        uint64_t Spare : 24;
    };

    struct EVMAX
    {
        uint64_t ConnectorId : 8;
        uint64_t EVMaxCurrent : 16;
        uint64_t EVMaxVoltage : 16;
        uint64_t controlPilotState : 4;
        uint64_t stateMachineState : 4;
        uint64_t Spare : 16;
    };

    struct EVTARTGET
    {
        uint64_t ConnectorId : 8;
        uint64_t EVTargetCurrent : 16;
        uint64_t EVTargetVoltage : 16;
        uint64_t controlPilotState : 4;
        uint64_t stateMachineState : 4;
        uint64_t Spare : 16;
    };

    struct Reserved_t
    {
        char idTag[25];
        char expiryDate[25];
        char parentidTag[25];
        int reservationId;
    };

    class Display
    {
    public:
        SemaphoreHandle_t counting_semaphore;
        bool INITIALIZING;
        bool RFID_TAPPED;
        bool AUTH_SUCCESS_PLUG_EV;
        bool AUTH_FAILED;
        bool AUTH_SUCCESS;
        bool FIRMWARE_UPDATE;
        bool COMMISSIONING;
        bool NETWORKSWITCH;
        bool AVAILABLE[NUM_OF_CONNECTORS];
        bool UNAVAILABLE[NUM_OF_CONNECTORS];
        bool CHARGING[NUM_OF_CONNECTORS];
        bool FINISHING[NUM_OF_CONNECTORS];
        bool EV_PLUGGED_TAP_RFID[NUM_OF_CONNECTORS];
        bool FAULT[NUM_OF_CONNECTORS];
        bool RESERVED[NUM_OF_CONNECTORS];
        bool SUSPENDED[NUM_OF_CONNECTORS];
        void Set_INITIALIZING(void);
        void Set_RFID_TAPPED(void);
        void Set_AUTH_SUCCESS_PLUG_EV(void);
        void Set_AUTH_FAILED(void);
        void Set_AUTH_SUCCESS(void);
        void Set_FIRMWARE_UPDATE(void);
        void Set_COMMISSIONING(void);
        void Set_NETWORKSWITCH(void);
        void Set_AVAILABLE(uint8_t ConnID);
        void Set_UNAVAILABLE(uint8_t ConnID);
        void Set_CHARGING(uint8_t ConnID);
        void Set_FINISHING(uint8_t ConnID);
        void Set_EV_PLUGGED_TAP_RFID(uint8_t ConnID);
        void Set_FAULT(uint8_t ConnID);
        void Set_RESERVED(uint8_t ConnID);
        void Set_SUSPENDED(uint8_t ConnID);
    };

    class Authentication
    {
    public:
        bool isRfidTappedFirst;
        bool Rfid;
        bool Alpr;
        bool Alpr_old;
        bool Rfid_old;
        bool RfidTappedLed;
        bool RfidAuthorized;
        bool TransactionAuthorized[NUM_OF_CONNECTORS];
        uint16_t TransactionResTimeout[NUM_OF_CONNECTORS];
        bool RemoteAuthorized[NUM_OF_CONNECTORS];
        bool ALPRAuthorized[NUM_OF_CONNECTORS];
        char Rfid_tag[25];
        char Remote_tag[25];
        char Alpr_tag[25];
    };

    struct MeterValueAlignedData_t
    {
        uint16_t temp;
        double voltage[4];
        double current[4];
        double power[4];
        double Energy;
        double meterStop;
    };

    struct ModuleStatus
    {
        bool isTransactionOngoing;
        bool offlineStarted;
        bool isReserved;
        bool CmsAvailableScheduled;
        bool CmsAvailable;
        bool CmsAvailableChanged;
        bool UnkownId;
        bool InvalidId;

        PLCModule::ControlPilotState controlPilotState;
        PLCModule::StateMachineState stateMachineState;
        PLCModule::ControlPilotDuty controlPilotDuty;
        OCPPModule::StopReasons stopReason;
        uint32_t transactionId;
        uint32_t chargingDuration;

        double meterStart;
        double meterStop;

        double DCmeterStart;
        double DCmeterStop;

        double ACmeterStart;
        double ACmeterStop;

        ConnectorMode mode;
        EnergyModule::MeterValueAC ACMeterValues;
        EnergyModule::MeterValueDC DCMeterValues;
        double Energy;
        Reserved_t ReservedData;
        EnergyModule::MeterValueAC ACMeterValuesFault;
        EnergyModule::MeterValueDC DCMeterValuesFault;
        uint64_t EVMacAddress;
        char idTag[25];
        char meterStart_time[50];
        char meterStop_time[50];
    };

    // Function pointer types
    typedef bool (*SendDataFunc)(const uint32_t id, const uint64_t data);
    // typedef bool (*ReceiveDataFunc)(const uint32_t id, const uint64_t data);

    class CCSConnectorController : public Display, Authentication
    {
    public:
        bool isChargerBooted;
        bool isBootRejected;
        bool bootedNow;
        bool isPoweredOn;
        bool isWifiConnected;
        bool isWebsocketConnected;
        bool isWebsocketConnected_old;
        bool ClockAlignedDataTime[NUM_OF_CONNECTORS];
        bool isPowerModuleAssigned;

        bool emergency;
        bool earthDisconnect;
        bool gfci[NUM_OF_CONNECTORS];
        bool relayWeld[NUM_OF_CONNECTORS];
        bool overVoltage[NUM_OF_CONNECTORS];
        bool underVoltage[NUM_OF_CONNECTORS];
        bool overCurrent[NUM_OF_CONNECTORS];
        bool overTemp[NUM_OF_CONNECTORS];
        bool powerLoss;
        bool connectorFault[NUM_OF_CONNECTORS];
        bool readerFailure;

        bool emergency_old[NUM_OF_CONNECTORS];
        bool earthDisconnect_old[NUM_OF_CONNECTORS];
        bool gfci_old[NUM_OF_CONNECTORS];
        bool relayWeld_old[NUM_OF_CONNECTORS];
        bool overVoltage_old[NUM_OF_CONNECTORS];
        bool underVoltage_old[NUM_OF_CONNECTORS];
        bool overCurrent_old[NUM_OF_CONNECTORS];
        bool overTemp_old[NUM_OF_CONNECTORS];
        bool powerLoss_old[NUM_OF_CONNECTORS];
        bool connectorFault_old[NUM_OF_CONNECTORS];

        bool updatingFirmwarePending;
        uint32_t ChargingFaultTimeCount[NUM_OF_CONNECTORS];
        uint32_t ClockAlignedDataCount[NUM_OF_CONNECTORS];
        uint8_t NumberOfConnectors;
        MeterValueAlignedData_t MeterValueAlignedData[NUM_OF_CONNECTORS];
        bool FirmwareUpdateFailed;
        bool FirmwareUpdateStarted;
        bool finishingStatusSent[NUM_OF_CONNECTORS];
        bool finishingStatusPending[NUM_OF_CONNECTORS];
        bool updatingDataToCms[NUM_OF_CONNECTORS];
        bool isConnectorCharging[NUM_OF_CONNECTORS];
        OCPPModule::LED_STATE ledState[NUM_OF_CONNECTORS];

        PLCModule::ControlPilotState controlPilotState_old[NUM_OF_CONNECTORS];
        PLCModule::ControlPilotDuty controlPilotDuty_old[NUM_OF_CONNECTORS];
        PLCModule::StateMachineState stateMachineState_old[NUM_OF_CONNECTORS];

        CCSConnectorController(SendDataFunc sendFunction);

        void Initialize();
        static void ConnectorTask(void *pvParameters);

        void updateMeterValues(uint8_t ConnID, bool Aligned, bool AC);
        static void OcppTask(void *pvParameters);
        static void AuthorizeTask(void *pvParameters);

        void updateStopStansactionReason(uint8_t connId, ModuleStatus moduleStatus);
        void updateStopReason(uint8_t connId);
        void ProcessStateMachineStateSNA(uint8_t ConnID);
        void ProcessStateMachineStateDefault(uint8_t ConnID);
        void ProcessStateMachineStateInit(uint8_t ConnID);
        void ProcessStateMachineStateAuthentication(uint8_t ConnID);
        void ProcessStateMachineStateParameter(uint8_t ConnID);
        void ProcessStateMachineStateIsolation(uint8_t ConnID);
        void ProcessStateMachineStatePreCharge(uint8_t ConnID);
        void ProcessStateMachineStateCharge(uint8_t ConnID);
        void ProcessStateMachineStateWelding(uint8_t ConnID);
        void ProcessStateMachineStateStopCharge(uint8_t ConnID);
        void ProcessStateMachineStateSessionStop(uint8_t ConnID);
        void ProcessStateMachineStateShutOff(uint8_t ConnID);
        void ProcessStateMachineStatePaused(uint8_t ConnID);
        void ProcessStateMachineStateError(uint8_t ConnID);

        esp_err_t readConnectorModuleStatus(uint8_t ConnID);
        esp_err_t writeConnectorModuleStatus(uint8_t ConnID);
        esp_err_t read_connectors_offline_data(uint8_t ConnID);
        esp_err_t write_connectors_offline_data(uint8_t ConnID);

        void encodeAndSendStatusNotification(uint8_t connId, char *errorCode, char *status, char *info, uint8_t trigger);
        void sendStatusNotificationRequest(uint8_t ConnID, std::string errorCode, std::string status, std::string vendorErrorCode);
        CCSConnector::ModuleStatus GetModuleStatus(uint8_t ConnId);
        void SetModuleStatusToSNA(uint8_t ConnID);

        ~CCSConnectorController();

        bool SendEVMaxData(uint8_t ConnId);
        bool SendEVTargetData(uint8_t ConnId);

        // Set function pointers
        void SetSendFunction(SendDataFunc func);
        // void SetReceiveFunction(ReceiveDataFunc func);

        // Call these instead of original methods
        bool SendData(const uint32_t id, const uint64_t data);
        bool ReceiveData(const uint32_t id, const uint64_t data);

        std::array<CCSConnector::ModuleStatus, Constants::MAX_MODULES> moduleStatus;

    private:
        SendDataFunc sendFunc;
        void PrintConnectorModuleStatusParameters(uint8_t ConnID);
        void PrintConnectorParameters();
        // ReceiveDataFunc receiveFunc;
    };

} // namespace PLCModule

extern CCSConnector::CCSConnectorController *connector;
#endif // __cplusplus
#endif /* INC_CCS_CONNECTOR_H_ */