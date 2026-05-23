#include "PLCModule.hpp"
#include "esp_log.h"
#include <cstring>
#include <cmath>

#define TAG "PLCModule"

PLCModule::PLCController *plc;
namespace PLCModule
{
    // Constructor
    PLCController::PLCController(SendDataFunc func)
    {
        this->sendFunc = func;
        for (auto &module : moduleStatus)
        {
            module = ModuleStatus();
        }
        Initialize();
    }
    void PLCController::Initialize()
    {
        // Initialization logic here
        for (uint8_t i = 0; i < Constants::MAX_MODULES; i++)
            SetModuleStatusToSNA(i);
    }

    // Destructor
    PLCController::~PLCController()
    {
        // Cleanup, if necessary
    }

    void PLCController::SetSendFunction(SendDataFunc func)
    {
        this->sendFunc = func;
    }

    bool PLCController::SendData(const uint32_t id, const uint64_t data)
    {
        uint32_t CanId = id & 0xFFFF0FFF;
        uint8_t ConnID = ((id >> 12) & 0x0F);
        uint8_t CmeID = ConnID - 1;
        if (!(ConnID >= 1))
            return false;

        CanId = (id & 0xFFFF0FFF) | ((CmeID << 12) & 0xF000);

        static uint32_t count = 0;
        if ((id == 0x300) && (count < 300))
            count++;
        else if ((id == 0x300) && (count == 300))
            count = 0;
        if (sendFunc)
        {
            if (count == 300)
                ESP_LOGD(TAG, "ID: 0x%08lx   data: 0x%16llx", CanId, data);
            return sendFunc(CanId, data);
        }
        return false; // or handle error
    }

