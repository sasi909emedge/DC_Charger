/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef INC_OCPPMODULE_H_
#define INC_OCPPMODULE_H_

#include <cstdint>
#include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <time.h>
#include "cJSON.h"
#include "ConfigModule.hpp"
#include "PLCModule.hpp"
#include "TimeModule.hpp"

#define CP_MESSAGEID_LENGTH 10
#define OCPP_REQUEST 2
#define OCPP_RESPONSE 3
#define OCPP_ERROR 4
#define setNULL(x) std::memset(x, '\0', sizeof(x))
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

#define Soft "Soft"
#define Hard "Hard"

#define Idle "Idle"
#define Uploaded "Uploaded"
#define UploadFailed "UploadFailed"
#define Uploading "Uploading"

#define Downloaded "Downloaded"
#define DownloadFailed "DownloadFailed"
#define Downloading "Downloading"
#define Idle "Idle"
#define InstallationFailed "InstallationFailed"
#define Installing "Installing"
#define Installed "Installed"

#define ConnectorLockFailure "ConnectorLockFailure"
#define EVCommunicationError "EVCommunicationError"
#define GroundFailure "GroundFailure"
#define HighTemperature "HighTemperature"
#define InternalError "InternalError"
#define LocalListConflict "LocalListConflict"
#define NoError "NoError"
#define OtherError "OtherError"
#define OverCurrentFailure "OverCurrentFailure"
#define PowerMeterFailure "PowerMeterFailure"
#define PowerSwitchFailure "PowerSwitchFailure"
#define ReaderFailure "ReaderFailure"
#define ResetFailure "ResetFailure"
#define UnderVoltage "UnderVoltage"
#define OverVoltage "OverVoltage"
#define WeakSignal "WeakSignal"
#define Emergency "Emergency"
#define EarthDisconnect "EarthDisconnect"
#define PowerLoss "PowerLoss"
#define WeldDetect "WeldDetect"

#define MeterValueSampleTime 5
#define SmartChargingSchedulePeriodCheckTime 30

#define ALIGNED_DATA true
#define SAMPLED_DATA false

#define OFFLINE_TRANSACTION true
#define ONLINE_TRANSACTION false

#define Available "Available"
#define Preparing "Preparing"
#define Charging "Charging"
#define SuspendedEVSE "SuspendedEVSE"
#define SuspendedEV "SuspendedEV"
#define Finishing "Finishing"
#define Reserved "Reserved"
#define Unavailable "Unavailable"
#define Faulted "Faulted"

#define Accepted "Accepted"
#define Pending "Pending"
#define Rejected "Rejected"
#define Scheduled "Scheduled"
#define Occupied "Occupied"
#define Inoperative "Inoperative"
#define Operative "Operative"

#define MeterValueSampleTime 5

#define POWERON_LED SOLID_WHITE
#define OTA_LED SOLID_ORANGE
#define COMMISSIONING_LED SOLID_ORANGE
#define STATION_CONNECTED_LED BLINK_WHITE
#define STATION_CONFIG_LED SOLID_WHITE

#define AVAILABLE_LED SOLID_GREEN
#define OFFLINE_AVAILABLE_LED SOLID_GREEN5_SOLID_WHITE1
#define PREPARING_LED BLINK_BLUE
#define OFFLINE_PREPARING_LED BLINK_BLUE5_SOLID_WHITE1
#define SUSPENDED_LED SOLID_BLUE
#define OFFLINE_SUSPENDED_LED SOLID_BLUE5_SOLID_WHITE1
#define CHARGING_LED BLINK_GREEN
#define OFFLINE_CHARGING_LED BLINK_GREEN5_SOLID_WHITE1
#define FINISHING_LED SOLID_GREEN3_SOLID_BLUE1
#define OFFLINE_FINISHING_LED SOLID_GREEN3_SOLID_BLUE1_SOLID_WHITE1
#define EMERGENCY_FAULT_LED BLINK_RED
#define OFFLINE_EMERGENCY_FAULT_LED BLINK_RED5_SOLID_WHITE1
#define FAULT_LED SOLID_RED
#define OFFLINE_FAULT_LED SOLID_RED5_SOLID_WHITE1
#define RESERVATION_LED SOLID_BLUE
#define OFFLINE_RESERVATION_LED SOLID_BLUE5_BLINK_WHITE1
#define UNAVAILABLE_LED SOLID_BLUE
#define OFFLINE_UNAVAILABLE_LED SOLID_BLUE5_BLINK_WHITE1
#define RFID_TAPPED_LED BLINK_WHITE

