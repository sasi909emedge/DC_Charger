/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef INC_PLCMODULE_H_
#define INC_PLCMODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <cstring>
#include <stdexcept>
#include "ConfigModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

namespace PLCModule
{
   namespace Constants
   {
      constexpr double VOLTAGE_SCALE = 0.1;
      constexpr double CURRENT_SCALE = 0.1;
      constexpr double CURRENT_REGULATION_SCALE = 0.2;
      constexpr double POWER_SCALE = 100.0;
      constexpr double ENERGY_SCALE = 100.0;
      constexpr double SOC_SCALE = 0.5;
      constexpr double CURRENT_OFFSET = 3250.0f;
      constexpr uint8_t MAX_MODULES = NUM_OF_CONNECTORS + 1;
      constexpr uint8_t MeasuredAttenuation_SNA = (0xFF);
      constexpr uint8_t EVSoC_SNA = (0xFF);
      constexpr uint8_t AliveCounter_SNA = (0xF);
      constexpr uint8_t EVBulkSoC_SNA = (0xFF);
      constexpr uint8_t Old_bootloader_SNA = (0xFF);
      constexpr uint8_t EVSECurrentRegulationTolerance_SNA = (0xFF);
      constexpr uint8_t EVFullSoC_SNA = (0xFF);
      constexpr uint8_t EVSEPeakCurrentRipple_SNA = (0xFF);
      constexpr uint16_t ProximityPilotVoltage_SNA = (0xFFF);
      constexpr uint16_t ControlPilotVoltage_SNA = (0x3FFF);
      constexpr uint16_t EVACMaxCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVSEACMaxCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVACMaxVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVACMinCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVEAmount_SNA = (0xFFFF);
      constexpr uint16_t EVTargetCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVTargetVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVEnergyRequest_SNA = (0xFFFF);
      constexpr uint16_t EVEnergyCapacity_SNA = (0xFFFF);
      constexpr uint16_t EVSEPresentCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVSEPresentVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVSEMinVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVSENominalVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVSEMinCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVSEEnergyToBeDelivered_SNA = (0xFFFF);
      constexpr uint16_t EVSEMaxPower_SNA = (0xFFFF);
      constexpr uint16_t EVSEMaxVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVSEMaxCurrent_SNA = (0xFFFF);
      constexpr uint16_t EVPreChargeVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVMaxVoltage_SNA = (0xFFFF);
      constexpr uint16_t EVMaxPower_SNA = (0xFFFF);
      constexpr uint16_t EVMaxCurrent_SNA = (0xFFFF);
      constexpr uint32_t EVTimeToFullSoC_SNA = (0x3FFFF);
      constexpr uint32_t EVTimeToBulkSoC_SNA = (0x3FFFF);
      constexpr uint32_t EVSEID_SNA = (0xDCBAABCD);
   }

   enum class Severity : uint8_t
   {
      Minor,
      Major,
      Critical
   };

   enum class Interface : uint8_t
   {
      Internal,
      Customer
   };

   enum class Category : uint8_t
   {
      CP_PP,
      EEPROM,
      CAN,
      SLAC_SVC,
      SDP,
      RESERVED_5,
      PILOT_SM,
      RESERVED_7,
      HLC_SVC,
      WATCHDOG,
      IP_STACK
   };

   typedef struct
   {
      Category category;
      Severity severity;
      Interface interface;
      uint8_t errorcode;
   } ErrorCode;

   enum class EC_CP_PP : uint8_t
   {
      UNPLUGGED,
      RESET_ERROR_CODE,
      ADC_READING_ERROR,
      PWM_READING_ERROR,
      CP_PP_STATE_ERROR_FAILED,
      T_CONN_MAX_COMM_TIMER_EXPIRED
   };

   enum class EC_EEPROM : uint8_t
   {
      Invalid,
      EEPROM_FAILED,
      EEPROM_WRITE_FAILED,
      INVALID_MAC_ADDRESS
   };

   enum class EC_CAN : uint8_t
   {
      INVALID,
      STATE_MACHINE_STATE_RANGE_OVERFLOW,
      PROXIMITY_PIN_STATE_RANGE_OVERFLOW,
      ACTUAL_CHARGE_PROTOCOL_RANGE_OVERFLOW,
      TCP_STATUS_RANGE_OVERFLOW,
      CONTROL_PILOT_DUTY_CYCLE_RANGE_OVERFLOW,
      CONTROL_PILOT_STATE_RANGE_OVERFLOW,
      EV_COMPATABLE_RANGE_OVERFLOW,
      DIODE_PRESENCE_RANGE_OVERFLOW,
      SLAC_STATE_RANGE_OVERFLOW,
      LINK_STATUS_RANGE_OVERFLOW,
      MEASUREMENT_ATTENUATION_RANGE_OVERFLOW,
      EV_MAX_CURRENT_RANGE_OVERFLOW,
      EV_MAX_VOLTAGE_RANGE_OVERFLOW,
      EV_MAX_POWER_RANGE_OVERFLOW,
      EV_FULL_SOC_RANGE_OVERFLOW,
      EV_BULK_SOC_RANGE_OVERFLOW,
      EV_TARGET_CURRENT_RANGE_OVERFLOW,
      EV_TARGET_VOLTAGE_RANGE_OVERFLOW,
      EV_PRE_CHARGE_VOLTAGE_RANGE_OVERFLOW,
      EV_SOC_RANGE_OVERFLOW,
      EV_CHARGING_COMPLETE_RANGE_OVERFLOW,
      EV_BULK_CHARGING_COMPLETE_RANGE_OVERFLOW,
      EV_CABIN_CONDITIONING_RANGE_OVERFLOW,
      EV_ERROR_CODE_RANGE_OVERFLOW,
      EV_TIME_TO_BULK_SOC_RANGE_OVERFLOW,
      EV_TIME_TO_FULL_SOC_RANGE_OVERFLOW,
      EV_RESS_CONDITIONING_RANGE_OVERFLOW,
      EV_READY_RANGE_OVERFLOW,
      EV_ENERGY_CAPACITY_RANGE_OVERFLOW,
      EV_ENERGY_REQUEST_RANGE_OVERFLOW,
      EV_MAC_ADDRESS_RANGE_OVERFLOW,
      EVSE_STATUS_CODE_SNA_ERROR,
      EVSE_MAX_CURRENT_SNA_ERROR,
      EVSE_MAX_VOLTAGE_SNA_ERROR,
      EVSE_MIN_CURRENT_SNA_ERROR,
      EVSE_MIN_VOLTAGE_SNA_ERROR,
      EVSE_PEAK_CURRENT_RIPPLE_SNA_ERROR,
      EVSE_PRESENT_VOLTAGE_SNA_ERROR,
      EVSE_PRESENT_CURRENT_SNA_ERROR,
      EVSE_VOLTAGE_LIMIT_ACHIEVED_SNA_ERROR,
      EVSE_CURRENT_LIMIT_ACHIEVED_SNA_ERROR,
      EVSE_POWER_LIMIT_ACHIEVED_SNA_ERROR,
      EVSE_MAX_POWER_SNA_ERROR,
      DO_NOT_UPDATE_CME_ID_WHEN_FRAMETYPE_IS_STANDARD,
      STANDARD_FRAMETYPE_NOT_POSSIBLE_WHEN_CME_ID_IS_NOT_ZERO,
      PILOT_STATES_STATUS_RANGE_OVERFLOW,
      HLC_STATUS_RANGE_OVERFLOW,
      V2G_STATUS_DIN_RANGE_OVERFLOW,
      V2G_STATUS_DC_ISO_RANGE_OVERFLOW,
      SLAC_STATUS_RANGE_OVERFLOW,
      PROXIMITY_PILOT_VOLTAGE_RANGE_OVERFLOW,
      EV_AC_MIN_CURRENT_RANGE_OVERFLOW,
      EV_AC_MAX_CURRENT_RANGE_OVERFLOW,
      EV_AC_MAX_VOLTAGE_RANGE_OVERFLOW,
      EV_AC_EAMOUNT_RANGE_OVERFLOW,
      V2G_STATUS_AC_ISO_RANGE_OVERFLOW,
      EVSE_AC_NOMINAL_VOLTAGE_SNA_ERROR,
      EVSE_AC_MAX_CURRENT_SNA_ERROR,
      EV_ENERGY_TRANSFER_MODE_RANGE_OVERFLOW,
   };

