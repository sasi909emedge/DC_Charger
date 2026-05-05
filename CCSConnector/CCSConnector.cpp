#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "CCSConnector.hpp"
#include <cstring>
#include <cmath>

#define TAG "Connector"
#define TAG_STATUS "Status"

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
      char data[2048];
      memset(data, 0, sizeof(data));

      esp_err_t err = storage->ReadConnectorModuleStatus(
          ConnID,
          (uint8_t *)data,
          sizeof(data));

      if (err == ESP_OK)
      {
         cJSON *json = cJSON_Parse(data);
         if (json == NULL)
         {
            return ESP_FAIL;
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

         cJSON *ac = cJSON_GetObjectItem(json, "ACMeterValues");

         cJSON *acVolt = cJSON_GetObjectItem(ac, "voltage");
         for (int i = 0; i < 4; i++)
            status.ACMeterValues.voltage[i] = cJSON_GetArrayItem(acVolt, i)->valuedouble;

         cJSON *acCurr = cJSON_GetObjectItem(ac, "current");
         for (int i = 0; i < 4; i++)
            status.ACMeterValues.current[i] = cJSON_GetArrayItem(acCurr, i)->valuedouble;

         cJSON *acPow = cJSON_GetObjectItem(ac, "power");
         for (int i = 0; i < 4; i++)
            status.ACMeterValues.power[i] = cJSON_GetArrayItem(acPow, i)->valuedouble;

         status.ACMeterValues.temperature =
             cJSON_GetObjectItem(ac, "temperature")->valuedouble;

         status.ACMeterValues.powerFactor =
             cJSON_GetObjectItem(ac, "powerFactor")->valuedouble;

         cJSON *dc = cJSON_GetObjectItem(json, "DCMeterValues");

         status.DCMeterValues.voltage =
             cJSON_GetObjectItem(dc, "voltage")->valuedouble;

         status.DCMeterValues.current =
             cJSON_GetObjectItem(dc, "current")->valuedouble;

         status.DCMeterValues.power =
             cJSON_GetObjectItem(dc, "power")->valuedouble;

         status.DCMeterValues.temperature =
             cJSON_GetObjectItem(dc, "temperature")->valuedouble;

         cJSON *res = cJSON_GetObjectItem(json, "ReservedData");
         strcpy(status.ReservedData.idTag, cJSON_GetObjectItem(res, "idTag")->valuestring);
         strcpy(status.ReservedData.expiryDate, cJSON_GetObjectItem(res, "expiryDate")->valuestring);
         strcpy(status.ReservedData.parentidTag, cJSON_GetObjectItem(res, "parentidTag")->valuestring);
         status.ReservedData.reservationId = cJSON_GetObjectItem(res, "reservationId")->valueint;

         cJSON *acF = cJSON_GetObjectItem(json, "ACMeterValuesFault");
         status.ACMeterValuesFault.voltage = cJSON_GetObjectItem(acF, "voltage")->valuedouble;
         status.ACMeterValuesFault.current = cJSON_GetObjectItem(acF, "current")->valuedouble;
         status.ACMeterValuesFault.power = cJSON_GetObjectItem(acF, "power")->valuedouble;

         cJSON *dcF = cJSON_GetObjectItem(json, "DCMeterValuesFault");
         status.DCMeterValuesFault.voltage = cJSON_GetObjectItem(dcF, "voltage")->valuedouble;
         status.DCMeterValuesFault.current = cJSON_GetObjectItem(dcF, "current")->valuedouble;
         status.DCMeterValuesFault.power = cJSON_GetObjectItem(dcF, "power")->valuedouble;

         status.EVMacAddress = (uint64_t)cJSON_GetObjectItem(json, "EVMacAddress")->valuedouble;
         status.Energy = cJSON_GetObjectItem(json, "Energy")->valuedouble;
         strcpy(status.idTag, cJSON_GetObjectItem(json, "idTag")->valuestring);
         strcpy(status.meterStart_time, cJSON_GetObjectItem(json, "meterStart_time")->valuestring);
         strcpy(status.meterStop_time, cJSON_GetObjectItem(json, "meterStop_time")->valuestring);
         status.ACMeterValues.temperature = cJSON_GetObjectItem(ac, "temperature")->valuedouble;
         status.ACMeterValues.powerFactor = cJSON_GetObjectItem(ac, "powerFactor")->valuedouble;

         cJSON_Delete(json);
      }
      switch (err)
      {
      case ESP_ERR_NOT_FOUND:
      case ESP_ERR_NVS_NOT_FOUND:
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
         strcpy(moduleStatus[ConnID].ReservedData.idTag, "");
         strcpy(moduleStatus[ConnID].ReservedData.expiryDate, "");
         strcpy(moduleStatus[ConnID].ReservedData.parentidTag, "");
         moduleStatus[ConnID].ReservedData.reservationId = 0;
         moduleStatus[ConnID].ACMeterValuesFault.voltage = 0;
         moduleStatus[ConnID].ACMeterValuesFault.current = 0;
         moduleStatus[ConnID].ACMeterValuesFault.power = 0;
         moduleStatus[ConnID].DCMeterValuesFault.voltage = 0;
         moduleStatus[ConnID].DCMeterValuesFault.current = 0;
         moduleStatus[ConnID].DCMeterValuesFault.power = 0;
         writeConnectorModuleStatus(ConnID);
         break;
      default:
         break;
      }
      return err;
   }

   esp_err_t CCSConnectorController::writeConnectorModuleStatus(uint8_t ConnID)
   {
      if (ConnID >= Constants::MAX_MODULES)
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

      cJSON *ac = cJSON_CreateObject();
      if (ac != NULL)
      {
         cJSON *voltArray = cJSON_CreateArray();
         for (int i = 0; i < 4; i++)
            cJSON_AddItemToArray(voltArray, cJSON_CreateNumber(status.ACMeterValues.voltage[i]));
         cJSON_AddItemToObject(ac, "voltage", voltArray);

         cJSON *currArray = cJSON_CreateArray();
         for (int i = 0; i < 4; i++)
            cJSON_AddItemToArray(currArray, cJSON_CreateNumber(status.ACMeterValues.current[i]));
         cJSON_AddItemToObject(ac, "current", currArray);

         cJSON *powArray = cJSON_CreateArray();
         for (int i = 0; i < 4; i++)
            cJSON_AddItemToArray(powArray, cJSON_CreateNumber(status.ACMeterValues.power[i]));
         cJSON_AddItemToObject(ac, "power", powArray);

         cJSON_AddNumberToObject(ac, "temperature", status.ACMeterValues.temperature);
         cJSON_AddNumberToObject(ac, "powerFactor", status.ACMeterValues.powerFactor);

         cJSON_AddItemToObject(json, "ACMeterValues", ac);
      }

      cJSON *dc = cJSON_CreateObject();
      if (dc != NULL)
      {
         cJSON_AddNumberToObject(dc, "voltage", status.DCMeterValues.voltage);
         cJSON_AddNumberToObject(dc, "current", status.DCMeterValues.current);
         cJSON_AddNumberToObject(dc, "power", status.DCMeterValues.power);
         cJSON_AddNumberToObject(dc, "temperature", status.DCMeterValues.temperature);

         cJSON_AddItemToObject(json, "DCMeterValues", dc);
      }

      cJSON *reserved = cJSON_CreateObject();
      if (reserved != NULL)
      {
         cJSON_AddStringToObject(reserved, "idTag", status.ReservedData.idTag);
         cJSON_AddStringToObject(reserved, "expiryDate", status.ReservedData.expiryDate);
         cJSON_AddStringToObject(reserved, "parentidTag", status.ReservedData.parentidTag);
         cJSON_AddNumberToObject(reserved, "reservationId", status.ReservedData.reservationId);
      }

      cJSON *acFault = cJSON_CreateObject();
      if (acFault != NULL)
      {
         cJSON_AddNumberToObject(acFault, "voltage", status.ACMeterValuesFault.voltage);
         cJSON_AddNumberToObject(acFault, "current", status.ACMeterValuesFault.current);
         cJSON_AddNumberToObject(acFault, "power", status.ACMeterValuesFault.power);
         cJSON_AddItemToObject(json, "ACMeterValuesFault", acFault);
      }
      cJSON *dcFault = cJSON_CreateObject();
      if (dcFault != NULL)
      {
         cJSON_AddNumberToObject(dcFault, "voltage", status.DCMeterValuesFault.voltage);
         cJSON_AddNumberToObject(dcFault, "current", status.DCMeterValuesFault.current);
         cJSON_AddNumberToObject(dcFault, "power", status.DCMeterValuesFault.power);
         cJSON_AddItemToObject(json, "DCMeterValuesFault", dcFault);
      }

      cJSON_AddNumberToObject(json, "EVMacAddress", (double)status.EVMacAddress);
      cJSON_AddNumberToObject(json, "Energy", status.Energy);
      cJSON_AddStringToObject(json, "idTag", status.idTag);
      cJSON_AddStringToObject(json, "meterStart_time", status.meterStart_time);
      cJSON_AddStringToObject(json, "meterStop_time", status.meterStop_time);
      cjson_AddNumberToObject(json, "ACMeterValues.temperature", status.ACMeterValues.temperature);
      cjson_AddNumberToObject(json, "ACMeterValues.powerFactor", status.ACMeterValues.powerFactor);
      cJSON_AddItemToObject(json, "ReservedData", reserved);

      char *jsonString = cJSON_PrintUnformatted(json);
      cJSON_Delete(json);

      if (jsonString == NULL)
      {
         ESP_LOGE(TAG, "Failed to print JSON");
         return ESP_FAIL;
      }

      esp_err_t err = storage->WriteConnectorModuleStatus(
          ConnID,
          (uint8_t *)jsonString,
          strlen(jsonString));

      free(jsonString);

      return err;
   }

   esp_err_t NVSStorage::write_connectors_offline_data(uint8_t ConnId, char data)
   {
      if (ConnId >= MAX_CONNECTORS)
      {
         ESP_LOGE("NVSStorage", "Invalid ConnId: %d", ConnId);
         return ESP_FAIL;
      }

      char fileName[64];
      memset(fileName, 0, sizeof(fileName));

      snprintf(fileName, sizeof(fileName), "/MyStorage/connector_%d.txt", ConnId);

      FILE *file = fopen(fileName, "w");
      if (file == NULL)
      {
         ESP_LOGE("NVSStorage", "Failed to open file for writing: %s", fileName);
         return ESP_FAIL;
      }

      size_t written = fwrite(&data, sizeof(char), 1, file);
      if (written != 1)
      {
         ESP_LOGE("NVSStorage", "Failed to write data to file");
         fclose(file);
         return ESP_FAIL;
      }

      fclose(file);
      return ESP_OK;
   }

   ModuleStatus &status = moduleStatus[ConnID];

   cJSON *json = cJSON_CreateObject();
   if (json == NULL)
   {
      cJSON_Delete(rootArray);
      ESP_LOGE(TAG, "Failed to create JSON object");
      return ESP_FAIL;
   }

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

   cJSON *ac = cJSON_CreateObject();
   if (ac != NULL)
   {
      cJSON *voltArray = cJSON_CreateArray();
      for (int i = 0; i < 4; i++)
         cJSON_AddItemToArray(voltArray, cJSON_CreateNumber(status.ACMeterValues.voltage[i]));
      cJSON_AddItemToObject(ac, "voltage", voltArray);

      cJSON *currArray = cJSON_CreateArray();
      for (int i = 0; i < 4; i++)
         cJSON_AddItemToArray(currArray, cJSON_CreateNumber(status.ACMeterValues.current[i]));
      cJSON_AddItemToObject(ac, "current", currArray);

      cJSON *powArray = cJSON_CreateArray();
      for (int i = 0; i < 4; i++)
         cJSON_AddItemToArray(powArray, cJSON_CreateNumber(status.ACMeterValues.power[i]));
      cJSON_AddItemToObject(ac, "power", powArray);

      cJSON_AddNumberToObject(ac, "temperature", status.ACMeterValues.temperature);
      cJSON_AddNumberToObject(ac, "powerFactor", status.ACMeterValues.powerFactor);

      cJSON_AddItemToObject(json, "ACMeterValues", ac);
   }

   cJSON *dc = cJSON_CreateObject();
   if (dc != NULL)
   {
      cJSON_AddNumberToObject(dc, "voltage", status.DCMeterValues.voltage);
      cJSON_AddNumberToObject(dc, "current", status.DCMeterValues.current);
      cJSON_AddNumberToObject(dc, "power", status.DCMeterValues.power);
      cJSON_AddNumberToObject(dc, "temperature", status.DCMeterValues.temperature);

      cJSON_AddItemToObject(json, "DCMeterValues", dc);
   }

   cJSON *reserved = cJSON_CreateObject();
   if (reserved != NULL)
   {
      cJSON_AddStringToObject(reserved, "idTag", status.ReservedData.idTag);
      cJSON_AddStringToObject(reserved, "expiryDate", status.ReservedData.expiryDate);
      cJSON_AddStringToObject(reserved, "parentidTag", status.ReservedData.parentidTag);
      cJSON_AddNumberToObject(reserved, "reservationId", status.ReservedData.reservationId);
   }

   cJSON *acFault = cJSON_CreateObject();
   if (acFault != NULL)
   {
      cJSON_AddNumberToObject(acFault, "voltage", status.ACMeterValuesFault.voltage);
      cJSON_AddNumberToObject(acFault, "current", status.ACMeterValuesFault.current);
      cJSON_AddNumberToObject(acFault, "power", status.ACMeterValuesFault.power);
      cJSON_AddItemToObject(json, "ACMeterValuesFault", acFault);
   }

   cJSON *dcFault = cJSON_CreateObject();
   if (dcFault != NULL)
   {
      cJSON_AddNumberToObject(dcFault, "voltage", status.DCMeterValuesFault.voltage);
      cJSON_AddNumberToObject(dcFault, "current", status.DCMeterValuesFault.current);
      cJSON_AddNumberToObject(dcFault, "power", status.DCMeterValuesFault.power);
      cJSON_AddItemToObject(json, "DCMeterValuesFault", dcFault);
   }

   cJSON_AddNumberToObject(json, "EVMacAddress", (double)status.EVMacAddress);
   cJSON_AddNumberToObject(json, "Energy", status.Energy);
   cJSON_AddStringToObject(json, "idTag", status.idTag);
   cJSON_AddStringToObject(json, "meterStart_time", status.meterStart_time);
   cJSON_AddStringToObject(json, "meterStop_time", status.meterStop_time);
   cJSON_AddItemToObject(json, "ReservedData", reserved);

   cJSON_AddItemToArray(rootArray, json);

   char *jsonString = cJSON_PrintUnformatted(rootArray);
   cJSON_Delete(rootArray);

   if (jsonString == NULL)
   {
      ESP_LOGE(TAG, "Failed to print JSON");
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
}
/*
void CCSConnectorController::ProcessStateMachineStateSNA(uint8_t ConnID)
{
   moduleStatus[ConnID].controlPilotState = PLCModule::ControlPilotState::SNA;
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::SNA;
   moduleStatus[ConnID].controlPilotDuty = PLCModule::ControlPilotDuty::SNA;

   moduleStatus[ConnID].isTransactionOngoing = false;
   moduleStatus[ConnID].offlineStarted = false;

   if (isWebsocketConnected)
   {
      sendStatusNotificationRequest(ConnID, "NoError", "Available", "");
   }
}

void CCSConnectorController::ProcessStateMachineStateDefault(uint8_t ConnID)
{
   moduleStatus[ConnID].isTransactionOngoing = false;

   if (connectorFault[ConnID])
   {
      sendStatusNotificationRequest(ConnID, "InternalError", "Faulted", "");
   }
   else
   {
      sendStatusNotificationRequest(ConnID, "NoError", "Available", "");
   }
}

void CCSConnectorController::ProcessStateMachineStateInit(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Init;

   if (!isChargerBooted)
   {
      return;
   }

   if (isWebsocketConnected)
   {
      sendStatusNotificationRequest(ConnID, "NoError", "Preparing", "");
   }
}

void CCSConnectorController::ProcessStateMachineStateAuthentication(uint8_t ConnID)
{
   if (TransactionAuthorized[ConnID])
   {
      moduleStatus[ConnID].isTransactionOngoing = true;
      sendStatusNotificationRequest(ConnID, "NoError", "Preparing", "");
   }
   else if (AUTH_FAILED)
   {
      sendStatusNotificationRequest(ConnID, "AuthFailed", "Unavailable", "");
   }
}

void CCSConnectorController::ProcessStateMachineStateCharge(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Charge;
   moduleStatus[ConnID].isTransactionOngoing = true;

   isConnectorCharging[ConnID] = true;

   if (connectorFault[ConnID])
   {
      moduleStatus[ConnID].stopReason = OCPPModule::StopReasons::Stop_EmergencyStop;
      sendStatusNotificationRequest(ConnID, "InternalError", "Faulted", "");
      return;
   }

   sendStatusNotificationRequest(ConnID, "NoError", "Charging", "");
}

void CCSConnectorController::ProcessStateMachineStateStopCharge(uint8_t ConnID)
{
   moduleStatus[ConnID].isTransactionOngoing = false;
   isConnectorCharging[ConnID] = false;

   sendStatusNotificationRequest(ConnID, "NoError", "Finishing", "");
}

void CCSConnectorController::ProcessStateMachineStateError(uint8_t ConnID)
{
   moduleStatus[ConnID].isTransactionOngoing = false;

   sendStatusNotificationRequest(ConnID, "InternalError", "Faulted", "");
}

void CCSConnectorController::ProcessStateMachineStateParameter(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Parameter;

   sendStatusNotificationRequest(ConnID, "NoError", "Preparing", "");
}

void CCSConnectorController::ProcessStateMachineStateIsolation(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Isolation;

   if (connectorFault[ConnID])
   {
      sendStatusNotificationRequest(ConnID, "IsolationFault", "Faulted", "");
      return;
   }

   sendStatusNotificationRequest(ConnID, "NoError", "Preparing", "");
}

void CCSConnectorController::ProcessStateMachineStatePreCharge(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::PreCharge;

   if (connectorFault[ConnID])
   {
      sendStatusNotificationRequest(ConnID, "PreChargeFault", "Faulted", "");
      return;
   }

   sendStatusNotificationRequest(ConnID, "NoError", "Preparing", "");
}

void CCSConnectorController::ProcessStateMachineStateWelding(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Welding;

   sendStatusNotificationRequest(ConnID, "NoError", "Finishing", "");
}

void CCSConnectorController::ProcessStateMachineStateSessionStop(uint8_t ConnID)
{
   moduleStatus[ConnID].isTransactionOngoing = false;
   isConnectorCharging[ConnID] = false;

   sendStatusNotificationRequest(ConnID, "NoError", "Finishing", "");
}

void CCSConnectorController::ProcessStateMachineStateShutOff(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::ShutOff;

   sendStatusNotificationRequest(ConnID, "NoError", "Unavailable", "");
}

void CCSConnectorController::ProcessStateMachineStatePaused(uint8_t ConnID)
{
   moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Paused;

   sendStatusNotificationRequest(ConnID, "NoError", "SuspendedEV", "");
}

void CCSConnectorController::updateMeterValues(uint8_t ConnID, bool Aligned, bool AC)
{
   if (ConnID >= Constants::MAX_MODULES)
      return;

   double voltage = 0;
   double current = 0;
   double power = 0;

   if (AC)
   {
      voltage = energy->GetVoltage(ConnID);
      current = energy->GetCurrent(ConnID);
      power = energy->GetPower(ConnID);

      moduleStatus[ConnID].ACMeterValues.voltage = voltage;
      moduleStatus[ConnID].ACMeterValues.current = current;
      moduleStatus[ConnID].ACMeterValues.power = power;
   }
   else
   {
      voltage = energy->GetVoltage(ConnID);
      current = energy->GetCurrent(ConnID);
      power = energy->GetPower(ConnID);

      moduleStatus[ConnID].DCMeterValues.voltage = voltage;
      moduleStatus[ConnID].DCMeterValues.current = current;
      moduleStatus[ConnID].DCMeterValues.power = power;
   }

   double energyValue = power * (Constants::TASK_DELAY_TIME / 1000.0);

   moduleStatus[ConnID].Energy += energyValue;

   if (Aligned)
   {
      MeterValueAlignedData[ConnID].Energy += energyValue;
      MeterValueAlignedData[ConnID].meterStop = moduleStatus[ConnID].Energy;
   }
}

void CCSConnectorController::updateStopReason(uint8_t connId)
{
   if (connId >= Constants::MAX_MODULES)
      return;

   if (emergency)
   {
      moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_EmergencyStop;
   }
   else if (connectorFault[connId])
   {
      moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
   }
   else if (powerLoss)
   {
      moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_PowerLoss;
   }
   else if (Rfid == false && moduleStatus[connId].isTransactionOngoing)
   {
      moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Local;
   }
   else
   {
      moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
   }
}

void CCSConnectorController::updateStopStansactionReason(uint8_t connId, ModuleStatus status)
{
   if (connId >= Constants::MAX_MODULES)
      return;

   updateStopReason(connId);

   moduleStatus[connId].chargingDuration = status.chargingDuration;

   moduleStatus[connId].meterStop = status.meterStop;
   moduleStatus[connId].DCmeterStop = status.DCmeterStop;
   moduleStatus[connId].ACmeterStop = status.ACmeterStop;

   moduleStatus[connId].isTransactionOngoing = false;
}

void CCSConnectorController::ConnectorTask(void *pvParameters)
{
   CCSConnectorController *self = static_cast<CCSConnectorController *>(pvParameters);

   while (true)
   {
      for (uint8_t ConnID = 1; ConnID < Constants::MAX_MODULES; ConnID++)
      {
         PLCModule::StateMachineState state = self->moduleStatus[ConnID].stateMachineState;

         switch (state)
         {
         case PLCModule::StateMachineState::SNA:
            self->ProcessStateMachineStateSNA(ConnID);
            break;

         case PLCModule::StateMachineState::Init:
            self->ProcessStateMachineStateInit(ConnID);
            break;

         case PLCModule::StateMachineState::Authentication:
            self->ProcessStateMachineStateAuthentication(ConnID);
            break;

         case PLCModule::StateMachineState::Parameter:
            self->ProcessStateMachineStateParameter(ConnID);
            break;

         case PLCModule::StateMachineState::Isolation:
            self->ProcessStateMachineStateIsolation(ConnID);
            break;

         case PLCModule::StateMachineState::PreCharge:
            self->ProcessStateMachineStatePreCharge(ConnID);
            break;

         case PLCModule::StateMachineState::Charge:
            self->ProcessStateMachineStateCharge(ConnID);
            break;

         case PLCModule::StateMachineState::Welding:
            self->ProcessStateMachineStateWelding(ConnID);
            break;

         case PLCModule::StateMachineState::StopCharge:
            self->ProcessStateMachineStateStopCharge(ConnID);
            break;

         case PLCModule::StateMachineState::SessionStop:
            self->ProcessStateMachineStateSessionStop(ConnID);
            break;

         case PLCModule::StateMachineState::ShutOff:
            self->ProcessStateMachineStateShutOff(ConnID);
            break;

         case PLCModule::StateMachineState::Paused:
            self->ProcessStateMachineStatePaused(ConnID);
            break;

         case PLCModule::StateMachineState::Error:
            self->ProcessStateMachineStateError(ConnID);
            break;

         default:
            self->ProcessStateMachineStateDefault(ConnID);
            break;
         }
         self->updateMeterValues(ConnID, true, false);

         if (!self->moduleStatus[ConnID].isTransactionOngoing)
         {
            self->updateStopReason(ConnID);
         }
      }

      vTaskDelay(pdMS_TO_TICKS(Constants::TASK_DELAY_TIME));
   }
}

void CCSConnectorController::OcppTask(void *pvParameters)
{
   CCSConnectorController *self = static_cast<CCSConnectorController *>(pvParameters);

   while (true)
   {
      if (self->isWebsocketConnected)
      {
         for (uint8_t ConnID = 1; ConnID < Constants::MAX_MODULES; ConnID++)
         {
            if (self->isConnectorCharging[ConnID])
            {
               self->sendStatusNotificationRequest(ConnID, "NoError", "Charging", "");
            }
            else
            {
               self->sendStatusNotificationRequest(ConnID, "NoError", "Available", "");
            }
         }
      }

      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

void CCSConnectorController::AuthorizeTask(void *pvParameters)
{
   CCSConnectorController *self = static_cast<CCSConnectorController *>(pvParameters);

   while (true)
   {
      for (uint8_t ConnID = 1; ConnID < Constants::MAX_MODULES; ConnID++)
      {
         if (self->Rfid && !self->TransactionAuthorized[ConnID])
         {
            self->TransactionAuthorized[ConnID] = true;
         }

         if (!self->Rfid && self->TransactionAuthorized[ConnID])
         {
            self->TransactionAuthorized[ConnID] = false;
         }
      }

      vTaskDelay(pdMS_TO_TICKS(500));
   }
}
*/