#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "CCSConnector.hpp"
#include <cstring>
#include <cmath>

#define TAG "Connector"
#define TAG_STATUS "Status"

static const char *GetControlPilotStateString(PLCModule::ControlPilotState state)
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
    case PLCModule::ControlPilotState::SNA:
        return "SNA";
    default:
        return "UNKNOWN";
    }
}

static const char *GetStateMachineStateString(PLCModule::StateMachineState state)
{
    switch (state)
    {
    case PLCModule::StateMachineState::Idle:
        return "Idle";
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
    case PLCModule::StateMachineState::Finish:
        return "Finish";
    case PLCModule::StateMachineState::Fault:
        return "Fault";
    case PLCModule::StateMachineState::SNA:
        return "SNA";
    default:
        return "UNKNOWN";
    }
}

static const char *GetControlPilotDutyString(PLCModule::ControlPilotDuty duty)
{
    switch (duty)
    {
    case PLCModule::ControlPilotDuty::Duty0:
        return "Duty0";
    case PLCModule::ControlPilotDuty::Duty100:
        return "Duty100";
    case PLCModule::ControlPilotDuty::HLC:
        return "HLC";
    case PLCModule::ControlPilotDuty::SNA:
        return "SNA";
    default:
        return "UNKNOWN";
    }
}

static const char *GetConnectorModeString(CCSConnector::ConnectorMode mode)
{
    switch (mode)
    {
    case CCSConnector::ConnectorMode::AC:
        return "AC";
    case CCSConnector::ConnectorMode::DC:
        return "DC";
    default:
        return "UNKNOWN";
    }
}

static const char *GetStopReasonString(OCPPModule::StopReasons reason)
{
    switch (reason)
    {
    case OCPPModule::StopReasons::Stop_Remote:
        return "RemoteStop";
    case OCPPModule::StopReasons::Stop_EVDisconnected:
        return "EVDisconnected";
    case OCPPModule::StopReasons::Stop_EmergencyStop:
        return "EmergencyStop";
    case OCPPModule::StopReasons::Stop_PowerLoss:
        return "PowerLoss";
    case OCPPModule::StopReasons::Stop_Other:
        return "Other";
    default:
        return "UNKNOWN";
    }
}

CCSConnector::CCSConnectorController *connector;
namespace CCSConnector
{
    // Constructor
    CCSConnectorController ::CCSConnectorController(SendDataFunc sendFunction)
    {
        sendFunc = sendFunction;
        counting_semaphore = xSemaphoreCreateCounting(5, 0);
        Initialize();

        NumberOfConnectors = NUM_OF_CONNECTORS - 1;
        moduleStatus[0].stateMachineState = PLCModule::StateMachineState::SNA;
        isBootRejected = false;
        isChargerBooted = false;
        ocpp->CPBootNotificationRequest.Sent = false;

        xTaskCreate(&OcppTask, "OcppTask", 10 * 1024, this, 2, NULL);
        xTaskCreate(&AuthorizeTask, "AuthorizeTask", 4 * 1024, this, 2, NULL);
        energy->SendEnergyConfig(0, EnergyModule::MeterEnable::ENABLE, EnergyModule::EnergyPacket::AC, 500);

        for (uint8_t i = 1; i < Constants::MAX_MODULES; i++)
        {
            energy->SendEnergyConfig(i, EnergyModule::MeterEnable::ENABLE, EnergyModule::EnergyPacket::DC, 500);
            char TaskName[20];
            snprintf(TaskName, sizeof(TaskName), "ConnectorTask_%d", i);
            xTaskCreate(&ConnectorTask, TaskName, 4096, this, 2, NULL);
        }
    }

    void CCSConnectorController::SetSendFunction(SendDataFunc func)
    {
        sendFunc = func;
    }
    bool CCSConnectorController::SendData(const uint32_t id, const uint64_t data)
    {
        if (sendFunc)
        {
            return sendFunc(id, data);
        }
        return true; // or handle error
    }

    bool CCSConnectorController::ReceiveData(const uint32_t id, const uint64_t data)
    {
        uint8_t ConnID = 0;
        switch (id)
        {
        case static_cast<uint32_t>(CCSConnector::PowerMuxCanId::EVSEMAX):
            CCSConnector::EVSEMAX EVSEMax;
            std::memcpy((void *)&EVSEMax, (const void *)&data, sizeof(data));
            ConnID = EVSEMax.ConnectorId;
            plc->Set_EVSEMaxCurrent(ConnID, static_cast<float>(EVSEMax.EVSEMaxCurrent * PLCModule::Constants::CURRENT_SCALE));
            plc->Set_EVSEMaxVoltage(ConnID, static_cast<float>(EVSEMax.EVSEMaxVoltage * PLCModule::Constants::VOLTAGE_SCALE));
            break;
        default:
            break;
        }
        return true; // or handle error
    }

    bool CCSConnectorController::SendEVMaxData(uint8_t connId)
    {
        CCSConnector::EVMAX EVMax;
        uint64_t data = 0;
        std::memset(&EVMax, 0, sizeof(EVMax));
        EVMax.ConnectorId = static_cast<uint8_t>(connId);
        EVMax.controlPilotState = static_cast<uint8_t>(plc->Get_ControlPilotState(connId));
        EVMax.stateMachineState = static_cast<uint8_t>(plc->Get_StateMachineState(connId));
        EVMax.EVMaxCurrent = static_cast<uint16_t>(plc->Get_EVMaxCurrent(connId) / PLCModule::Constants::CURRENT_SCALE);
        EVMax.EVMaxVoltage = static_cast<uint16_t>(plc->Get_EVMaxVoltage(connId) / PLCModule::Constants::VOLTAGE_SCALE);
        std::memcpy(&data, &EVMax, sizeof(data));
        return SendData(static_cast<uint32_t>(CCSConnector::PowerMuxCanId::EVMAX), data);
    }