   enum class EC_SLAC_SVC : uint8_t
   {
      Invalid,
      TRAFFIC_GENERATION_STATE_RUNNING,
      QCA_SLACCONFIG_RANGE_OVERFLOW
   };

   enum class EC_SDP : uint8_t
   {
      Invalid,
      SDP_ERROR,
      SDP_REQUEST_INVALID,
      SDP_REQUEST_TIMEOUT
   };

   enum class EC_PILOT_SM : uint8_t
   {
      Invalid,
      ERROR,
      CP_NOT_B_OR_C_OR_D_DURING_HLC,
      TCP_NOT_CONNECTED_AFTER_INIT,
      PP_TYPE1_BUTTON_PRESSED_OR_INVALID,
      HLC_CHARGING_FAILED,
   };

   enum class EC_HLC_SVC : uint8_t
   {
      Invalid,
      EMERGENCY,
      SOCKET_FAIL,
      PORT_BIND_FAIL,
      SOCKET_START_LISTENING_FAILED,
      CLIENT_ERROR,
      MESSAGE_SEND_FAILED,
      SOCKET_DISCONNECT,
      SOCKET_FREE,
      CLIENT_DISCONNECTED,
   };

   enum class EC_WATCHDOG : uint8_t
   {
      Invalid,
      CAUSE_UNKNOWN,
      SM,
      PILOT,
      QCATIMER,
      QCATASK,
      CANTX,
      CANRX,
      IP,
      UNKNOWN_JUST_IRQ,
   };

   enum class EC_IP_STACK : uint8_t
   {
      Invalid,
      INIT_FAILED,
      SEND_FAILED,
      RECV_FAILED,
      INT_ERROR,
      WRONG_MAC_ADDRESS,
   };

   enum class PLC_ID : uint32_t
   {
      ChargeInfo = 0x00100,
      SoftwareInfo,
      ErrorCodes,
      HardwareStatus,
      SLACInfo,
      DebugInfo,

      EVDCMaxLimits = 0x00200,
      EVDCChargeTargets,
      EVStatusDisplay,
      EVDCEnergyLimits,
      EVMacAddress,
      EVACChargeControl,
      EVEMAID0,
      EVEMAID1,

      EVSEDCMaxLimits = 0x00300,
      EVSEDCRegulationLimits = 0x00301,
      EVSEStatus = 0x00302,
      EVSEPlugStatus = 0x00303,
      EVSEDateTime = 0x00305,
      HardwareConfig = 0x00306,
      EVSEACChargeControl = 0x00307,

      RTTLog = 0x00400,
      BootConfig = 0x90000
   };

   enum class ACFallBackStatus : uint8_t
   {
      Disabled = 0,
      Enabled
   };

   enum class ActualChargeProtocol : uint8_t
   {
      SNA = 15,
      DIN70121 = 1,
      ISO15118 = 2,
      Not_Supported = 3,
      Not_Defined = 0
   };

   enum class CerValidationStatus : uint8_t
   {
      ValidationSuccessful,
      ValidationFailed,
      CertificateExpired,
      NoValidSigner,
      SNA = 0x07
   };

   enum class ConfigGPIO : uint8_t
   {
      SNA = 2,
      HIGH = 1,
      LOW = 0
   };

   enum class ControlPilotDuty : uint8_t
   {
      Invalid = 0x7E,
      SNA
   };

   enum class ControlPilotState : uint8_t
   {
      A,
      B,
      C,
      D,
      E,
      F,
      Invalid = 0xE,
      SNA
   };

   enum class DiodePresence : uint8_t
   {
      Diode_Detected,
      No_Diode_Detected,
      SNA = 3
   };

   enum class EmaidValidationStatus : uint8_t
   {
      ValidationSuccessful,
      ValidationFailed,
      SNA = 3
   };

   enum class EVBulkChargingComplete : uint8_t
   {
      False,
      True,
      SNA = 3,
   };

   enum class EVCabinConditioning : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class EVChargingComplete : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class EVErrorCode : uint8_t
   {
      NO_ERROR,
      FAILED_RESSTemperatureInhibit,
      FAILED_EVShiftPosition,
      FAILED_ChargerConnectorLockFault,
      FAILED_EVRESSMalfunction,
      FAILED_ChargingCurrentdiff,
      FAILED_ChargingVoltageOutOfRange,
      Reserved_A,
      Reserved_B,
      Reserved_C,
      FAILED_SystemIncompatibility,
      NoData,
      SNA = 15
   };

   enum class EVReady : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class EVRequestedETM : uint8_t
   {
      AC_Single_Phase_Core,
      AC_Three_Phase_Core,
      DC_Core,
      DC_Extended,
      DC_Combo_Core,
      DC_Unique,
      SNA = 0x07
   };

   enum class EVRESSConditioning : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class EVSECurrentLimit : uint16_t
   {
      DutyCycle_0_Percent,
      DutyCycle_100_Percent,
      HLC,
      SNA = 0x03FF
   };

   enum class EVSECurrentLimitAchieved : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class EVSEIsolationStatus : uint8_t
   {
      Invalid,
      Valid,
      Warning,
      Fault,
      No_IMD,
      SNA = 7,
   };

   enum class EVSEPowerLimitAchieved : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class EVSEProcessing : uint8_t
   {
      Finished,
      Ongoing,
      WaitingForCustomerInteraction,
      SNA = 3,
   };

   enum class ProximityPinState : uint8_t
   {
      Not_Connected,
      Type2_Connected13A,
      Type2_Connected20A,
      Type2_Connected32A,
      Type2_Connected63A,
      Type2_Connected,
      Type1_Connected,
      Type1_Connected_Button_Pressed,
      Invalid = 0xE,
      SNA
   };

