#include "TonhePowerModule.hpp"
#include <cstring>
#include <esp_log.h>

#define TAG "PMC"

PowerModule::TonhePMController *pmc;

namespace PowerModule
{
    // Constructor
    TonhePMController::TonhePMController(SendDataFunc func)
    {
        sendFunc = func;
        this->ModuleInputModeConfiguration(static_cast<uint8_t>(InputMode::AC));
    }

    // Destructor
    TonhePMController::~TonhePMController()
    {
        // Cleanup, if necessary
    }

    void TonhePMController::SetSendFunction(SendDataFunc func)
    {
        sendFunc = func;
    }

    bool TonhePMController::SendData(const uint32_t id, const uint64_t data)
    {
        if (sendFunc)
        {
            return sendFunc(id, data);
        }
        return false; // or handle error
    }

    bool TonhePMController::ReceiveData(const uint32_t id, const uint64_t data)
    {
        TonhePMCanId CanId = DecodePowerModuleCanExtId(id);
        switch (CanId.PGN)
        {
        case static_cast<uint32_t>(PGN::CHARGING_MODULE_STATE):
            ChargingModuleState(id, data);
            break;
        case static_cast<uint32_t>(PGN::MODULE_START_STOP_CONFIRMATION):
            SpecificModuleStartStopConfirmation(id, data);
            break;
        case static_cast<uint32_t>(PGN::AC_INFORMATION_UPLOAD):
            AcInformationUpload(id, data);
            break;
        case static_cast<uint32_t>(PGN::EXTENDED_STATE_FAULT_INFO):
            ExtendedStateFaultInfoUpload(id, data);
            break;
        default:
            // Invalid Packet Received
            break;
        }
        return true; // or handle error
    }

    // C function wrappers
    TonhePMCanId TonhePMController::getCanExtId(uint8_t Priority, uint8_t PGN, uint8_t DestinationAddress, uint8_t SourceAddress)
    {
        TonhePMCanId CanId;
        CanId.Priority = Priority;
        CanId.PGN = PGN;
        CanId.SA = SourceAddress;
        CanId.DA = DestinationAddress;

        return CanId;
    }