#define ONLINE_ONLY_AVAILABLE_LED SOLID_GREEN
#define ONLINE_ONLY_OFFLINE_AVAILABLE_LED BLINK_WHITE
#define ONLINE_ONLY_PREPARING_LED BLINK_BLUE
#define ONLINE_ONLY_OFFLINE_PREPARING_LED BLINK_WHITE
#define ONLINE_ONLY_SUSPENDED_LED SOLID_BLUE
#define ONLINE_ONLY_OFFLINE_SUSPENDED_LED SOLID_BLUE5_SOLID_WHITE1
#define ONLINE_ONLY_CHARGING_LED BLINK_GREEN
#define ONLINE_ONLY_OFFLINE_CHARGING_LED BLINK_GREEN5_SOLID_WHITE1
#define ONLINE_ONLY_FINISHING_LED SOLID_GREEN3_SOLID_BLUE1
#define ONLINE_ONLY_OFFLINE_FINISHING_LED SOLID_GREEN3_SOLID_BLUE1_SOLID_WHITE1
#define ONLINE_ONLY_EMERGENCY_LED BLINK_RED
#define ONLINE_ONLY_OFFLINE_EMERGENCY_LED BLINK_RED5_SOLID_WHITE1
#define ONLINE_ONLY_FAULT_LED SOLID_RED
#define ONLINE_ONLY_OFFLINE_FAULT_LED SOLID_RED5_SOLID_WHITE1
#define ONLINE_ONLY_RESERVATION_LED SOLID_BLUE
#define ONLINE_ONLY_OFFLINE_RESERVATION_LED SOLID_BLUE5_SOLID_WHITE1
#define ONLINE_ONLY_UNAVAILABLE_LED SOLID_BLUE
#define ONLINE_ONLY_OFFLINE_UNAVAILABLE_LED SOLID_BLUE5_SOLID_WHITE1

#define OFFLINE_ONLY_AVAILABLE_LED SOLID_GREEN
#define OFFLINE_ONLY_PREPARING_LED BLINK_BLUE
#define OFFLINE_ONLY_SUSPENDED_LED SOLID_BLUE
#define OFFLINE_ONLY_CHARGING_LED BLINK_GREEN
#define OFFLINE_ONLY_FINISHING_LED SOLID_GREEN3_SOLID_BLUE1
#define OFFLINE_ONLY_EMERGENCY_LED BLINK_RED
#define OFFLINE_ONLY_FAULT_LED SOLID_RED
#define OFFLINE_ONLY_RFID_TAPPED_LED SOLID_WHITE

namespace OCPPModule
{

#define LOCAL_LIST_COUNT 10
#define idTagLength 25           // 20 plus extra 5
#define OCPP_MESSAGEID_LENGTH 40 // 36 plus extra 4
#define SchedulePeriodCount 20

   enum class StopReasons : uint8_t
   {
      Stop_EmergencyStop = 0,
      Stop_EVDisconnected,
      Stop_HardReset,
      Stop_Local,
      Stop_Other,
      Stop_PowerLoss,
      Stop_Reboot,
      Stop_Remote,
      Stop_SoftReset,
      Stop_UnlockCommand,
      Stop_DeAuthorized,
   };

   enum class LED_STATE : uint8_t
   {
      INIT_STATE = 0,
      POWERON_LED_STATE,
      OTA_LED_STATE,
      COMMISSIONING_LED_STATE,
      AVAILABLE_LED_STATE,
      OFFLINE_AVAILABLE_LED_STATE,
      PREPARING_LED_STATE,
      OFFLINE_PREPARING_LED_STATE,
      SUSPENDED_LED_STATE,
      OFFLINE_SUSPENDED_LED_STATE,
      CHARGING_LED_STATE,
      OFFLINE_CHARGING_LED_STATE,
      FINISHING_LED_STATE,
      OFFLINE_FINISHING_LED_STATE,
      EMERGENCY_FAULT_LED_STATE,
      OFFLINE_EMERGENCY_FAULT_LED_STATE,
      FAULT_LED_STATE,
      OFFLINE_FAULT_LED_STATE,
      RESERVATION_LED_STATE,
      OFFLINE_RESERVATION_LED_STATE,
      UNAVAILABLE_LED_STATE,
      OFFLINE_UNAVAILABLE_LED_STATE,
      RFID_TAPPED_LED_STATE,
      ONLINE_ONLY_AVAILABLE_LED_STATE,
      ONLINE_ONLY_OFFLINE_AVAILABLE_LED_STATE,
      ONLINE_ONLY_PREPARING_LED_STATE,
      ONLINE_ONLY_OFFLINE_PREPARING_LED_STATE,
      ONLINE_ONLY_SUSPENDED_LED_STATE,
      ONLINE_ONLY_OFFLINE_SUSPENDED_LED_STATE,
      ONLINE_ONLY_CHARGING_LED_STATE,
      ONLINE_ONLY_OFFLINE_CHARGING_LED_STATE,
      ONLINE_ONLY_FINISHING_LED_STATE,
      ONLINE_ONLY_OFFLINE_FINISHING_LED_STATE,
      ONLINE_ONLY_EMERGENCY_LED_STATE,
      ONLINE_ONLY_OFFLINE_EMERGENCY_LED_STATE,
      ONLINE_ONLY_FAULT_LED_STATE,
      ONLINE_ONLY_OFFLINE_FAULT_LED_STATE,
      ONLINE_ONLY_RESERVATION_LED_STATE,
      ONLINE_ONLY_OFFLINE_RESERVATION_LED_STATE,
      ONLINE_ONLY_UNAVAILABLE_LED_STATE,
      ONLINE_ONLY_OFFLINE_UNAVAILABLE_LED_STATE,
      OFFLINE_ONLY_AVAILABLE_LED_STATE,
      OFFLINE_ONLY_PREPARING_LED_STATE,
      OFFLINE_ONLY_SUSPENDED_LED_STATE,
      OFFLINE_ONLY_CHARGING_LED_STATE,
      OFFLINE_ONLY_FINISHING_LED_STATE,
      OFFLINE_ONLY_EMERGENCY_LED_STATE,
      OFFLINE_ONLY_FAULT_LED_STATE,
      OFFLINE_ONLY_RFID_TAPPED_LED_STATE,
      EMERGENCY_LOGS_LED_STATE
   };