   enum class EVSEStatusCode : uint8_t
   {
      EVSE_NotReady,
      EVSE_Ready,
      EVSE_Shutdown,
      EVSE_UtilityInterruptEvent,
      EVSE_IsolationMonitoringActive,
      EVSE_EmergencyShutdown,
      EVSE_Malfunction,
      Reserved_8,
      Reserved_9,
      Reserved_A,
      Reserved_B,
      Reserved_C,
      EVSE_CustomerNotReady,
      Invalid = 0x0E,
      SNA
   };

   enum class EVSESupportedETM : uint8_t
   {
      AC_Single_Phase_Core,
      AC_Three_Phase_Core,
      Reserved_2,
      DC_Extended,
      Reserved_4,
      Reserved_5,
      AC_Single_and_Three_Phase_Core,
      SNA
   };

   enum class EVSEVoltageLimitAchieved : uint8_t
   {
      False,
      True,
      SNA = 3
   };

   enum class GPIOStatus : uint8_t
   {
      LOW,
      HIGH,
      SNA,
   };

   enum class PilotStatesStatus : uint8_t
   {
      CP_PP_STATE_INVALID,
      CP_PP_STATE_INIT,
      CP_PP_STATE_UNPLUGGED,
      CP_PP_STATE_PLUGGED,
      CP_PP_STATE_PLUGGED_5PER,
      CP_PP_STATE_PLUGGED_SLAC_START,
      CP_PP_STATE_PLUGGED_SLAC_WAIT,
      CP_PP_STATE_PLUGGED_SDP,
      CP_PP_STATE_HLC,
      CP_PP_STATE_WAIT_UNPLUG,
      CP_PP_STATE_EVSE_EMERGENCY,
      CP_PP_STATE_MAINTENANCE,
      CP_PP_STATE_RE_INIT,
      CP_PP_STATE_AC_LL_FALLBACK,
      SNA = 0xF
   };

   enum class QcaSlacConfig : uint8_t
   {
      DISABLE_SLAC,
      ENABLE_PEV,
      ENABLE_EVSE,
      SNA
   };

   enum class QcaSwEnum : uint8_t
   {
      QCA7005_V1_2_5_3207_00_20180927,
      QCA7005_V3_0_0_18_00_20200826,
      QcaSwEnum_Unkown = 0xF
   };

   enum class SigValidationStatus : uint8_t
   {
      ValidationSuccessful,
      ValidationFailed,
      SNA = 3
   };

   enum class SLACStatus : uint8_t
   {
      SLAC_STATE_DISABLED = 1,
      SLAC_STATE_UNMATCHED,
      SLAC_STATE_MATCHING_WAIT_START_ATTEN_CHAR,
      SLAC_STATE_MATCHING_WAIT_MNBC_TIMEOUT,
      SLAC_STATE_MATCHING_WAIT_ATTEN_CHAR_RSP,
      SLAC_STATE_MATCHING_WAIT_MATCH_REQ_OR_INIT_VALIDATE_REQ,
      SLAC_STATE_MATCHING_WAIT_START_VALIDATE_OR_MATCH_REQ,
      SLAC_STATE_MATCHING_WAIT_CP_C,
      SLAC_STATE_MATCHING_WAIT_CP_B,
      SLAC_STATE_MATCHING_WAIT_MATCH_REQ,
      SLAC_STATE_MATCHED,
      SLAC_STATE_FAILED,
      SNA = 15
   };

   enum class SLACLinkStatus : uint8_t
   {
      Down,
      UP,
      SNA = 0x03
   };

   enum class TCPStatus : uint8_t
   {
      Not_Connected,
      TCP_Connected,
      TLS_Connected,
      SNA
   };

   enum class StateMachineState : uint8_t
   {
      Default,
      Init,
      Authentication,
      Parameter,
      Isolation,
      PreCharge,
      Charge,
      Welding,
      StopCharge,
      SessionStop,
      ShutOff,
      Paused,
      Error,
      SNA = 15
   };

   enum class EVSELimitAchieved : uint8_t
   {
      SNA = 3,
      True = 1,
      False = 0
   };

   enum class V2GStatusDIN : uint8_t
   {
      HLC_CORE_STATE_DIN_INVALID_V2GTP_HEADER_SHUTDOWN = 37,
      HLC_CORE_STATE_DIN_EMERGENCY_SHUTDOWN = 36,
      HLC_CORE_STATE_DIN_WAIT_ERR_SEQ_RES_SEND = 35,
      HLC_CORE_STATE_DIN_CHARGING_FINISHED = 34,
      SNA = 63,
      HLC_CORE_STATE_DIN_WAIT_SSTOP_RES_SEND = 33,
      HLC_CORE_STATE_DIN_WAIT_REPEATED_WD_RES_SEND = 32,
      HLC_CORE_STATE_DIN_WAIT_WD_RES_SEND = 31,
      HLC_CORE_STATE_DIN_WAIT_SSTOP_OR_REPEATED_WD_REQ_RECV = 30,
      HLC_CORE_STATE_DIN_WAIT_FOR_CP_STATE_B_SSTOP = 29,
      HLC_CORE_STATE_DIN_WAIT_FOR_CP_STATE_B_WD = 28,
      HLC_CORE_STATE_DIN_WAIT_WD_OR_SSTOP_REQ_RECV = 27,
      HLC_CORE_STATE_DIN_WAIT_PD_POST_RES_SEND = 26,
      HLC_CORE_STATE_DIN_WAIT_PD_POST_REQ_RECV = 25,
      HLC_CORE_STATE_DIN_WAIT_CD_OR_PD_POST_REQ_RECV = 24,
      HLC_CORE_STATE_DIN_WAIT_CD_RES_SEND = 23,
      HLC_CORE_STATE_DIN_WAIT_CD_REQ_RECV = 22,
      HLC_CORE_STATE_DIN_WAIT_PD_PRE_RES_SEND = 21,
      HLC_CORE_STATE_DIN_WAIT_PC_OR_PD_PRE_REQ_RECV = 20,
      HLC_CORE_STATE_DIN_WAIT_PC_RES_SEND = 19,
      HLC_CORE_STATE_DIN_WAIT_PC_REQ_RECV = 18,
      HLC_CORE_STATE_DIN_WAIT_REPEATED_CC_RES_SEND = 17,
      HLC_CORE_STATE_DIN_WAIT_REPEATED_CC_REQ_RECV = 16,
      HLC_CORE_STATE_DIN_WAIT_CC_RES_SEND = 15,
      HLC_CORE_STATE_DIN_WAIT_FOR_CP_STATE_CD = 14,
      HLC_CORE_STATE_DIN_WAIT_CC_REQ_RECV = 13,
      HLC_CORE_STATE_DIN_WAIT_CPD_RES_SEND = 12,
      HLC_CORE_STATE_DIN_WAIT_CPD_REQ_RECV = 11,
      HLC_CORE_STATE_DIN_WAIT_CA_RES_SEND = 10,
      HLC_CORE_STATE_DIN_WAIT_CA_REQ_RECV = 9,
      HLC_CORE_STATE_DIN_WAIT_SPS_RES_SEND = 8,
      HLC_CORE_STATE_DIN_WAIT_SPS_REQ_RECV = 7,
      HLC_CORE_STATE_DIN_WAIT_SD_RES_SEND = 6,
      HLC_CORE_STATE_DIN_WAIT_SD_REQ_RECV = 5,
      HLC_CORE_STATE_DIN_WAIT_SSETUP_RES_SEND = 4,
      HLC_CORE_STATE_DIN_WAIT_SSETUP_REQ_RECV = 3,
      HLC_CORE_STATE_DIN_WAIT_LL_CONNECTION_COMPLETE = 2,
      HLC_CORE_STATE_DIN_WAIT_PLUG = 1,
      HLC_CORE_STATE_DIN_DISABLED = 0,
   };