    bool TonhePMController::SpecificModuleStartStop(uint8_t ModuleAddress, uint8_t StartStop, uint16_t ChargingVoltage, uint16_t ChargingCurrent)
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::SPECIFIC_MODULE_START_STOP), static_cast<uint8_t>(PGN::SPECIFIC_MODULE_START_STOP), ModuleAddress, static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.specificModuleStartStop.moduleStartStop = StartStop;
        data.specificModuleStartStop.chargingVoltage = ChargingVoltage;
        data.specificModuleStartStop.chargingCurrent = ChargingCurrent;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.specificModuleStartStop));
    }

    bool TonhePMController::ModuleStartStop(uint32_t Modules, uint8_t StartStop, uint8_t ModuleGroupNumber, uint8_t AddressMultiple)
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::MODULE_START_STOP), static_cast<uint8_t>(PGN::MODULE_START_STOP), static_cast<uint8_t>(ModuleAddress::BROADCAST), static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.moduleStartStop.module1 = (Modules >> 0) & 0x01;
        data.moduleStartStop.module2 = (Modules >> 1) & 0x01;
        data.moduleStartStop.module3 = (Modules >> 2) & 0x01;
        data.moduleStartStop.module4 = (Modules >> 3) & 0x01;
        data.moduleStartStop.module5 = (Modules >> 4) & 0x01;
        data.moduleStartStop.module6 = (Modules >> 5) & 0x01;
        data.moduleStartStop.module7 = (Modules >> 6) & 0x01;
        data.moduleStartStop.module8 = (Modules >> 7) & 0x01;
        data.moduleStartStop.module9 = (Modules >> 8) & 0x01;
        data.moduleStartStop.module10 = (Modules >> 9) & 0x01;
        data.moduleStartStop.module11 = (Modules >> 10) & 0x01;
        data.moduleStartStop.module12 = (Modules >> 11) & 0x01;
        data.moduleStartStop.module13 = (Modules >> 12) & 0x01;
        data.moduleStartStop.module14 = (Modules >> 13) & 0x01;
        data.moduleStartStop.module15 = (Modules >> 14) & 0x01;
        data.moduleStartStop.module16 = (Modules >> 15) & 0x01;
        data.moduleStartStop.module17 = (Modules >> 16) & 0x01;
        data.moduleStartStop.module18 = (Modules >> 17) & 0x01;
        data.moduleStartStop.module19 = (Modules >> 18) & 0x01;
        data.moduleStartStop.module20 = (Modules >> 19) & 0x01;
        data.moduleStartStop.module21 = (Modules >> 20) & 0x01;
        data.moduleStartStop.module22 = (Modules >> 21) & 0x01;
        data.moduleStartStop.module23 = (Modules >> 22) & 0x01;
        data.moduleStartStop.module24 = (Modules >> 23) & 0x01;

        data.moduleStartStop.moduleStartStop = StartStop;
        data.moduleStartStop.moduleGroupNumber = ModuleGroupNumber;
        data.moduleStartStop.addressMultiple = AddressMultiple;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.moduleStartStop));
    }

    bool TonhePMController::ModuleParameterSetting(uint32_t Modules, uint8_t ModuleGroupNumber, uint8_t AddressMultiple, uint16_t Voltage, uint16_t Current)
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::MODULE_PARAMETER_SETTING), static_cast<uint8_t>(PGN::MODULE_PARAMETER_SETTING), static_cast<uint8_t>(ModuleAddress::BROADCAST), static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.moduleParameterSetting.module1 = (Modules >> 0) & 0x01;
        data.moduleParameterSetting.module2 = (Modules >> 1) & 0x01;
        data.moduleParameterSetting.module3 = (Modules >> 2) & 0x01;
        data.moduleParameterSetting.module4 = (Modules >> 3) & 0x01;
        data.moduleParameterSetting.module5 = (Modules >> 4) & 0x01;
        data.moduleParameterSetting.module6 = (Modules >> 5) & 0x01;
        data.moduleParameterSetting.module7 = (Modules >> 6) & 0x01;
        data.moduleParameterSetting.module8 = (Modules >> 7) & 0x01;
        data.moduleParameterSetting.module9 = (Modules >> 8) & 0x01;
        data.moduleParameterSetting.module10 = (Modules >> 9) & 0x01;
        data.moduleParameterSetting.module11 = (Modules >> 10) & 0x01;
        data.moduleParameterSetting.module12 = (Modules >> 11) & 0x01;
        data.moduleParameterSetting.module13 = (Modules >> 12) & 0x01;
        data.moduleParameterSetting.module14 = (Modules >> 13) & 0x01;
        data.moduleParameterSetting.module15 = (Modules >> 14) & 0x01;
        data.moduleParameterSetting.module16 = (Modules >> 15) & 0x01;
        data.moduleParameterSetting.module17 = (Modules >> 16) & 0x01;
        data.moduleParameterSetting.module18 = (Modules >> 17) & 0x01;
        data.moduleParameterSetting.module19 = (Modules >> 18) & 0x01;
        data.moduleParameterSetting.module20 = (Modules >> 19) & 0x01;
        data.moduleParameterSetting.module21 = (Modules >> 20) & 0x01;
        data.moduleParameterSetting.module22 = (Modules >> 21) & 0x01;
        data.moduleParameterSetting.module23 = (Modules >> 22) & 0x01;
        data.moduleParameterSetting.module24 = (Modules >> 23) & 0x01;

        data.moduleParameterSetting.moduleGroupNumber = ModuleGroupNumber;
        data.moduleParameterSetting.addressMultiple = AddressMultiple;
        data.moduleParameterSetting.Voltage = Voltage;
        data.moduleParameterSetting.Current = Current;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.moduleParameterSetting));
    }

    bool TonhePMController::ModuleTimingCommand()
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::MODULE_TIMING_COMMAND), static_cast<uint8_t>(PGN::MODULE_TIMING_COMMAND), static_cast<uint8_t>(ModuleAddress::BROADCAST), static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.moduleTimingCommand.spare = 0;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.moduleTimingCommand));
    }

    bool TonhePMController::ModuleAddressSetting(uint8_t Address)
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::MODULE_ADDRESS_SETTING), static_cast<uint8_t>(PGN::MODULE_ADDRESS_SETTING), static_cast<uint8_t>(ModuleAddress::BROADCAST), static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.moduleAddressSetting.moduleAddress = Address;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.moduleAddressSetting));
    }

    bool TonhePMController::ModuleAddressSettingSelection(uint8_t SelectionType)
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::MODULE_ADDRESS_SETTING_SELECTION), static_cast<uint8_t>(PGN::MODULE_ADDRESS_SETTING_SELECTION), static_cast<uint8_t>(ModuleAddress::BROADCAST), static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.addressSettingSelection.addressSetting = SelectionType;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.addressSettingSelection));
    }

    bool TonhePMController::ModuleInputModeConfiguration(uint8_t Mode)
    {
        TonhePMCanId CanId = getCanExtId(static_cast<uint8_t>(Priority::MODULE_INPUT_MODE_CONFIG), static_cast<uint8_t>(PGN::MODULE_INPUT_MODE_CONFIG), static_cast<uint8_t>(ModuleAddress::BROADCAST), static_cast<uint8_t>(ModuleAddress::MASTER));
        TonhePMData data;
        data.inputModeConfiguration.inputMode = Mode;

        return SendData(*reinterpret_cast<uint32_t *>(&CanId), *reinterpret_cast<uint64_t *>(&data.inputModeConfiguration));
    }

    TonhePMCanId TonhePMController::DecodePowerModuleCanExtId(uint32_t RxCanExtId)
    {
        TonhePMCanId CanId;
        CanId = *reinterpret_cast<TonhePMCanId *>(&RxCanExtId);
        return CanId;
    }

    void TonhePMController::ChargingModuleState(const uint32_t id, const uint64_t Candata)
    {
        TonhePMCanId CanId = DecodePowerModuleCanExtId(id);
        TonhePMData data;
        std::memcpy(&data.chargingModuleState, &Candata, sizeof(uint64_t));
        uint8_t moduleId = 0;
        ESP_LOGI(TAG, "ChargingModuleState: Received data for module address: %d", CanId.SA);
        for (uint8_t i = 1; i <= PowerModule::Constants::MAX_MODULES; i++)
        {
            if (moduleStatus[i].moduleAddress == CanId.SA)
            {
                moduleId = i;
                break;
            }
        }
        if (moduleId != 0)
        {
            switch (static_cast<uint8_t>(data.chargingModuleState.state))
            {
            case 0x00:
                moduleStatus[moduleId].state = ChargingModuleState::NORMAL_OFF;
                break;
            case 0x01:
                moduleStatus[moduleId].state = ChargingModuleState::ON;
                break;
            case 0x11:
                moduleStatus[moduleId].state = ChargingModuleState::FAULT_OFF;
                break;
            default: /* handle unknown state */
                break;
            }
            moduleStatus[moduleId].outputVoltage = data.chargingModuleState.outputVoltage / Constants::VOLTAGE_FACTOR;
            moduleStatus[moduleId].outputCurrent = data.chargingModuleState.outputCurrent / Constants::CURRENT_FACTOR;
        }
    }

    void TonhePMController::SpecificModuleStartStopConfirmation(const uint32_t id, const uint64_t Candata)
    {
        TonhePMCanId CanId = DecodePowerModuleCanExtId(id);
        TonhePMData data;
        std::memcpy(&data.moduleStartStopConfirmation, &Candata, sizeof(uint64_t));
        uint8_t moduleId = 0;
        for (uint8_t i = 1; i <= PowerModule::Constants::MAX_MODULES; i++)
        {
            if (moduleStatus[i].moduleAddress == CanId.SA)
            {
                moduleId = i;
                break;
            }
        }
        if (moduleId != 0)
        {
        }
    }

    void TonhePMController::AcInformationUpload(const uint32_t id, const uint64_t Candata)
    {
        TonhePMCanId CanId = DecodePowerModuleCanExtId(id);
        TonhePMData data;
        std::memcpy(&data.acInformationUpload, &Candata, sizeof(uint64_t));
        uint8_t moduleId = 0;
        for (uint8_t i = 1; i <= PowerModule::Constants::MAX_MODULES; i++)
        {
            if (moduleStatus[i].moduleAddress == CanId.SA)
            {
                moduleId = i;
                break;
            }
        }
        if (moduleId != 0)
        {
            moduleStatus[moduleId].PhaseAVoltage = data.acInformationUpload.PhaseAVoltage / Constants::VOLTAGE_FACTOR;
            moduleStatus[moduleId].PhaseBVoltage = data.acInformationUpload.PhaseBVoltage / Constants::VOLTAGE_FACTOR;
            moduleStatus[moduleId].PhaseCVoltage = data.acInformationUpload.PhaseCVoltage / Constants::VOLTAGE_FACTOR;
            moduleStatus[moduleId].temperature = data.acInformationUpload.temperature / Constants::TEMPERATURE_FACTOR;
        }
    }

    void TonhePMController::ExtendedStateFaultInfoUpload(const uint32_t id, const uint64_t Candata)
    {
        TonhePMCanId CanId = DecodePowerModuleCanExtId(id);
        TonhePMData data;
        std::memcpy(&data.extendedStateFaultInfo, &Candata, sizeof(uint64_t));
        uint8_t moduleId = 0;
        for (uint8_t i = 1; i <= PowerModule::Constants::MAX_MODULES; i++)
        {
            if (moduleStatus[i].moduleAddress == CanId.SA)
            {
                moduleId = i;
                break;
            }
        }
        if (moduleId != 0)
        {
            moduleStatus[moduleId].faultBits[0] = data.extendedStateFaultInfo.currentEqualization;
            moduleStatus[moduleId].faultBits[1] = data.extendedStateFaultInfo.mute;
            moduleStatus[moduleId].faultBits[2] = data.extendedStateFaultInfo.e2FaultOverflow;
            moduleStatus[moduleId].faultBits[3] = data.extendedStateFaultInfo.inputSource;
            moduleStatus[moduleId].faultBits[4] = data.extendedStateFaultInfo.e2FaultEnable;
            moduleStatus[moduleId].faultBits[5] = data.extendedStateFaultInfo.hotPlugStatus;
            moduleStatus[moduleId].faultBits[6] = data.extendedStateFaultInfo.preLevelWaveStop;
        }
    }

    // Enhanced C++ methods with better type safety and ergonomics
    void TonhePMController::Initialize()
    {
        // Success indicator - could be modified to detect errors
    }

    bool TonhePMController::ModuleStart(uint8_t moduleAddress, float voltage, float current)
    {
        // Convert from float to integer representation with scaling factor
        uint16_t intputVoltage = static_cast<uint16_t>(voltage * Constants::VOLTAGE_FACTOR);
        uint16_t intputCurrent = static_cast<uint16_t>(current * Constants::CURRENT_FACTOR);

        bool status = SpecificModuleStartStop(
            moduleAddress,
            static_cast<uint8_t>(ModuleStartStopCommand::START),
            intputVoltage,
            intputCurrent);

        uint8_t moduleId = 0;
        for (uint8_t i = 1; i <= PowerModule::Constants::MAX_MODULES; i++)
        {
            if (moduleStatus[i].moduleAddress == moduleAddress)
            {
                moduleId = i;
                break;
            }
        }
        if (moduleId != 0)
        {
            moduleStatus[moduleId].inputVoltage = voltage;
            moduleStatus[moduleId].inputCurrent = current;
        }

        return status;
    }

    bool TonhePMController::BulkModuleStart(uint64_t Modules, float voltage, float current)
    {
        bool status;

        status = ModuleStartStop(
            static_cast<uint32_t>(Modules),
            static_cast<uint8_t>(ModuleStartStopCommand::START),
            static_cast<uint8_t>(ModuleGroupNumber::GROUP0),
            static_cast<uint8_t>(AddressMultiple::MULTIPLE0));

        if (Constants::MAX_MODULES > 24)
        {
            status = ModuleStartStop(
                static_cast<uint32_t>(Modules >> 24),
                static_cast<uint8_t>(ModuleStartStopCommand::START),
                static_cast<uint8_t>(ModuleGroupNumber::GROUP0),
                static_cast<uint8_t>(AddressMultiple::MULTIPLE0));
        }

        return status;
    }

    bool TonhePMController::ModuleStop(uint8_t moduleAddress, float voltage, float current)
    {
        // Convert from float to integer representation with scaling factor
        uint16_t intputVoltage = static_cast<uint16_t>(voltage * Constants::VOLTAGE_FACTOR);
        uint16_t intputCurrent = static_cast<uint16_t>(current * Constants::CURRENT_FACTOR);

        bool status = SpecificModuleStartStop(
            moduleAddress,
            static_cast<uint8_t>(ModuleStartStopCommand::STOP),
            intputVoltage,
            intputCurrent);
        uint8_t moduleId = 0;
        for (uint8_t i = 1; i <= PowerModule::Constants::MAX_MODULES; i++)
        {
            if (moduleStatus[i].moduleAddress == moduleAddress)
            {
                moduleId = i;
                break;
            }
        }
        if (moduleId != 0)
        {
            moduleStatus[moduleId].inputVoltage = voltage;
            moduleStatus[moduleId].inputCurrent = current;
        }

        return status;
    }

    bool TonhePMController::BulkModuleStop(uint64_t Modules, float voltage, float current)
    {
        bool status;
        status = ModuleStartStop(
            static_cast<uint32_t>(Modules),
            static_cast<uint8_t>(ModuleStartStopCommand::STOP),
            static_cast<uint8_t>(ModuleGroupNumber::GROUP0),
            static_cast<uint8_t>(AddressMultiple::MULTIPLE0));
        if ((Constants::MAX_MODULES) > 24)
        {
            status = ModuleStartStop(
                static_cast<uint32_t>(Modules >> 24),
                static_cast<uint8_t>(ModuleStartStopCommand::STOP),
                static_cast<uint8_t>(ModuleGroupNumber::GROUP0),
                static_cast<uint8_t>(AddressMultiple::MULTIPLE0));
        }

        for (uint8_t k = 1; k <= (Constants::MAX_MODULES); k++)
        {
            if ((1 << (moduleStatus[k].moduleAddress - 1)) & Modules)
            {
                moduleStatus[k].inputVoltage = 0.0f;
                moduleStatus[k].inputCurrent = 0.0f;
            }
        }

        return status;
    }

    bool TonhePMController::BulkModuleParameters(uint64_t Modules, float voltage, float current)
    {
        // Convert from float to integer representation with scaling factor
        uint16_t intputVoltage = static_cast<uint16_t>(voltage * Constants::VOLTAGE_FACTOR);
        uint16_t intputCurrent = static_cast<uint16_t>(current * Constants::CURRENT_FACTOR);

        bool status;
        status = ModuleParameterSetting(
            static_cast<uint32_t>(Modules),
            static_cast<uint8_t>(ModuleGroupNumber::GROUP0),
            static_cast<uint8_t>(AddressMultiple::MULTIPLE0),
            intputVoltage,
            intputCurrent);

        if (Constants::MAX_MODULES > 24)
        {
            status = ModuleParameterSetting(
                static_cast<uint32_t>(Modules >> 24),
                static_cast<uint8_t>(ModuleGroupNumber::GROUP0),
                static_cast<uint8_t>(AddressMultiple::MULTIPLE0),
                intputVoltage,
                intputCurrent);
        }

        for (uint8_t k = 1; k <= (Constants::MAX_MODULES); k++)
        {
            if ((1 << (moduleStatus[k].moduleAddress - 1)) & Modules)
            {
                moduleStatus[k].inputVoltage = 0.0f;
                moduleStatus[k].inputCurrent = 0.0f;
            }
        }

        return status;
    }

    bool TonhePMController::ModuleParameters(uint8_t moduleAddress, float voltage, float current)
    {
        return BulkModuleParameters(
            static_cast<uint64_t>(1 << moduleAddress),
            voltage,
            current);
    }

    ModuleStatus TonhePMController::GetModuleStatus(uint8_t moduleAddress) const
    {
        return moduleStatus[moduleAddress];
    }

} // namespace PowerModule