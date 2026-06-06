/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef INC_CONFIG_MODULE_H_
#define INC_CONFIG_MODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include "Settings.hpp"
#include "NVSModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

namespace ConfigModule
{
    enum class EVSECurrentLimit : uint16_t
    {
        SNA = 0x03FF,
        HLC = 2,
        Duty100 = 1,
        Duty0 = 0
    };

    enum class ChargerType : uint8_t
    {
        STANDALONE = 1,
        DISPENSER,
        STACK
    };
    enum class ChargerModel : uint8_t
    {
        DC30S = 1,
        DC60S,
        DC120S,
        DC180S,
        DC240S,
        DC60D,
        DC120D,
    };

    enum class BoardModel : uint8_t
    {
        DC1 = 1,
        DC2,
        DC3
    };
    enum class EthernetConfigType : uint8_t
    {
        STATIC = 0,
        DHCP
    };
    enum class NetworkMode : uint8_t
    {
        ONLINE = 0,
        OFFLINE,
        ONLINE_OFFLINE,
        PLUGNPLAY
    };

    enum class ChargingMode : uint8_t
    {
        DC = 1,
        DC_AC
    };

    struct PowerModuleConfig
    {
        bool isAvailable;
        uint8_t moduleAddress;
        float MaxVoltage;
        float MaxCurrent;
        float MinVoltage;
        float MinCurrent;
        float MaxPower;
        float MinPower;
        float MaxTemperature;
        float MinTemperature;
    };

    struct ChargerSubSet
    {
        uint8_t NumberOfConnectors;
        PowerModuleConfig PowerModule[4][2];
    };

    struct Config_t
    {
        uint32_t spare[100];
    };

    // C++ interface class with original methods
    class Configuration
    {
    public:
        Configuration();

        ~Configuration();

        bool defaultConfig;
        ChargerType chargerType;
        char serialNumber[20];
        char chargerName[20];
        char chargePointVendor[20];
        char chargePointModel[20];
        char commissionedBy[20];
        char commissionedDate[20];
        char simIMEINumber[20];
        char simIMSINumber[20];
        char webSocketURL[100];
        char firmwareVersion[30];
        char slavefirmwareVersion[30];
        char adminpassword[20];
        char factorypassword[20];
        char servicepassword[20];
        char customerpassword[20];

        bool wifiEnable;
        uint8_t wifiPriority;
        char wifiSSID[32];
        char wifiPassword[64];

        bool gsmEnable;
        uint8_t gsmPriority;
        char gsmAPN[50];

        bool ethernetEnable;
        uint8_t ethernetPriority;
        EthernetConfigType ethernetConfig;
        char ipAddress[20];
        char gatewayAddress[20];
        char dnsAddress[20];
        char subnetMask[20];
        char macAddress[25];

        NetworkMode networkMode;
        ChargingMode chargingMode;

        bool ResumeSessionAfterPowerLoss;
        bool OtaUrlFromCMSEnable;
        char OtaURLConfig[100];

        double CurrentGain;
        double CurrentOffset;
        double CurrentGain1;
        double CurrentGain2;

        bool restoreSessionFromFault;
        uint16_t restoreSessionFromFaultTime;
        ChargerModel chargerModel;
        BoardModel boardModel;

        uint8_t NumberOfDisplays;
        uint8_t NumberOfConnectors;
        ChargerSubSet NumberOfSubSets[MAX_NUM_OF_SUBSETS];
        bool SubSetRelay[MAX_NUM_OF_SUBSETS][3];
        bool SuperSetMux[MAX_NUM_OF_SUPERSETS][4];
        bool StackMux[4];

        double DCoverVoltageThreshold;
        double ACoverVoltageThreshold;
        double DCunderVoltageThreshold;
        double ACunderVoltageThreshold;
        double DCoverCurrentThreshold[MAX_NUM_OF_CONNECTORS + 1];
        double ACoverCurrentThreshold[MAX_NUM_OF_CONNECTORS + 1];
        double overTemperatureThreshold;

        bool smartCharging;
        bool BatteryBackup;

        bool DiagnosticServer;
        char DiagnosticServerUrl[100];

        uint8_t ByteMacAddress[6];

        bool StopTransactionInSuspendedState;
        uint16_t StopTransactionInSuspendedStateTime;
        bool ALPREnable;
        uint8_t NumOfAlprDevices;
        uint8_t macAddressofAlprDevice[4][6];
        uint8_t serverSetCpDuty[4];

        esp_err_t WriteConfigurationToFlash(void);
        esp_err_t ReadConfigurationFromFlash(void);

    private:
        void printConfigParameters();
        const char *GetChargerTypeString(ChargerType chargerType);
        const char *GetChargerModelString(ChargerModel chargerModel);
        const char *GetBoardModelString(BoardModel boardModel);
        const char *GetEthernetConfigTypeString(EthernetConfigType ethernetConfigType);
        const char *GetNetworkModeString(NetworkMode networkMode);
        const char *GetChargingModeString(ChargingMode chargingMode);
    };

} // namespace ConfigModule

extern ConfigModule::Configuration *config;

#endif // __cplusplus
#endif /* INC_CONFIG_MODULE_H_ */