   enum class V2GStatusDCISO2_1 : uint8_t
   {
      HLC_CORE_STATE_ISO2_DC_EMERGENCY_SHUTDOWN = 36,
      HLC_CORE_STATE_ISO2_DC_WAIT_ERR_SEQ_RES_SEND = 35,
      HLC_CORE_STATE_ISO2_DC_CHARGING_FINISHED = 34,
      SNA = 63,
      HLC_CORE_STATE_ISO2_DC_WAIT_SSTOP_RES_SEND = 33,
      HLC_CORE_STATE_ISO2_DC_WAIT_REPEATED_WD_RES_SEND = 32,
      HLC_CORE_STATE_ISO2_DC_WAIT_WD_RES_SEND = 31,
      HLC_CORE_STATE_ISO2_DC_WAIT_SSTOP_OR_REPEATED_WD_REQ_RECV = 30,
      HLC_CORE_STATE_ISO2_DC_WAIT_FOR_CP_STATE_B_SSTOP = 29,
      HLC_CORE_STATE_ISO2_DC_WAIT_FOR_CP_STATE_B_WD = 28,
      HLC_CORE_STATE_ISO2_DC_WAIT_WD_OR_SSTOP_REQ_RECV = 27,
      HLC_CORE_STATE_ISO2_DC_WAIT_PD_POST_RES_SEND = 26,
      HLC_CORE_STATE_ISO2_DC_WAIT_PD_POST_REQ_RECV = 25,
      HLC_CORE_STATE_ISO2_DC_WAIT_CD_OR_PD_POST_REQ_RECV = 24,
      HLC_CORE_STATE_ISO2_DC_WAIT_CD_RES_SEND = 23,
      HLC_CORE_STATE_ISO2_DC_WAIT_CD_REQ_RECV = 22,
      HLC_CORE_STATE_ISO2_DC_WAIT_PD_PRE_RES_SEND = 21,
      HLC_CORE_STATE_ISO2_DC_WAIT_PC_OR_PD_PRE_REQ_RECV = 20,
      HLC_CORE_STATE_ISO2_DC_WAIT_PC_RES_SEND = 19,
      HLC_CORE_STATE_ISO2_DC_WAIT_PC_REQ_RECV = 18,
      HLC_CORE_STATE_ISO2_DC_WAIT_REPEATED_CC_RES_SEND = 17,
      HLC_CORE_STATE_ISO2_DC_WAIT_REPEATED_CC_REQ_RECV = 16,
      HLC_CORE_STATE_ISO2_DC_WAIT_CC_RES_SEND = 15,
      HLC_CORE_STATE_ISO2_DC_WAIT_FOR_CP_STATE_CD = 14,
      HLC_CORE_STATE_ISO2_DC_WAIT_CC_REQ_RECV = 13,
      HLC_CORE_STATE_ISO2_DC_WAIT_CPD_RES_SEND = 12,
      HLC_CORE_STATE_ISO2_DC_WAIT_CPD_REQ_RECV = 11,
      HLC_CORE_STATE_ISO2_DC_WAIT_AU_RES_SEND = 10,
      HLC_CORE_STATE_ISO2_DC_WAIT_AU_REQ_RECV = 9,
      HLC_CORE_STATE_ISO2_DC_WAIT_PSS_RES_SEND = 8,
      HLC_CORE_STATE_ISO2_DC_WAIT_PSS_REQ_RECV = 7,
      HLC_CORE_STATE_ISO2_DC_WAIT_SD_RES_SEND = 6,
      HLC_CORE_STATE_ISO2_DC_WAIT_SD_REQ_RECV = 5,
      HLC_CORE_STATE_ISO2_DC_WAIT_SSETUP_RES_SEND = 4,
      HLC_CORE_STATE_ISO2_DC_WAIT_SSETUP_REQ_RECV = 3,
      HLC_CORE_STATE_ISO2_DC_WAIT_LL_CONNECTION_COMPLETE = 2,
      HLC_CORE_STATE_ISO2_DC_WAIT_PLUG = 1,
      HLC_CORE_STATE_ISO2_DC_DISABLED = 0,
   };

   enum class V2GStatusACISO2_1 : uint8_t
   {
      HLC_CORE_STATE_ISO2_AC_EMERGENCY_SHUTDOWN = 28,
      SNA = 63,
      HLC_CORE_STATE_ISO2_AC_WAIT_ERR_SEQ_RES_SEND = 27,
      HLC_CORE_STATE_ISO2_AC_CHARGING_FINISHED = 26,
      HLC_CORE_STATE_ISO2_AC_WAIT_SSTOP_RES_SEND = 25,
      HLC_CORE_STATE_ISO2_AC_WAIT_SSTOP_REQ_RECV = 24,
      HLC_CORE_STATE_ISO2_AC_WAIT_PD_OFF_RES_SEND = 23,
      HLC_CORE_STATE_ISO2_AC_WAIT_CP_STATE_B = 22,
      HLC_CORE_STATE_ISO2_STATE_AC_WAIT_PD_OFF_REQ_RECV = 21,
      HLC_CORE_STATE_ISO2_AC_WAIT_CS_OR_PD_OFF_REQ_RECV = 20,
      HLC_CORE_STATE_ISO2_AC_WAIT_CS_RES_SEND = 19,
      HLC_CORE_STATE_ISO2_AC_WAIT_CS_REQ_RECV = 18,
      HLC_CORE_STATE_ISO2_AC_WAIT_PD_ON_RES_SEND = 17,
      HLC_CORE_STATE_ISO2_AC_WAIT_CP_STATE_C = 16,
      HLC_CORE_STATE_ISO2_AC_WAIT_PD_ON_REQ_RECV = 15,
      HLC_CORE_STATE_ISO2_AC_WAIT_CPD_RES_SEND = 14,
      HLC_CORE_STATE_ISO2_AC_WAIT_CPD_REQ_RECV = 13,
      HLC_CORE_STATE_ISO2_AC_WAIT_AU_RES_SEND = 12,
      HLC_CORE_STATE_ISO2_AC_WAIT_AU_REQ_RECV = 11,
      HLC_CORE_STATE_ISO2_AC_WAIT_PDETAIL_RES_SEND = 10,
      HLC_CORE_STATE_ISO2_AC_WAIT_PDETAIL_REQ_RECV = 9,
      HLC_CORE_STATE_ISO2_AC_WAIT_PSS_RES_SEND = 8,
      HLC_CORE_STATE_ISO2_AC_WAIT_PSS_REQ_RECV = 7,
      HLC_CORE_STATE_ISO2_AC_WAIT_SD_RES_SEND = 6,
      HLC_CORE_STATE_ISO2_AC_WAIT_SD_REQ_RECV = 5,
      HLC_CORE_STATE_ISO2_AC_WAIT_SSETUP_RES_SEND = 4,
      HLC_CORE_STATE_ISO2_AC_WAIT_SSETUP_REQ_RECV = 3,
      HLC_CORE_STATE_ISO2_AC_WAIT_LL_CONNECTION_COMPLETE = 2,
      HLC_CORE_STATE_ISO2_AC_WAIT_PLUG = 1,
      HLC_CORE_STATE_ISO2_AC_DISABLED = 0,
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

