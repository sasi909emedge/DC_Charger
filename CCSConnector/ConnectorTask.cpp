#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "CCSConnector.hpp"
#include <cstring>
#include <cmath>

#define TAG "Connector"
#define TAG_STATUS "Status"
namespace CCSConnector
{
    void CCSConnectorController::updateStopStansactionReason(uint8_t connId, ModuleStatus moduleStatus)
    {
        ModuleStatus moduleStatus_temp = moduleStatus;

        if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_DeAuthorized)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "DeAuthorized", strlen("DeAuthorized"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_EVDisconnected)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "EVDisconnected", strlen("EVDisconnected"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_HardReset)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "HardReset", strlen("HardReset"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_Local)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "Local", strlen("Local"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_Other)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "Other", strlen("Other"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_PowerLoss)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "PowerLoss", strlen("PowerLoss"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_Reboot)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "Reboot", strlen("Reboot"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_Remote)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "Remote", strlen("Remote"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_SoftReset)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "SoftReset", strlen("SoftReset"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_UnlockCommand)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "UnlockCommand", strlen("UnlockCommand"));
        }
        else if (moduleStatus_temp.stopReason == OCPPModule::StopReasons::Stop_EmergencyStop)
        {
            memcpy(ocpp->CPStopTransactionRequest[connId].reason, "EmergencyStop", strlen("EmergencyStop"));
        }
    }

    void CCSConnectorController::updateStopReason(uint8_t connId)
    {
        if (emergency)
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_EmergencyStop;
        else if (powerLoss)
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_PowerLoss;
        else if (earthDisconnect)
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else if (gfci[connId])
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else if (overTemp[connId])
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else if (overVoltage[connId])
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else if (underVoltage[connId])
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else if (overCurrent[connId])
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else if (relayWeld[connId])
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
        else
            moduleStatus[connId].stopReason = OCPPModule::StopReasons::Stop_Other;
    }

    void CCSConnectorController::ProcessStateMachineStateSNA(uint8_t ConnID)
    {
        plc->SetModuleStatusToSNA(ConnID);
        SetModuleStatusToSNA(ConnID);
    }
    void CCSConnectorController::ProcessStateMachineStateDefault(uint8_t ConnID)
    {
        if ((moduleStatus[ConnID].controlPilotState == PLCModule::ControlPilotState::A) &&
            moduleStatus[ConnID].isTransactionOngoing)
        {
            moduleStatus[ConnID].isTransactionOngoing = false;
            write_connectors_offline_data(ConnID);
            writeConnectorModuleStatus(ConnID);
        }
        else if (moduleStatus[ConnID].isTransactionOngoing == false)
        {
            moduleStatus[ConnID].offlineStarted = false;
            plc->SetModuleStatusToSNA(ConnID);
            SetModuleStatusToSNA(ConnID);
        }
    }
    void CCSConnectorController::ProcessStateMachineStateInit(uint8_t ConnID)
    {
        plc->Set_EVSEFreeService(ConnID, false);
        plc->Set_EVSEStatusCode(ConnID, PLCModule::EVSEStatusCode::EVSE_Ready);
        plc->Set_EVSEIsolationStatus(ConnID, PLCModule::EVSEIsolationStatus::SNA);
        TransactionAuthorized[ConnID] = false;
    }
    void CCSConnectorController::ProcessStateMachineStateAuthentication(uint8_t ConnID)
    {
        if (moduleStatus[ConnID].isTransactionOngoing && (moduleStatus[ConnID].EVMacAddress == plc->Get_EVMacAddress(ConnID)))
        {
            TransactionAuthorized[ConnID] = true;
        }

        if ((RfidAuthorized || ALPRAuthorized[ConnID] || RemoteAuthorized[ConnID]) && (TransactionAuthorized[ConnID] == false))
        {
            setNULL(moduleStatus[ConnID].idTag);
            if (RfidAuthorized)
                memcpy(moduleStatus[ConnID].idTag, Rfid_tag, sizeof(Rfid_tag));
            if (ALPRAuthorized[ConnID])
                memcpy(moduleStatus[ConnID].idTag, Alpr_tag, sizeof(Alpr_tag));
            if (RemoteAuthorized[ConnID])
                memcpy(moduleStatus[ConnID].idTag, Remote_tag, sizeof(Remote_tag));

            if (isWebsocketConnected && isChargerBooted)
            {
                moduleStatus[ConnID].offlineStarted = false;
                setNULL(ocpp->CPStartTransactionRequest[ConnID].idTag);
                ocpp->CPStartTransactionRequest[ConnID].connectorId = ConnID;
                memcpy(ocpp->CPStartTransactionRequest[ConnID].idTag, moduleStatus[ConnID].idTag, sizeof(moduleStatus[ConnID].idTag));
                ocpp->CPStartTransactionRequest[ConnID].meterStart = (int)(moduleStatus[ConnID].meterStop);
                ocpp->CPStartTransactionRequest[ConnID].reservationIdPresent = false;
                setNULL(ocpp->CPStartTransactionRequest[ConnID].timestamp);
                SystemTime->getTimeInOcppFormat(ocpp->CPStartTransactionRequest[ConnID].timestamp);
                SystemTime->getTimeInOcppFormat(moduleStatus[ConnID].meterStart_time);
                ocpp->sendStartTransactionRequest(ConnID);
            }
            else if ((config->networkMode == ConfigModule::NetworkMode::OFFLINE) ||
                     (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE) ||
                     (config->networkMode == ConfigModule::NetworkMode::PLUGNPLAY))
            {
                moduleStatus[ConnID].offlineStarted = true;
                TransactionAuthorized[ConnID] = true;
            }
            else
            {
            }

            if (RfidAuthorized)
                RfidAuthorized = false;
            if (ALPRAuthorized[ConnID])
                ALPRAuthorized[ConnID] = false;
            if (RemoteAuthorized[ConnID])
                RemoteAuthorized[ConnID] = false;
        }

        if (ocpp->CPStartTransactionRequest[ConnID].Sent)
        {
            TransactionResTimeout[ConnID]++;
            if (TransactionResTimeout[ConnID] > ocpp->CPGetConfigurationResponse.TransactionMessageAttempts)
            {
                if (isWebsocketConnected && isChargerBooted)
                {
                    TransactionResTimeout[ConnID] = 0;
                    ocpp->sendStartTransactionRequest(ConnID);
                }
                else if ((config->networkMode == ConfigModule::NetworkMode::OFFLINE) ||
                         (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE) ||
                         (config->networkMode == ConfigModule::NetworkMode::PLUGNPLAY))
                {
                    moduleStatus[ConnID].offlineStarted = true;
                    TransactionAuthorized[ConnID] = true;
                }
            }
        }

        if (ocpp->CMSStartTransactionResponse[ConnID].Received)
        {
            ocpp->CMSStartTransactionResponse[ConnID].Received = false;
            ocpp->CPStartTransactionRequest[ConnID].Sent = false;
            if (memcmp(ocpp->CMSStartTransactionResponse[ConnID].idtaginfo.status, Accepted, sizeof(Accepted)) == 0)
            {
                TransactionAuthorized[ConnID] = true;
                moduleStatus[ConnID].transactionId = ocpp->CMSStartTransactionResponse[ConnID].transactionId;
                TransactionResTimeout[ConnID] = 0;
            }
        }

        if (TransactionAuthorized[ConnID])
        {
            plc->Set_EVSEStatusCode(ConnID, PLCModule::EVSEStatusCode::EVSE_Ready);
            plc->Set_EVSEIsolationStatus(ConnID, PLCModule::EVSEIsolationStatus::Invalid);
            plc->Set_EVSEProcessingCA(ConnID, PLCModule::EVSEProcessing::Finished);
        }
        else
        {
            plc->Set_EVSEProcessingCA(ConnID, PLCModule::EVSEProcessing::Ongoing);
        }
    }

    void CCSConnectorController::ProcessStateMachineStateParameter(uint8_t ConnID)
    {
        if (connector->moduleStatus[ConnID].isParameterStateFinished)
        {
            plc->Set_EVSEProcessingCPD(ConnID, PLCModule::EVSEProcessing::Finished);
            plc->Set_EVSEIsolationStatus(ConnID, PLCModule::EVSEIsolationStatus::Invalid);
        }
        else
        {
            plc->Set_EVSEProcessingCPD(ConnID, PLCModule::EVSEProcessing::Ongoing);
        }
    }
    void CCSConnectorController::ProcessStateMachineStateIsolation(uint8_t ConnID)
    {
        moduleStatus[ConnID].chargingDuration = 0;
        plc->Set_EVSEProcessingCC(ConnID, PLCModule::EVSEProcessing::Finished);
        // Actual IMD will update the isolation status
        plc->Set_EVSEIsolationStatus(ConnID, PLCModule::EVSEIsolationStatus::Valid); // forcing isolation status to valid
    }
    void CCSConnectorController::ProcessStateMachineStatePreCharge(uint8_t ConnID)
    {
        gpio->GpioStateSet(gpio->DcGun[ConnID], GpioModule::GpioState::ON);
    }

    void CCSConnectorController::ProcessStateMachineStateCharge(uint8_t ConnID)
    {
        static uint32_t loopCount[NUM_OF_CONNECTORS];
        static uint32_t meterValueTimeout[NUM_OF_CONNECTORS];
        if (connectorFault[ConnID])
        {
            if (config->restoreSessionFromFault && powerLoss)
            {
                ESP_LOGI(TAG, "Waiting for power to resume the session");
            }
            else if ((config->restoreSessionFromFault == false) ||
                     overCurrent[ConnID] ||
                     gfci[ConnID])
            {
                updateStopReason(ConnID);
                plc->Set_EVSEStatusCode(ConnID, PLCModule::EVSEStatusCode::EVSE_EmergencyShutdown);
            }
            else
            {
                ChargingFaultTimeCount[ConnID]++;
                ESP_LOGI(TAG, "Connector %hhu Charging Fault Time %ld", ConnID, ChargingFaultTimeCount[ConnID]);
                if (ChargingFaultTimeCount[ConnID] > config->restoreSessionFromFaultTime)
                {
                    updateStopReason(ConnID);
                    plc->Set_EVSEStatusCode(ConnID, PLCModule::EVSEStatusCode::EVSE_EmergencyShutdown);
                }
            }
        }
        else
        {
            ChargingFaultTimeCount[ConnID] = 0;

            moduleStatus[ConnID].chargingDuration++;
            ESP_LOGD(TAG, "Charging Time %ld", moduleStatus[ConnID].chargingDuration);
            if (loopCount[ConnID] % MeterValueSampleTime == 0)
            {
                moduleStatus[ConnID].DCMeterValues = energy->GetDCEnergyMeterValue(ConnID);
                moduleStatus[ConnID].meterStop = moduleStatus[ConnID].meterStop + ((moduleStatus[ConnID].DCMeterValues.power * MeterValueSampleTime) / 3600.0);
                moduleStatus[ConnID].Energy = moduleStatus[ConnID].meterStop - moduleStatus[ConnID].meterStart;
                writeConnectorModuleStatus(ConnID);
                if (ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval != 0)
                {
                    MeterValueAlignedData[ConnID].temp = (MeterValueAlignedData[ConnID].temp * ClockAlignedDataCount[ConnID] + moduleStatus[ConnID].DCMeterValues.temperature) / (ClockAlignedDataCount[ConnID] + 1);
                    MeterValueAlignedData[ConnID].voltage[ConnID] = (MeterValueAlignedData[ConnID].voltage[ConnID] * ClockAlignedDataCount[ConnID] + moduleStatus[ConnID].DCMeterValues.voltage) / (ClockAlignedDataCount[ConnID] + 1);
                    MeterValueAlignedData[ConnID].current[ConnID] = (MeterValueAlignedData[ConnID].current[ConnID] * ClockAlignedDataCount[ConnID] + moduleStatus[ConnID].DCMeterValues.current) / (ClockAlignedDataCount[ConnID] + 1);
                    MeterValueAlignedData[ConnID].power[0] = (MeterValueAlignedData[ConnID].power[0] * ClockAlignedDataCount[ConnID] + moduleStatus[ConnID].DCMeterValues.power) / (ClockAlignedDataCount[ConnID] + 1);
                    MeterValueAlignedData[ConnID].Energy = MeterValueAlignedData[ConnID].Energy + moduleStatus[ConnID].Energy;
                }
                ClockAlignedDataCount[ConnID]++;
            }
            if (isWebsocketConnected & isChargerBooted)
            {
                if (ClockAlignedDataTime[ConnID])
                {
                    ClockAlignedDataTime[ConnID] = false;
                    updateMeterValues(ConnID, ALIGNED_DATA, false);
                    ocpp->sendMeterValuesRequest(ConnID, OCPPModule::TransactionContext::SampleClock, ALIGNED_DATA, false);
                    ClockAlignedDataCount[ConnID] = 0;
                }
                if (ocpp->CPMeterValuesRequest[ConnID].Sent == false)
                {
                    // vTaskDelay(100 / portTICK_PERIOD_MS);
                    updateMeterValues(ConnID, SAMPLED_DATA, false);
                    ocpp->sendMeterValuesRequest(ConnID, OCPPModule::TransactionContext::SamplePeriodic, SAMPLED_DATA, false);
                    // if (DS.isConnected)
                    //     sendMeterValuesToDS(ConnID);
                    meterValueTimeout[ConnID] = 0;
                }

                if ((ocpp->CMSMeterValuesResponse[ConnID].Received == false) && (ocpp->CPMeterValuesRequest[ConnID].Sent == true))
                {
                    meterValueTimeout[ConnID]++;
                }
                else
                {
                    meterValueTimeout[ConnID] = 0;
                }
                if (meterValueTimeout[ConnID] > 60)
                {
                    meterValueTimeout[ConnID] = 0;
                    updateMeterValues(ConnID, SAMPLED_DATA, false);
                    ocpp->sendMeterValuesRequest(ConnID, OCPPModule::TransactionContext::SamplePeriodic, SAMPLED_DATA, false);
                }
            }
            if ((loopCount[ConnID] % 30 == 0) && (isWebsocketConnected == false))
            {
                ESP_LOGI(TAG, "Con-%hhu-> Energy %f, Power %f, Voltage %f, Current %f",
                         ConnID,
                         moduleStatus[ConnID].Energy,
                         moduleStatus[ConnID].DCMeterValues.power,
                         moduleStatus[ConnID].DCMeterValues.voltage,
                         moduleStatus[ConnID].DCMeterValues.current);
            }
            loopCount[ConnID]++;
            if (loopCount[ConnID] % ocpp->CPGetConfigurationResponse.MeterValueSampleInterval == 0)
            {
                ocpp->CPMeterValuesRequest[ConnID].Sent = false;
            }
        }
    }
    void CCSConnectorController::ProcessStateMachineStateStopCharge(uint8_t ConnID)
    {
        if (connectorFault[ConnID])
            plc->Set_EVSEStatusCode(ConnID, PLCModule::EVSEStatusCode::EVSE_EmergencyShutdown);
        if (moduleStatus[ConnID].DCMeterValues.current < 1.0f)
        {
            gpio->GpioStateSet(gpio->DcGun[ConnID], GpioModule::GpioState::OFF);
        }
    }
    void CCSConnectorController::ProcessStateMachineStateWelding(uint8_t ConnID)
    {
    }
    void CCSConnectorController::ProcessStateMachineStateSessionStop(uint8_t ConnID)
    {
        // Stop Power Modules
        if (connectorFault[ConnID])
            plc->Set_EVSEStatusCode(ConnID, PLCModule::EVSEStatusCode::EVSE_EmergencyShutdown);

        plc->Set_EVSEIsolationStatus(ConnID, PLCModule::EVSEIsolationStatus::SNA);
        static uint32_t timeout[NUM_OF_CONNECTORS] = {0};
        static uint32_t retryCount[NUM_OF_CONNECTORS] = {0};
        isConnectorCharging[ConnID] = false;
        moduleStatus[ConnID].isTransactionOngoing = false;
        if (isWebsocketConnected & isChargerBooted)
        {
            if (ocpp->CPStopTransactionRequest[ConnID].Sent == false)
            {
                setNULL(ocpp->CPStopTransactionRequest[ConnID].idTag);
                setNULL(ocpp->CPStopTransactionRequest[ConnID].reason);
                setNULL(moduleStatus[ConnID].meterStop_time);
                SystemTime->getTimeInOcppFormat(moduleStatus[ConnID].meterStop_time);
                memcpy(ocpp->CPStopTransactionRequest[ConnID].idTag, moduleStatus[ConnID].idTag, sizeof(moduleStatus[ConnID].idTag));
                ocpp->CPStopTransactionRequest[ConnID].meterStop = (int)moduleStatus[ConnID].meterStop;
                memcpy(ocpp->CPStopTransactionRequest[ConnID].timestamp, moduleStatus[ConnID].meterStop_time, strlen(moduleStatus[ConnID].meterStop_time));
                ocpp->CPStopTransactionRequest[ConnID].transactionId = (int)moduleStatus[ConnID].transactionId;
                updateStopStansactionReason(ConnID, moduleStatus[ConnID]);
                ocpp->sendStopTransactionRequest(ConnID, OCPPModule::TransactionContext::InterruptionEnd, SAMPLED_DATA, false, ONLINE_TRANSACTION);
                retryCount[ConnID] = 0;
                if (connectorFault[ConnID] == 0)
                    finishingStatusSent[ConnID] = true;
                else
                    finishingStatusPending[ConnID] = true;
                timeout[ConnID] = 0;
            }
        }
        else
        {
            setNULL(moduleStatus[ConnID].meterStop_time);
            SystemTime->getTimeInOcppFormat(moduleStatus[ConnID].meterStop_time);
            write_connectors_offline_data(ConnID);
            if (connectorFault[ConnID] == 0)
            {
                finishingStatusSent[ConnID] = true;
                Set_FINISHING(ConnID);
                if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                    ledState[ConnID] = OCPPModule::LED_STATE::OFFLINE_FINISHING_LED_STATE;
                else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                    ledState[ConnID] = OCPPModule::LED_STATE::OFFLINE_ONLY_FINISHING_LED_STATE;
                else
                    ledState[ConnID] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FINISHING_LED_STATE;
            }
            else
                finishingStatusPending[ConnID] = true;

            ESP_LOGI(TAG_STATUS, "Connector %d Finishing", ConnID);
            // logmoduleStatus(ConnID);
            moduleStatus[ConnID].InvalidId = false;
            moduleStatus[ConnID].UnkownId = false;
            if (moduleStatus[ConnID].meterStop > 10000000)
            {
                moduleStatus[ConnID].meterStop = 0;
            }
            writeConnectorModuleStatus(ConnID);
        }
        if (isWebsocketConnected & isChargerBooted)
        {
            if ((ocpp->CMSStopTransactionResponse[ConnID].Received == false) && (ocpp->CPStopTransactionRequest[ConnID].Sent == true))
            {
                timeout[ConnID]++;
            }
            else
            {
                moduleStatus[ConnID].isTransactionOngoing = false;
                timeout[ConnID] = 0;
                ocpp->CMSStopTransactionResponse[ConnID].Received = false;
                ocpp->CPStopTransactionRequest[ConnID].Sent = false;
                moduleStatus[ConnID].InvalidId = false;
                moduleStatus[ConnID].UnkownId = false;
                // logmoduleStatus(ConnID);
                if (moduleStatus[ConnID].meterStop > 10000000)
                {
                    moduleStatus[ConnID].meterStop = 0;
                    writeConnectorModuleStatus(ConnID);
                }
                else
                {
                    writeConnectorModuleStatus(ConnID);
                }
                if (connectorFault[ConnID] == 0)
                {
                    finishingStatusSent[ConnID] = true;
                    sendStatusNotificationRequest(ConnID, NoError, Finishing, NoError);
                    Set_FINISHING(ConnID);
                    if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                        ledState[ConnID] = OCPPModule::LED_STATE::FINISHING_LED_STATE;
                    else
                        ledState[ConnID] = OCPPModule::LED_STATE::ONLINE_ONLY_FINISHING_LED_STATE;
                }
                else
                    finishingStatusPending[ConnID] = true;

                ESP_LOGI(TAG_STATUS, "Connector %d Finishing", ConnID);
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }
            if (timeout[ConnID] > ocpp->CPGetConfigurationResponse.TransactionMessageRetryInterval)
            {
                timeout[ConnID] = 0;
                retryCount[ConnID]++;
                if (retryCount[ConnID] > ocpp->CPGetConfigurationResponse.TransactionMessageAttempts)
                {
                    // taskState[ConnID] = flag_EVSE_Read_Id_Tag;
                }
                ocpp->sendStopTransactionRequest(ConnID, OCPPModule::TransactionContext::InterruptionEnd, SAMPLED_DATA, false, ONLINE_TRANSACTION);
            }
        }
    }
    void CCSConnectorController::ProcessStateMachineStateShutOff(uint8_t ConnID)
    {
        // do nothing
    }
    void CCSConnectorController::ProcessStateMachineStatePaused(uint8_t ConnID)
    {
        // do nothing
    }
    void CCSConnectorController::ProcessStateMachineStateError(uint8_t ConnID)
    {
        // do nothing
    }

    void CCSConnectorController::ConnectorTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay
        CCSConnectorController *connector = static_cast<CCSConnectorController *>(pvParameters);
        // get task name
        const char *currentTaskName = pcTaskGetName(NULL);
        // get connector number from task name
        uint8_t ConnID = 0;
        uint32_t loopCount = 0;
        TickType_t xLastWakeTime = xTaskGetTickCount();
        if (sscanf(currentTaskName, "ConnectorTask_%hhu", &ConnID) != 1)
        {
            // Handle error
            return;
        }

        ESP_LOGI("Connector", "Task %hhu started for Connector %hhu", ConnID, ConnID);
        while (true)
        {
            loopCount++;
            time_t currentTime = 0;
            // get current time in seconds since epoch
            SystemTime->getTime(&currentTime);
            plc->Set_EVSEDateTimeNow(ConnID, static_cast<uint64_t>(currentTime));
            connector->moduleStatus[ConnID].controlPilotState = plc->Get_ControlPilotState(ConnID);
            connector->moduleStatus[ConnID].stateMachineState = plc->Get_StateMachineState(ConnID);
            connector->moduleStatus[ConnID].controlPilotDuty = plc->Get_ControlPilotDuty(ConnID);
            bool isStateMachineChanged = (connector->moduleStatus[ConnID].stateMachineState != connector->stateMachineState_old[ConnID]) ? true : false;

            switch (connector->moduleStatus[ConnID].stateMachineState)
            {
            case PLCModule::StateMachineState::SNA:
                // do nothing
                break;
            case PLCModule::StateMachineState::Default:
                if (isStateMachineChanged)
                {
                    connector->ProcessStateMachineStateDefault(ConnID);
                }
                break;
            case PLCModule::StateMachineState::Init:
                connector->ProcessStateMachineStateInit(ConnID);
                break;
            case PLCModule::StateMachineState::Authentication:
                connector->ProcessStateMachineStateAuthentication(ConnID);
                break;
            case PLCModule::StateMachineState::Parameter:
                connector->ProcessStateMachineStateParameter(ConnID);
                break;
            case PLCModule::StateMachineState::Isolation:
                connector->ProcessStateMachineStateIsolation(ConnID);
                break;
            case PLCModule::StateMachineState::PreCharge:
                connector->ProcessStateMachineStatePreCharge(ConnID);
                break;
            case PLCModule::StateMachineState::Charge:
                connector->ProcessStateMachineStateCharge(ConnID);
                break;
            case PLCModule::StateMachineState::Welding:
                connector->ProcessStateMachineStateWelding(ConnID);
                break;
            case PLCModule::StateMachineState::StopCharge:
                connector->ProcessStateMachineStateStopCharge(ConnID);
                break;
            case PLCModule::StateMachineState::SessionStop:
                connector->ProcessStateMachineStateSessionStop(ConnID);
                break;
            case PLCModule::StateMachineState::ShutOff:
                connector->ProcessStateMachineStateShutOff(ConnID);
                break;
            case PLCModule::StateMachineState::Paused:
                connector->ProcessStateMachineStatePaused(ConnID);
                break;
            case PLCModule::StateMachineState::Error:
                connector->ProcessStateMachineStateError(ConnID);
                break;
            default:
                break;
            }

            connector->moduleStatus[ConnID].DCMeterValues = energy->GetDCEnergyMeterValue(ConnID);

            plc->Set_EVSEPresentCurrent(ConnID, connector->moduleStatus[ConnID].DCMeterValues.current);
            plc->Set_EVSEPresentVoltage(ConnID, connector->moduleStatus[ConnID].DCMeterValues.voltage);

            plc->SendEVSEDateTime(ConnID);
            plc->SendEVSEDCMaxLimits(ConnID);
            plc->SendEVSEDCRegulationLimits(ConnID);
            plc->SendEVSEStatus(ConnID);
            plc->SendEVSEDCMaxLimits(ConnID);
            plc->SendEVSEPlugStatus(ConnID);

            connector->SendEVMaxData(ConnID);
            connector->SendEVTargetData(ConnID);

            if ((loopCount % 300 == 0) || (connector->moduleStatus[ConnID].stateMachineState != connector->stateMachineState_old[ConnID]))
            {
                switch (connector->moduleStatus[ConnID].stateMachineState)
                {
                case PLCModule::StateMachineState::SNA:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: SNA", ConnID);
                    break;
                case PLCModule::StateMachineState::Default:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Default", ConnID);
                    break;
                case PLCModule::StateMachineState::Init:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Init", ConnID);
                    break;
                case PLCModule::StateMachineState::Authentication:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Authentication", ConnID);
                    break;
                case PLCModule::StateMachineState::Parameter:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Parameter", ConnID);
                    break;
                case PLCModule::StateMachineState::Isolation:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Isolation", ConnID);
                    break;
                case PLCModule::StateMachineState::PreCharge:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: PreCharge", ConnID);
                    break;
                case PLCModule::StateMachineState::Charge:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Charge", ConnID);
                    break;
                case PLCModule::StateMachineState::Welding:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Welding", ConnID);
                    break;
                case PLCModule::StateMachineState::StopCharge:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: StopCharge", ConnID);
                    break;
                case PLCModule::StateMachineState::SessionStop:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: SessionStop", ConnID);
                    break;
                case PLCModule::StateMachineState::ShutOff:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: ShutOff", ConnID);
                    break;
                case PLCModule::StateMachineState::Paused:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Paused", ConnID);
                    break;
                case PLCModule::StateMachineState::Error:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Error", ConnID);
                    break;
                default:
                    ESP_LOGI(TAG, "Connector %hhu: SMState: Unkown", ConnID);
                    break;
                }

                switch (plc->Get_ProximityPinState(ConnID))
                {
                case PLCModule::ProximityPinState::Not_Connected:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Not_Connected", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type2_Connected13A:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type2_Connected13A", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type2_Connected20A:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type2_Connected20A", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type2_Connected32A:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type2_Connected32A", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type2_Connected63A:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type2_Connected63A", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type2_Connected:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type2_Connected", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type1_Connected:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type1_Connected", ConnID);
                    break;
                case PLCModule::ProximityPinState::Type1_Connected_Button_Pressed:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Type1_Connected_Button_Pressed", ConnID);
                    break;
                case PLCModule::ProximityPinState::Invalid:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Invalid", ConnID);
                    break;
                case PLCModule::ProximityPinState::SNA:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: SNA", ConnID);
                    break;
                default:
                    ESP_LOGD(TAG, "Connector %hhu: PPState: Unkown", ConnID);
                    break;
                }

                switch (plc->Get_ControlPilotState(ConnID))
                {
                case PLCModule::ControlPilotState::A:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: A", ConnID);
                    break;
                case PLCModule::ControlPilotState::B:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: B", ConnID);
                    break;
                case PLCModule::ControlPilotState::C:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: C", ConnID);
                    break;
                case PLCModule::ControlPilotState::D:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: D", ConnID);
                    break;
                case PLCModule::ControlPilotState::E:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: E", ConnID);
                    break;
                case PLCModule::ControlPilotState::F:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: F", ConnID);
                    break;
                case PLCModule::ControlPilotState::Invalid:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: Invalid", ConnID);
                    break;
                case PLCModule::ControlPilotState::SNA:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: SNA", ConnID);
                    break;

                default:
                    ESP_LOGI(TAG, "Connector %hhu: CPState: Unkown", ConnID);
                    break;
                }
            }
            connector->stateMachineState_old[ConnID] = connector->moduleStatus[ConnID].stateMachineState;

            vTaskDelayUntil(&xLastWakeTime, (Constants::TASK_DELAY_TIME / portTICK_PERIOD_MS));
        }
    }
}