    bool CCSConnectorController::SendEVTargetData(uint8_t connId)
    {
        CCSConnector::EVTARTGET EVTarget;
        uint64_t data = 0;
        std::memset(&EVTarget, 0, sizeof(EVTarget));
        EVTarget.ConnectorId = static_cast<uint8_t>(connId);
        EVTarget.controlPilotState = static_cast<uint8_t>(plc->Get_ControlPilotState(connId));
        EVTarget.stateMachineState = static_cast<uint8_t>(plc->Get_StateMachineState(connId));
        EVTarget.EVTargetCurrent = static_cast<uint16_t>(plc->Get_EVTargetCurrent(connId) / PLCModule::Constants::CURRENT_SCALE);
        EVTarget.EVTargetVoltage = static_cast<uint16_t>(plc->Get_EVTargetVoltage(connId) / PLCModule::Constants::VOLTAGE_SCALE);
        if (moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::PreCharge)
        {
            EVTarget.EVTargetVoltage = static_cast<uint16_t>(plc->Get_EVPreChargeVoltage(connId) / PLCModule::Constants::VOLTAGE_SCALE);
        }
        std::memcpy(&data, &EVTarget, sizeof(data));
        return SendData(static_cast<uint32_t>(CCSConnector::PowerMuxCanId::EVTARTGET), data);
    }