   struct PACKED ChargeInfo
   {
      uint64_t controlPilotDuty : 7;
      uint64_t Reserved1 : 1;
      uint64_t stateMachineState : 4;
      uint64_t proximityPinState : 4;
      uint64_t actualChargeProtocol : 4;
      uint64_t controlPilotState : 4;
      uint64_t AliveCounter : 4;
      uint64_t TCPStatus : 2;
      uint64_t DiodePresence : 2;
      uint64_t CerValidationStatus : 3;
      uint64_t SigValidationStatus : 2;
      uint64_t EmaidValidationStatus : 2;
      uint64_t Reserved2 : 1;
      uint64_t ACFallBackStatus : 1;
      uint64_t Reserved3 : 23;
   };

   struct PACKED SoftwareInfo
   {
      uint64_t SoftwareVersionMajor : 8;
      uint64_t SoftwareVersionMinor : 8;
      uint64_t SoftwareVersionPatch : 8;
      uint64_t SoftwareVersionConfig : 8;
      uint64_t SoftwareVersionHash : 32;
   };

   struct PACKED ErrorCodes
   {
      uint64_t category0 : 5;
      uint64_t severity0 : 2;
      uint64_t interface0 : 1;
      uint64_t errorCode0 : 8;
      uint64_t category1 : 5;
      uint64_t severity1 : 2;
      uint64_t interface1 : 1;
      uint64_t errorCode1 : 8;
      uint64_t category2 : 5;
      uint64_t severity2 : 2;
      uint64_t interface2 : 1;
      uint64_t errorCode2 : 8;
      uint64_t category3 : 5;
      uint64_t severity3 : 2;
      uint64_t interface3 : 1;
      uint64_t errorCode3 : 8;
   };

   struct PACKED HardwareStatus
   {
      uint64_t GPIO2Status : 2;
      uint64_t GPIO3Status : 2;
      uint64_t GPIO4Status : 2;
      uint64_t GPIO5Status : 2;
      uint64_t Reserved : 56;
   };

   struct PACKED SLACInfo
   {
      uint64_t SLACLinkStatus : 2;
      uint64_t QcaSlacConfig : 2;
      uint64_t QcaSwEnum : 4;
      uint64_t MeasuredAttenuation : 8;
      uint64_t QcaSwVersionMajor : 8;
      uint64_t QcaSwVersionMinor : 8;
      uint64_t QcaSwVersionPatch : 8;
      uint64_t Reserved : 24;
   };

   struct PACKED DebugInfo
   {
      uint64_t PilotStatesStatus : 4;
      uint64_t SLACStatus : 4;
      uint64_t V2GStatusDIN : 6;
      uint64_t Reserved1 : 2;
      uint64_t ProximityPilotVoltage : 12;
      uint64_t Reserved2 : 4;
      uint64_t ControlPilotVoltage : 14;
      uint64_t Reserved3 : 2;
      uint64_t V2GStatusDCISO : 6;
      uint64_t Reserved4 : 2;
      uint64_t V2GStatusACISO : 6;
      uint64_t Reserved5 : 2;
   };

   struct PACKED EVDCMaxLimits
   {
      uint64_t EVMaxCurrent : 16;
      uint64_t EVMaxPower : 16;
      uint64_t EVMaxVoltage : 16;
      uint64_t EVFullSoC : 8;
      uint64_t EVBulkSoC : 8;
   };

   struct PACKED EVDCChargeTargets
   {
      uint64_t EVTargetCurrent : 16;
      uint64_t EVTargetVoltage : 16;
      uint64_t EVPreChargeVoltage : 16;
      uint64_t Reserved : 16;
   };

   struct PACKED EVStatusDisplay
   {
      uint64_t EVSoC : 8;
      uint64_t EVChargingComplete : 2;
      uint64_t EVBulkChargingComplete : 2;
      uint64_t EVErrorCode : 4;
      uint64_t EVCabinConditioning : 2;
      uint64_t EVRESSConditioning : 2;
      uint64_t EVTimeToBulkSoC : 18;
      uint64_t EVReady : 2;
      uint64_t EVTimeToFullSoC : 18;
      uint64_t Reserved1 : 2;
      uint64_t EVRequestedETM : 3;
      uint64_t Reserved2 : 1;
   };

   struct PACKED EVDCEnergyLimits
   {
      uint64_t EVEnergyCapacity : 16;
      uint64_t EVEnergyRequest : 16;
      uint64_t Reserved : 32;
   };

   struct PACKED EVMacAddress
   {
      uint64_t EVMacAddress;
   };

   struct PACKED EVACChargeControl
   {
      uint64_t EVACMinCurrent : 16;
      uint64_t EVACMaxCurrent : 16;
      uint64_t EVACMaxVoltage : 16;
      uint64_t EVEAmount : 16;
   };

   struct PACKED EVEMAID0
   {
      uint64_t EVEMAID0;
   };
   struct PACKED EVEMAID1
   {
      uint64_t EVEMAID1;
   };

   struct PACKED EVSEDCMaxLimits
   {
      uint64_t EVSEMaxCurrent : 16;
      uint64_t EVSEMaxVoltage : 16;
      uint64_t EVSEMaxPower : 16;
      uint64_t EVSEEnergyToBeDelivered : 16;
   };

   struct PACKED EVSEDCRegulationLimits
   {
      uint64_t EVSEMinCurrent : 16;
      uint64_t EVSEMinVoltage : 16;
      uint64_t EVSEPeakCurrentRipple : 8;
      uint64_t EVSECurrentRegulationTolerance : 8;
      uint64_t Reserved : 16;
   };

   struct PACKED EVSEStatus
   {
      uint64_t EVSEPresentCurrent : 16;
      uint64_t EVSEPresentVoltage : 16;
      uint64_t EVSEIsolationStatus : 3;
      uint64_t EVSEFreeService : 1;
      uint64_t EVSEVoltageLimitAchieved : 2;
      uint64_t Reserved1 : 2;
      uint64_t EVSEStatusCode : 4;
      uint64_t EVSECurrentLimitAchieved : 2;
      uint64_t EVSEPowerLimitAchieved : 2;
      uint64_t EVSEProcessingCA : 2;
      uint64_t EVSEProcessingCPD : 2;
      uint64_t EVSEProcessingCC : 2;
      uint64_t Reserved2 : 2;
      uint64_t EVSESupportedETM : 3;
      uint64_t Reserved3 : 5;
   };