   namespace Constants
   {
      constexpr uint8_t MAX_MODULES = NUM_OF_CONNECTORS;
      constexpr uint8_t BOOT_RETRY_TIME = 60;
   }

   enum class PLC_ID : uint32_t
   {
      BootConfig = 0x90000,
      ChargeInfo = 0x00100,
      SoftwareInfo = 0x00101,
      ErrorCodes = 0x00102,
      HardwareStatus = 0x00103,
      SLACInfo = 0x00104,
      EVDCMaxLimits = 0x00200,
      EVDCChargeTargets = 0x00201,
      EVStatusDisplay = 0x00202,
      EVDCEnergyLimits = 0x00203,
      EVMacAddress = 0x00204,
      EVSEDCMaxLimits = 0x00300,
      EVSEDCRegulationLimits = 0x00301,
      EVSEDCStatus = 0x00302,
      EVSEPlugStatus = 0x00303,
      EVSEID = 0x00304,
      EVSEDateTime = 0x00305,
      HardwareConfig = 0x00306,
      EVSEACChargeControl = 0x00307,
      CME0 = 0x00000,
      CME1 = 0x01000,
      CME2 = 0x02000,
      CME3 = 0x03000,
      CME4 = 0x04000,
      CME5 = 0x05000,
      CME6 = 0x06000,
      CME7 = 0x07000,
      CME8 = 0x08000,
      CME9 = 0x09000
   };
   enum class TransactionContext : uint8_t
   {
      InterruptionBegin = 1,
      InterruptionEnd,
      SampleClock,
      SamplePeriodic,
      TransactionBegin,
      TransactionEnd,
      Trigger,
      Other,
   };

   enum class ProfileType : uint8_t
   {
      ChargePointMaxProfile = 1,
      TxDefaultProfile,
      TxProfile
   };