    esp_err_t CCSConnectorController::readConnectorModuleStatus(uint8_t ConnID)
    {
        char *connectorModuleStatusData = NULL;
        esp_err_t err = storage->ReadConnectorModuleStatus(ConnID, &connectorModuleStatusData);
        if (err != ESP_OK)
        {
            free(connectorModuleStatusData);
            connectorModuleStatusData = NULL;
        }
        else
        {
            ESP_LOGD(TAG, "READ :: ConnID: %hhu, Config JSON string: %s",
                     ConnID, connectorModuleStatusData);
        }

        // esp_err_t err = ESP_ERR_NVS_NOT_FOUND;

        switch (err)
        {
        case ESP_OK:
        {
            cJSON *json = cJSON_Parse(connectorModuleStatusData);
            free(connectorModuleStatusData);
            connectorModuleStatusData = NULL;
            if (json == NULL)
            {
                ESP_LOGE(TAG, "INVALID connectorModuleStatusData loaded from flash");
                return ESP_FAIL; // TODO : check how handle if parse fails
            }

            ModuleStatus &status = moduleStatus[ConnID];
            status.isTransactionOngoing = cJSON_GetObjectItem(json, "isTransactionOngoing")->valueint;
            status.offlineStarted = cJSON_GetObjectItem(json, "offlineStarted")->valueint;
            status.isReserved = cJSON_GetObjectItem(json, "isReserved")->valueint;
            status.CmsAvailableScheduled = cJSON_GetObjectItem(json, "CmsAvailableScheduled")->valueint;
            status.CmsAvailable = cJSON_GetObjectItem(json, "CmsAvailable")->valueint;
            status.CmsAvailableChanged = cJSON_GetObjectItem(json, "CmsAvailableChanged")->valueint;
            status.UnkownId = cJSON_GetObjectItem(json, "UnkownId")->valueint;
            status.InvalidId = cJSON_GetObjectItem(json, "InvalidId")->valueint;
            status.controlPilotState = (PLCModule::ControlPilotState)cJSON_GetObjectItem(json, "controlPilotState")->valueint;
            status.stateMachineState = (PLCModule::StateMachineState)cJSON_GetObjectItem(json, "stateMachineState")->valueint;
            status.controlPilotDuty = (PLCModule::ControlPilotDuty)cJSON_GetObjectItem(json, "controlPilotDuty")->valueint;
            status.stopReason = (OCPPModule::StopReasons)cJSON_GetObjectItem(json, "stopReason")->valueint;
            status.transactionId = cJSON_GetObjectItem(json, "transactionId")->valueint;
            status.chargingDuration = cJSON_GetObjectItem(json, "chargingDuration")->valueint;
            status.meterStart = cJSON_GetObjectItem(json, "meterStart")->valuedouble;
            status.meterStop = cJSON_GetObjectItem(json, "meterStop")->valuedouble;
            status.DCmeterStart = cJSON_GetObjectItem(json, "DCmeterStart")->valuedouble;
            status.DCmeterStop = cJSON_GetObjectItem(json, "DCmeterStop")->valuedouble;
            status.ACmeterStart = cJSON_GetObjectItem(json, "ACmeterStart")->valuedouble;
            status.ACmeterStop = cJSON_GetObjectItem(json, "ACmeterStop")->valuedouble;

            status.mode = (ConnectorMode)cJSON_GetObjectItem(json, "mode")->valueint;

            cJSON *acMeterValues = cJSON_GetObjectItem(json, "ACMeterValues");
            cJSON *acVolt = cJSON_GetObjectItem(acMeterValues, "voltage");
            for (int i = 0; i < 4; i++)
            {
                status.ACMeterValues.voltage[i] = cJSON_GetArrayItem(acVolt, i)->valuedouble;
            }
            cJSON *acCurr = cJSON_GetObjectItem(acMeterValues, "current");
            for (int i = 0; i < 4; i++)
            {
                status.ACMeterValues.current[i] = cJSON_GetArrayItem(acCurr, i)->valuedouble;
            }
            cJSON *acPow = cJSON_GetObjectItem(acMeterValues, "power");
            for (int i = 0; i < 4; i++)
            {
                status.ACMeterValues.power[i] = cJSON_GetArrayItem(acPow, i)->valuedouble;
            }
            status.ACMeterValues.temperature = cJSON_GetObjectItem(acMeterValues, "temperature")->valuedouble;
            status.ACMeterValues.powerFactor = cJSON_GetObjectItem(acMeterValues, "powerFactor")->valuedouble;

            cJSON *dcMeterValues = cJSON_GetObjectItem(json, "DCMeterValues");
            status.DCMeterValues.voltage = cJSON_GetObjectItem(dcMeterValues, "voltage")->valuedouble;
            status.DCMeterValues.current = cJSON_GetObjectItem(dcMeterValues, "current")->valuedouble;
            status.DCMeterValues.power = cJSON_GetObjectItem(dcMeterValues, "power")->valuedouble;
            status.DCMeterValues.temperature = cJSON_GetObjectItem(dcMeterValues, "temperature")->valuedouble;

            status.Energy = cJSON_GetObjectItem(json, "Energy")->valuedouble;

            cJSON *res = cJSON_GetObjectItem(json, "ReservedData");
            strcpy(status.ReservedData.idTag, cJSON_GetObjectItem(res, "idTag")->valuestring);
            strcpy(status.ReservedData.expiryDate, cJSON_GetObjectItem(res, "expiryDate")->valuestring);
            strcpy(status.ReservedData.parentidTag, cJSON_GetObjectItem(res, "parentidTag")->valuestring);
            status.ReservedData.reservationId = cJSON_GetObjectItem(res, "reservationId")->valueint;

            cJSON *acMeterValuesFault = cJSON_GetObjectItem(json, "ACMeterValues");
            cJSON *acFVolt = cJSON_GetObjectItem(acMeterValuesFault, "voltage");
            for (int i = 0; i < 4; i++)
            {
                status.ACMeterValues.voltage[i] = cJSON_GetArrayItem(acFVolt, i)->valuedouble;
            }
            cJSON *acFCurr = cJSON_GetObjectItem(acMeterValuesFault, "current");
            for (int i = 0; i < 4; i++)
            {
                status.ACMeterValues.current[i] = cJSON_GetArrayItem(acFCurr, i)->valuedouble;
            }
            cJSON *acFPow = cJSON_GetObjectItem(acMeterValuesFault, "power");
            for (int i = 0; i < 4; i++)
            {
                status.ACMeterValues.power[i] = cJSON_GetArrayItem(acFPow, i)->valuedouble;
            }
            status.ACMeterValues.temperature = cJSON_GetObjectItem(acMeterValuesFault, "temperature")->valuedouble;
            status.ACMeterValues.powerFactor = cJSON_GetObjectItem(acMeterValuesFault, "powerFactor")->valuedouble;

            cJSON *dcMeterValuesFault = cJSON_GetObjectItem(json, "DCMeterValuesFault");
            status.DCMeterValuesFault.voltage = cJSON_GetObjectItem(dcMeterValuesFault, "voltage")->valuedouble;
            status.DCMeterValuesFault.current = cJSON_GetObjectItem(dcMeterValuesFault, "current")->valuedouble;
            status.DCMeterValuesFault.power = cJSON_GetObjectItem(dcMeterValuesFault, "power")->valuedouble;
            status.DCMeterValues.temperature = cJSON_GetObjectItem(dcMeterValuesFault, "temperature")->valuedouble;

            status.EVMacAddress = (uint64_t)cJSON_GetObjectItem(json, "EVMacAddress")->valuedouble;
            strcpy(status.idTag, cJSON_GetObjectItem(json, "idTag")->valuestring);
            strcpy(status.meterStart_time, cJSON_GetObjectItem(json, "meterStart_time")->valuestring);
            strcpy(status.meterStop_time, cJSON_GetObjectItem(json, "meterStop_time")->valuestring);

            cJSON_Delete(json);
        }

        case ESP_ERR_NOT_FOUND:
        case ESP_FAIL:
        case ESP_ERR_NO_MEM:
        case ESP_ERR_NVS_NOT_FOUND:
        {
            memset(&moduleStatus[ConnID], 0, sizeof(ModuleStatus));
            moduleStatus[ConnID].isTransactionOngoing = false;
            moduleStatus[ConnID].offlineStarted = false;
            moduleStatus[ConnID].isReserved = false;
            moduleStatus[ConnID].CmsAvailableScheduled = false;
            moduleStatus[ConnID].CmsAvailable = false;
            moduleStatus[ConnID].CmsAvailableChanged = false;
            moduleStatus[ConnID].UnkownId = false;
            moduleStatus[ConnID].InvalidId = false;

            moduleStatus[ConnID].controlPilotState = PLCModule::ControlPilotState::SNA;
            moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::SNA;
            moduleStatus[ConnID].controlPilotDuty = PLCModule::ControlPilotDuty::SNA;
            moduleStatus[ConnID].stopReason = OCPPModule::StopReasons::Stop_Other;
            moduleStatus[ConnID].transactionId = 0;
            moduleStatus[ConnID].chargingDuration = 0;

            moduleStatus[ConnID].meterStart = 0;
            moduleStatus[ConnID].meterStop = 0;

            moduleStatus[ConnID].DCmeterStart = 0;
            moduleStatus[ConnID].DCmeterStop = 0;

            moduleStatus[ConnID].ACmeterStart = 0;
            moduleStatus[ConnID].ACmeterStop = 0;

            moduleStatus[ConnID].mode = CCSConnector::ConnectorMode::DC;
            memset(&moduleStatus[ConnID].ACMeterValues, 0, sizeof(moduleStatus[ConnID].ACMeterValues));
            memset(&moduleStatus[ConnID].DCMeterValues, 0, sizeof(moduleStatus[ConnID].DCMeterValues));
            moduleStatus[ConnID].Energy = 0;
            memset(&moduleStatus[ConnID].ReservedData, 0, sizeof(moduleStatus[ConnID].ReservedData));
            memset(&moduleStatus[ConnID].ACMeterValuesFault, 0, sizeof(moduleStatus[ConnID].ACMeterValuesFault));
            memset(&moduleStatus[ConnID].DCMeterValuesFault, 0, sizeof(moduleStatus[ConnID].DCMeterValuesFault));
            moduleStatus[ConnID].EVMacAddress = 0;
            memset(moduleStatus[ConnID].idTag, 0, sizeof(moduleStatus[ConnID].idTag));
            memset(moduleStatus[ConnID].meterStart_time, 0, sizeof(moduleStatus[ConnID].meterStart_time));
            memset(moduleStatus[ConnID].meterStop_time, 0, sizeof(moduleStatus[ConnID].meterStop_time));
            writeConnectorModuleStatus(ConnID);
            break;
        }
        default:
            ESP_LOGE(TAG, "Error reading connector module status for ConnID %d: %s", ConnID, esp_err_to_name(err));
            break;
        }
        PrintConnectorModuleStatusParameters(ConnID);
        return err;
    }