   struct PACKED EVSEPlugStatus
   {
      uint64_t EVSEControlPilotDuty : 7;
      uint64_t Reserved1 : 1;
      uint64_t EVSEControlPilotState : 4;
      uint64_t EVSEProximityPinState : 4;
      uint64_t Reserved2 : 48;
   };

   struct PACKED EVSEDateTime
   {
      uint64_t EVSEDateTimeNow;
   };

   struct PACKED HardwareConfig
   {
      uint64_t ConfigGPIO2 : 2;
      uint64_t ConfigGPIO3 : 2;
      uint64_t ConfigGPIO4 : 2;
      uint64_t ConfigGPIO5 : 2;
      uint64_t Reserved : 56;
   };

   struct PACKED EVSEACChargeControl
   {
      uint64_t EVSEACCurrentLimit : 10;
      uint64_t Reserved1 : 6;
      uint64_t EVSEACMaxCurrent : 16;
      uint64_t EVSENominalVoltage : 16;
      uint64_t Reserved2 : 16;
   };

   struct PACKED RTTLog
   {
      uint64_t RTTLogInfo;
   };

   struct PACKED BootConfig
   {
      uint64_t BootloaderVersion : 8;
      uint64_t Spare : 56;
   };

   struct ModuleStatus
   {
      bool isActive;
      ControlPilotState controlPilotState_old;
      StateMachineState stateMachineState_old;
      PilotStatesStatus pilotStatesStatus_old;
      SLACStatus slacStatus_old;

      // ChargeInfo
      ControlPilotDuty controlPilotDuty;
      StateMachineState stateMachineState;
      ProximityPinState proximityPinState;
      ActualChargeProtocol actualChargeProtocol;
      ControlPilotState controlPilotState;
      uint8_t AliveCounter;
      TCPStatus tcpStatus;
      DiodePresence diodePresence;
      CerValidationStatus cerValidationStatus;
      SigValidationStatus sigValidationStatus;
      EmaidValidationStatus emaidValidationStatus;
      ACFallBackStatus acFallBackStatus;

      // SoftwareInfo
      uint8_t SoftwareVersionMajor;
      uint8_t SoftwareVersionMinor;
      uint8_t SoftwareVersionPatch;
      uint8_t SoftwareVersionConfig;
      uint32_t SoftwareVersionHash;

      // ErrorCodes
      uint8_t errorcount;
      Category category[4];
      uint8_t errorCode[4];
      Severity severity[4];
      Interface interface[4];

      // HardwareStatus
      GPIOStatus gpio2Status;
      GPIOStatus gpio3Status;
      GPIOStatus gpio4Status;
      GPIOStatus gpio5Status;

      // SLACInfo
      SLACLinkStatus slacLinkStatus;
      QcaSlacConfig qcaSlacConfig;
      QcaSwEnum qcaSwEnum;
      uint8_t measuredAttenuation;
      uint8_t qcaSwVersionMajor;
      uint8_t qcaSwVersionMinor;
      uint8_t qcaSwVersionPatch;

      // DebugInfo
      PilotStatesStatus pilotStatesStatus;
      SLACStatus slacStatus;
      V2GStatusDIN dinStatus;
      uint32_t proximityPilotVoltage;
      uint32_t controlPilotVoltage;
      V2GStatusDCISO2_1 dcISO2_1_Status;
      V2GStatusACISO2_1 acISO2_1_Status;

      // EVDCMaxLimits
      float evMaxCurrent;
      float evMaxPower;
      float evMaxVoltage;
      float evFullSoC;
      float evBulkSoC;

      // EVDCChargeTargets
      float evTargetCurrent;
      float evTargetVoltage;
      float evPreChargeVoltage;

      // EVStatusDisplay
      uint8_t evSoC;
      EVChargingComplete evChargingComplete;
      EVBulkChargingComplete evBulkChargingComplete;
      EVErrorCode evErrorCode;
      EVCabinConditioning evCabinConditioning;
      EVRESSConditioning evRESSConditioning;
      uint32_t evTimeToBulkSoC;
      uint32_t evTimeToFullSoC;
      EVReady evReady;
      EVRequestedETM evRequestedETM;

      // EVDCEnergyLimits
      float evEnergyCapacity;
      float evEnergyRequest;

      // EVMacAddress
      uint64_t evMacAddress;

      // EVACChargeControl
      float evACMinCurrent;
      float evACMaxCurrent;
      float evACMaxVoltage;
      float evEAmount;

      // EVEMAID
      uint64_t evEMAID0;
      uint64_t evEMAID1;

      // EVSEDCMaxLimits
      float evseMaxCurrent;
      float evseMaxVoltage;
      float evseMaxPower;
      float evseEnergyToBeDelivered;

      // EVSEDCRegulationLimits
      float evseMinCurrent;
      float evseMinVoltage;
      float evsePeakCurrentRipple;
      float evseCurrentRegulationTolerance;

      // EVSEStatus
      float evsePresentCurrent;
      float evsePresentVoltage;
      EVSEIsolationStatus evseIsolationStatus;
      bool evseFreeService;
      EVSEVoltageLimitAchieved evseVoltageLimitAchieved;
      EVSEStatusCode evseStatusCode;
      EVSECurrentLimitAchieved evseCurrentLimitAchieved;
      EVSEPowerLimitAchieved evsePowerLimitAchieved;
      EVSEProcessing evseProcessingCA;
      EVSEProcessing evseProcessingCPD;
      EVSEProcessing evseProcessingCC;
      EVSESupportedETM evseSupportedETM;

      // EVSEPlugStatus
      ControlPilotDuty evseControlPilotDuty;
      ControlPilotState evseControlPilotState;
      ProximityPinState evseProximityPinState;

      // EVSEDateTime
      uint64_t evseDateTimeNow;

      // HardwareConfig
      ConfigGPIO configGPIO2;
      ConfigGPIO configGPIO3;
      ConfigGPIO configGPIO4;
      ConfigGPIO configGPIO5;

      // EVSEACChargeControl
      float evseACCurrentLimit;
      float evseACMaxCurrent;
      float evseNominalVoltage;

      uint8_t bootLoaderVersion;
      uint64_t rttLog;
   };

#if defined(_MSC_VER)
#pragma pack(pop)
#endif
#undef PACKED

   // Function pointer types
   typedef bool (*SendDataFunc)(const uint32_t id, const uint64_t data);
   // typedef bool (*ReceiveDataFunc)(const uint32_t id, const uint64_t data);
   // C++ interface class with original methods
   class PLCController
   {
   public:
      PLCController(SendDataFunc sendFunction);

      void Initialize();

      bool Get_isActive(uint8_t ConnID);