   struct CPBootNotificationRequest_t
   {
      char chargePointVendor[20];
      char chargePointModel[20];
      char chargePointSerialNumber[25];
      char chargeBoxSerialNumber[25];
      char firmwareVersion[70];
      char iccid[20];
      char imsi[20];
      // char meterType[25];
      // char meterSerialNumber[25];
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CPStatusNotificationRequest_t
   {
      uint32_t connectorId;
      char errorCode[25];
      // char info[50];
      char status[15];
      char timestamp[25];
      // char vendorId[255];
      char vendorErrorCode[25];
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CPAuthorizeRequest_t
   {
      char idTag[idTagLength];
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CMSCancelReservationRequest_t
   {
      uint32_t reservationId;
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSChangeAvailabilityRequest_t
   {
      uint8_t connectorId;
      char type[15];
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSChangeConfigurationRequest_t
   {
      char key[50];
      char value[200];
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSClearCacheRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSGetConfigurationRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSClearChargingProfileRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      uint32_t id;
      uint8_t connectorId;
      uint8_t chargingProfilePurpose;
      uint32_t stackLevel;
      bool Received;
   };

   struct CMSDataTransferRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSGetCompositeScheduleRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSGetDiagnosticsRequest_t
   {
      char location[100];
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSSetChargingProfileRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct ChargingProfiles_t
   {
      uint8_t connectorId;
      struct
      {
         uint32_t chargingProfileId;
         uint32_t transactionId;
         uint32_t stackLevel;
         uint8_t chargingProfilePurpose;
         uint8_t chargingProfileKind;
         uint8_t recurrencyKind;
         uint32_t validFrom;
         uint32_t validTo;
         struct
         {
            uint32_t duration;
            uint32_t startSchedule;
            uint8_t chargingRateUnit;
            struct
            {
               bool schedulePresent;
               uint32_t startPeriod;
               float limit;
               uint8_t numberPhases;
            } chargingSchedulePeriod[SchedulePeriodCount];
            float minChargingRate;
         } chargingSchedule;
      } csChargingProfiles;
   };

   struct CMSUnlockConnectorRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSGetLocalListVersionRequest_t
   {
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CPHeartbeatRequest_t
   {
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CPFirmwareStatusNotificationRequest_t
   {
      char status[20];
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CPDiagnosticsStatusNotificationRequest_t
   {
      char status[15];
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CMSTriggerMessageRequest_t
   {
      char requestedMessage[35];
      bool isConnectorIdReceived;
      uint8_t connectorId;
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CPStartTransactionRequest_t
   {
      bool reservationIdPresent;
      int connectorId;
      char idTag[idTagLength];
      int meterStart;
      int reservationId;
      char timestamp[25];
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CPStopTransactionRequest_t
   {
      char idTag[idTagLength]; // Including null terminator
      int meterStop;
      char timestamp[25]; // Including null terminator
      int transactionId;
      char reason[15]; // Adjust size based on the longest enum value
      // Define a structure for sampledValue, you can adapt based on your needs
      // For simplicity, I'm using a fixed-size array
      struct
      {
         char timestamp[28];
         struct
         {
            char value[15];
            char context[20];   // Adjust size based on the longest enum value
            char format[10];    // Adjust size based on the longest enum value
            char measurand[32]; // Adjust size based on the longest enum value
            char phase[5];      // Adjust size based on the longest enum value
            char location[7];   // Adjust size based on the longest enum value
            char unit[11];      // Adjust size based on the longest enum value
         } sampledValue[2];
      } transactionData[2]; // Assuming there are two elements in the array
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };

   struct CMSRemoteStartTransactionRequest_t
   {
      bool isConnectorIdReceived;
      int connectorId;
      char idTag[idTagLength]; // assuming a maximum length of 20 characters for idTag
      struct
      {
         int chargingProfileId;
         int transactionId;
         int stackLevel;
         char chargingProfilePurpose[50]; // adjust size accordingly
         char chargingProfileKind[20];    // adjust size accordingly
         char recurrencyKind[10];         // adjust size accordingly
         char validFrom[30];              // adjust size accordingly
         char validTo[30];                // adjust size accordingly
         struct
         {
            int duration;
            char startSchedule[30];   // adjust size accordingly
            char chargingRateUnit[2]; // adjust size accordingly
            struct
            {
               int startPeriod;
               float limit;
               int numberPhases;
            } chargingSchedulePeriod[SchedulePeriodCount]; // assuming a maximum of 2 periods
            float minChargingRate;
         } chargingSchedule;
      } chargingProfile;
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSRemoteStopTransactionRequest_t
   {
      int transactionId;
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSReserveNowRequest_t
   {
      int connectorId;
      char expiryDate[25];
      char idTag[idTagLength];
      bool isparentidTagReceived;
      char parentidTag[idTagLength];
      int reservationId;
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSResetRequest_t
   {
      char type[5];
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSUpdateFirmwareRequest_t
   {
      char location[256];
      int retries;
      char retrieveDate[25];
      int retryInterval;
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct CMSSendLocalListRequest_t
   {
      int listVersion;
      struct
      {
         bool idTagPresent;
         char idTag[idTagLength];
         struct
         {
            char expiryDate[25];
            char parentidTag[idTagLength];
            char status[13];
         } idTagInfo;
      } localAuthorizationList[LOCAL_LIST_COUNT]; // Assuming an array of size 2
      char updateType[13];
      char UniqId[OCPP_MESSAGEID_LENGTH];
      bool Received;
   };

   struct LocalAuthorizationList_t
   {
      char idTag[LOCAL_LIST_COUNT][20];
      bool idTagPresent[LOCAL_LIST_COUNT];
   };

   // SampledValue 0 - Energy
   // SampledValue 1 - Power
   // SampledValue 2 - voltage
   // SampledValue 3 - Current
   // SampledValue 4 - Temperature
   // SampledValue 5 - SoC
   // SampledValue 6 - voltage2
   // SampledValue 7 - Current2
   // SampledValue 8 - voltage3
   // SampledValue 9 - Current3
   // SampledValue 10 - PowerFactor

   struct CPMeterValuesRequest_t
   {
      int connectorId;
      int transactionId;
      struct
      {
         char timestamp[25]; // Assuming a fixed size for simplicity
         struct
         {
            char value[15]; // Assuming a fixed size for simplicity
            char context[20];
            char format[10];
            char measurand[32];
            char phase[5];
            char location[7];
            char unit[11];
         } sampledValue[11]; // Assuming a fixed size array for simplicity
      } meterValue;          // Assuming a fixed size array for simplicity
      char UniqId[CP_MESSAGEID_LENGTH];
      bool Sent;
   };
   /********************************************END Request Structures******************************************/
   /********************************************START Response Structures********************************************/
   struct CMSBootNotificationResponse_t
   {
      char status[10];
      char currentTime[25];
      uint32_t interval;
      bool Received;
   };

   struct CMSStatusNotificationResponse_t
   {
      bool Received;
   };

   struct CMSAuthorizeResponse_t
   {
      struct
      {
         bool expiryDateReceived;
         char expiryDate[25];
         bool parentidTagReceived;
         char parentidTag[idTagLength];
         char status[10];
         /* data */
      } idtaginfo;
      bool Received;
   };

   struct CPCancelReservationResponse_t
   {
      char status[10];
      bool Sent;
   };

   struct CPChangeAvailabilityResponse_t
   {
      char status[10];
      bool Sent;
   };

   struct CPChangeConfigurationResponse_t
   {
      char status[10];
      bool Sent;
   };

   struct CPClearCacheResponse_t
   {
      char status[10];
      bool Sent;
   };

   struct CPGetLocalListVersionResponse_t
   {
      bool Sent;
   };

   struct CMSHeartbeatResponse_t
   {
      char currentTime[25];
      bool Received;
   };

   struct CPUpdateFirmwareResponse_t
   {
      bool Sent;
      /* data */
   };

   struct CPGetDiagnosticsResponse_t
   {
      char fileName[255];
      bool Sent;
      /* data */
   };

   struct CMSFirmwareStatusNotificationResponse_t
   {
      bool Received;
      /* data */
   };

   struct CMSDiagnosticsStatusNotificationResponse_t
   {
      bool Received;
      /* data */
   };

   struct CPRemoteStartTransactionResponse_t
   {
      char status[10];
      bool Sent;
   };

   struct CPRemoteStopTransactionResponse_t
   {
      char status[10];
      bool Sent;
   };

   struct CPReserveNowResponse_t
   {
      char status[10];
      bool Sent;
      /* data */
   };

   struct CPResetResponse_t
   {
      char status[10];
      bool Sent;
      /* data */
   };

   struct CPSendLocalListResponse_t
   {
      char status[10];
      bool Sent;
      /* data */
   };

   struct Measurand_t
   {
      bool EnergyActiveExportRegister;
      bool EnergyActiveImportRegister;
      bool EnergyReactiveExportRegister;
      bool EnergyReactiveImportRegister;
      bool EnergyActiveExportInterval;
      bool EnergyActiveImportInterval;
      bool EnergyReactiveExportInterval;
      bool EnergyReactiveImportInterval;
      bool PowerActiveExport;
      bool PowerActiveImport;
      bool PowerOffered;
      bool PowerReactiveExport;
      bool PowerReactiveImport;
      bool PowerFactor;
      bool CurrentImport;
      bool CurrentExport;
      bool CurrentOffered;
      bool Voltage;
      bool Frequency;
      bool Temperature;
      bool SoC;
      bool RPM;
   };

   struct SupportedFeatureProfiles_t
   {
      bool Core;
      bool LocalAuthListManagement;
      bool Reservation;
      bool RemoteTrigger;
      bool FirmwareManagement;
      bool SmartCharging;
   };

   struct CPGetConfigurationResponse_t
   {
      // core profile
      bool AuthorizeRemoteTxRequestsReadOnly;
      bool ClockAlignedDataIntervalReadOnly;
      bool ConnectionTimeOutReadOnly;
      bool GetConfigurationMaxKeysReadOnly;
      bool HeartbeatIntervalReadOnly;
      bool LocalAuthorizeOfflineReadOnly;
      bool LocalPreAuthorizeReadOnly;
      bool MeterValuesAlignedDataReadOnly;
      bool MeterValuesSampledDataReadOnly;
      bool MeterValueSampleIntervalReadOnly;
      bool NumberOfConnectorsReadOnly;
      bool ResetRetriesReadOnly;
      bool ConnectorPhaseRotationReadOnly;
      bool StopTransactionOnEVSideDisconnectReadOnly;
      bool StopTransactionOnInvalidIdReadOnly;
      bool StopTxnAlignedDataReadOnly;
      bool StopTxnSampledDataReadOnly;
      bool SupportedFeatureProfilesReadOnly;
      bool TransactionMessageAttemptsReadOnly;
      bool TransactionMessageRetryIntervalReadOnly;
      bool UnlockConnectorOnEVSideDisconnectReadOnly;
      bool AllowOfflineTxForUnknownIdReadOnly;
      bool AuthorizationCacheEnabledReadOnly;
      bool BlinkRepeatReadOnly;
      bool LightIntensityReadOnly;
      bool MaxEnergyOnInvalidIdReadOnly;
      bool MeterValuesAlignedDataMaxLengthReadOnly;
      bool MeterValuesSampledDataMaxLengthReadOnly;
      bool MinimumStatusDurationReadOnly;
      bool ConnectorPhaseRotationMaxLengthReadOnly;
      bool StopTxnAlignedDataMaxLengthReadOnly;
      bool StopTxnSampledDataMaxLengthReadOnly;
      bool SupportedFeatureProfilesMaxLengthReadOnly;
      bool WebSocketPingIntervalReadOnly;
      // Local Auth List Management Profile
      bool LocalAuthListEnabledReadOnly;
      bool LocalAuthListMaxLengthReadOnly;
      bool SendLocalListMaxLengthReadOnly;
      // Reservation Profile
      bool ReserveConnectorZeroSupportedReadOnly;
      // Smart Charging Profile
      bool ChargeProfileMaxStackLevelReadOnly;
      bool ChargingScheduleAllowedChargingRateUnitReadOnly;
      bool ChargingScheduleMaxPeriodsReadOnly;
      bool ConnectorSwitch3to1PhaseSupportedReadOnly;
      bool MaxChargingProfilesInstalledReadOnly;

      // core profile
      char AuthorizeRemoteTxRequestsValue[10];
      char ClockAlignedDataIntervalValue[10];
      char ConnectionTimeOutValue[10];
      char GetConfigurationMaxKeysValue[10];
      char HeartbeatIntervalValue[10];
      char LocalAuthorizeOfflineValue[10];
      char LocalPreAuthorizeValue[10];
      char MeterValuesAlignedDataValue[100];
      char MeterValuesSampledDataValue[100];
      char MeterValueSampleIntervalValue[10];
      char NumberOfConnectorsValue[10];
      char ResetRetriesValue[10];
      char ConnectorPhaseRotationValue[20];
      char StopTransactionOnEVSideDisconnectValue[10];
      char StopTransactionOnInvalidIdValue[10];
      char StopTxnAlignedDataValue[100];
      char StopTxnSampledDataValue[100];
      char SupportedFeatureProfilesValue[100];
      char TransactionMessageAttemptsValue[10];
      char TransactionMessageRetryIntervalValue[10];
      char UnlockConnectorOnEVSideDisconnectValue[10];
      char AllowOfflineTxForUnknownIdValue[10];
      char AuthorizationCacheEnabledValue[10];
      char BlinkRepeatValue[10];
      char LightIntensityValue[10];
      char MaxEnergyOnInvalidIdValue[10];
      char MeterValuesAlignedDataMaxLengthValue[10];
      char MeterValuesSampledDataMaxLengthValue[10];
      char MinimumStatusDurationValue[10];
      char ConnectorPhaseRotationMaxLengthValue[10];
      char StopTxnAlignedDataMaxLengthValue[10];
      char StopTxnSampledDataMaxLengthValue[10];
      char SupportedFeatureProfilesMaxLengthValue[10];
      char WebSocketPingIntervalValue[10];
      // Local Auth List Management Profile
      char LocalAuthListEnabledValue[10];
      char LocalAuthListMaxLengthValue[10];
      char SendLocalListMaxLengthValue[10];
      // Reservation Profile
      char ReserveConnectorZeroSupportedValue[10];
      // Smart Charging Profile
      char ChargeProfileMaxStackLevelValue[10];
      char ChargingScheduleAllowedChargingRateUnitValue[10];
      char ChargingScheduleMaxPeriodsValue[10];
      char ConnectorSwitch3to1PhaseSupportedValue[10];
      char MaxChargingProfilesInstalledValue[10];

      bool AuthorizeRemoteTxRequests;
      uint32_t ClockAlignedDataInterval;
      uint32_t ConnectionTimeOut;
      uint32_t GetConfigurationMaxKeys;
      uint32_t HeartbeatInterval;
      bool LocalAuthorizeOffline;
      bool LocalPreAuthorize;
      Measurand_t MeterValuesAlignedData;
      Measurand_t MeterValuesSampledData;
      uint32_t MeterValueSampleInterval;
      uint32_t NumberOfConnectors;
      uint32_t ResetRetries;
      bool StopTransactionOnEVSideDisconnect;
      bool StopTransactionOnInvalidId;
      Measurand_t StopTxnAlignedData;
      Measurand_t StopTxnSampledData;
      SupportedFeatureProfiles_t SupportedFeatureProfiles;
      uint32_t TransactionMessageAttempts;
      uint32_t TransactionMessageRetryInterval;
      bool UnlockConnectorOnEVSideDisconnect;
      bool AllowOfflineTxForUnknownId;
      bool AuthorizationCacheEnabled;
      uint32_t BlinkRepeat;
      uint32_t LightIntensity;
      uint32_t MaxEnergyOnInvalidId;
      uint32_t MeterValuesAlignedDataMaxLength;
      uint32_t MeterValuesSampledDataMaxLength;
      uint32_t MinimumStatusDuration;
      uint32_t ConnectorPhaseRotationMaxLength;
      uint32_t StopTxnAlignedDataMaxLength;
      uint32_t StopTxnSampledDataMaxLength;
      uint32_t SupportedFeatureProfilesMaxLength;
      size_t WebSocketPingInterval;
      bool LocalAuthListEnabled;
      uint32_t LocalAuthListMaxLength;
      uint32_t SendLocalListMaxLength;
      bool ReserveConnectorZeroSupported;
      uint32_t ChargeProfileMaxStackLevel;
      uint32_t ChargingScheduleMaxPeriods;
      bool ConnectorSwitch3to1PhaseSupported;
      uint32_t MaxChargingProfilesInstalled;
   };

   struct CPTriggerMessageResponse_t
   {
      char status[10];
      bool Sent;
      /* data */
   };

   struct CMSStartTransactionResponse_t
   {

      struct
      {
         bool expiryDateReceived;
         char expiryDate[25];
         bool parentidTagReceived;
         char parentidTag[idTagLength];
         char status[13];

      } idtaginfo;
      uint32_t transactionId;
      bool Received;
   };

   struct CMSStopTransactionResponse_t
   {

      struct
      {
         bool expiryDateReceived;
         char expiryDate[25];
         bool parentidTagReceived;
         char parentidTag[idTagLength];
         char status[13];
         /* data */
      } idtaginfo;
      bool Received;
   };

   struct CMSMeterValuesResponse_t
   {
      bool Received;
      /* data */
   };

   // C++ interface class with original methods
   class OCPPController
   {
   public:
      OCPPController();
      void Initialize();

      // Call these instead of original methods
      bool SendData(std::string jsonData);
      bool ReceiveData(std::string jsonData);

      ~OCPPController();

      // functions
      void ocpp_response(const char *jsonString);
      void ocpp_struct_init(void);
      void sendHeartbeatRequest(void);
      void sendBootNotificationRequest(void);
      void sendStatusNotificationRequest(uint8_t connId);
      void sendAuthorizationRequest(uint8_t connId);
      void sendFirmwareStatusNotificationRequest(void);
      void sendDiagnosticsStatusNotificationRequest(void);
      void sendStartTransactionRequest(uint8_t connId);
      void sendStopTransactionRequest(uint8_t connId, TransactionContext context, bool Aligned, bool AC, bool offline);
      void sendMeterValuesRequest(uint8_t connId, TransactionContext context, bool Aligned, bool AC);
      void sendRemoteStartTransactionRequest(void);

      void CMSChangeAvailabilityRequestProcess(cJSON *jsonArray);
      void CMSChangeConfigurationRequestProcess(cJSON *jsonArray);
      void CMSCancelReservationRequestProcess(cJSON *jsonArray);
      void CMSFirmwareStatusNotificationRequestProcess(cJSON *jsonArray);
      void CMSTriggerMessageRequestProcess(cJSON *jsonArray);
      void CMSSendLocalListRequestProcess(cJSON *jsonArray);
      void CMSRemoteStartTransactionRequestProcess(cJSON *jsonArray);
      void CMSRemoteStopTransactionRequestProcess(cJSON *jsonArray);
      void CMSReserveNowRequestProcess(cJSON *jsonArray);
      void CMSResetRequestProcess(cJSON *jsonArray);
      void CMSUpdateFirmwareRequestProcess(cJSON *jsonArray);
      void CMSGetDiagnosticsRequestProcess(cJSON *jsonArray);
      void CMSSetChargingProfileRequestProcess(cJSON *jsonArray);
      void CMSClearChargingProfileRequestProcess(cJSON *jsonArray);

      void sendClearChargingProfileResponse(void);
      void sendDataTransferResponse(void);
      void sendGetCompositeScheduleResponse(void);
      void sendGetDiagnosticsResponse(void);
      void sendSetChargingProfileResponse(void);
      void sendUnlockConnectorResponse(void);
      void sendCancelReservationResponse(void);
      void sendChangeAvailabilityResponse(uint8_t connId);
      void sendChangeConfigurationResponse(void);
      void sendClearCacheResponse(void);
      void sendUpdateFirmwareResponse(void);
      void sendGetLocalListVersionResponse(void);
      void sendHeartbeatResponse(void);
      void sendRemoteStartTransactionResponse(uint8_t connId);
      void sendRemoteStopTransactionResponse(void);
      void sendReserveNowResponse(uint8_t connId);
      void sendResetResponse(void);
      void sendLocalListResponse(void);
      void sendTriggerMessageResponse(void);
      void StartTransactionResponse(void);
      void StopTransactionResponse(void);
      void sendGetConfigurationResponse(void);

      void BootNotificationResponseProcess(cJSON *bootResponse);
      void AuthorizationResponseProcess(cJSON *authorizationResponse, uint8_t connId);
      void CancelReservationResponseProcess(cJSON *CancelReservationResponseV);
      void ChangeAvailabilityResponseProcess(cJSON *ChangeAvailabilityResponseV);
      void ChangeConfigurationResponseProcess(cJSON *ChangeConfigurationResponseV);
      void ClearCacheResponseProcess(cJSON *ClearCacheResponseV);
      void HeartbeatResponseProcess(cJSON *HeartbeatResponseV);
      void RemoteStartTransactionResponseProcess(cJSON *RemoteStartTransactionV);
      void RemoteStopTransactionResponseProcess(cJSON *RemoteStartTransactionV);
      void ReserveNowResponseProcess(cJSON *ReserveNowResponseV);
      void ResetResponseProcess(cJSON *ResetResponseV);
      void SendLocalListResponseProcess(cJSON *SendLocalListResponseV);
      void TriggerMessageResponseProcess(cJSON *TriggerMessageResponseV);
      void StartTransactionResponseProcess(cJSON *StartTransactionResponseV, uint8_t connId);
      void StopTransactionResponseProcess(cJSON *StopTransactionResponseV, uint8_t connId);

      bool rfidMatchingLocalList(char *idTag);
      bool rfidMatchingAuthorizationCache(char *idTag);
      bool saveTagToLocalAuthenticationList(char *idTag);

      esp_err_t read_config_ocpp(void);
      esp_err_t write_config_ocpp(void);
      esp_err_t read_localist(void);
      esp_err_t write_localist(void);
      esp_err_t read_LocalAuthorizationList(void);
      esp_err_t write_LocalAuthorizationList(void);
      esp_err_t SetDefaultOcppConfig(void);

      uint64_t randomNumber = 1000;
      // EV Transmit messages
      CPBootNotificationRequest_t CPBootNotificationRequest;
      CPHeartbeatRequest_t CPHeartbeatRequest;
      CPStatusNotificationRequest_t CPStatusNotificationRequest[OCPPModule::Constants::MAX_MODULES];
      CPAuthorizeRequest_t CPAuthorizeRequest[OCPPModule::Constants::MAX_MODULES];
      CPStartTransactionRequest_t CPStartTransactionRequest[OCPPModule::Constants::MAX_MODULES];
      CPMeterValuesRequest_t CPMeterValuesRequest[OCPPModule::Constants::MAX_MODULES];
      CPStopTransactionRequest_t CPStopTransactionRequest[OCPPModule::Constants::MAX_MODULES];
      CPFirmwareStatusNotificationRequest_t CPFirmwareStatusNotificationRequest;
      CPDiagnosticsStatusNotificationRequest_t CPDiagnosticsStatusNotificationRequest;

      CMSRemoteStartTransactionRequest_t CMSRemoteStartTransactionRequest[OCPPModule::Constants::MAX_MODULES];
      CMSRemoteStopTransactionRequest_t CMSRemoteStopTransactionRequest;
      CMSCancelReservationRequest_t CMSCancelReservationRequest;
      CMSChangeAvailabilityRequest_t CMSChangeAvailabilityRequest[OCPPModule::Constants::MAX_MODULES];
      CMSChangeConfigurationRequest_t CMSChangeConfigurationRequest;
      CMSTriggerMessageRequest_t CMSTriggerMessageRequest;
      CMSReserveNowRequest_t CMSReserveNowRequest[OCPPModule::Constants::MAX_MODULES];
      CMSResetRequest_t CMSResetRequest;
      CMSUpdateFirmwareRequest_t CMSUpdateFirmwareRequest;
      CMSSendLocalListRequest_t CMSSendLocalListRequest;
      CMSClearCacheRequest_t CMSClearCacheRequest;
      CMSGetLocalListVersionRequest_t CMSGetLocalListVersionRequest;
      CMSGetConfigurationRequest_t CMSGetConfigurationRequest;
      CMSClearChargingProfileRequest_t CMSClearChargingProfileRequest;
      CMSDataTransferRequest_t CMSDataTransferRequest;
      CMSGetCompositeScheduleRequest_t CMSGetCompositeScheduleRequest;
      CMSGetDiagnosticsRequest_t CMSGetDiagnosticsRequest;
      CMSSetChargingProfileRequest_t CMSSetChargingProfileRequest;
      CMSUnlockConnectorRequest_t CMSUnlockConnectorRequest;

      CPRemoteStartTransactionResponse_t CPRemoteStartTransactionResponse[OCPPModule::Constants::MAX_MODULES];
      CPRemoteStopTransactionResponse_t CPRemoteStopTransactionResponse;
      CPReserveNowResponse_t CPReserveNowResponse;
      CPResetResponse_t CPResetResponse;
      CPSendLocalListResponse_t CPSendLocalListResponse;
      CPTriggerMessageResponse_t CPTriggerMessageResponse;
      CPCancelReservationResponse_t CPCancelReservationResponse;
      CPChangeAvailabilityResponse_t CPChangeAvailabilityResponse;
      CPChangeConfigurationResponse_t CPChangeConfigurationResponse;
      CPClearCacheResponse_t CPClearCacheResponse;
      CPGetLocalListVersionResponse_t CPGetLocalListVersionResponse;
      CPUpdateFirmwareResponse_t CPUpdateFirmwareResponse;
      CPGetDiagnosticsResponse_t CPGetDiagnosticsResponse;
      CPGetConfigurationResponse_t CPGetConfigurationResponse;

      CMSFirmwareStatusNotificationResponse_t CMSFirmwareStatusNotificationResponse;
      CMSDiagnosticsStatusNotificationResponse_t CMSDiagnosticsStatusNotificationResponse;
      CMSHeartbeatResponse_t CMSHeartbeatResponse;
      CMSBootNotificationResponse_t CMSBootNotificationResponse;
      CMSStatusNotificationResponse_t CMSStatusNotificationResponse[OCPPModule::Constants::MAX_MODULES];
      CMSMeterValuesResponse_t CMSMeterValuesResponse[OCPPModule::Constants::MAX_MODULES];
      CMSAuthorizeResponse_t CMSAuthorizeResponse[OCPPModule::Constants::MAX_MODULES];
      CMSStartTransactionResponse_t CMSStartTransactionResponse[OCPPModule::Constants::MAX_MODULES];
      CMSStopTransactionResponse_t CMSStopTransactionResponse[OCPPModule::Constants::MAX_MODULES];

      LocalAuthorizationList_t LocalAuthorizationList;

      void sendGetConfigurationResponseKeys(uint8_t count);
   };

} // namespace PLCModule

extern OCPPModule::OCPPController *ocpp;
#endif // __cplusplus
#endif /* INC_OCPPMODULE_H_ */