    esp_err_t CCSConnectorController::writeConnectorModuleStatus(uint8_t ConnID)
    {
        if (ConnID > Constants::MAX_MODULES)
        {
            ESP_LOGE(TAG, "Invalid ConnID: %d", ConnID);
            return ESP_FAIL;
        }

        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
        {
            ESP_LOGE(TAG, "Failed to create JSON object");
            return ESP_FAIL;
        }

        ModuleStatus &status = moduleStatus[ConnID];

        cJSON_AddBoolToObject(json, "isTransactionOngoing", status.isTransactionOngoing);
        cJSON_AddBoolToObject(json, "offlineStarted", status.offlineStarted);
        cJSON_AddBoolToObject(json, "isReserved", status.isReserved);
        cJSON_AddBoolToObject(json, "CmsAvailableScheduled", status.CmsAvailableScheduled);
        cJSON_AddBoolToObject(json, "CmsAvailable", status.CmsAvailable);
        cJSON_AddBoolToObject(json, "CmsAvailableChanged", status.CmsAvailableChanged);
        cJSON_AddBoolToObject(json, "UnkownId", status.UnkownId);
        cJSON_AddBoolToObject(json, "InvalidId", status.InvalidId);
        cJSON_AddNumberToObject(json, "controlPilotState", (int)status.controlPilotState);
        cJSON_AddNumberToObject(json, "stateMachineState", (int)status.stateMachineState);
        cJSON_AddNumberToObject(json, "controlPilotDuty", (int)status.controlPilotDuty);
        cJSON_AddNumberToObject(json, "stopReason", (int)status.stopReason);
        cJSON_AddNumberToObject(json, "transactionId", status.transactionId);
        cJSON_AddNumberToObject(json, "chargingDuration", status.chargingDuration);
        cJSON_AddNumberToObject(json, "meterStart", status.meterStart);
        cJSON_AddNumberToObject(json, "meterStop", status.meterStop);
        cJSON_AddNumberToObject(json, "DCmeterStart", status.DCmeterStart);
        cJSON_AddNumberToObject(json, "DCmeterStop", status.DCmeterStop);
        cJSON_AddNumberToObject(json, "ACmeterStart", status.ACmeterStart);
        cJSON_AddNumberToObject(json, "ACmeterStop", status.ACmeterStop);
        cJSON_AddNumberToObject(json, "mode", (int)status.mode);

        cJSON *acMeterValues = cJSON_CreateObject();
        if (acMeterValues != NULL)
        {
            cJSON *voltArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(voltArray, cJSON_CreateNumber(status.ACMeterValues.voltage[i]));
            cJSON_AddItemToObject(acMeterValues, "voltage", voltArray);

            cJSON *currArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(currArray, cJSON_CreateNumber(status.ACMeterValues.current[i]));
            cJSON_AddItemToObject(acMeterValues, "current", currArray);

            cJSON *powArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(powArray, cJSON_CreateNumber(status.ACMeterValues.power[i]));
            cJSON_AddItemToObject(acMeterValues, "power", powArray);

            cJSON_AddNumberToObject(acMeterValues, "temperature", status.ACMeterValues.temperature);
            cJSON_AddNumberToObject(acMeterValues, "powerFactor", status.ACMeterValues.powerFactor);

            cJSON_AddItemToObject(json, "ACMeterValues", acMeterValues);
        }

        cJSON *dcMeterValues = cJSON_CreateObject();
        if (dcMeterValues != NULL)
        {
            cJSON_AddNumberToObject(dcMeterValues, "voltage", status.DCMeterValues.voltage);
            cJSON_AddNumberToObject(dcMeterValues, "current", status.DCMeterValues.current);
            cJSON_AddNumberToObject(dcMeterValues, "power", status.DCMeterValues.power);
            cJSON_AddNumberToObject(dcMeterValues, "temperature", status.DCMeterValues.temperature);

            cJSON_AddItemToObject(json, "DCMeterValues", dcMeterValues);
        }

        cJSON_AddNumberToObject(json, "Energy", status.Energy);

        cJSON *reservedData = cJSON_CreateObject();
        if (reservedData != NULL)
        {
            cJSON_AddStringToObject(reservedData, "idTag", status.ReservedData.idTag);
            cJSON_AddStringToObject(reservedData, "expiryDate", status.ReservedData.expiryDate);
            cJSON_AddStringToObject(reservedData, "parentidTag", status.ReservedData.parentidTag);
            cJSON_AddNumberToObject(reservedData, "reservationId", status.ReservedData.reservationId);

            cJSON_AddItemToObject(json, "ReservedData", reservedData);
        }

        cJSON *acMeterValuesFault = cJSON_CreateObject();
        if (acMeterValuesFault != NULL)
        {
            cJSON *voltArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(voltArray, cJSON_CreateNumber(status.ACMeterValues.voltage[i]));
            cJSON_AddItemToObject(acMeterValuesFault, "voltage", voltArray);

            cJSON *currArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(currArray, cJSON_CreateNumber(status.ACMeterValues.current[i]));
            cJSON_AddItemToObject(acMeterValuesFault, "current", currArray);

            cJSON *powArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(powArray, cJSON_CreateNumber(status.ACMeterValues.power[i]));
            cJSON_AddItemToObject(acMeterValuesFault, "power", powArray);

            cJSON_AddNumberToObject(acMeterValuesFault, "temperature", status.ACMeterValues.temperature);
            cJSON_AddNumberToObject(acMeterValuesFault, "powerFactor", status.ACMeterValues.powerFactor);

            cJSON_AddItemToObject(json, "ACMeterValuesFault", acMeterValuesFault);
        }

        cJSON *dcMeterValuesFault = cJSON_CreateObject();
        if (dcMeterValuesFault != NULL)
        {
            cJSON_AddNumberToObject(dcMeterValuesFault, "voltage", status.DCMeterValuesFault.voltage);
            cJSON_AddNumberToObject(dcMeterValuesFault, "current", status.DCMeterValuesFault.current);
            cJSON_AddNumberToObject(dcMeterValuesFault, "power", status.DCMeterValuesFault.power);
            cJSON_AddNumberToObject(dcMeterValuesFault, "temperature", status.DCMeterValuesFault.temperature);

            cJSON_AddItemToObject(json, "DCMeterValuesFault", dcMeterValuesFault);
        }

        cJSON_AddNumberToObject(json, "EVMacAddress", (double)status.EVMacAddress);
        cJSON_AddStringToObject(json, "idTag", status.idTag);
        cJSON_AddStringToObject(json, "meterStart_time", status.meterStart_time);
        cJSON_AddStringToObject(json, "meterStop_time", status.meterStop_time);

        char *jsonString = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);