      ControlPilotDuty Get_ControlPilotDuty(uint8_t ConnID);
      StateMachineState Get_StateMachineState(uint8_t ConnID);
      ProximityPinState Get_ProximityPinState(uint8_t ConnID);
      ActualChargeProtocol Get_ActualChargeProtocol(uint8_t ConnID);
      ControlPilotState Get_ControlPilotState(uint8_t ConnID);
      uint8_t Get_AliveCounter(uint8_t ConnID);
      TCPStatus Get_TCPStatus(uint8_t ConnID);
      DiodePresence Get_DiodePresence(uint8_t ConnID);
      CerValidationStatus Get_CerValidationStatus(uint8_t ConnID);
      SigValidationStatus Get_SigValidationStatus(uint8_t ConnID);
      EmaidValidationStatus Get_EmaidValidationStatus(uint8_t ConnID);
      ACFallBackStatus Get_ACFallBackStatus(uint8_t ConnID);

      uint8_t Get_SoftwareVersionMajor(uint8_t ConnID);
      uint8_t Get_SoftwareVersionMinor(uint8_t ConnID);
      uint8_t Get_SoftwareVersionPatch(uint8_t ConnID);
      uint8_t Get_SoftwareVersionConfig(uint8_t ConnID);
      uint32_t Get_SoftwareVersionHash(uint8_t ConnID);

      Category Get_Category(uint8_t ConnID, uint8_t id);
      uint8_t Get_ErrorCode(uint8_t ConnID, uint8_t id);
      Severity Get_Severity(uint8_t ConnID, uint8_t id);
      Interface Get_Interface(uint8_t ConnID, uint8_t id);

      GPIOStatus Get_GPIO2Status(uint8_t ConnID);
      GPIOStatus Get_GPIO3Status(uint8_t ConnID);
      GPIOStatus Get_GPIO4Status(uint8_t ConnID);
      GPIOStatus Get_GPIO5Status(uint8_t ConnID);

      SLACLinkStatus Get_SLACLinkStatus(uint8_t ConnID);
      QcaSlacConfig Get_QcaSlacConfig(uint8_t ConnID);
      QcaSwEnum Get_QcaSwEnum(uint8_t ConnID);
      uint8_t Get_MeasuredAttenuation(uint8_t ConnID);
      uint8_t Get_QcaSwVersionMajor(uint8_t ConnID);
      uint8_t Get_QcaSwVersionMinor(uint8_t ConnID);
      uint8_t Get_QcaSwVersionPatch(uint8_t ConnID);

      float Get_EVMaxCurrent(uint8_t ConnID);
      float Get_EVMaxPower(uint8_t ConnID);
      float Get_EVMaxVoltage(uint8_t ConnID);
      float Get_EVFullSoC(uint8_t ConnID);
      float Get_EVBulkSoC(uint8_t ConnID);
      float Get_EVTargetCurrent(uint8_t ConnID);
      float Get_EVTargetVoltage(uint8_t ConnID);
      float Get_EVPreChargeVoltage(uint8_t ConnID);

      uint8_t Get_EVSoC(uint8_t ConnID);
      EVChargingComplete Get_EVChargingComplete(uint8_t ConnID);
      EVBulkChargingComplete Get_EVBulkChargingComplete(uint8_t ConnID);
      EVErrorCode Get_EVErrorCode(uint8_t ConnID);
      EVCabinConditioning Get_EVCabinConditioning(uint8_t ConnID);
      EVRESSConditioning Get_EVRESSConditioning(uint8_t ConnID);
      uint32_t Get_EVTimeToBulkSoC(uint8_t ConnID);
      uint32_t Get_EVTimeToFullSoC(uint8_t ConnID);
      EVReady Get_EVReady(uint8_t ConnID);
      EVRequestedETM Get_EVRequestedETM(uint8_t ConnID);

      float Get_EVEnergyCapacity(uint8_t ConnID);
      float Get_EVEnergyRequest(uint8_t ConnID);

      uint64_t Get_EVMacAddress(uint8_t ConnID);

      float Get_EVACMinCurrent(uint8_t ConnID);
      float Get_EVACMaxCurrent(uint8_t ConnID);
      float Get_EVACMaxVoltage(uint8_t ConnID);
      float Get_EVEAmount(uint8_t ConnID);

      uint64_t Get_EVEMAID0(uint8_t ConnID);
      uint64_t Get_EVEMAID1(uint8_t ConnID);

      float Get_EVSEMaxCurrent(uint8_t ConnID);
      float Get_EVSEMaxVoltage(uint8_t ConnID);
      float Get_EVSEMaxPower(uint8_t ConnID);
      float Get_EVSEEnergyToBeDelivered(uint8_t ConnID);

      float Get_EVSEMinCurrent(uint8_t ConnID);
      float Get_EVSEMinVoltage(uint8_t ConnID);
      float Get_EVSEPeakCurrentRipple(uint8_t ConnID);
      float Get_EVSECurrentRegulationTolerance(uint8_t ConnID);

      float Get_EVSEPresentCurrent(uint8_t ConnID);
      float Get_EVSEPresentVoltage(uint8_t ConnID);
      EVSEIsolationStatus Get_EVSEIsolationStatus(uint8_t ConnID);
      bool Get_EVSEFreeService(uint8_t ConnID);
      EVSEVoltageLimitAchieved Get_EVSEVoltageLimitAchieved(uint8_t ConnID);
      EVSEStatusCode Get_EVSEStatusCode(uint8_t ConnID);
      EVSECurrentLimitAchieved Get_EVSECurrentLimitAchieved(uint8_t ConnID);
      EVSEPowerLimitAchieved Get_EVSEPowerLimitAchieved(uint8_t ConnID);
      EVSEProcessing Get_EVSEProcessingCA(uint8_t ConnID);
      EVSEProcessing Get_EVSEProcessingCPD(uint8_t ConnID);
      EVSEProcessing Get_EVSEProcessingCC(uint8_t ConnID);
      EVSESupportedETM Get_EVSESupportedETM(uint8_t ConnID);

      ControlPilotDuty Get_EVSEControlPilotDuty(uint8_t ConnID);
      ControlPilotState Get_EVSEControlPilotState(uint8_t ConnID);
      ProximityPinState Get_EVSEProximityPinState(uint8_t ConnID);
      uint64_t Get_EVSEDateTimeNow(uint8_t ConnID);
      ConfigGPIO Get_ConfigGPIO2(uint8_t ConnID);
      ConfigGPIO Get_ConfigGPIO3(uint8_t ConnID);
      ConfigGPIO Get_ConfigGPIO4(uint8_t ConnID);
      ConfigGPIO Get_ConfigGPIO5(uint8_t ConnID);
      float Get_EVSEACCurrentLimit(uint8_t ConnID);
      float Get_EVSEACMaxCurrent(uint8_t ConnID);
      float Get_EVSENominalVoltage(uint8_t ConnID);
      uint8_t Get_BootLoaderVersion(uint8_t ConnID);
      uint64_t Get_RTTLog(uint8_t ConnID);