    bool PLCController::ReceiveData(const uint32_t id, const uint64_t data)
    {
        uint32_t CanId = id & 0xFFFF0FFF;
        uint8_t CmeID = ((id >> 12) & 0x0F);
        uint8_t ConnID = CmeID + 1;
        if (ConnID >= Constants::MAX_MODULES)
            return false;

        switch (CanId)
        {
        case static_cast<uint32_t>(PLC_ID::ChargeInfo):
            Set_ChargeInfo(ConnID, data);
            ESP_LOGD(TAG, "Received ChargeInfo: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::SoftwareInfo):
            Set_SoftwareInfo(ConnID, data);
            ESP_LOGD(TAG, "Received SoftwareInfo: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::ErrorCodes):
            Set_ErrorCodes(ConnID, data);
            ESP_LOGD(TAG, "Received ErrorCodes: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::HardwareStatus):
            Set_HardwareStatus(ConnID, data);
            ESP_LOGD(TAG, "Received HardwareStatus: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::SLACInfo):
            Set_SLACInfo(ConnID, data);
            ESP_LOGD(TAG, "Received SLACInfo: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::DebugInfo):
            Set_DebugInfo(ConnID, data);
            ESP_LOGD(TAG, "Received DebugInfo: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::RTTLog):
            Set_RTTLogInfo(ConnID, data);
            ESP_LOGD(TAG, "Received RTTLogInfo: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::BootConfig):
            Set_BootConfig(ConnID, data);
            ESP_LOGD(TAG, "Received BootConfig: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;

        case static_cast<uint32_t>(PLC_ID::EVDCMaxLimits):
            Set_EVDCMaxLimits(ConnID, data);
            ESP_LOGD(TAG, "Received EVDCMaxLimits: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVDCChargeTargets):
            Set_EVDCChargeTargets(ConnID, data);
            ESP_LOGD(TAG, "Received EVDCChargeTargets: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVStatusDisplay):
            Set_EVStatusDisplay(ConnID, data);
            ESP_LOGD(TAG, "Received EVStatusDisplay: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVDCEnergyLimits):
            Set_EVDCEnergyLimits(ConnID, data);
            ESP_LOGD(TAG, "Received EVDCEnergyLimits: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVMacAddress):
            Set_EVMacAddress(ConnID, data);
            ESP_LOGD(TAG, "Received EVMacAddress: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVACChargeControl):
            Set_EVACChargeControl(ConnID, data);
            ESP_LOGD(TAG, "Received EVACChargeControl: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVEMAID0):
            Set_EVEMAID0(ConnID, data);
            ESP_LOGD(TAG, "Received EVEMAID0: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;
        case static_cast<uint32_t>(PLC_ID::EVEMAID1):
            Set_EVEMAID1(ConnID, data);
            ESP_LOGD(TAG, "Received EVEMAID1: 0x%16llx from CME:%hhu for Con:%hhu", data, CmeID, ConnID);
            break;

        default:
            return false; // or handle error
            break;
        }

        return true; // or handle error
    }

    bool PLCController::Get_isActive(uint8_t ConnID)
    {
        return moduleStatus[ConnID].isActive;
    }

    ControlPilotDuty PLCController::Get_ControlPilotDuty(uint8_t ConnID)
    {
        return moduleStatus[ConnID].controlPilotDuty;
    }
    StateMachineState PLCController::Get_StateMachineState(uint8_t ConnID)
    {
        return moduleStatus[ConnID].stateMachineState;
    }
    ProximityPinState PLCController::Get_ProximityPinState(uint8_t ConnID)
    {
        return moduleStatus[ConnID].proximityPinState;
    }
    ActualChargeProtocol PLCController::Get_ActualChargeProtocol(uint8_t ConnID)
    {
        return moduleStatus[ConnID].actualChargeProtocol;
    }
    ControlPilotState PLCController::Get_ControlPilotState(uint8_t ConnID)
    {
        return moduleStatus[ConnID].controlPilotState;
    }
    uint8_t PLCController::Get_AliveCounter(uint8_t ConnID)
    {
        return moduleStatus[ConnID].AliveCounter;
    }
    TCPStatus PLCController::Get_TCPStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].tcpStatus;
    }
    DiodePresence PLCController::Get_DiodePresence(uint8_t ConnID)
    {
        return moduleStatus[ConnID].diodePresence;
    }
    CerValidationStatus PLCController::Get_CerValidationStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].cerValidationStatus;
    }
    SigValidationStatus PLCController::Get_SigValidationStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].sigValidationStatus;
    }
    EmaidValidationStatus PLCController::Get_EmaidValidationStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].emaidValidationStatus;
    }
    ACFallBackStatus PLCController::Get_ACFallBackStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].acFallBackStatus;
    }
    uint8_t PLCController::Get_SoftwareVersionMajor(uint8_t ConnID)
    {
        return moduleStatus[ConnID].SoftwareVersionMajor;
    }
    uint8_t PLCController::Get_SoftwareVersionMinor(uint8_t ConnID)
    {
        return moduleStatus[ConnID].SoftwareVersionMinor;
    }
    uint8_t PLCController::Get_SoftwareVersionPatch(uint8_t ConnID)
    {
        return moduleStatus[ConnID].SoftwareVersionPatch;
    }
    uint8_t PLCController::Get_SoftwareVersionConfig(uint8_t ConnID)
    {
        return moduleStatus[ConnID].SoftwareVersionConfig;
    }
    uint32_t PLCController::Get_SoftwareVersionHash(uint8_t ConnID)
    {
        return moduleStatus[ConnID].SoftwareVersionHash;
    }
    Category PLCController::Get_Category(uint8_t ConnID, uint8_t id)
    {
        if (id >= 4)
            return Category::CP_PP;
        return moduleStatus[ConnID].category[id];
    }
    uint8_t PLCController::Get_ErrorCode(uint8_t ConnID, uint8_t id)
    {
        if (id >= 4)
            return 0;
        return moduleStatus[ConnID].errorCode[id];
    }
    Severity PLCController::Get_Severity(uint8_t ConnID, uint8_t id)
    {
        if (id >= 4)
            return Severity::Minor;
        return moduleStatus[ConnID].severity[id];
    }
    Interface PLCController::Get_Interface(uint8_t ConnID, uint8_t id)
    {
        if (id >= 4)
            return Interface::Internal;
        return moduleStatus[ConnID].interface[id];
    }

    GPIOStatus PLCController::Get_GPIO2Status(uint8_t ConnID)
    {
        return moduleStatus[ConnID].gpio2Status;
    }
    GPIOStatus PLCController::Get_GPIO3Status(uint8_t ConnID)
    {
        return moduleStatus[ConnID].gpio3Status;
    }
    GPIOStatus PLCController::Get_GPIO4Status(uint8_t ConnID)
    {
        return moduleStatus[ConnID].gpio4Status;
    }
    GPIOStatus PLCController::Get_GPIO5Status(uint8_t ConnID)
    {
        return moduleStatus[ConnID].gpio5Status;
    }

    SLACLinkStatus PLCController::Get_SLACLinkStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].slacLinkStatus;
    }
    QcaSlacConfig PLCController::Get_QcaSlacConfig(uint8_t ConnID)
    {
        return moduleStatus[ConnID].qcaSlacConfig;
    }
    QcaSwEnum PLCController::Get_QcaSwEnum(uint8_t ConnID)
    {
        return moduleStatus[ConnID].qcaSwEnum;
    }
    uint8_t PLCController::Get_MeasuredAttenuation(uint8_t ConnID)
    {
        return moduleStatus[ConnID].measuredAttenuation;
    }
    uint8_t PLCController::Get_QcaSwVersionMajor(uint8_t ConnID)
    {
        return moduleStatus[ConnID].qcaSwVersionMajor;
    }
    uint8_t PLCController::Get_QcaSwVersionMinor(uint8_t ConnID)
    {
        return moduleStatus[ConnID].qcaSwVersionMinor;
    }
    uint8_t PLCController::Get_QcaSwVersionPatch(uint8_t ConnID)
    {
        return moduleStatus[ConnID].qcaSwVersionPatch;
    }
    float PLCController::Get_EVMaxCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evMaxCurrent;
    }
    float PLCController::Get_EVMaxPower(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evMaxPower;
    }
    float PLCController::Get_EVMaxVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evMaxVoltage;
    }
    float PLCController::Get_EVFullSoC(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evFullSoC;
    }
    float PLCController::Get_EVBulkSoC(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evBulkSoC;
    }
    float PLCController::Get_EVTargetCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evTargetCurrent;
    }
    float PLCController::Get_EVTargetVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evTargetVoltage;
    }
    float PLCController::Get_EVPreChargeVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evPreChargeVoltage;
    }
    uint8_t PLCController::Get_EVSoC(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evSoC;
    }
    EVChargingComplete PLCController::Get_EVChargingComplete(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evChargingComplete;
    }
    EVBulkChargingComplete PLCController::Get_EVBulkChargingComplete(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evBulkChargingComplete;
    }
    EVErrorCode PLCController::Get_EVErrorCode(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evErrorCode;
    }
    EVCabinConditioning PLCController::Get_EVCabinConditioning(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evCabinConditioning;
    }
    EVRESSConditioning PLCController::Get_EVRESSConditioning(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evRESSConditioning;
    }
    uint32_t PLCController::Get_EVTimeToBulkSoC(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evTimeToBulkSoC;
    }
    uint32_t PLCController::Get_EVTimeToFullSoC(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evTimeToFullSoC;
    }
    EVReady PLCController::Get_EVReady(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evReady;
    }
    EVRequestedETM PLCController::Get_EVRequestedETM(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evRequestedETM;
    }
    float PLCController::Get_EVEnergyCapacity(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evEnergyCapacity;
    }
    float PLCController::Get_EVEnergyRequest(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evEnergyRequest;
    }
    uint64_t PLCController::Get_EVMacAddress(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evMacAddress;
    }
    float PLCController::Get_EVACMinCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evACMinCurrent;
    }
    float PLCController::Get_EVACMaxCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evACMaxCurrent;
    }
    float PLCController::Get_EVACMaxVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evACMaxVoltage;
    }
    float PLCController::Get_EVEAmount(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evEAmount;
    }
    uint64_t PLCController::Get_EVEMAID0(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evEMAID0;
    }
    uint64_t PLCController::Get_EVEMAID1(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evEMAID1;
    }
    float PLCController::Get_EVSEMaxCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseMaxCurrent;
    }
    float PLCController::Get_EVSEMaxVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseMaxVoltage;
    }
    float PLCController::Get_EVSEMaxPower(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseMaxPower;
    }
    float PLCController::Get_EVSEEnergyToBeDelivered(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseEnergyToBeDelivered;
    }
    float PLCController::Get_EVSEMinCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseMinCurrent;
    }
    float PLCController::Get_EVSEMinVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseMinVoltage;
    }
    float PLCController::Get_EVSEPeakCurrentRipple(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evsePeakCurrentRipple;
    }
    float PLCController::Get_EVSECurrentRegulationTolerance(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseCurrentRegulationTolerance;
    }
    float PLCController::Get_EVSEPresentCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evsePresentCurrent;
    }
    float PLCController::Get_EVSEPresentVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evsePresentVoltage;
    }
    EVSEIsolationStatus PLCController::Get_EVSEIsolationStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseIsolationStatus;
    }
    bool PLCController::Get_EVSEFreeService(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseFreeService;
    }
    EVSEVoltageLimitAchieved PLCController::Get_EVSEVoltageLimitAchieved(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseVoltageLimitAchieved;
    }
    EVSEStatusCode PLCController::Get_EVSEStatusCode(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseStatusCode;
    }
    EVSECurrentLimitAchieved PLCController::Get_EVSECurrentLimitAchieved(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseCurrentLimitAchieved;
    }
    EVSEPowerLimitAchieved PLCController::Get_EVSEPowerLimitAchieved(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evsePowerLimitAchieved;
    }
    EVSEProcessing PLCController::Get_EVSEProcessingCA(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseProcessingCA;
    }
    EVSEProcessing PLCController::Get_EVSEProcessingCPD(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseProcessingCPD;
    }
    EVSEProcessing PLCController::Get_EVSEProcessingCC(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseProcessingCC;
    }
    EVSESupportedETM PLCController::Get_EVSESupportedETM(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseSupportedETM;
    }
    ControlPilotDuty PLCController::Get_EVSEControlPilotDuty(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseControlPilotDuty;
    }
    ControlPilotState PLCController::Get_EVSEControlPilotState(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseControlPilotState;
    }
    ProximityPinState PLCController::Get_EVSEProximityPinState(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseProximityPinState;
    }
    uint64_t PLCController::Get_EVSEDateTimeNow(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseDateTimeNow;
    }
    ConfigGPIO PLCController::Get_ConfigGPIO2(uint8_t ConnID)
    {
        return moduleStatus[ConnID].configGPIO2;
    }
    ConfigGPIO PLCController::Get_ConfigGPIO3(uint8_t ConnID)
    {
        return moduleStatus[ConnID].configGPIO3;
    }
    ConfigGPIO PLCController::Get_ConfigGPIO4(uint8_t ConnID)
    {
        return moduleStatus[ConnID].configGPIO4;
    }
    ConfigGPIO PLCController::Get_ConfigGPIO5(uint8_t ConnID)
    {
        return moduleStatus[ConnID].configGPIO5;
    }
    float PLCController::Get_EVSEACCurrentLimit(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseACCurrentLimit;
    }
    float PLCController::Get_EVSEACMaxCurrent(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseACMaxCurrent;
    }
    float PLCController::Get_EVSENominalVoltage(uint8_t ConnID)
    {
        return moduleStatus[ConnID].evseNominalVoltage;
    }
    uint8_t PLCController::Get_BootLoaderVersion(uint8_t ConnID)
    {
        return moduleStatus[ConnID].bootLoaderVersion;
    }
    uint64_t PLCController::Get_RTTLog(uint8_t ConnID)
    {
        return moduleStatus[ConnID].rttLog;
    }

    bool PLCController::Set_isActive(uint8_t ConnID, bool isActive)
    {
        moduleStatus[ConnID].isActive = isActive;
        return true;
    }
    bool PLCController::Set_EVSEMaxCurrent(uint8_t ConnID, float EVSEMaxCurrent)
    {
        moduleStatus[ConnID].evseMaxCurrent = EVSEMaxCurrent;
        return true;
    }
    bool PLCController::Set_EVSEMaxVoltage(uint8_t ConnID, float EVSEMaxVoltage)
    {
        moduleStatus[ConnID].evseMaxVoltage = EVSEMaxVoltage;
        return true;
    }
    bool PLCController::Set_EVSEMaxPower(uint8_t ConnID, float EVSEMaxPower)
    {
        moduleStatus[ConnID].evseMaxPower = EVSEMaxPower;
        return true;
    }
    bool PLCController::Set_EVSEEnergyToBeDelivered(uint8_t ConnID, float EVSEEnergyToBeDelivered)
    {
        moduleStatus[ConnID].evseEnergyToBeDelivered = EVSEEnergyToBeDelivered;
        return true;
    }
    bool PLCController::Set_EVSEMinCurrent(uint8_t ConnID, float EVSEMinCurrent)
    {
        moduleStatus[ConnID].evseMinCurrent = EVSEMinCurrent;
        return true;
    }
    bool PLCController::Set_EVSEMinVoltage(uint8_t ConnID, float EVSEMinVoltage)
    {
        moduleStatus[ConnID].evseMinVoltage = EVSEMinVoltage;
        return true;
    }
    bool PLCController::Set_EVSEPeakCurrentRipple(uint8_t ConnID, float EVSEPeakCurrentRipple)
    {
        moduleStatus[ConnID].evsePeakCurrentRipple = EVSEPeakCurrentRipple;
        return true;
    }
    bool PLCController::Set_EVSECurrentRegulationTolerance(uint8_t ConnID, float EVSECurrentRegulationTolerance)
    {
        moduleStatus[ConnID].evseCurrentRegulationTolerance = EVSECurrentRegulationTolerance;
        return true;
    }
    bool PLCController::Set_EVSEPresentCurrent(uint8_t ConnID, float EVSEPresentCurrent)
    {
        moduleStatus[ConnID].evsePresentCurrent = EVSEPresentCurrent;
        return true;
    }
    bool PLCController::Set_EVSEPresentVoltage(uint8_t ConnID, float EVSEPresentVoltage)
    {
        moduleStatus[ConnID].evsePresentVoltage = EVSEPresentVoltage;
        return true;
    }
    bool PLCController::Set_EVSEIsolationStatus(uint8_t ConnID, EVSEIsolationStatus EVSEIsolationStatus)
    {
        moduleStatus[ConnID].evseIsolationStatus = EVSEIsolationStatus;
        return true;
    }
    bool PLCController::Set_EVSEFreeService(uint8_t ConnID, bool EVSEFreeService)
    {
        moduleStatus[ConnID].evseFreeService = EVSEFreeService;
        return true;
    }
    bool PLCController::Set_EVSEVoltageLimitAchieved(uint8_t ConnID, EVSEVoltageLimitAchieved EVSEVoltageLimitAchieved)
    {
        moduleStatus[ConnID].evseVoltageLimitAchieved = EVSEVoltageLimitAchieved;
        return true;
    }
    bool PLCController::Set_EVSEStatusCode(uint8_t ConnID, EVSEStatusCode EVSEStatusCode)
    {
        moduleStatus[ConnID].evseStatusCode = EVSEStatusCode;
        return true;
    }
    bool PLCController::Set_EVSECurrentLimitAchieved(uint8_t ConnID, EVSECurrentLimitAchieved EVSECurrentLimitAchieved)
    {
        moduleStatus[ConnID].evseCurrentLimitAchieved = EVSECurrentLimitAchieved;
        return true;
    }
    bool PLCController::Set_EVSEPowerLimitAchieved(uint8_t ConnID, EVSEPowerLimitAchieved EVSEPowerLimitAchieved)
    {
        moduleStatus[ConnID].evsePowerLimitAchieved = EVSEPowerLimitAchieved;
        return true;
    }
    bool PLCController::Set_EVSEProcessingCA(uint8_t ConnID, EVSEProcessing EVSEProcessingCA)
    {
        moduleStatus[ConnID].evseProcessingCA = EVSEProcessingCA;
        return true;
    }
    bool PLCController::Set_EVSEProcessingCPD(uint8_t ConnID, EVSEProcessing EVSEProcessingCPD)
    {
        moduleStatus[ConnID].evseProcessingCPD = EVSEProcessingCPD;
        return true;
    }
    bool PLCController::Set_EVSEProcessingCC(uint8_t ConnID, EVSEProcessing EVSEProcessingCC)
    {
        moduleStatus[ConnID].evseProcessingCC = EVSEProcessingCC;
        return true;
    }
    bool PLCController::Set_EVSESupportedETM(uint8_t ConnID, EVSESupportedETM EVSESupportedETM)
    {
        moduleStatus[ConnID].evseSupportedETM = EVSESupportedETM;
        return true;
    }
    bool PLCController::Set_EVSEControlPilotDuty(uint8_t ConnID, ControlPilotDuty EVSEControlPilotDuty)
    {
        moduleStatus[ConnID].evseControlPilotDuty = EVSEControlPilotDuty;
        return true;
    }
    bool PLCController::Set_EVSEControlPilotState(uint8_t ConnID, ControlPilotState EVSEControlPilotState)
    {
        moduleStatus[ConnID].evseControlPilotState = EVSEControlPilotState;
        return true;
    }
    bool PLCController::Set_EVSEProximityPinState(uint8_t ConnID, ProximityPinState EVSEProximityPinState)
    {
        moduleStatus[ConnID].evseProximityPinState = EVSEProximityPinState;
        return true;
    }
    bool PLCController::Set_EVSEDateTimeNow(uint8_t ConnID, uint64_t EVSEDateTimeNow)
    {
        moduleStatus[ConnID].evseDateTimeNow = EVSEDateTimeNow;
        return true;
    }
    bool PLCController::Set_ConfigGPIO2(uint8_t ConnID, ConfigGPIO ConfigGPIO2)
    {
        moduleStatus[ConnID].configGPIO2 = ConfigGPIO2;
        return true;
    }
    bool PLCController::Set_ConfigGPIO3(uint8_t ConnID, ConfigGPIO ConfigGPIO3)
    {
        moduleStatus[ConnID].configGPIO3 = ConfigGPIO3;
        return true;
    }
    bool PLCController::Set_ConfigGPIO4(uint8_t ConnID, ConfigGPIO ConfigGPIO4)
    {
        moduleStatus[ConnID].configGPIO4 = ConfigGPIO4;
        return true;
    }
    bool PLCController::Set_ConfigGPIO5(uint8_t ConnID, ConfigGPIO ConfigGPIO5)
    {
        moduleStatus[ConnID].configGPIO5 = ConfigGPIO5;
        return true;
    }
    bool PLCController::Set_EVSEACCurrentLimit(uint8_t ConnID, float EVSEACCurrentLimit)
    {
        moduleStatus[ConnID].evseACCurrentLimit = EVSEACCurrentLimit;
        return true;
    }
    bool PLCController::Set_EVSEACMaxCurrent(uint8_t ConnID, float EVSEACMaxCurrent)
    {
        moduleStatus[ConnID].evseACMaxCurrent = EVSEACMaxCurrent;
        return true;
    }
    bool PLCController::Set_EVSENominalVoltage(uint8_t ConnID, float EVSENominalVoltage)
    {
        moduleStatus[ConnID].evseNominalVoltage = EVSENominalVoltage;
        return true;
    }

    bool PLCController::Set_EVSEContactorState(uint8_t ConnID, uint8_t state)
    {
        return true;
    }
    bool PLCController::Get_EVSEContactorState(uint8_t ConnID)
    {
        return true;
    }
    ModuleStatus PLCController::GetModuleStatus(uint8_t ConnID)
    {
        return moduleStatus[ConnID];
    }
    void PLCController::SetModuleStatusToSNA(uint8_t ConnID)
    {
        // ChargeInfo
        moduleStatus[ConnID].controlPilotDuty = ControlPilotDuty::SNA;
        moduleStatus[ConnID].controlPilotState = ControlPilotState::SNA;
        moduleStatus[ConnID].proximityPinState = ProximityPinState::SNA;
        moduleStatus[ConnID].stateMachineState = StateMachineState::SNA;
        moduleStatus[ConnID].actualChargeProtocol = ActualChargeProtocol::SNA;
        moduleStatus[ConnID].AliveCounter = Constants::AliveCounter_SNA;
        moduleStatus[ConnID].tcpStatus = TCPStatus::SNA;
        moduleStatus[ConnID].diodePresence = DiodePresence::SNA;
        moduleStatus[ConnID].cerValidationStatus = CerValidationStatus::SNA;
        moduleStatus[ConnID].sigValidationStatus = SigValidationStatus::SNA;
        moduleStatus[ConnID].emaidValidationStatus = EmaidValidationStatus::SNA;
        moduleStatus[ConnID].acFallBackStatus = ACFallBackStatus::Disabled;

        // ErrorCodes

        moduleStatus[ConnID].category[0] = Category::CP_PP;
        moduleStatus[ConnID].category[1] = Category::CP_PP;
        moduleStatus[ConnID].category[2] = Category::CP_PP;
        moduleStatus[ConnID].category[3] = Category::CP_PP;
        moduleStatus[ConnID].errorCode[0] = 0;
        moduleStatus[ConnID].errorCode[1] = 0;
        moduleStatus[ConnID].errorCode[2] = 0;
        moduleStatus[ConnID].errorCode[3] = 0;
        moduleStatus[ConnID].severity[0] = Severity::Minor;
        moduleStatus[ConnID].severity[1] = Severity::Minor;
        moduleStatus[ConnID].severity[2] = Severity::Minor;
        moduleStatus[ConnID].severity[3] = Severity::Minor;
        moduleStatus[ConnID].interface[0] = Interface::Internal;
        moduleStatus[ConnID].interface[1] = Interface::Internal;
        moduleStatus[ConnID].interface[2] = Interface::Internal;
        moduleStatus[ConnID].interface[3] = Interface::Internal;

        // HardwareStatus
        moduleStatus[ConnID].gpio2Status = GPIOStatus::SNA;
        moduleStatus[ConnID].gpio3Status = GPIOStatus::SNA;
        moduleStatus[ConnID].gpio4Status = GPIOStatus::SNA;
        moduleStatus[ConnID].gpio5Status = GPIOStatus::SNA;

        // SLACInfo
        moduleStatus[ConnID].slacLinkStatus = SLACLinkStatus::SNA;
        moduleStatus[ConnID].qcaSlacConfig = QcaSlacConfig::SNA;
        moduleStatus[ConnID].qcaSwEnum = QcaSwEnum::QcaSwEnum_Unkown;
        moduleStatus[ConnID].measuredAttenuation = Constants::MeasuredAttenuation_SNA;
        moduleStatus[ConnID].qcaSwVersionMajor = 0;
        moduleStatus[ConnID].qcaSwVersionMinor = 0;
        moduleStatus[ConnID].qcaSwVersionPatch = 0;

        // DebugInfo
        moduleStatus[ConnID].pilotStatesStatus = PilotStatesStatus::SNA;
        moduleStatus[ConnID].slacStatus = SLACStatus::SNA;
        moduleStatus[ConnID].dinStatus = V2GStatusDIN::SNA;
        moduleStatus[ConnID].proximityPilotVoltage = Constants::ProximityPilotVoltage_SNA;
        moduleStatus[ConnID].controlPilotVoltage = Constants::ControlPilotVoltage_SNA;
        moduleStatus[ConnID].dcISO2_1_Status = V2GStatusDCISO2_1::SNA;
        moduleStatus[ConnID].acISO2_1_Status = V2GStatusACISO2_1::SNA;

        // EVDCMaxLimits
        moduleStatus[ConnID].evMaxCurrent = static_cast<float>(Constants::EVMaxCurrent_SNA * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evMaxPower = static_cast<float>(Constants::EVMaxPower_SNA * Constants::POWER_SCALE);
        moduleStatus[ConnID].evMaxVoltage = static_cast<float>(Constants::EVMaxVoltage_SNA * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evFullSoC = static_cast<float>(Constants::EVFullSoC_SNA);
        moduleStatus[ConnID].evBulkSoC = static_cast<float>(Constants::EVBulkSoC_SNA);

        // EVDCChargeTargets
        moduleStatus[ConnID].evTargetCurrent = static_cast<float>(Constants::EVTargetCurrent_SNA * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evTargetVoltage = static_cast<float>(Constants::EVTargetVoltage_SNA * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evPreChargeVoltage = static_cast<float>(Constants::EVPreChargeVoltage_SNA * Constants::VOLTAGE_SCALE);

        // EVStatusDisplay
        moduleStatus[ConnID].evSoC = static_cast<uint8_t>(Constants::EVSoC_SNA);
        moduleStatus[ConnID].evChargingComplete = EVChargingComplete::SNA;
        moduleStatus[ConnID].evBulkChargingComplete = EVBulkChargingComplete::SNA;
        moduleStatus[ConnID].evErrorCode = EVErrorCode::SNA;
        moduleStatus[ConnID].evCabinConditioning = EVCabinConditioning::SNA;
        moduleStatus[ConnID].evRESSConditioning = EVRESSConditioning::SNA;
        moduleStatus[ConnID].evTimeToBulkSoC = static_cast<uint32_t>(Constants::EVTimeToBulkSoC_SNA);
        moduleStatus[ConnID].evTimeToFullSoC = static_cast<uint32_t>(Constants::EVTimeToFullSoC_SNA);
        moduleStatus[ConnID].evReady = EVReady::SNA;
        moduleStatus[ConnID].evRequestedETM = EVRequestedETM::SNA;

        // EVDCEnergyLimits
        moduleStatus[ConnID].evEnergyCapacity = Constants::EVEnergyCapacity_SNA * Constants::ENERGY_SCALE;
        moduleStatus[ConnID].evEnergyRequest = Constants::EVEnergyRequest_SNA * Constants::ENERGY_SCALE;

        // EVMacAddress
        moduleStatus[ConnID].evMacAddress = 0xFFFFFFFFFFFF;

        // EVACChargeControl
        moduleStatus[ConnID].evACMinCurrent = Constants::EVACMinCurrent_SNA * Constants::CURRENT_SCALE;
        moduleStatus[ConnID].evACMaxCurrent = Constants::EVACMaxCurrent_SNA * Constants::CURRENT_SCALE;
        moduleStatus[ConnID].evACMaxVoltage = Constants::EVACMaxVoltage_SNA * Constants::VOLTAGE_SCALE;
        moduleStatus[ConnID].evEAmount = Constants::EVEAmount_SNA;

        // EVEMAID
        moduleStatus[ConnID].evEMAID0 = 0xFFFFFFFFFFFF;
        moduleStatus[ConnID].evEMAID1 = 0xFFFFFFFFFFFF;

        // EVSEDCMaxLimits
        moduleStatus[ConnID].evseMinCurrent = static_cast<float>(Constants::EVSEMaxCurrent_SNA * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evseMinVoltage = static_cast<float>(Constants::EVSEMaxVoltage_SNA * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evsePeakCurrentRipple = static_cast<float>(Constants::EVSEMaxPower_SNA * Constants::POWER_SCALE);
        moduleStatus[ConnID].evseCurrentRegulationTolerance = static_cast<float>(Constants::EVSEEnergyToBeDelivered_SNA * Constants::ENERGY_SCALE);

        // EVSEDCRegulationLimits
        moduleStatus[ConnID].evseMinCurrent = static_cast<float>(Constants::EVSEMinCurrent_SNA * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evseMinVoltage = static_cast<float>(Constants::EVSEMinVoltage_SNA * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evsePeakCurrentRipple = static_cast<float>(Constants::EVSEPeakCurrentRipple_SNA * Constants::CURRENT_REGULATION_SCALE);
        moduleStatus[ConnID].evseCurrentRegulationTolerance = static_cast<float>(Constants::EVSECurrentRegulationTolerance_SNA * Constants::CURRENT_REGULATION_SCALE);

        // EVSEStatus
        // moduleStatus[ConnID].EVSEPresentCurrent_ = 0.0f;
        // moduleStatus[ConnID].EVSEPresentVoltage_ = 0.0f;
        moduleStatus[ConnID].evsePresentCurrent = static_cast<float>(Constants::EVSEPresentCurrent_SNA * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evsePresentVoltage = static_cast<float>(Constants::EVSEPresentVoltage_SNA * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evseIsolationStatus = EVSEIsolationStatus::SNA;
        moduleStatus[ConnID].evseFreeService = false;
        moduleStatus[ConnID].evseVoltageLimitAchieved = EVSEVoltageLimitAchieved::SNA;
        moduleStatus[ConnID].evseStatusCode = EVSEStatusCode::EVSE_CustomerNotReady;
        moduleStatus[ConnID].evseCurrentLimitAchieved = EVSECurrentLimitAchieved::SNA;
        moduleStatus[ConnID].evsePowerLimitAchieved = EVSEPowerLimitAchieved::SNA;
        moduleStatus[ConnID].evseProcessingCA = EVSEProcessing::SNA;
        moduleStatus[ConnID].evseProcessingCPD = EVSEProcessing::SNA;
        moduleStatus[ConnID].evseProcessingCC = EVSEProcessing::SNA;
        moduleStatus[ConnID].evseSupportedETM = EVSESupportedETM::SNA;

        // EVSEPlugStatus
        moduleStatus[ConnID].evseControlPilotDuty = static_cast<ControlPilotDuty>(0);
        moduleStatus[ConnID].evseControlPilotState = ControlPilotState::SNA;
        moduleStatus[ConnID].evseProximityPinState = ProximityPinState::SNA;

        // HardwareConfig
        moduleStatus[ConnID].configGPIO2 = ConfigGPIO::SNA;
        moduleStatus[ConnID].configGPIO3 = ConfigGPIO::SNA;
        moduleStatus[ConnID].configGPIO4 = ConfigGPIO::SNA;
        moduleStatus[ConnID].configGPIO5 = ConfigGPIO::SNA;

        // EVSEACChargeControl
        moduleStatus[ConnID].evseACCurrentLimit = static_cast<float>(EVSECurrentLimit::SNA) * Constants::CURRENT_SCALE;
        moduleStatus[ConnID].evseACMaxCurrent = static_cast<float>(Constants::EVSEACMaxCurrent_SNA) * Constants::CURRENT_SCALE;
        moduleStatus[ConnID].evseNominalVoltage = static_cast<float>(Constants::EVSENominalVoltage_SNA) * Constants::VOLTAGE_SCALE;
    }

    bool PLCController::SendEVSEDateTime(uint8_t ConnID)
    {
        // return SendData((static_cast<uint32_t>(PLC_ID::EVSEDateTime) | (static_cast<uint32_t>(ConnID) << 12)), moduleStatus[ConnID].EVSEDateTimeNow_);
        // SendData((772 | (static_cast<uint32_t>(ConnID) << 12)), 0);
        return SendData((static_cast<uint32_t>(PLC_ID::EVSEDateTime) | (static_cast<uint32_t>(ConnID) << 12)), 0);
    }

    bool PLCController::SendEVSEStatus(uint8_t ConnID)
    {
        uint64_t data = 0;
        EVSEStatus EVSEStatus = {};
        EVSEStatus.EVSEPresentCurrent = static_cast<uint16_t>((moduleStatus[ConnID].evsePresentCurrent + Constants::CURRENT_OFFSET) / Constants::CURRENT_SCALE);
        EVSEStatus.EVSEPresentVoltage = static_cast<uint16_t>(moduleStatus[ConnID].evsePresentVoltage / Constants::VOLTAGE_SCALE);
        EVSEStatus.EVSEIsolationStatus = static_cast<uint8_t>(moduleStatus[ConnID].evseIsolationStatus);
        EVSEStatus.EVSEFreeService = static_cast<uint8_t>(moduleStatus[ConnID].evseFreeService);
        EVSEStatus.EVSEVoltageLimitAchieved = static_cast<uint8_t>(moduleStatus[ConnID].evseVoltageLimitAchieved);
        EVSEStatus.EVSEStatusCode = static_cast<uint8_t>(moduleStatus[ConnID].evseStatusCode);
        EVSEStatus.EVSECurrentLimitAchieved = static_cast<uint8_t>(moduleStatus[ConnID].evseCurrentLimitAchieved);
        EVSEStatus.EVSEPowerLimitAchieved = static_cast<uint8_t>(moduleStatus[ConnID].evsePowerLimitAchieved);
        EVSEStatus.EVSEProcessingCA = static_cast<uint8_t>(moduleStatus[ConnID].evseProcessingCA);
        EVSEStatus.EVSEProcessingCPD = static_cast<uint8_t>(moduleStatus[ConnID].evseProcessingCPD);
        EVSEStatus.EVSEProcessingCC = static_cast<uint8_t>(moduleStatus[ConnID].evseProcessingCC);
        EVSEStatus.EVSESupportedETM = static_cast<uint8_t>(moduleStatus[ConnID].evseSupportedETM);
        std::memcpy(&data, &EVSEStatus, sizeof(EVSEStatus));
        return SendData((static_cast<uint32_t>(PLC_ID::EVSEStatus) | (static_cast<uint32_t>(ConnID) << 12)), data);
    }
    bool PLCController::SendEVSEPlugStatus(uint8_t ConnID)
    {
        uint64_t data = 0;
        EVSEPlugStatus EVSEPlugStatus = {};
#if 0
         EVSEPlugStatus.EVSEControlPilotDuty = static_cast<uint8_t>(moduleStatus[ConnID].EVSEcontrolPilotDuty);
         EVSEPlugStatus.EVSEControlPilotState = static_cast<uint8_t>(moduleStatus[ConnID].EVSEcontrolPilotState);
         EVSEPlugStatus.EVSEProximityPinState = static_cast<uint8_t>(moduleStatus[ConnID].EVSEproximityPinState);
#else
        EVSEPlugStatus.EVSEControlPilotDuty = static_cast<uint8_t>(PLCModule::ControlPilotDuty::SNA);
        EVSEPlugStatus.EVSEControlPilotState = static_cast<uint8_t>(PLCModule::ControlPilotState::SNA);
        EVSEPlugStatus.EVSEProximityPinState = static_cast<uint8_t>(PLCModule::ProximityPinState::SNA);
#endif
        std::memcpy(&data, &EVSEPlugStatus, sizeof(EVSEPlugStatus));
        return SendData((static_cast<uint32_t>(PLC_ID::EVSEPlugStatus) | (static_cast<uint32_t>(ConnID) << 12)), data);
    }
    bool PLCController::SendHardwareConfig(uint8_t ConnID)
    {
        uint64_t data = 0;
        HardwareConfig HardwareConfig = {};
        HardwareConfig.ConfigGPIO2 = static_cast<uint8_t>(moduleStatus[ConnID].configGPIO2);
        HardwareConfig.ConfigGPIO3 = static_cast<uint8_t>(moduleStatus[ConnID].configGPIO3);
        HardwareConfig.ConfigGPIO4 = static_cast<uint8_t>(moduleStatus[ConnID].configGPIO4);
        HardwareConfig.ConfigGPIO5 = static_cast<uint8_t>(moduleStatus[ConnID].configGPIO5);
        std::memcpy(&data, &HardwareConfig, sizeof(HardwareConfig));
        return SendData((static_cast<uint32_t>(PLC_ID::HardwareConfig) | (static_cast<uint32_t>(ConnID) << 12)), data);
    }
    bool PLCController::SendEVSEDCMaxLimits(uint8_t ConnID)
    {
        uint64_t data = 0;
        EVSEDCMaxLimits EVSEDCMaxLimits = {};
        uint32_t id = (static_cast<uint32_t>(PLC_ID::EVSEDCMaxLimits) | (static_cast<uint32_t>(ConnID) << 12));
        EVSEDCMaxLimits.EVSEMaxCurrent = static_cast<uint16_t>(moduleStatus[ConnID].evseMaxCurrent / Constants::CURRENT_SCALE);
        EVSEDCMaxLimits.EVSEMaxPower = static_cast<uint16_t>(moduleStatus[ConnID].evseMaxVoltage / Constants::VOLTAGE_SCALE);
        EVSEDCMaxLimits.EVSEMaxVoltage = static_cast<uint16_t>(moduleStatus[ConnID].evseMaxPower / Constants::POWER_SCALE);
        EVSEDCMaxLimits.EVSEEnergyToBeDelivered = static_cast<uint16_t>(moduleStatus[ConnID].evseEnergyToBeDelivered / Constants::ENERGY_SCALE);
        std::memcpy(&data, &EVSEDCMaxLimits, sizeof(EVSEDCMaxLimits));
        return SendData(id, data);
    }
    bool PLCController::SendEVSEDCRegulationLimits(uint8_t ConnID)
    {
        uint64_t data = 0;
        EVSEDCRegulationLimits EVSEDCRegulationLimits = {};
        // EVSEDCRegulationLimits.EVSEMinCurrent = static_cast<uint16_t>((moduleStatus[ConnID].evseMinCurrent + Constants::CURRENT_OFFSET) / Constants::CURRENT_SCALE);
        EVSEDCRegulationLimits.EVSEMinCurrent = 0xFFFF;
        EVSEDCRegulationLimits.EVSEMinVoltage = static_cast<uint16_t>(moduleStatus[ConnID].evseMinVoltage / Constants::VOLTAGE_SCALE);
        EVSEDCRegulationLimits.EVSEPeakCurrentRipple = static_cast<uint16_t>(moduleStatus[ConnID].evsePeakCurrentRipple / Constants::CURRENT_REGULATION_SCALE);
        EVSEDCRegulationLimits.EVSECurrentRegulationTolerance = static_cast<uint16_t>(moduleStatus[ConnID].evseCurrentRegulationTolerance / Constants::CURRENT_REGULATION_SCALE);
        std::memcpy(&data, &EVSEDCRegulationLimits, sizeof(EVSEDCRegulationLimits));
        return SendData((static_cast<uint32_t>(PLC_ID::EVSEDCRegulationLimits) | (static_cast<uint32_t>(ConnID) << 12)), data);
    }
    bool PLCController::SendEVSEACChargeControl(uint8_t ConnID)
    {
        uint64_t data = 0;
        EVSEACChargeControl EVSEACChargeControl = {};
        EVSEACChargeControl.EVSEACCurrentLimit = static_cast<uint16_t>(moduleStatus[ConnID].evseACCurrentLimit / Constants::CURRENT_SCALE);
        EVSEACChargeControl.EVSEACMaxCurrent = static_cast<uint16_t>(moduleStatus[ConnID].evseACMaxCurrent / Constants::CURRENT_SCALE);
        EVSEACChargeControl.EVSENominalVoltage = static_cast<uint16_t>(moduleStatus[ConnID].evseNominalVoltage / Constants::VOLTAGE_SCALE);
        std::memcpy(&data, &EVSEACChargeControl, sizeof(EVSEACChargeControl));
        return SendData((static_cast<uint32_t>(PLC_ID::EVSEACChargeControl) | (static_cast<uint32_t>(ConnID) << 12)), data);
    }

    void PLCController::GetEnumInterfaceString(Interface interface, char *buffer)
    {
        if (interface == Interface::Internal)
        {
            std::strcpy(buffer, "Internal");
        }
        else
        {
            std::strcpy(buffer, "Customer");
        }
    }

    void PLCController::GetEnumSeverityString(Severity severity, char *buffer)
    {
        switch (severity)
        {
        case Severity::Minor:
            std::strcpy(buffer, "Minor");
            break;
        case Severity::Major:
            std::strcpy(buffer, "Major");
            break;
        case Severity::Critical:
            std::strcpy(buffer, "Critical");
            break;
        default:
            std::strcpy(buffer, "Unknown");
            break;
        }
    }

    void PLCController::GetEnumCategoryString(Category category, char *buffer)
    {
        switch (category)
        {
        case Category::CP_PP:
            std::strcpy(buffer, "CP_PP");
            break;
        case Category::EEPROM:
            std::strcpy(buffer, "EEPROM");
            break;
        case Category::CAN:
            std::strcpy(buffer, "CAN");
            break;
        case Category::SLAC_SVC:
            std::strcpy(buffer, "SLAC_SVC");
            break;
        case Category::SDP:
            std::strcpy(buffer, "SDP");
            break;
        case Category::RESERVED_5:
            std::strcpy(buffer, "RESERVED_5");
            break;
        case Category::PILOT_SM:
            std::strcpy(buffer, "PILOT_SM");
            break;
        case Category::RESERVED_7:
            std::strcpy(buffer, "RESERVED_7");
            break;
        case Category::HLC_SVC:
            std::strcpy(buffer, "HLC_SVC");
            break;
        case Category::WATCHDOG:
            std::strcpy(buffer, "WATCHDOG");
            break;
        case Category::IP_STACK:
            std::strcpy(buffer, "IP_STACK");
            break;
        default:
            std::strcpy(buffer, "Unknown");
            break;
        }
    }

    void PLCController::GetErrorCodeString(Category category, uint8_t errorCode, char *buffer)
    {
        switch (category)
        {
        case Category::CP_PP:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_CP_PP::UNPLUGGED):
                std::strcpy(buffer, "UNPLUGGED");
                break;
            case static_cast<uint8_t>(EC_CP_PP::RESET_ERROR_CODE):
                std::strcpy(buffer, "RESET_ERROR_CODE");
                break;
            case static_cast<uint8_t>(EC_CP_PP::ADC_READING_ERROR):
                std::strcpy(buffer, "ADC_READING_ERROR");
                break;
            case static_cast<uint8_t>(EC_CP_PP::PWM_READING_ERROR):
                std::strcpy(buffer, "PWM_READING_ERROR");
                break;
            case static_cast<uint8_t>(EC_CP_PP::CP_PP_STATE_ERROR_FAILED):
                std::strcpy(buffer, "CP_PP_STATE_ERROR_FAILED");
                break;
            case static_cast<uint8_t>(EC_CP_PP::T_CONN_MAX_COMM_TIMER_EXPIRED):
                std::strcpy(buffer, "T_CONN_MAX_COMM_TIMER_EXPIRED");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::EEPROM:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_EEPROM::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_EEPROM::EEPROM_FAILED):
                std::strcpy(buffer, "EEPROM_FAILED");
                break;
            case static_cast<uint8_t>(EC_EEPROM::EEPROM_WRITE_FAILED):
                std::strcpy(buffer, "EEPROM_WRITE_FAILED");
                break;
            case static_cast<uint8_t>(EC_EEPROM::INVALID_MAC_ADDRESS):
                std::strcpy(buffer, "INVALID_MAC_ADDRESS");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::CAN:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_CAN::INVALID):
                std::strcpy(buffer, "INVALID");
                break;
            case static_cast<uint8_t>(EC_CAN::STATE_MACHINE_STATE_RANGE_OVERFLOW):
                std::strcpy(buffer, "STATE_MACHINE_STATE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::PROXIMITY_PIN_STATE_RANGE_OVERFLOW):
                std::strcpy(buffer, "PROXIMITY_PIN_STATE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::ACTUAL_CHARGE_PROTOCOL_RANGE_OVERFLOW):
                std::strcpy(buffer, "ACTUAL_CHARGE_PROTOCOL_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::TCP_STATUS_RANGE_OVERFLOW):
                std::strcpy(buffer, "TCP_STATUS_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::CONTROL_PILOT_DUTY_CYCLE_RANGE_OVERFLOW):
                std::strcpy(buffer, "CONTROL_PILOT_DUTY_CYCLE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::CONTROL_PILOT_STATE_RANGE_OVERFLOW):
                std::strcpy(buffer, "CONTROL_PILOT_STATE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_COMPATABLE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_COMPATABLE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::DIODE_PRESENCE_RANGE_OVERFLOW):
                std::strcpy(buffer, "DIODE_PRESENCE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::SLAC_STATE_RANGE_OVERFLOW):
                std::strcpy(buffer, "SLAC_STATE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::LINK_STATUS_RANGE_OVERFLOW):
                std::strcpy(buffer, "LINK_STATUS_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::MEASUREMENT_ATTENUATION_RANGE_OVERFLOW):
                std::strcpy(buffer, "MEASUREMENT_ATTENUATION_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_MAX_CURRENT_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_MAX_CURRENT_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_MAX_VOLTAGE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_MAX_VOLTAGE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_MAX_POWER_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_MAX_POWER_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_FULL_SOC_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_FULL_SOC_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_BULK_SOC_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_BULK_SOC_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_TARGET_CURRENT_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_TARGET_CURRENT_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_TARGET_VOLTAGE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_TARGET_VOLTAGE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_PRE_CHARGE_VOLTAGE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_PRE_CHARGE_VOLTAGE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_SOC_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_SOC_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_CHARGING_COMPLETE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_CHARGING_COMPLETE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_BULK_CHARGING_COMPLETE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_BULK_CHARGING_COMPLETE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_CABIN_CONDITIONING_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_CABIN_CONDITIONING_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_ERROR_CODE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_ERROR_CODE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_TIME_TO_BULK_SOC_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_TIME_TO_BULK_SOC_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_TIME_TO_FULL_SOC_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_TIME_TO_FULL_SOC_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_RESS_CONDITIONING_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_RESS_CONDITIONING_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_READY_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_READY_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_ENERGY_CAPACITY_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_ENERGY_CAPACITY_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_ENERGY_REQUEST_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_ENERGY_REQUEST_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_MAC_ADDRESS_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_MAC_ADDRESS_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_STATUS_CODE_SNA_ERROR):
                std::strcpy(buffer, "EVSE_STATUS_CODE_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_MAX_CURRENT_SNA_ERROR):
                std::strcpy(buffer, "EVSE_MAX_CURRENT_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_MAX_VOLTAGE_SNA_ERROR):
                std::strcpy(buffer, "EVSE_MAX_VOLTAGE_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_MIN_CURRENT_SNA_ERROR):
                std::strcpy(buffer, "EVSE_MIN_CURRENT_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_MIN_VOLTAGE_SNA_ERROR):
                std::strcpy(buffer, "EVSE_MIN_VOLTAGE_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_PEAK_CURRENT_RIPPLE_SNA_ERROR):
                std::strcpy(buffer, "EVSE_PEAK_CURRENT_RIPPLE_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_PRESENT_VOLTAGE_SNA_ERROR):
                std::strcpy(buffer, "EVSE_PRESENT_VOLTAGE_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_PRESENT_CURRENT_SNA_ERROR):
                std::strcpy(buffer, "EVSE_PRESENT_CURRENT_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_VOLTAGE_LIMIT_ACHIEVED_SNA_ERROR):
                std::strcpy(buffer, "EVSE_VOLTAGE_LIMIT_ACHIEVED_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_CURRENT_LIMIT_ACHIEVED_SNA_ERROR):
                std::strcpy(buffer, "EVSE_CURRENT_LIMIT_ACHIEVED_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_POWER_LIMIT_ACHIEVED_SNA_ERROR):
                std::strcpy(buffer, "EVSE_POWER_LIMIT_ACHIEVED_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_MAX_POWER_SNA_ERROR):
                std::strcpy(buffer, "EVSE_MAX_POWER_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::DO_NOT_UPDATE_CME_ID_WHEN_FRAMETYPE_IS_STANDARD):
                std::strcpy(buffer, "DO_NOT_UPDATE_CME_ID_WHEN_FRAMETYPE_IS_STANDARD");
                break;
            case static_cast<uint8_t>(EC_CAN::STANDARD_FRAMETYPE_NOT_POSSIBLE_WHEN_CME_ID_IS_NOT_ZERO):
                std::strcpy(buffer, "STANDARD_FRAMETYPE_NOT_POSSIBLE_WHEN_CME_ID_IS_NOT_ZERO");
                break;
            case static_cast<uint8_t>(EC_CAN::PILOT_STATES_STATUS_RANGE_OVERFLOW):
                std::strcpy(buffer, "PILOT_STATES_STATUS_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::HLC_STATUS_RANGE_OVERFLOW):
                std::strcpy(buffer, "HLC_STATUS_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::V2G_STATUS_DIN_RANGE_OVERFLOW):
                std::strcpy(buffer, "V2G_STATUS_DIN_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::V2G_STATUS_DC_ISO_RANGE_OVERFLOW):
                std::strcpy(buffer, "V2G_STATUS_DC_ISO_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::SLAC_STATUS_RANGE_OVERFLOW):
                std::strcpy(buffer, "SLAC_STATUS_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::PROXIMITY_PILOT_VOLTAGE_RANGE_OVERFLOW):
                std::strcpy(buffer, "PROXIMITY_PILOT_VOLTAGE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_AC_MIN_CURRENT_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_AC_MIN_CURRENT_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_AC_MAX_CURRENT_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_AC_MAX_CURRENT_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_AC_MAX_VOLTAGE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_AC_MAX_VOLTAGE_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_AC_EAMOUNT_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_AC_EAMOUNT_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::V2G_STATUS_AC_ISO_RANGE_OVERFLOW):
                std::strcpy(buffer, "V2G_STATUS_AC_ISO_RANGE_OVERFLOW");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_AC_NOMINAL_VOLTAGE_SNA_ERROR):
                std::strcpy(buffer, "EVSE_AC_NOMINAL_VOLTAGE_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EVSE_AC_MAX_CURRENT_SNA_ERROR):
                std::strcpy(buffer, "EVSE_AC_MAX_CURRENT_SNA_ERROR");
                break;
            case static_cast<uint8_t>(EC_CAN::EV_ENERGY_TRANSFER_MODE_RANGE_OVERFLOW):
                std::strcpy(buffer, "EV_ENERGY_TRANSFER_MODE_RANGE_OVERFLOW");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::SLAC_SVC:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_SLAC_SVC::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_SLAC_SVC::TRAFFIC_GENERATION_STATE_RUNNING):
                std::strcpy(buffer, "TRAFFIC_GENERATION_STATE_RUNNING");
                break;
            case static_cast<uint8_t>(EC_SLAC_SVC::QCA_SLACCONFIG_RANGE_OVERFLOW):
                std::strcpy(buffer, "QCA_SLACCONFIG_RANGE_OVERFLOW");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::SDP:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_SDP::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_SDP::SDP_ERROR):
                std::strcpy(buffer, "SDP_ERROR");
                break;
            case static_cast<uint8_t>(EC_SDP::SDP_REQUEST_INVALID):
                std::strcpy(buffer, "SDP_REQUEST_INVALID");
                break;
            case static_cast<uint8_t>(EC_SDP::SDP_REQUEST_TIMEOUT):
                std::strcpy(buffer, "SDP_REQUEST_TIMEOUT");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::RESERVED_5:
        {
        }
        break;
        case Category::PILOT_SM:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_PILOT_SM::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_PILOT_SM::ERROR):
                std::strcpy(buffer, "ERROR");
                break;
            case static_cast<uint8_t>(EC_PILOT_SM::CP_NOT_B_OR_C_OR_D_DURING_HLC):
                std::strcpy(buffer, "CP_NOT_B_OR_C_OR_D_DURING_HLC");
                break;
            case static_cast<uint8_t>(EC_PILOT_SM::TCP_NOT_CONNECTED_AFTER_INIT):
                std::strcpy(buffer, "TCP_NOT_CONNECTED_AFTER_INIT");
                break;
            case static_cast<uint8_t>(EC_PILOT_SM::PP_TYPE1_BUTTON_PRESSED_OR_INVALID):
                std::strcpy(buffer, "PP_TYPE1_BUTTON_PRESSED_OR_INVALID");
                break;
            case static_cast<uint8_t>(EC_PILOT_SM::HLC_CHARGING_FAILED):
                std::strcpy(buffer, "HLC_CHARGING_FAILED");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::RESERVED_7:
        {
        }
        break;
        case Category::HLC_SVC:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_HLC_SVC::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::EMERGENCY):
                std::strcpy(buffer, "EMERGENCY");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::SOCKET_FAIL):
                std::strcpy(buffer, "SOCKET_FAIL");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::PORT_BIND_FAIL):
                std::strcpy(buffer, "PORT_BIND_FAIL");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::SOCKET_START_LISTENING_FAILED):
                std::strcpy(buffer, "SOCKET_START_LISTENING_FAILED");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::CLIENT_ERROR):
                std::strcpy(buffer, "CLIENT_ERROR");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::MESSAGE_SEND_FAILED):
                std::strcpy(buffer, "MESSAGE_SEND_FAILED");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::SOCKET_DISCONNECT):
                std::strcpy(buffer, "SOCKET_DISCONNECT");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::SOCKET_FREE):
                std::strcpy(buffer, "SOCKET_FREE");
                break;
            case static_cast<uint8_t>(EC_HLC_SVC::CLIENT_DISCONNECTED):
                std::strcpy(buffer, "CLIENT_DISCONNECTED");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::WATCHDOG:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_WATCHDOG::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::CAUSE_UNKNOWN):
                std::strcpy(buffer, "CAUSE_UNKNOWN");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::SM):
                std::strcpy(buffer, "SM");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::PILOT):
                std::strcpy(buffer, "PILOT");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::QCATIMER):
                std::strcpy(buffer, "QCATIMER");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::QCATASK):
                std::strcpy(buffer, "QCATASK");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::CANTX):
                std::strcpy(buffer, "CANTX");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::CANRX):
                std::strcpy(buffer, "CANRX");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::IP):
                std::strcpy(buffer, "IP");
                break;
            case static_cast<uint8_t>(EC_WATCHDOG::UNKNOWN_JUST_IRQ):
                std::strcpy(buffer, "UNKNOWN_JUST_IRQ");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        case Category::IP_STACK:
        {
            switch (errorCode)
            {
            case static_cast<uint8_t>(EC_IP_STACK::Invalid):
                std::strcpy(buffer, "Invalid");
                break;
            case static_cast<uint8_t>(EC_IP_STACK::INIT_FAILED):
                std::strcpy(buffer, "INIT_FAILED");
                break;
            case static_cast<uint8_t>(EC_IP_STACK::SEND_FAILED):
                std::strcpy(buffer, "SEND_FAILED");
                break;
            case static_cast<uint8_t>(EC_IP_STACK::RECV_FAILED):
                std::strcpy(buffer, "RECV_FAILED");
                break;
            case static_cast<uint8_t>(EC_IP_STACK::INT_ERROR):
                std::strcpy(buffer, "INT_ERROR");
                break;
            case static_cast<uint8_t>(EC_IP_STACK::WRONG_MAC_ADDRESS):
                std::strcpy(buffer, "WRONG_MAC_ADDRESS");
                break;
            default:
                std::strcpy(buffer, "Unknown");
                break;
            }
        }
        break;
        default:
            std::strcpy(buffer, "Unknown");
            break;
        }
    }

    void PLCController::Set_ChargeInfo(uint8_t ConnID, uint64_t data)
    {
        ChargeInfo ChargeInfo = {};
        std::memcpy(&ChargeInfo, &data, sizeof(data));
        moduleStatus[ConnID].controlPilotDuty = static_cast<ControlPilotDuty>(ChargeInfo.controlPilotDuty);
        moduleStatus[ConnID].stateMachineState = static_cast<StateMachineState>(ChargeInfo.stateMachineState);
        moduleStatus[ConnID].proximityPinState = static_cast<ProximityPinState>(ChargeInfo.proximityPinState);
        moduleStatus[ConnID].actualChargeProtocol = static_cast<ActualChargeProtocol>(ChargeInfo.actualChargeProtocol);
        moduleStatus[ConnID].controlPilotState = static_cast<ControlPilotState>(ChargeInfo.controlPilotState);
        moduleStatus[ConnID].AliveCounter = static_cast<uint8_t>(ChargeInfo.AliveCounter);
        moduleStatus[ConnID].tcpStatus = static_cast<TCPStatus>(ChargeInfo.TCPStatus);
        moduleStatus[ConnID].diodePresence = static_cast<DiodePresence>(ChargeInfo.DiodePresence);
        moduleStatus[ConnID].cerValidationStatus = static_cast<CerValidationStatus>(ChargeInfo.CerValidationStatus);
        moduleStatus[ConnID].sigValidationStatus = static_cast<SigValidationStatus>(ChargeInfo.SigValidationStatus);
        moduleStatus[ConnID].emaidValidationStatus = static_cast<EmaidValidationStatus>(ChargeInfo.EmaidValidationStatus);
        moduleStatus[ConnID].acFallBackStatus = static_cast<ACFallBackStatus>(ChargeInfo.ACFallBackStatus);
    }
    void PLCController::Set_SoftwareInfo(uint8_t ConnID, uint64_t data)
    {
        SoftwareInfo SoftwareInfo = {};
        std::memcpy(&SoftwareInfo, &data, sizeof(data));
        moduleStatus[ConnID].SoftwareVersionMajor = static_cast<uint8_t>(SoftwareInfo.SoftwareVersionMajor);
        moduleStatus[ConnID].SoftwareVersionMinor = static_cast<uint8_t>(SoftwareInfo.SoftwareVersionMinor);
        moduleStatus[ConnID].SoftwareVersionPatch = static_cast<uint8_t>(SoftwareInfo.SoftwareVersionPatch);
        moduleStatus[ConnID].SoftwareVersionConfig = static_cast<uint8_t>(SoftwareInfo.SoftwareVersionConfig);
        moduleStatus[ConnID].SoftwareVersionHash = static_cast<uint32_t>(SoftwareInfo.SoftwareVersionHash);
        ESP_LOGI(TAG, "PLC Software Version: %hhu.%hhu.%hhu-%hhu (Hash: 0x%08lx)",
                 moduleStatus[ConnID].SoftwareVersionMajor,
                 moduleStatus[ConnID].SoftwareVersionMinor,
                 moduleStatus[ConnID].SoftwareVersionPatch,
                 moduleStatus[ConnID].SoftwareVersionConfig,
                 moduleStatus[ConnID].SoftwareVersionHash);
    }
    void PLCController::Set_ErrorCodes(uint8_t ConnID, uint64_t data)
    {
        ErrorCodes ErrorCodes = {};
        std::memcpy(&ErrorCodes, &data, sizeof(data));
        // Set error codes in moduleStatus[CmeID]
        moduleStatus[ConnID].category[0] = static_cast<Category>(ErrorCodes.category0);
        moduleStatus[ConnID].category[1] = static_cast<Category>(ErrorCodes.category1);
        moduleStatus[ConnID].category[2] = static_cast<Category>(ErrorCodes.category2);
        moduleStatus[ConnID].category[3] = static_cast<Category>(ErrorCodes.category3);
        moduleStatus[ConnID].errorCode[0] = ErrorCodes.errorCode0;
        moduleStatus[ConnID].errorCode[1] = ErrorCodes.errorCode1;
        moduleStatus[ConnID].errorCode[2] = ErrorCodes.errorCode2;
        moduleStatus[ConnID].errorCode[3] = ErrorCodes.errorCode3;
        moduleStatus[ConnID].severity[0] = static_cast<Severity>(ErrorCodes.severity0);
        moduleStatus[ConnID].severity[1] = static_cast<Severity>(ErrorCodes.severity1);
        moduleStatus[ConnID].severity[2] = static_cast<Severity>(ErrorCodes.severity2);
        moduleStatus[ConnID].severity[3] = static_cast<Severity>(ErrorCodes.severity3);
        moduleStatus[ConnID].interface[0] = static_cast<Interface>(ErrorCodes.interface0);
        moduleStatus[ConnID].interface[1] = static_cast<Interface>(ErrorCodes.interface1);
        moduleStatus[ConnID].interface[2] = static_cast<Interface>(ErrorCodes.interface2);
        moduleStatus[ConnID].interface[3] = static_cast<Interface>(ErrorCodes.interface3);

        for (uint8_t i = 0; i < 4; i++)
        {
            if ((moduleStatus[ConnID].errorCode[i] != 0) ||
                (moduleStatus[ConnID].category[i] != Category::CP_PP) ||
                (moduleStatus[ConnID].severity[i] != Severity::Minor) ||
                (moduleStatus[ConnID].interface[i] != Interface::Internal))
            {
                char InterfaceStr[20];
                char SeverityStr[20];
                char CategoryStr[20];
                char ErrorCodeStr[100];
                memset(InterfaceStr, 0, sizeof(InterfaceStr));
                memset(SeverityStr, 0, sizeof(SeverityStr));
                memset(CategoryStr, 0, sizeof(CategoryStr));
                memset(ErrorCodeStr, 0, sizeof(ErrorCodeStr));
                GetEnumInterfaceString(moduleStatus[ConnID].interface[i], InterfaceStr);
                GetEnumSeverityString(moduleStatus[ConnID].severity[i], SeverityStr);
                GetEnumCategoryString(moduleStatus[ConnID].category[i], CategoryStr);
                GetErrorCodeString(moduleStatus[ConnID].category[i], moduleStatus[ConnID].errorCode[i], ErrorCodeStr);

                ESP_LOGE(TAG, "PLC %hhu ErrorCode:\tInterface: %s\tSeverity: %s\tCategory: %s\tErrorCode: %s",
                         ConnID,
                         InterfaceStr,
                         SeverityStr,
                         CategoryStr,
                         ErrorCodeStr);
            }
        }
    }

    void PLCController::Set_HardwareStatus(uint8_t ConnID, uint64_t data)
    {
        HardwareStatus HardwareStatus = {};
        std::memcpy(&HardwareStatus, &data, sizeof(data));
        moduleStatus[ConnID].gpio2Status = static_cast<GPIOStatus>(HardwareStatus.GPIO2Status);
        moduleStatus[ConnID].gpio3Status = static_cast<GPIOStatus>(HardwareStatus.GPIO3Status);
        moduleStatus[ConnID].gpio4Status = static_cast<GPIOStatus>(HardwareStatus.GPIO4Status);
        moduleStatus[ConnID].gpio5Status = static_cast<GPIOStatus>(HardwareStatus.GPIO5Status);
    }
    void PLCController::Set_SLACInfo(uint8_t ConnID, uint64_t data)
    {
        SLACInfo SLACInfo = {};
        std::memcpy(&SLACInfo, &data, sizeof(data));
        moduleStatus[ConnID].slacLinkStatus = static_cast<SLACLinkStatus>(SLACInfo.SLACLinkStatus);
        moduleStatus[ConnID].qcaSlacConfig = static_cast<QcaSlacConfig>(SLACInfo.QcaSlacConfig);
        moduleStatus[ConnID].qcaSwEnum = static_cast<QcaSwEnum>(SLACInfo.QcaSwEnum);
        moduleStatus[ConnID].measuredAttenuation = static_cast<uint8_t>(SLACInfo.MeasuredAttenuation);
        moduleStatus[ConnID].qcaSwVersionMajor = static_cast<uint8_t>(SLACInfo.QcaSwVersionMajor);
        moduleStatus[ConnID].qcaSwVersionMinor = static_cast<uint8_t>(SLACInfo.QcaSwVersionMinor);
        moduleStatus[ConnID].qcaSwVersionPatch = static_cast<uint8_t>(SLACInfo.QcaSwVersionPatch);
    }
    void PLCController::Set_DebugInfo(uint8_t ConnID, uint64_t data)
    {
        DebugInfo DebugInfo = {};
        std::memcpy(&DebugInfo, &data, sizeof(data));
        moduleStatus[ConnID].pilotStatesStatus = static_cast<PilotStatesStatus>(DebugInfo.PilotStatesStatus);
        moduleStatus[ConnID].slacStatus = static_cast<SLACStatus>(DebugInfo.SLACStatus);
        moduleStatus[ConnID].dinStatus = static_cast<V2GStatusDIN>(DebugInfo.V2GStatusDIN);
        moduleStatus[ConnID].proximityPilotVoltage = static_cast<uint32_t>(DebugInfo.ProximityPilotVoltage);
        moduleStatus[ConnID].controlPilotVoltage = static_cast<uint32_t>(DebugInfo.ControlPilotVoltage);
        moduleStatus[ConnID].dcISO2_1_Status = static_cast<V2GStatusDCISO2_1>(DebugInfo.V2GStatusDCISO);
        moduleStatus[ConnID].acISO2_1_Status = static_cast<V2GStatusACISO2_1>(DebugInfo.V2GStatusACISO);
    }
    void PLCController::Set_RTTLogInfo(uint8_t ConnID, uint64_t data)
    {
        moduleStatus[ConnID].rttLog = static_cast<uint8_t>(data);
    }
    void PLCController::Set_BootConfig(uint8_t ConnID, uint64_t data)
    {
        moduleStatus[ConnID].bootLoaderVersion = static_cast<uint8_t>(data);
    }

    void PLCController::Set_EVDCMaxLimits(uint8_t ConnID, uint64_t data)
    {
        EVDCMaxLimits EVDCMaxLimits = {};
        std::memcpy(&EVDCMaxLimits, &data, sizeof(data));
        moduleStatus[ConnID].evMaxCurrent = static_cast<float>(EVDCMaxLimits.EVMaxCurrent * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evMaxPower = static_cast<float>(EVDCMaxLimits.EVMaxPower * Constants::POWER_SCALE);
        moduleStatus[ConnID].evMaxVoltage = static_cast<float>(EVDCMaxLimits.EVMaxVoltage * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evFullSoC = static_cast<float>(EVDCMaxLimits.EVFullSoC * Constants::SOC_SCALE);
        moduleStatus[ConnID].evBulkSoC = static_cast<float>(EVDCMaxLimits.EVBulkSoC * Constants::SOC_SCALE);
    }
    void PLCController::Set_EVDCChargeTargets(uint8_t ConnID, uint64_t data)
    {
        EVDCChargeTargets EVDCChargeTargets = {};
        std::memcpy(&EVDCChargeTargets, &data, sizeof(data));
        moduleStatus[ConnID].evTargetCurrent = static_cast<float>(EVDCChargeTargets.EVTargetCurrent * Constants::CURRENT_SCALE) - Constants::CURRENT_OFFSET;
        moduleStatus[ConnID].evTargetVoltage = static_cast<float>(EVDCChargeTargets.EVTargetVoltage * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evPreChargeVoltage = static_cast<float>(EVDCChargeTargets.EVPreChargeVoltage * Constants::VOLTAGE_SCALE);
    }
    void PLCController::Set_EVStatusDisplay(uint8_t ConnID, uint64_t data)
    {
        EVStatusDisplay EVStatusDisplay = {};
        std::memcpy(&EVStatusDisplay, &data, sizeof(data));
        moduleStatus[ConnID].evSoC = static_cast<uint8_t>(EVStatusDisplay.EVSoC * Constants::SOC_SCALE);
        moduleStatus[ConnID].evChargingComplete = static_cast<EVChargingComplete>(EVStatusDisplay.EVChargingComplete);
        moduleStatus[ConnID].evBulkChargingComplete = static_cast<EVBulkChargingComplete>(EVStatusDisplay.EVBulkChargingComplete);
        moduleStatus[ConnID].evErrorCode = static_cast<EVErrorCode>(EVStatusDisplay.EVErrorCode);
        moduleStatus[ConnID].evCabinConditioning = static_cast<EVCabinConditioning>(EVStatusDisplay.EVCabinConditioning);
        moduleStatus[ConnID].evRESSConditioning = static_cast<EVRESSConditioning>(EVStatusDisplay.EVRESSConditioning);
        moduleStatus[ConnID].evTimeToBulkSoC = static_cast<uint32_t>(EVStatusDisplay.EVTimeToBulkSoC);
        moduleStatus[ConnID].evTimeToFullSoC = static_cast<uint32_t>(EVStatusDisplay.EVTimeToFullSoC);
        moduleStatus[ConnID].evReady = static_cast<EVReady>(EVStatusDisplay.EVReady);
        moduleStatus[ConnID].evRequestedETM = static_cast<EVRequestedETM>(EVStatusDisplay.EVRequestedETM);
    }
    void PLCController::Set_EVDCEnergyLimits(uint8_t ConnID, uint64_t data)
    {
        EVDCEnergyLimits EVDCEnergyLimits = {};
        std::memcpy(&EVDCEnergyLimits, &data, sizeof(data));
        moduleStatus[ConnID].evEnergyCapacity = static_cast<float>(EVDCEnergyLimits.EVEnergyCapacity * Constants::ENERGY_SCALE);
        moduleStatus[ConnID].evEnergyRequest = static_cast<float>(EVDCEnergyLimits.EVEnergyRequest * Constants::ENERGY_SCALE);
    }
    void PLCController::Set_EVMacAddress(uint8_t ConnID, uint64_t data)
    {
        moduleStatus[ConnID].evMacAddress = data;
    }
    void PLCController::Set_EVACChargeControl(uint8_t ConnID, uint64_t data)
    {
        EVACChargeControl EVACChargeControl = {};
        std::memcpy(&EVACChargeControl, &data, sizeof(data));
        moduleStatus[ConnID].evACMinCurrent = static_cast<float>(EVACChargeControl.EVACMinCurrent * Constants::CURRENT_SCALE) - Constants::CURRENT_OFFSET;
        moduleStatus[ConnID].evACMaxCurrent = static_cast<float>(EVACChargeControl.EVACMaxCurrent * Constants::CURRENT_SCALE);
        moduleStatus[ConnID].evACMaxVoltage = static_cast<float>(EVACChargeControl.EVACMaxVoltage * Constants::VOLTAGE_SCALE);
        moduleStatus[ConnID].evEAmount = static_cast<float>(EVACChargeControl.EVEAmount);
    }
    void PLCController::Set_EVEMAID0(uint8_t ConnID, uint64_t data)
    {
        moduleStatus[ConnID].evEMAID0 = data;
    }
    void PLCController::Set_EVEMAID1(uint8_t ConnID, uint64_t data)
    {
        moduleStatus[ConnID].evEMAID1 = data;
    }

    const char *PLCController::GetControlPilotStateString(ControlPilotState state)
    {
        switch (state)
        {
        case PLCModule::ControlPilotState::A:
            return "A";
        case PLCModule::ControlPilotState::B:
            return "B";
        case PLCModule::ControlPilotState::C:
            return "C";
        case PLCModule::ControlPilotState::D:
            return "D";
        case PLCModule::ControlPilotState::E:
            return "E";
        case PLCModule::ControlPilotState::F:
            return "F";
        case PLCModule::ControlPilotState::Invalid:
            return "Invalid";
        case PLCModule::ControlPilotState::SNA:
            return "SNA";
        default:
            return "Unknown";
        }
    }

    const char *PLCController::GetStateMachineStateString(PLCModule::StateMachineState state)
    {
        switch (state)
        {
        case PLCModule::StateMachineState::Default:
            return "Default";
        case PLCModule::StateMachineState::Init:
            return "Init";
        case PLCModule::StateMachineState::Authentication:
            return "Authentication";
        case PLCModule::StateMachineState::Parameter:
            return "Parameter";
        case PLCModule::StateMachineState::Isolation:
            return "Isolation";
        case PLCModule::StateMachineState::PreCharge:
            return "PreCharge";
        case PLCModule::StateMachineState::Charge:
            return "Charge";
        case PLCModule::StateMachineState::StopCharge:
            return "StopCharge";
        case PLCModule::StateMachineState::SessionStop:
            return "SessionStop";
        case PLCModule::StateMachineState::ShutOff:
            return "ShutOff";
        case PLCModule::StateMachineState::Paused:
            return "Paused";
        case PLCModule::StateMachineState::Error:
            return "Error";
        case PLCModule::StateMachineState::SNA:
            return "SNA";
        default:
            return "UNKNOWN";
        }
    }

    const char *PLCController::GetControlPilotDutyString(PLCModule::ControlPilotDuty duty)
    {
        switch (duty)
        {
        case PLCModule::ControlPilotDuty::Invalid:
            return "Invalid";
        case PLCModule::ControlPilotDuty::SNA:
            return "SNA";
        default:
            return "UNKNOWN";
        }
    }
}