        if (jsonString == NULL)
        {
            ESP_LOGE(TAG, "Failed to create JsonString");
            return ESP_FAIL;
        }

        esp_err_t err = storage->WriteConnectorModuleStatus(ConnID, jsonString);
        ESP_LOGD(TAG, "MODULE-STATUS-JSON: %s", jsonString);
        free(jsonString);

        return err;
    }

    esp_err_t CCSConnectorController::read_connectors_offline_data(uint8_t ConnID)
    {

        return ESP_OK; // ToDo storage->write_connectors_offline_data(ConnID, (uint8_t *)&moduleStatus[ConnID], sizeof(CCSConnector::ModuleStatus));
    }

    esp_err_t CCSConnectorController::write_connectors_offline_data(uint8_t ConnID)
    {
        if (ConnID > Constants::MAX_MODULES)
        {
            ESP_LOGE(TAG, "Invalid ConnID: %d", ConnID);
            return ESP_FAIL;
        }

        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
        {
            ESP_LOGE(TAG, "Failed to create JSON object");
            return ESP_FAIL;
        }

        ModuleStatus &status = moduleStatus[ConnID];

        cJSON_AddBoolToObject(json, "isTransactionOngoing", status.isTransactionOngoing);
        cJSON_AddBoolToObject(json, "offlineStarted", status.offlineStarted);
        cJSON_AddBoolToObject(json, "isReserved", status.isReserved);
        cJSON_AddBoolToObject(json, "CmsAvailableScheduled", status.CmsAvailableScheduled);
        cJSON_AddBoolToObject(json, "CmsAvailable", status.CmsAvailable);
        cJSON_AddBoolToObject(json, "CmsAvailableChanged", status.CmsAvailableChanged);
        cJSON_AddBoolToObject(json, "UnkownId", status.UnkownId);
        cJSON_AddBoolToObject(json, "InvalidId", status.InvalidId);
        cJSON_AddNumberToObject(json, "controlPilotState", (int)status.controlPilotState);
        cJSON_AddNumberToObject(json, "stateMachineState", (int)status.stateMachineState);
        cJSON_AddNumberToObject(json, "controlPilotDuty", (int)status.controlPilotDuty);
        cJSON_AddNumberToObject(json, "stopReason", (int)status.stopReason);
        cJSON_AddNumberToObject(json, "transactionId", status.transactionId);
        cJSON_AddNumberToObject(json, "chargingDuration", status.chargingDuration);
        cJSON_AddNumberToObject(json, "meterStart", status.meterStart);
        cJSON_AddNumberToObject(json, "meterStop", status.meterStop);
        cJSON_AddNumberToObject(json, "DCmeterStart", status.DCmeterStart);
        cJSON_AddNumberToObject(json, "DCmeterStop", status.DCmeterStop);
        cJSON_AddNumberToObject(json, "ACmeterStart", status.ACmeterStart);
        cJSON_AddNumberToObject(json, "ACmeterStop", status.ACmeterStop);
        cJSON_AddNumberToObject(json, "mode", (int)status.mode);

        cJSON *acMeterValues = cJSON_CreateObject();
        if (acMeterValues != NULL)
        {
            cJSON *voltArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(voltArray, cJSON_CreateNumber(status.ACMeterValues.voltage[i]));
            cJSON_AddItemToObject(acMeterValues, "voltage", voltArray);

            cJSON *currArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(currArray, cJSON_CreateNumber(status.ACMeterValues.current[i]));
            cJSON_AddItemToObject(acMeterValues, "current", currArray);

            cJSON *powArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(powArray, cJSON_CreateNumber(status.ACMeterValues.power[i]));
            cJSON_AddItemToObject(acMeterValues, "power", powArray);

            cJSON_AddNumberToObject(acMeterValues, "temperature", status.ACMeterValues.temperature);
            cJSON_AddNumberToObject(acMeterValues, "powerFactor", status.ACMeterValues.powerFactor);

            cJSON_AddItemToObject(json, "ACMeterValues", acMeterValues);
        }

        cJSON *dcMeterValues = cJSON_CreateObject();
        if (dcMeterValues != NULL)
        {
            cJSON_AddNumberToObject(dcMeterValues, "voltage", status.DCMeterValues.voltage);
            cJSON_AddNumberToObject(dcMeterValues, "current", status.DCMeterValues.current);
            cJSON_AddNumberToObject(dcMeterValues, "power", status.DCMeterValues.power);
            cJSON_AddNumberToObject(dcMeterValues, "temperature", status.DCMeterValues.temperature);

            cJSON_AddItemToObject(json, "DCMeterValues", dcMeterValues);
        }

        cJSON_AddNumberToObject(json, "Energy", status.Energy);

        cJSON *reservedData = cJSON_CreateObject();
        if (reservedData != NULL)
        {
            cJSON_AddStringToObject(reservedData, "idTag", status.ReservedData.idTag);
            cJSON_AddStringToObject(reservedData, "expiryDate", status.ReservedData.expiryDate);
            cJSON_AddStringToObject(reservedData, "parentidTag", status.ReservedData.parentidTag);
            cJSON_AddNumberToObject(reservedData, "reservationId", status.ReservedData.reservationId);

            cJSON_AddItemToObject(json, "ReservedData", reservedData);
        }

        cJSON *acMeterValuesFault = cJSON_CreateObject();
        if (acMeterValuesFault != NULL)
        {
            cJSON *voltArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(voltArray, cJSON_CreateNumber(status.ACMeterValues.voltage[i]));
            cJSON_AddItemToObject(acMeterValuesFault, "voltage", voltArray);

            cJSON *currArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(currArray, cJSON_CreateNumber(status.ACMeterValues.current[i]));
            cJSON_AddItemToObject(acMeterValuesFault, "current", currArray);

            cJSON *powArray = cJSON_CreateArray();
            for (int i = 0; i < 4; i++)
                cJSON_AddItemToArray(powArray, cJSON_CreateNumber(status.ACMeterValues.power[i]));
            cJSON_AddItemToObject(acMeterValuesFault, "power", powArray);

            cJSON_AddNumberToObject(acMeterValuesFault, "temperature", status.ACMeterValues.temperature);
            cJSON_AddNumberToObject(acMeterValuesFault, "powerFactor", status.ACMeterValues.powerFactor);

            cJSON_AddItemToObject(json, "ACMeterValuesFault", acMeterValuesFault);
        }

        cJSON *dcMeterValuesFault = cJSON_CreateObject();
        if (dcMeterValuesFault != NULL)
        {
            cJSON_AddNumberToObject(dcMeterValuesFault, "voltage", status.DCMeterValuesFault.voltage);
            cJSON_AddNumberToObject(dcMeterValuesFault, "current", status.DCMeterValuesFault.current);
            cJSON_AddNumberToObject(dcMeterValuesFault, "power", status.DCMeterValuesFault.power);
            cJSON_AddNumberToObject(dcMeterValuesFault, "temperature", status.DCMeterValuesFault.temperature);

            cJSON_AddItemToObject(json, "DCMeterValuesFault", dcMeterValuesFault);
        }

        cJSON_AddNumberToObject(json, "EVMacAddress", (double)status.EVMacAddress);
        cJSON_AddStringToObject(json, "idTag", status.idTag);
        cJSON_AddStringToObject(json, "meterStart_time", status.meterStart_time);
        cJSON_AddStringToObject(json, "meterStop_time", status.meterStop_time);

        char *jsonString = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);

        if (jsonString == NULL)
        {
            ESP_LOGE(TAG, "Failed to create JsonString");
            return ESP_FAIL;
        }

        esp_err_t err = storage->write_connectors_offline_data(ConnID, jsonString);
        free(jsonString);
        return err;
    }

    void CCSConnectorController::sendStatusNotificationRequest(uint8_t ConnID, std::string errorCode, std::string status, std::string vendorErrorCode)
    {
        ocpp->CPStatusNotificationRequest[ConnID].connectorId = ConnID;
        setNULL(ocpp->CPStatusNotificationRequest[ConnID].errorCode);
        setNULL(ocpp->CPStatusNotificationRequest[ConnID].status);
        setNULL(ocpp->CPStatusNotificationRequest[ConnID].vendorErrorCode);
        std::strcpy(ocpp->CPStatusNotificationRequest[ConnID].errorCode, errorCode.c_str());
        std::strcpy(ocpp->CPStatusNotificationRequest[ConnID].status, status.c_str());
        std::strcpy(ocpp->CPStatusNotificationRequest[ConnID].vendorErrorCode, vendorErrorCode.c_str());
        setNULL(ocpp->CPStatusNotificationRequest[ConnID].timestamp);
        SystemTime->getTimeInOcppFormat(ocpp->CPStatusNotificationRequest[ConnID].timestamp);
        ocpp->sendStatusNotificationRequest(ConnID);
    }

    // Destructor
    CCSConnectorController::~CCSConnectorController()
    {
        // Cleanup, if necessary
    }

    void CCSConnectorController::Initialize()
    {
        for (uint8_t i = 1; i < Constants::MAX_MODULES; i++)
        {
            readConnectorModuleStatus(i);
        }
        isChargerBooted = false;
        isBootRejected = false;
        bootedNow = false;
        isPoweredOn = false;
        isWifiConnected = false;
        isWebsocketConnected = false;
        isWebsocketConnected_old = false;
        isPowerModuleAssigned = false;
        emergency = false;
        earthDisconnect = false;
        powerLoss = false;
        updatingFirmwarePending = false;
        uint8_t NumberOfConnectors = NUM_OF_CONNECTORS - 1;
        FirmwareUpdateFailed = false;
        FirmwareUpdateStarted = false;

        INITIALIZING = false;
        RFID_TAPPED = false;
        AUTH_SUCCESS_PLUG_EV = false;
        AUTH_FAILED = false;
        AUTH_SUCCESS = false;
        FIRMWARE_UPDATE = false;
        COMMISSIONING = false;
        NETWORKSWITCH = false;
        memset(AVAILABLE, 0, sizeof(AVAILABLE));
        memset(UNAVAILABLE, 0, sizeof(UNAVAILABLE));
        memset(CHARGING, 0, sizeof(CHARGING));
        memset(FINISHING, 0, sizeof(FINISHING));
        memset(EV_PLUGGED_TAP_RFID, 0, sizeof(EV_PLUGGED_TAP_RFID));
        memset(FAULT, 0, sizeof(FAULT));
        memset(RESERVED, 0, sizeof(RESERVED));
        memset(SUSPENDED, 0, sizeof(SUSPENDED));

        isRfidTappedFirst = false;
        Rfid = false;
        Alpr = false;
        Alpr_old = false;
        Rfid_old = false;
        RfidTappedLed = false;
        RfidAuthorized = false;
        memset(TransactionAuthorized, 0, sizeof(TransactionAuthorized));
        memset(TransactionResTimeout, 0, sizeof(TransactionResTimeout));
        memset(RemoteAuthorized, 0, sizeof(RemoteAuthorized));
        memset(ALPRAuthorized, 0, sizeof(ALPRAuthorized));
        memset(Rfid_tag, 0, sizeof(Rfid_tag));
        memset(Remote_tag, 0, sizeof(Remote_tag));
        memset(Alpr_tag, 0, sizeof(Alpr_tag));

        for (uint8_t i = 0; i < NUM_OF_CONNECTORS; i++)
        {
            ClockAlignedDataTime[i] = false;
            gfci[i] = false;
            relayWeld[i] = false;
            overVoltage[i] = false;
            underVoltage[i] = false;
            overCurrent[i] = false;
            overTemp[i] = false;
            connectorFault[i] = false;
            emergency_old[i] = false;
            earthDisconnect_old[i] = false;
            gfci_old[i] = false;
            relayWeld_old[i] = false;
            overVoltage_old[i] = false;
            underVoltage_old[i] = false;
            overCurrent_old[i] = false;
            overTemp_old[i] = false;
            powerLoss_old[i] = false;
            connectorFault_old[i] = false;
            finishingStatusSent[i] = false;
            finishingStatusPending[i] = false;
            updatingDataToCms[i] = false;
            isConnectorCharging[i] = false;

            ChargingFaultTimeCount[i] = 0;
            ClockAlignedDataCount[i] = 0;
            MeterValueAlignedData[i].temp = 0;
            MeterValueAlignedData[i].Energy = 0;
            MeterValueAlignedData[i].meterStop = 0;
            for (uint8_t j = 0; j < 4; j++)
            {
                MeterValueAlignedData[i].voltage[j] = 0;
                MeterValueAlignedData[i].current[j] = 0;
                MeterValueAlignedData[i].power[j] = 0;
            }
            ledState[i] = OCPPModule::LED_STATE::INIT_STATE;
            controlPilotState_old[i] = PLCModule::ControlPilotState::SNA;
            controlPilotDuty_old[i] = PLCModule::ControlPilotDuty::SNA;
            stateMachineState_old[i] = PLCModule::StateMachineState::SNA;
        }
        PrintConnectorParameters();
    }

    CCSConnector::ModuleStatus CCSConnectorController::GetModuleStatus(uint8_t ConnID)
    {
        if (ConnID < Constants::MAX_MODULES)
        {
            return moduleStatus[ConnID];
        }
        return moduleStatus[ConnID];
    }

    void CCSConnectorController::SetModuleStatusToSNA(uint8_t ConnID)
    {
        if (ConnID < Constants::MAX_MODULES)
        {
            moduleStatus[ConnID].mode = ConnectorMode::DC;
        }
    }

    void CCSConnectorController::PrintConnectorParameters()
    {
        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG, "     CCS CONNECTOR PARAMETERS");
        ESP_LOGI(TAG, "========================================");

        ESP_LOGI(TAG, "Number of Connectors : %d", NUM_OF_CONNECTORS);
        ESP_LOGI(TAG, "Charger boot staus   : %d", this->isChargerBooted);
        ESP_LOGI(TAG, "isPoweredOn          : %d", this->isPoweredOn);
        ESP_LOGI(TAG, "isWifiConnected      : %d", this->isWifiConnected);
        ESP_LOGI(TAG, "isWebsocketConnected : %d", this->isWebsocketConnected);
        ESP_LOGI(TAG, "isPowerModuleAssined : %d", this->isPowerModuleAssigned);
        ESP_LOGI(TAG, "Emergency            : %d", this->emergency);
        ESP_LOGI(TAG, "earthDisconnect      : %d", this->earthDisconnect);
        ESP_LOGI(TAG, ".....");
        ESP_LOGI(TAG, ".....");
        ESP_LOGI(TAG, "========================================");
    }

    void CCSConnectorController::PrintConnectorModuleStatusParameters(uint8_t ConnID)
    {
        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG, "     CCS CONNECTOR MODULE STATUS ");
        ESP_LOGI(TAG, "========================================");

        ESP_LOGI(TAG, "Connector ID                : %d", ConnID);

        ESP_LOGI(TAG, "----- CONNECTOR STATE -----");
        ESP_LOGI(TAG, "Transaction Ongoing         : %d", moduleStatus[ConnID].isTransactionOngoing);
        ESP_LOGI(TAG, "Offline Started             : %d", moduleStatus[ConnID].offlineStarted);
        ESP_LOGI(TAG, "Reserved                    : %d", moduleStatus[ConnID].isReserved);
        ESP_LOGI(TAG, "CMS Available               : %d", moduleStatus[ConnID].CmsAvailable);
        ESP_LOGI(TAG, "CMS Available Changed       : %d", moduleStatus[ConnID].CmsAvailableChanged);

        ESP_LOGI(TAG, "Control Pilot State         : %s (%d)",
                 GetControlPilotStateString(moduleStatus[ConnID].controlPilotState),
                 (int)moduleStatus[ConnID].controlPilotState);
        ESP_LOGI(TAG, "State Machine State         : %s (%d)",
                 GetStateMachineStateString(moduleStatus[ConnID].stateMachineState),
                 (int)moduleStatus[ConnID].stateMachineState);

        ESP_LOGI(TAG, "Control Pilot Duty          : %s (%d)",
                 GetControlPilotDutyString(moduleStatus[ConnID].controlPilotDuty),
                 (int)moduleStatus[ConnID].controlPilotDuty);

        ESP_LOGI(TAG, "Connector Mode              : %s (%d)",
                 GetConnectorModeString(moduleStatus[ConnID].mode),
                 (int)moduleStatus[ConnID].mode);
        ESP_LOGI(TAG, "----- TRANSACTION -----");
        ESP_LOGI(TAG, "Transaction ID              : %d", moduleStatus[ConnID].transactionId);
        ESP_LOGI(TAG, "Charging Duration           : %d", moduleStatus[ConnID].chargingDuration);
        ESP_LOGI(TAG, "Stop Reason                 : %s (%d)",
                 GetStopReasonString(moduleStatus[ConnID].stopReason),
                 (int)moduleStatus[ConnID].stopReason);
        ESP_LOGI(TAG, "Meter Start                 : %.2f", moduleStatus[ConnID].meterStart);
        ESP_LOGI(TAG, "Meter Stop                  : %.2f", moduleStatus[ConnID].meterStop);

        ESP_LOGI(TAG, "DC Meter Start              : %.2f", moduleStatus[ConnID].DCmeterStart);
        ESP_LOGI(TAG, "DC Meter Stop               : %.2f", moduleStatus[ConnID].DCmeterStop);

        ESP_LOGI(TAG, "AC Meter Start              : %.2f", moduleStatus[ConnID].ACmeterStart);
        ESP_LOGI(TAG, "AC Meter Stop               : %.2f", moduleStatus[ConnID].ACmeterStop);

        ESP_LOGI(TAG, "Energy                      : %.2f", moduleStatus[ConnID].Energy);

        ESP_LOGI(TAG, "----- AC METER VALUES -----");
        ESP_LOGI(TAG, "Voltage L1                  : %.2f", moduleStatus[ConnID].ACMeterValues.voltage[0]);
        ESP_LOGI(TAG, "Voltage L2                  : %.2f", moduleStatus[ConnID].ACMeterValues.voltage[1]);
        ESP_LOGI(TAG, "Voltage L3                  : %.2f", moduleStatus[ConnID].ACMeterValues.voltage[2]);
        ESP_LOGI(TAG, "Voltage L4                  : %.2f", moduleStatus[ConnID].ACMeterValues.voltage[3]);

        ESP_LOGI(TAG, "Current L1                  : %.2f", moduleStatus[ConnID].ACMeterValues.current[0]);
        ESP_LOGI(TAG, "Current L2                  : %.2f", moduleStatus[ConnID].ACMeterValues.current[1]);
        ESP_LOGI(TAG, "Current L3                  : %.2f", moduleStatus[ConnID].ACMeterValues.current[2]);
        ESP_LOGI(TAG, "Current L4                  : %.2f", moduleStatus[ConnID].ACMeterValues.current[3]);

        ESP_LOGI(TAG, "Power L1                    : %.2f", moduleStatus[ConnID].ACMeterValues.power[0]);
        ESP_LOGI(TAG, "Power L2                    : %.2f", moduleStatus[ConnID].ACMeterValues.power[1]);
        ESP_LOGI(TAG, "Power L3                    : %.2f", moduleStatus[ConnID].ACMeterValues.power[2]);
        ESP_LOGI(TAG, "Power L4                    : %.2f", moduleStatus[ConnID].ACMeterValues.power[3]);

        ESP_LOGI(TAG, "Temperature                 : %.2f", moduleStatus[ConnID].ACMeterValues.temperature);
        ESP_LOGI(TAG, "Power Factor                : %.2f", moduleStatus[ConnID].ACMeterValues.powerFactor);

        ESP_LOGI(TAG, "----- DC METER VALUES -----");
        ESP_LOGI(TAG, "DC Voltage                  : %.2f", moduleStatus[ConnID].DCMeterValues.voltage);
        ESP_LOGI(TAG, "DC Current                  : %.2f", moduleStatus[ConnID].DCMeterValues.current);
        ESP_LOGI(TAG, "DC Power                    : %.2f", moduleStatus[ConnID].DCMeterValues.power);
        ESP_LOGI(TAG, "DC Temperature              : %.2f", moduleStatus[ConnID].DCMeterValues.temperature);

        ESP_LOGI(TAG, "----- RESERVATION DATA -----");
        ESP_LOGI(TAG, "Reserved ID Tag             : %s", moduleStatus[ConnID].ReservedData.idTag);
        ESP_LOGI(TAG, "Expiry Date                 : %s", moduleStatus[ConnID].ReservedData.expiryDate);
        ESP_LOGI(TAG, "Parent ID Tag               : %s", moduleStatus[ConnID].ReservedData.parentidTag);
        ESP_LOGI(TAG, "Reservation ID              : %d", moduleStatus[ConnID].ReservedData.reservationId);

        ESP_LOGI(TAG, "----- USER DATA -----");
        ESP_LOGI(TAG, "ID Tag                      : %s", moduleStatus[ConnID].idTag);
        ESP_LOGI(TAG, "Meter Start Time            : %s", moduleStatus[ConnID].meterStart_time);
        ESP_LOGI(TAG, "Meter Stop Time             : %s", moduleStatus[ConnID].meterStop_time);

        ESP_LOGI(TAG, "----- FAULT VALUES -----");
        ESP_LOGI(TAG, "AC Fault Voltage            : %.2f", moduleStatus[ConnID].ACMeterValuesFault.voltage);
        ESP_LOGI(TAG, "AC Fault Current            : %.2f", moduleStatus[ConnID].ACMeterValuesFault.current);
        ESP_LOGI(TAG, "AC Fault Power              : %.2f", moduleStatus[ConnID].ACMeterValuesFault.power);

        ESP_LOGI(TAG, "DC Fault Voltage            : %.2f", moduleStatus[ConnID].DCMeterValuesFault.voltage);
        ESP_LOGI(TAG, "DC Fault Current            : %.2f", moduleStatus[ConnID].DCMeterValuesFault.current);
        ESP_LOGI(TAG, "DC Fault Power              : %.2f", moduleStatus[ConnID].DCMeterValuesFault.power);

        ESP_LOGI(TAG, "EV MAC Address              : %llu", moduleStatus[ConnID].EVMacAddress);

        ESP_LOGI(TAG, "========================================");
    }

}