      // Setting Values
      bool Set_isActive(uint8_t ConnID, bool isActive);
      bool Set_EVSEMaxCurrent(uint8_t ConnID, float EVSEMaxCurrent);
      bool Set_EVSEMaxVoltage(uint8_t ConnID, float EVSEMaxVoltage);
      bool Set_EVSEMaxPower(uint8_t ConnID, float EVSEMaxPower);
      bool Set_EVSEEnergyToBeDelivered(uint8_t ConnID, float EVSEEnergyToBeDelivered);
      bool Set_EVSEMinCurrent(uint8_t ConnID, float EVSEMinCurrent);
      bool Set_EVSEMinVoltage(uint8_t ConnID, float EVSEMinVoltage);
      bool Set_EVSEPeakCurrentRipple(uint8_t ConnID, float EVSEPeakCurrentRipple);
      bool Set_EVSECurrentRegulationTolerance(uint8_t ConnID, float EVSECurrentRegulationTolerance);
      bool Set_EVSEPresentCurrent(uint8_t ConnID, float EVSEPresentCurrent);
      bool Set_EVSEPresentVoltage(uint8_t ConnID, float EVSEPresentVoltage);
      bool Set_EVSEIsolationStatus(uint8_t ConnID, EVSEIsolationStatus EVSEIsolationStatus);
      bool Set_EVSEFreeService(uint8_t ConnID, bool EVSEFreeService);
      bool Set_EVSEVoltageLimitAchieved(uint8_t ConnID, EVSEVoltageLimitAchieved EVSEVoltageLimitAchieved);
      bool Set_EVSEStatusCode(uint8_t ConnID, EVSEStatusCode EVSEStatusCode);
      bool Set_EVSECurrentLimitAchieved(uint8_t ConnID, EVSECurrentLimitAchieved EVSECurrentLimitAchieved);
      bool Set_EVSEPowerLimitAchieved(uint8_t ConnID, EVSEPowerLimitAchieved EVSEPowerLimitAchieved);
      bool Set_EVSEProcessingCA(uint8_t ConnID, EVSEProcessing EVSEProcessingCA);
      bool Set_EVSEProcessingCPD(uint8_t ConnID, EVSEProcessing EVSEProcessingCPD);
      bool Set_EVSEProcessingCC(uint8_t ConnID, EVSEProcessing EVSEProcessingCC);
      bool Set_EVSESupportedETM(uint8_t ConnID, EVSESupportedETM EVSESupportedETM);
      bool Set_EVSEControlPilotDuty(uint8_t ConnID, ControlPilotDuty EVSEControlPilotDuty);
      bool Set_EVSEControlPilotState(uint8_t ConnID, ControlPilotState EVSEControlPilotState);
      bool Set_EVSEProximityPinState(uint8_t ConnID, ProximityPinState EVSEProximityPinState);
      bool Set_EVSEDateTimeNow(uint8_t ConnID, uint64_t EVSEDateTimeNow);
      bool Set_ConfigGPIO2(uint8_t ConnID, ConfigGPIO ConfigGPIO2);
      bool Set_ConfigGPIO3(uint8_t ConnID, ConfigGPIO ConfigGPIO3);
      bool Set_ConfigGPIO4(uint8_t ConnID, ConfigGPIO ConfigGPIO4);
      bool Set_ConfigGPIO5(uint8_t ConnID, ConfigGPIO ConfigGPIO5);
      bool Set_EVSEACCurrentLimit(uint8_t ConnID, float EVSEACCurrentLimit);
      bool Set_EVSEACMaxCurrent(uint8_t ConnID, float EVSEACMaxCurrent);
      bool Set_EVSENominalVoltage(uint8_t ConnID, float EVSENominalVoltage);

      bool Set_EVSEContactorState(uint8_t ConnID, uint8_t state);
      bool Get_EVSEContactorState(uint8_t ConnID);
      PLCModule::ModuleStatus GetModuleStatus(uint8_t ConnID);
      void SetModuleStatusToSNA(uint8_t ConnID);

      // Set function pointers
      void SetSendFunction(SendDataFunc func);
      // void SetReceiveFunction(ReceiveDataFunc func);

      // Call these instead of original methods
      bool SendData(const uint32_t id, const uint64_t data);
      bool ReceiveData(const uint32_t id, const uint64_t data);

      // EVSE Transmit messages
      // Common For AC and DC
      bool SendEVSEDateTime(uint8_t ConnID);
      bool SendEVSEStatus(uint8_t ConnID);
      bool SendEVSEPlugStatus(uint8_t ConnID); // only if Control Pilot is in Host Control
      bool SendHardwareConfig(uint8_t ConnID); //  if using hardware gpio's

      // Only for DC
      bool SendEVSEDCMaxLimits(uint8_t ConnID);
      bool SendEVSEDCRegulationLimits(uint8_t ConnID);

      // only for AC
      bool SendEVSEACChargeControl(uint8_t ConnID);

      // enum to string utilities
      const char *GetControlPilotStateString(ControlPilotState state);
      const char *GetStateMachineStateString(StateMachineState state);
      const char *GetControlPilotDutyString(ControlPilotDuty duty);

      ~PLCController();

      std::array<PLCModule::ModuleStatus, PLCModule::Constants::MAX_MODULES> moduleStatus;

   private:
      SendDataFunc sendFunc;
      // ReceiveDataFunc receiveFunc;

      // CME Transmit messages
      void Set_ChargeInfo(uint8_t ConnID, uint64_t data);
      void Set_SoftwareInfo(uint8_t ConnID, uint64_t data);
      void Set_ErrorCodes(uint8_t ConnID, uint64_t data);
      void Set_HardwareStatus(uint8_t ConnID, uint64_t data);
      void Set_SLACInfo(uint8_t ConnID, uint64_t data);
      void Set_DebugInfo(uint8_t ConnID, uint64_t data);

      void Set_RTTLogInfo(uint8_t ConnID, uint64_t data);
      void Set_BootConfig(uint8_t ConnID, uint64_t data);

      // EV Transmit messages
      void Set_EVDCMaxLimits(uint8_t ConnID, uint64_t data);
      void Set_EVDCChargeTargets(uint8_t ConnID, uint64_t data);
      void Set_EVStatusDisplay(uint8_t ConnID, uint64_t data);
      void Set_EVDCEnergyLimits(uint8_t ConnID, uint64_t data);
      void Set_EVMacAddress(uint8_t ConnID, uint64_t data);
      void Set_EVACChargeControl(uint8_t ConnID, uint64_t data);
      void Set_EVEMAID0(uint8_t ConnID, uint64_t data);
      void Set_EVEMAID1(uint8_t ConnID, uint64_t data);

      void GetEnumInterfaceString(Interface interface, char *buffer);
      void GetEnumCategoryString(Category category, char *buffer);
      void GetEnumSeverityString(Severity severity, char *buffer);
      void GetErrorCodeString(Category category, uint8_t errorCode, char *buffer);
   };

} // namespace PLCModule

extern PLCModule::PLCController *plc;

#endif // __cplusplus
#endif /* INC_PLCMODULE_H_ */