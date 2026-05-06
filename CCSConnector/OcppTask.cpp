#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "CCSConnector.hpp"
#include <cstring>
#include <cmath>

#define TAG "OCPP_TASK"
#define TAG_STATUS "Status"

namespace CCSConnector
{

    void CCSConnectorController::encodeAndSendStatusNotification(uint8_t connId, char *errorCode, char *status, char *info, uint8_t trigger)
    {
        bool weakfaultstatusChanged = false;
        bool connectedToEthernet = false;
        bool connectedToWifi = false;
        bool connectedToGsm = false;

        esp_netif_t *default_netif = esp_netif_get_default_netif();
        if (default_netif != NULL)
        {
            if (strcmp(esp_netif_get_desc(default_netif), "ppp") == 0)
            {
                connectedToGsm = true;
            }
            else if (strcmp(esp_netif_get_desc(default_netif), "sta") == 0)
            {
                connectedToWifi = true;
            }
            else if (strcmp(esp_netif_get_desc(default_netif), "eth") == 0)
            {
                connectedToEthernet = true;
            }
        }

        if (trigger == WEAK_SIGNAL_TRIGGER)
        {

            if (memcmp(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted) != 0) && network->gsmWeakSignal && connectedToGsm)
            {
                setNULL(ocpp->CPStatusNotificationRequest[0].status);
                memcpy(ocpp->CPStatusNotificationRequest[0].status, ocpp->CPStatusNotificationRequest[connId].status, strlen(ocpp->CPStatusNotificationRequest[connId].status));
                weakfaultstatusChanged = true;
                setNULL(ocpp->CPStatusNotificationRequest[connId].status);
                setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
                setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                memcpy(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted));
                memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, WeakSignal, strlen(WeakSignal));
                snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "GSM %ddB", network->gsmSignalStrength);
            }
            else if (memcmp(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted) != 0) && network->wifiWeakSignal && connectedToWifi)
            {
                setNULL(ocpp->CPStatusNotificationRequest[0].status);
                memcpy(ocpp->CPStatusNotificationRequest[0].status, ocpp->CPStatusNotificationRequest[connId].status, strlen(ocpp->CPStatusNotificationRequest[connId].status));
                weakfaultstatusChanged = true;
                setNULL(ocpp->CPStatusNotificationRequest[connId].status);
                setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
                setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                memcpy(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted));
                memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, WeakSignal, strlen(WeakSignal));
                snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "WIFI %ddB", network->wifiSignalStrength);
            }
            else if (memcmp(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted) != 0))
            {
                setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
                setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, errorCode, strlen(errorCode));
                memcpy(ocpp->CPStatusNotificationRequest[connId].info, info, strlen(info));
            }
            else
            {
                return;
            }
        }
        else
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
            setNULL(ocpp->CPStatusNotificationRequest[connId].info);
            setNULL(ocpp->CPStatusNotificationRequest[connId].status);
            memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, errorCode, strlen(errorCode));
            memcpy(ocpp->CPStatusNotificationRequest[connId].info, info, strlen(info));
            memcpy(ocpp->CPStatusNotificationRequest[connId].status, status, strlen(status));
            if ((memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, OverVoltage, sizeof(OverVoltage)) == 0) ||
                (memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, UnderVoltage, sizeof(UnderVoltage)) == 0))
            {
                setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "%.1f", moduleStatus[connId].DCMeterValuesFault.voltage);
            }
            if (memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, EVCommunicationError, sizeof(EVCommunicationError)) == 0)
            {
                // ToDo
                //  setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                //  snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "%d", cp_adc_value[connId]);
            }
            if (memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, HighTemperature, sizeof(HighTemperature)) == 0)
            {
                setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "%.1f", moduleStatus[connId].DCMeterValuesFault.temperature);
            }
            if (memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, OverCurrentFailure, sizeof(OverCurrentFailure)) == 0)
            {
                setNULL(ocpp->CPStatusNotificationRequest[connId].info);
                snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "%.1f", moduleStatus[connId].DCMeterValuesFault.current);
            }
        }
#if WEAK_SIGNAL_ERRORCODE
        if ((memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, NoError, sizeof(NoError)) == 0) && network->gsmWeakSignal && connectedToGsm)
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
            setNULL(ocpp->CPStatusNotificationRequest[connId].info);
            memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, WeakSignal, strlen(WeakSignal));
            snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "GSM %ddB", network->gsmSignalStrength);
        }
        else if ((memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, NoError, sizeof(NoError)) == 0) && network->wifiWeakSignal && connectedToWifi)
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
            setNULL(ocpp->CPStatusNotificationRequest[connId].info);
            memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, WeakSignal, strlen(WeakSignal));
            snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "WIFI %lddB", network->wifiSignalStrength);
        }
#endif
#if READER_FAILURE
        else if ((memcmp(ocpp->CPStatusNotificationRequest[connId].errorCode, NoError, sizeof(NoError)) == 0) && readerFailure)
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].errorCode);
            memcpy(ocpp->CPStatusNotificationRequest[connId].errorCode, ReaderFailure, strlen(ReaderFailure));
        }
#endif

        if (memcmp(ocpp->CPStatusNotificationRequest[connId].info, "EmergencyPressed", strlen("EmergencyPressed")) == 0)
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].vendorErrorCode);
            memcpy(ocpp->CPStatusNotificationRequest[connId].vendorErrorCode, "EmergencyPressed", strlen("EmergencyPressed"));
        }
        else if (memcmp(ocpp->CPStatusNotificationRequest[connId].info, "EarthDisconnect", strlen("EarthDisconnect")) == 0)
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].vendorErrorCode);
            memcpy(ocpp->CPStatusNotificationRequest[connId].vendorErrorCode, "EarthDisconnect", strlen("EarthDisconnect"));
        }
        else
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].vendorErrorCode);
            memcpy(ocpp->CPStatusNotificationRequest[connId].vendorErrorCode, "None", strlen("None"));
        }

        if ((memcmp(ocpp->CPStatusNotificationRequest[connId].info, "None", strlen("None")) == 0) && config->gsmEnable && network->gsmSignalStrengthReceived && connectedToGsm)
            snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "GSM %ddB", network->gsmSignalStrength);
        if ((memcmp(ocpp->CPStatusNotificationRequest[connId].info, "None", strlen("None")) == 0) && config->wifiEnable && network->wifiSignalStrengthReceived && connectedToWifi)
            snprintf(ocpp->CPStatusNotificationRequest[connId].info, sizeof(ocpp->CPStatusNotificationRequest[connId].info), "WIFI %ddB", network->wifiSignalStrength);
        ocpp->CPStatusNotificationRequest[connId].connectorId = connId;

        setNULL(ocpp->CPStatusNotificationRequest[connId].timestamp);
        SystemTime->getTimeInOcppFormat(ocpp->CPStatusNotificationRequest[connId].timestamp);
        ocpp->sendStatusNotificationRequest(connId);
        if (weakfaultstatusChanged)
        {
            setNULL(ocpp->CPStatusNotificationRequest[connId].status);
            memcpy(ocpp->CPStatusNotificationRequest[connId].status, ocpp->CPStatusNotificationRequest[0].status, strlen(ocpp->CPStatusNotificationRequest[0].status));
        }
    }

    void CCSConnectorController::updateMeterValues(uint8_t connId, bool Aligned, bool AC)
    {
        char EnergyValuestr[20];
        char PowerValuestr[20];
        char VoltageValuestr[4][20];
        char CurrentValuestr[4][20];
        char TempValuestr[20];
        char SocValuestr[20];
        char PowerFactorValuestr[20];

        setNULL(EnergyValuestr);
        setNULL(PowerValuestr);
        setNULL(VoltageValuestr);
        setNULL(CurrentValuestr);
        setNULL(TempValuestr);
        setNULL(SocValuestr);
        setNULL(PowerFactorValuestr);

        if (Aligned)
        {
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[1].context, "Sample.Clock", strlen("Sample.Clock"));
            snprintf(PowerValuestr, sizeof(PowerValuestr), "%.2lf", MeterValueAlignedData[connId].power[0] / 1000);
            snprintf(VoltageValuestr[1], sizeof(VoltageValuestr[1]), "%.2lf", MeterValueAlignedData[connId].voltage[0]);
            snprintf(CurrentValuestr[1], sizeof(CurrentValuestr[1]), "%.2lf", MeterValueAlignedData[connId].current[0]);
            snprintf(TempValuestr, sizeof(TempValuestr), "%u", MeterValueAlignedData[connId].temp);
            snprintf(SocValuestr, sizeof(SocValuestr), "%u", plc->Get_EVSoC(connId));
        }
        else
        {
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[1].context, "Sample.Periodic", strlen("Sample.Periodic"));
            snprintf(PowerValuestr, sizeof(PowerValuestr), "%.2lf", moduleStatus[connId].DCMeterValues.power / 1000);
            snprintf(VoltageValuestr[1], sizeof(VoltageValuestr[1]), "%.2lf", moduleStatus[connId].DCMeterValues.voltage);
            snprintf(CurrentValuestr[1], sizeof(CurrentValuestr[1]), "%.2lf", moduleStatus[connId].DCMeterValues.current);
            snprintf(TempValuestr, sizeof(TempValuestr), "%.2lf", moduleStatus[connId].DCMeterValues.temperature);
        }

        snprintf(EnergyValuestr, sizeof(EnergyValuestr), "%.2lf", moduleStatus[connId].meterStop);
        snprintf(SocValuestr, sizeof(SocValuestr), "%u", plc->Get_EVSoC(connId));
        ocpp->CPMeterValuesRequest[connId].connectorId = connId;
        ocpp->CPMeterValuesRequest[connId].transactionId = moduleStatus[connId].transactionId;

        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.timestamp);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[0].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[1].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[2].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[3].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[4].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[5].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[6].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[7].value);
        setNULL(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[8].value);

        SystemTime->getTimeInOcppFormat(ocpp->CPMeterValuesRequest[connId].meterValue.timestamp);
        memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[0].value, EnergyValuestr, strlen(EnergyValuestr));
        memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[1].value, PowerValuestr, strlen(PowerValuestr));
        memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[2].value, VoltageValuestr[1], strlen(VoltageValuestr[1]));
        memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[3].value, CurrentValuestr[1], strlen(CurrentValuestr[1]));
        memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[4].value, TempValuestr, strlen(TempValuestr));
        memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[5].value, SocValuestr, strlen(SocValuestr));
        if (AC)
        {
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[6].value, VoltageValuestr[1], strlen(VoltageValuestr[1]));
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[7].value, VoltageValuestr[2], strlen(VoltageValuestr[2]));
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[8].value, VoltageValuestr[3], strlen(VoltageValuestr[3]));
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[9].value, CurrentValuestr[1], strlen(CurrentValuestr[1]));
            memcpy(ocpp->CPMeterValuesRequest[connId].meterValue.sampledValue[10].value, PowerFactorValuestr, strlen(PowerFactorValuestr));
        }
    }

    // Constructor
    void CCSConnectorController::OcppTask(void *pvParameters)
    {
        CCSConnectorController *connector = static_cast<CCSConnectorController *>(pvParameters);

        uint8_t ConnID = 0;
        uint32_t loopCount = 0;
        uint32_t timeout = 0;
        uint32_t offlineTimeout = 0;
        bool isWebsocketStateChanged = false;
        bool resetOldVariables[NUM_OF_CONNECTORS];
        bool isFaultRecovered[NUM_OF_CONNECTORS];
        bool isReserved_old[NUM_OF_CONNECTORS];
        bool finishingStatusSent[NUM_OF_CONNECTORS];
        bool isControlPilotStateChanged[NUM_OF_CONNECTORS];
        bool finishingStatusPending[NUM_OF_CONNECTORS];
        bool connectorChargingStatusSent[NUM_OF_CONNECTORS];
        TickType_t xLastWakeTime = xTaskGetTickCount();
        for (uint8_t i = 0; i < NUM_OF_CONNECTORS; i++)
            connector->ledState[i] = OCPPModule::LED_STATE::OFFLINE_UNAVAILABLE_LED_STATE;

        ESP_LOGI(TAG, "OCPP Task started");
        while (true)
        {
            if (network)
            {
                connector->isWebsocketConnected = network->isWebsocketConnected;
                connector->isWifiConnected = network->isWifiConnected;
            }
            switch (connector->moduleStatus[ConnID].stateMachineState)
            {
            case PLCModule::StateMachineState::SNA:
            {
                if (connector->isWebsocketConnected)
                {
                    if ((ocpp->CPBootNotificationRequest.Sent == false) && (connector->isBootRejected == false))
                    {
                        connector->isChargerBooted = false;
                        ocpp->sendBootNotificationRequest();
                    }
                    if (ocpp->CMSBootNotificationResponse.Received == false)
                    {
                        timeout++;
                    }
                    else if (ocpp->CMSBootNotificationResponse.Received &
                             (memcmp(ocpp->CMSBootNotificationResponse.status, Accepted, strlen(Accepted)) == 0))
                    {
                        offlineTimeout = 0;
                        timeout = 0;
                        connector->isBootRejected = false;
                        connector->moduleStatus[ConnID].stateMachineState = PLCModule::StateMachineState::Init;
                        SystemTime->setTimeFromOcppFormat(ocpp->CMSBootNotificationResponse.currentTime);
                        SystemTime->writeCurrentTimeToFlash();
                        connector->isChargerBooted = true;
                        connector->isWebsocketConnected_old = false;
                        connector->bootedNow = true;
                        connector->isPoweredOn = false;
                    }
                    else if (ocpp->CMSBootNotificationResponse.Received &
                             (memcmp(ocpp->CMSBootNotificationResponse.status, Accepted, strlen(Accepted)) != 0))
                    {
                        timeout = 0;
                        connector->isBootRejected = true;
                        ocpp->CMSBootNotificationResponse.Received = false;
                    }

                    if (connector->isBootRejected)
                    {
                        if (timeout > OCPPModule::Constants::BOOT_RETRY_TIME)
                        {
                            connector->isBootRejected = false;
                            timeout = 0;
                            ocpp->sendBootNotificationRequest();
                        }
                    }
                    else if (timeout > OCPPModule::Constants::BOOT_RETRY_TIME)
                    {
                        timeout = 0;
                        ocpp->sendBootNotificationRequest();
                    }
                }
            }
            break;
            case PLCModule::StateMachineState::Init:
                connector->isChargerBooted = true;

                break;
            default:
                break;
            }
            isWebsocketStateChanged = (connector->isWebsocketConnected != connector->isWebsocketConnected_old) ? true : false;
            if (connector->isChargerBooted && connector->isWebsocketConnected)
            {
                if (ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval != 0)
                {
                    time_t time;
                    SystemTime->getTime(&time);
                    if (((static_cast<uint64_t>(time)) % 86400) % ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval == 0)
                    {
                        for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                            connector->ClockAlignedDataTime[i] = true;
                    }
                    else
                    {
                        for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                            connector->ClockAlignedDataTime[i] = false;
                    }
                }
                if (loopCount >= ocpp->CPGetConfigurationResponse.HeartbeatInterval)
                {
                    loopCount = 0;
                    ocpp->sendHeartbeatRequest();
                }

                for (uint8_t i = 0; i <= connector->NumberOfConnectors; i++)
                {
                    bool SendAuthorizeRequest = false;
                    if (ocpp->CMSAuthorizeResponse[i].Received)
                    {
                        // processRfidTappedState();
                    }
                    if (ocpp->CMSRemoteStartTransactionRequest[i].Received)
                    {
                        ocpp->CMSRemoteStartTransactionRequest[i].Received = false;
                        setNULL(ocpp->CPRemoteStartTransactionResponse[i].status);
                        if ((connector->connectorFault[i]) ||
                            (connector->moduleStatus[i].controlPilotState != PLCModule::ControlPilotState::B))
                        {
                            setNULL(ocpp->CPRemoteStartTransactionResponse[i].status);
                            std::memcpy(ocpp->CPRemoteStartTransactionResponse[i].status, Rejected, strlen(Rejected));
                        }
                        else if (ocpp->CPGetConfigurationResponse.AuthorizeRemoteTxRequests)
                        {
                            SendAuthorizeRequest = true;
                            setNULL(connector->Remote_tag);
                            setNULL(ocpp->CPRemoteStartTransactionResponse[i].status);
                            memcpy(connector->Remote_tag, ocpp->CMSRemoteStartTransactionRequest[i].idTag, sizeof(ocpp->CMSRemoteStartTransactionRequest[i].idTag));
                            memcpy(ocpp->CPRemoteStartTransactionResponse[i].status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            connector->RemoteAuthorized[i] = true;
                            setNULL(connector->moduleStatus[i].idTag);
                            setNULL(ocpp->CPRemoteStartTransactionResponse[i].status);
                            memcpy(connector->moduleStatus[i].idTag, ocpp->CMSRemoteStartTransactionRequest[i].idTag, sizeof(ocpp->CMSRemoteStartTransactionRequest[i].idTag));
                            memcpy(ocpp->CPRemoteStartTransactionResponse[i].status, Accepted, strlen(Accepted));
                        }
                        ocpp->sendRemoteStartTransactionResponse(i);

                        if (SendAuthorizeRequest)
                        {
                            setNULL(ocpp->CPAuthorizeRequest[i].idTag);
                            memcpy(ocpp->CPAuthorizeRequest[i].idTag, connector->Remote_tag, strlen(connector->Remote_tag));
                            ocpp->sendAuthorizationRequest(i);
                        }
                    }
                    if (ocpp->CMSChangeAvailabilityRequest[i].Received)
                    {
                        ocpp->CMSChangeAvailabilityRequest[i].Received = false;
                        connector->moduleStatus[i].CmsAvailableChanged = false;
                        if ((connector->moduleStatus[i].stateMachineState > PLCModule::StateMachineState::Default) &&
                            (connector->moduleStatus[i].stateMachineState < PLCModule::StateMachineState::SNA))
                        {
                            connector->moduleStatus[i].CmsAvailableScheduled = (memcmp(ocpp->CMSChangeAvailabilityRequest[i].type, Inoperative, strlen(Inoperative)) == 0) ? true : false;
                            setNULL(ocpp->CPChangeAvailabilityResponse.status);
                            memcpy(ocpp->CPChangeAvailabilityResponse.status, Scheduled, strlen(Scheduled));
                        }
                        else
                        {
                            connector->moduleStatus[i].CmsAvailable = (memcmp(ocpp->CMSChangeAvailabilityRequest[i].type, Inoperative, strlen(Inoperative)) == 0) ? false : true;
                            connector->moduleStatus[i].CmsAvailableChanged = true;
                            setNULL(ocpp->CPChangeAvailabilityResponse.status);
                            memcpy(ocpp->CPChangeAvailabilityResponse.status, Accepted, strlen(Accepted));
                        }
                        connector->writeConnectorModuleStatus(i);
                        ocpp->sendChangeAvailabilityResponse(i);
                    }
                    if ((connector->moduleStatus[i].stateMachineState == PLCModule::StateMachineState::Default) && (connector->moduleStatus[i].CmsAvailableScheduled == true))
                    {
                        connector->moduleStatus[i].CmsAvailableScheduled = false;
                        connector->moduleStatus[i].CmsAvailable = false;
                        connector->moduleStatus[i].CmsAvailableChanged = true;
                        setNULL(ocpp->CPChangeAvailabilityResponse.status);
                        memcpy(ocpp->CPChangeAvailabilityResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendChangeAvailabilityResponse(i);
                        connector->writeConnectorModuleStatus(i);
                    }
                    if (ocpp->CMSReserveNowRequest[i].Received)
                    {
                        ocpp->CMSReserveNowRequest[i].Received = false;
                        if (connector->connectorFault[i])
                        {
                            setNULL(ocpp->CPReserveNowResponse.status);
                            memcpy(ocpp->CPReserveNowResponse.status, Faulted, strlen(Faulted));
                        }
                        else if (connector->moduleStatus[i].CmsAvailable == false)
                        {
                            setNULL(ocpp->CPReserveNowResponse.status);
                            memcpy(ocpp->CPReserveNowResponse.status, Unavailable, strlen(Unavailable));
                        }
                        else if (connector->moduleStatus[i].stateMachineState > PLCModule::StateMachineState::Default)
                        {
                            setNULL(ocpp->CPReserveNowResponse.status);
                            memcpy(ocpp->CPReserveNowResponse.status, Occupied, strlen(Occupied));
                        }
                        else
                        {
                            connector->moduleStatus[i].isReserved = true;
                            connector->moduleStatus[i].ReservedData.reservationId = ocpp->CMSReserveNowRequest[i].reservationId;
                            setNULL(connector->moduleStatus[i].ReservedData.idTag);
                            setNULL(connector->moduleStatus[i].ReservedData.expiryDate);
                            setNULL(connector->moduleStatus[i].ReservedData.parentidTag);
                            memcpy(connector->moduleStatus[i].ReservedData.idTag, ocpp->CMSReserveNowRequest[i].idTag, strlen(ocpp->CMSReserveNowRequest[i].idTag));
                            memcpy(connector->moduleStatus[i].ReservedData.expiryDate, ocpp->CMSReserveNowRequest[i].expiryDate, strlen(ocpp->CMSReserveNowRequest[i].expiryDate));
                            if (ocpp->CMSReserveNowRequest[i].isparentidTagReceived)
                            {
                                memcpy(connector->moduleStatus[i].ReservedData.parentidTag, ocpp->CMSReserveNowRequest[i].parentidTag, strlen(ocpp->CMSReserveNowRequest[i].parentidTag));
                            }
                            connector->writeConnectorModuleStatus(i);
                            setNULL(ocpp->CPReserveNowResponse.status);
                            memcpy(ocpp->CPReserveNowResponse.status, Accepted, strlen(Accepted));
                        }
                        ocpp->sendReserveNowResponse(i);
                    }
                }
                if (ocpp->CMSHeartbeatResponse.Received)
                {
                    ocpp->CMSHeartbeatResponse.Received = false;
                    SystemTime->setTimeFromOcppFormat(ocpp->CMSHeartbeatResponse.currentTime);
                    SystemTime->writeCurrentTimeToFlash();
                }
                if (ocpp->CMSRemoteStopTransactionRequest.Received)
                {
                    for (uint8_t i = 0; i <= NUM_OF_CONNECTORS; i++)
                    {
                        if (ocpp->CMSRemoteStopTransactionRequest.transactionId == connector->moduleStatus[i].transactionId)
                        {
                            connector->moduleStatus[i].stopReason = OCPPModule::StopReasons::Stop_Remote;
                            plc->Set_EVSEStatusCode(i, PLCModule::EVSEStatusCode::EVSE_Shutdown);
                            ocpp->sendRemoteStopTransactionResponse();
                            ocpp->CMSRemoteStopTransactionRequest.Received = false;
                        }
                    }
                }
                if (ocpp->CMSGetConfigurationRequest.Received)
                {
                    ocpp->CMSGetConfigurationRequest.Received = false;
                    ocpp->sendGetConfigurationResponse();
                }
                if (ocpp->CMSSendLocalListRequest.Received)
                {
                    ocpp->sendLocalListResponse();
                    ocpp->write_localist();
                    ocpp->CMSSendLocalListRequest.Received = false;
                    for (uint32_t i = 0; i < LOCAL_LIST_COUNT; i++)
                    {
                        if (ocpp->CMSSendLocalListRequest.localAuthorizationList[i].idTagPresent)
                        {
                            ESP_LOGI(TAG, "ocpp->CMS Local List Tag%ld : %s", i, ocpp->CMSSendLocalListRequest.localAuthorizationList[i].idTag);
                        }
                    }
                }
                if (ocpp->CMSTriggerMessageRequest.Received)
                {
                    ocpp->CMSTriggerMessageRequest.Received = false;
                    setNULL(ocpp->CPTriggerMessageResponse.status);
                    if (memcmp(ocpp->CMSTriggerMessageRequest.requestedMessage, "StatusNotification", strlen("StatusNotification")) == 0)
                    {
                        memcpy(ocpp->CPTriggerMessageResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendTriggerMessageResponse();
                        setNULL(ocpp->CPStatusNotificationRequest[ocpp->CMSTriggerMessageRequest.connectorId].timestamp);
                        SystemTime->getTimeInOcppFormat(ocpp->CPStatusNotificationRequest[ocpp->CMSTriggerMessageRequest.connectorId].timestamp);
                        ocpp->sendStatusNotificationRequest(ocpp->CMSTriggerMessageRequest.connectorId);
                    }
                    else if (memcmp(ocpp->CMSTriggerMessageRequest.requestedMessage, "MeterValues", strlen("MeterValues")) == 0)
                    {
                        memcpy(ocpp->CPTriggerMessageResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendTriggerMessageResponse();
                        uint8_t ConnID = ocpp->CMSTriggerMessageRequest.connectorId;
                        if (connector->moduleStatus[ConnID].mode == ConnectorMode::AC)
                        {
                            connector->moduleStatus[ConnID].ACMeterValues = energy->GetACEnergyMeterValue(ConnID);
                            connector->moduleStatus[ConnID].meterStop = connector->moduleStatus[ConnID].meterStop + ((connector->moduleStatus[ConnID].ACMeterValues.power[ConnID] * MeterValueSampleTime) / 3600.0);
                            connector->moduleStatus[ConnID].Energy = connector->moduleStatus[ConnID].meterStop - connector->moduleStatus[ConnID].meterStart;
                        }
                        if (connector->moduleStatus[ConnID].mode == ConnectorMode::DC)
                        {
                            connector->moduleStatus[ConnID].DCMeterValues = energy->GetDCEnergyMeterValue(ConnID);
                            connector->moduleStatus[ConnID].meterStop = connector->moduleStatus[ConnID].meterStop + ((connector->moduleStatus[ConnID].DCMeterValues.power * MeterValueSampleTime) / 3600.0);
                            connector->moduleStatus[ConnID].Energy = connector->moduleStatus[ConnID].meterStop - connector->moduleStatus[ConnID].meterStart;
                        }

                        connector->updateMeterValues(ConnID, SAMPLED_DATA, false);
                        ocpp->sendMeterValuesRequest(ConnID, OCPPModule::TransactionContext::Trigger, false, false);
                    }
                    else if (memcmp(ocpp->CMSTriggerMessageRequest.requestedMessage, "BootNotification", strlen("BootNotification")) == 0)
                    {
                        memcpy(ocpp->CPTriggerMessageResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendTriggerMessageResponse();
                        ocpp->sendBootNotificationRequest();
                    }
                    else if (memcmp(ocpp->CMSTriggerMessageRequest.requestedMessage, "DiagnosticsStatusNotification", strlen("DiagnosticsStatusNotification")) == 0)
                    {
                        memcpy(ocpp->CPTriggerMessageResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendTriggerMessageResponse();
                        ocpp->sendDiagnosticsStatusNotificationRequest();
                    }
                    else if (memcmp(ocpp->CMSTriggerMessageRequest.requestedMessage, "FirmwareStatusNotification", strlen("FirmwareStatusNotification")) == 0)
                    {
                        memcpy(ocpp->CPTriggerMessageResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendTriggerMessageResponse();
                        ocpp->sendFirmwareStatusNotificationRequest();
                    }
                    else if (memcmp(ocpp->CMSTriggerMessageRequest.requestedMessage, "Heartbeat", strlen("Heartbeat")) == 0)
                    {
                        memcpy(ocpp->CPTriggerMessageResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendTriggerMessageResponse();
                        ocpp->sendHeartbeatRequest();
                    }
                }
                if (ocpp->CMSGetLocalListVersionRequest.Received)
                {
                    ocpp->sendGetLocalListVersionResponse();
                    ocpp->CMSGetLocalListVersionRequest.Received = false;
                }
                if (ocpp->CMSClearCacheRequest.Received)
                {
                    memset(&ocpp->LocalAuthorizationList, 0, sizeof(OCPPModule::LocalAuthorizationList_t));
                    ocpp->write_LocalAuthorizationList();
                    setNULL(ocpp->CPClearCacheResponse.status);
                    memcpy(ocpp->CPClearCacheResponse.status, Accepted, strlen(Accepted));
                    ocpp->sendClearCacheResponse();
                    ocpp->CMSClearCacheRequest.Received = false;
                }
                if (ocpp->CMSCancelReservationRequest.Received)
                {
                    ocpp->CMSCancelReservationRequest.Received = false;
                    ocpp->CPCancelReservationResponse.Sent = false;
                    for (uint8_t i = 0; i <= NUM_OF_CONNECTORS; i++)
                    {
                        if ((ocpp->CMSCancelReservationRequest.reservationId == connector->moduleStatus[i].ReservedData.reservationId) && (connector->moduleStatus[i].isReserved))
                        {
                            connector->moduleStatus[i].isReserved = false;
                            setNULL(ocpp->CPCancelReservationResponse.status);
                            memcpy(ocpp->CPCancelReservationResponse.status, Accepted, strlen(Accepted));
                            ocpp->sendCancelReservationResponse();
                        }
                    }
                    if (ocpp->CPCancelReservationResponse.Sent == false)
                    {
                        setNULL(ocpp->CPCancelReservationResponse.status);
                        memcpy(ocpp->CPCancelReservationResponse.status, Rejected, strlen(Rejected));
                        ocpp->sendCancelReservationResponse();
                    }
                }
                if (ocpp->CMSChangeConfigurationRequest.Received)
                {
                    ocpp->CMSChangeConfigurationRequest.Received = false;
                    setNULL(ocpp->CPChangeConfigurationResponse.status);
                    if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "AuthorizeRemoteTxRequests", strlen("AuthorizeRemoteTxRequests")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.AuthorizeRemoteTxRequestsReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.AuthorizeRemoteTxRequestsValue);
                            memcpy(ocpp->CPGetConfigurationResponse.AuthorizeRemoteTxRequestsValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.AuthorizeRemoteTxRequests = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ClockAlignedDataInterval", strlen("ClockAlignedDataInterval")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ClockAlignedDataIntervalReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ClockAlignedDataIntervalValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ClockAlignedDataIntervalValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval > 0 && ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval < 30)
                            {
                                ocpp->CPGetConfigurationResponse.ClockAlignedDataInterval = 30;
                                setNULL(ocpp->CPGetConfigurationResponse.ClockAlignedDataIntervalValue);
                                memcpy(ocpp->CPGetConfigurationResponse.ClockAlignedDataIntervalValue, "30", strlen("30"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ConnectionTimeOut", strlen("ConnectionTimeOut")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ConnectionTimeOutReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ConnectionTimeOutValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ConnectionTimeOutValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ConnectionTimeOut = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.ConnectionTimeOut == 0)
                            {
                                ocpp->CPGetConfigurationResponse.ConnectionTimeOut = 120;
                                setNULL(ocpp->CPGetConfigurationResponse.ConnectionTimeOutValue);
                                memcpy(ocpp->CPGetConfigurationResponse.ConnectionTimeOutValue, "120", strlen("120"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "GetConfigurationMaxKeys", strlen("GetConfigurationMaxKeys")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeysReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeysValue);
                            memcpy(ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeysValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeys = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeys == 0)
                            {
                                ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeys = 60;
                                setNULL(ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeysValue);
                                memcpy(ocpp->CPGetConfigurationResponse.GetConfigurationMaxKeysValue, "60", strlen("60"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "HeartbeatInterval", strlen("HeartbeatInterval")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.HeartbeatIntervalReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.HeartbeatIntervalValue);
                            memcpy(ocpp->CPGetConfigurationResponse.HeartbeatIntervalValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.HeartbeatInterval = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.HeartbeatInterval == 0)
                            {
                                ocpp->CPGetConfigurationResponse.HeartbeatInterval = 60;
                                setNULL(ocpp->CPGetConfigurationResponse.HeartbeatIntervalValue);
                                memcpy(ocpp->CPGetConfigurationResponse.HeartbeatIntervalValue, "60", strlen("60"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "LocalAuthorizeOffline", strlen("LocalAuthorizeOffline")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.LocalAuthorizeOfflineReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.LocalAuthorizeOfflineValue);
                            memcpy(ocpp->CPGetConfigurationResponse.LocalAuthorizeOfflineValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.LocalAuthorizeOffline = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "LocalPreAuthorize", strlen("LocalPreAuthorize")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.LocalPreAuthorizeReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.LocalPreAuthorizeValue);
                            memcpy(ocpp->CPGetConfigurationResponse.LocalPreAuthorizeValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.LocalPreAuthorize = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MeterValuesAlignedData", strlen("MeterValuesAlignedData")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyActiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyActiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyReactiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyReactiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyActiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyActiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyReactiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.EnergyReactiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.PowerActiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.PowerActiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.PowerOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.PowerReactiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.PowerReactiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.PowerFactor = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Factor") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.CurrentExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.CurrentImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.CurrentOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.Voltage = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Voltage") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.Temperature = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Temperature") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.SoC = (strstr(ocpp->CMSChangeConfigurationRequest.value, "SoC") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedData.RPM = (strstr(ocpp->CMSChangeConfigurationRequest.value, "RPM") != NULL) ? true : false;

                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MeterValuesSampledData", strlen("MeterValuesSampledData")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MeterValuesSampledDataReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MeterValuesSampledDataValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MeterValuesSampledDataValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyActiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyActiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyReactiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyReactiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyActiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyActiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyReactiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.EnergyReactiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.PowerActiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.PowerActiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.PowerOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.PowerReactiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.PowerReactiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.PowerFactor = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Factor") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.CurrentExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.CurrentImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.CurrentOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.Voltage = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Voltage") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.Temperature = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Temperature") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.SoC = (strstr(ocpp->CMSChangeConfigurationRequest.value, "SoC") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledData.RPM = (strstr(ocpp->CMSChangeConfigurationRequest.value, "RPM") != NULL) ? true : false;

                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MeterValueSampleInterval", strlen("MeterValueSampleInterval")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MeterValueSampleIntervalReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MeterValueSampleIntervalValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MeterValueSampleIntervalValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MeterValueSampleInterval = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.MeterValueSampleInterval == 0)
                            {
                                ocpp->CPGetConfigurationResponse.MeterValueSampleInterval = 30;
                                setNULL(ocpp->CPGetConfigurationResponse.MeterValueSampleIntervalValue);
                                memcpy(ocpp->CPGetConfigurationResponse.MeterValueSampleIntervalValue, "30", strlen("30"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "NumberOfConnectors", strlen("NumberOfConnectors")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.NumberOfConnectorsReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.NumberOfConnectorsValue);
                            memcpy(ocpp->CPGetConfigurationResponse.NumberOfConnectorsValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.NumberOfConnectors = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.NumberOfConnectors == 0)
                            {
                                ocpp->CPGetConfigurationResponse.NumberOfConnectors = 1;
                                setNULL(ocpp->CPGetConfigurationResponse.NumberOfConnectorsValue);
                                memcpy(ocpp->CPGetConfigurationResponse.NumberOfConnectorsValue, "1", strlen("1"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ResetRetries", strlen("ResetRetries")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ResetRetriesReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ResetRetriesValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ResetRetriesValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ResetRetries = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.ResetRetries == 0)
                            {
                                ocpp->CPGetConfigurationResponse.ResetRetries = 3;
                                setNULL(ocpp->CPGetConfigurationResponse.ResetRetriesValue);
                                memcpy(ocpp->CPGetConfigurationResponse.ResetRetriesValue, "3", strlen("3"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ConnectorPhaseRotation", strlen("ConnectorPhaseRotation")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "StopTransactionOnEVSideDisconnect", strlen("StopTransactionOnEVSideDisconnect")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectValue);
                            memcpy(ocpp->CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.StopTransactionOnEVSideDisconnect = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "StopTransactionOnInvalidId", strlen("StopTransactionOnInvalidId")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.StopTransactionOnInvalidIdReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.StopTransactionOnInvalidIdValue);
                            memcpy(ocpp->CPGetConfigurationResponse.StopTransactionOnInvalidIdValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.StopTransactionOnInvalidId = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "StopTxnAlignedData", strlen("StopTxnAlignedData")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.StopTxnAlignedDataReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.StopTxnAlignedDataValue);
                            memcpy(ocpp->CPGetConfigurationResponse.StopTxnAlignedDataValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyActiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyActiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyReactiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyReactiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyActiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyActiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyReactiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.EnergyReactiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.PowerActiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.PowerActiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.PowerOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.PowerReactiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.PowerReactiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.PowerFactor = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Factor") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.CurrentExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.CurrentImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.CurrentOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.Voltage = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Voltage") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.Temperature = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Temperature") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.SoC = (strstr(ocpp->CMSChangeConfigurationRequest.value, "SoC") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedData.RPM = (strstr(ocpp->CMSChangeConfigurationRequest.value, "RPM") != NULL) ? true : false;

                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "StopTxnSampledData", strlen("StopTxnSampledData")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.StopTxnSampledDataReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.StopTxnSampledDataValue);
                            memcpy(ocpp->CPGetConfigurationResponse.StopTxnSampledDataValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyActiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyActiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyReactiveExportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyReactiveImportRegister = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Register") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyActiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyActiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Active.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyReactiveExportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Export.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.EnergyReactiveImportInterval = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Energy.Reactive.Import.Interval") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.PowerActiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.PowerActiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Active.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.PowerOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.PowerReactiveExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.PowerReactiveImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Reactive.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.PowerFactor = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Power.Factor") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.CurrentExport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Export") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.CurrentImport = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Import") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.CurrentOffered = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Current.Offered") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.Voltage = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Voltage") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.Temperature = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Temperature") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.SoC = (strstr(ocpp->CMSChangeConfigurationRequest.value, "SoC") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.StopTxnSampledData.RPM = (strstr(ocpp->CMSChangeConfigurationRequest.value, "RPM") != NULL) ? true : false;

                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "SupportedFeatureProfiles", strlen("SupportedFeatureProfiles")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesValue);
                            memcpy(ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfiles.Core = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Core") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfiles.LocalAuthListManagement = (strstr(ocpp->CMSChangeConfigurationRequest.value, "LocalAuthListManagement") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfiles.Reservation = (strstr(ocpp->CMSChangeConfigurationRequest.value, "Reservation") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfiles.RemoteTrigger = (strstr(ocpp->CMSChangeConfigurationRequest.value, "RemoteTrigger") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfiles.FirmwareManagement = (strstr(ocpp->CMSChangeConfigurationRequest.value, "FirmwareManagement") != NULL) ? true : false;
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfiles.SmartCharging = (strstr(ocpp->CMSChangeConfigurationRequest.value, "SmartCharging") != NULL) ? true : false;

                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "TransactionMessageAttempts", strlen("TransactionMessageAttempts")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.TransactionMessageAttemptsReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.TransactionMessageAttemptsValue);
                            memcpy(ocpp->CPGetConfigurationResponse.TransactionMessageAttemptsValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.TransactionMessageAttempts = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.TransactionMessageAttempts == 0)
                            {
                                ocpp->CPGetConfigurationResponse.TransactionMessageAttempts = 1;
                                setNULL(ocpp->CPGetConfigurationResponse.TransactionMessageAttemptsValue);
                                memcpy(ocpp->CPGetConfigurationResponse.TransactionMessageAttemptsValue, "1", strlen("1"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "TransactionMessageRetryInterval", strlen("TransactionMessageRetryInterval")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.TransactionMessageRetryIntervalReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.TransactionMessageRetryIntervalValue);
                            memcpy(ocpp->CPGetConfigurationResponse.TransactionMessageRetryIntervalValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.TransactionMessageRetryInterval = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.TransactionMessageRetryInterval == 0)
                            {
                                ocpp->CPGetConfigurationResponse.TransactionMessageRetryInterval = 60;
                                setNULL(ocpp->CPGetConfigurationResponse.TransactionMessageRetryIntervalValue);
                                memcpy(ocpp->CPGetConfigurationResponse.TransactionMessageRetryIntervalValue, "60", strlen("60"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "UnlockConnectorOnEVSideDisconnect", strlen("UnlockConnectorOnEVSideDisconnect")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectValue);
                            memcpy(ocpp->CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnect = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "AllowOfflineTxForUnknownId", strlen("AllowOfflineTxForUnknownId")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.AllowOfflineTxForUnknownIdReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.AllowOfflineTxForUnknownIdValue);
                            memcpy(ocpp->CPGetConfigurationResponse.AllowOfflineTxForUnknownIdValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.AllowOfflineTxForUnknownId = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "AuthorizationCacheEnabled", strlen("AuthorizationCacheEnabled")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.AuthorizationCacheEnabledReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.AuthorizationCacheEnabledValue);
                            memcpy(ocpp->CPGetConfigurationResponse.AuthorizationCacheEnabledValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.AuthorizationCacheEnabled = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "BlinkRepeat", strlen("BlinkRepeat")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.BlinkRepeatReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.BlinkRepeatValue);
                            memcpy(ocpp->CPGetConfigurationResponse.BlinkRepeatValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.BlinkRepeat = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.BlinkRepeat == 0)
                            {
                                ocpp->CPGetConfigurationResponse.BlinkRepeat = 500;
                                setNULL(ocpp->CPGetConfigurationResponse.BlinkRepeatValue);
                                memcpy(ocpp->CPGetConfigurationResponse.BlinkRepeatValue, "500", strlen("500"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "LightIntensity", strlen("LightIntensity")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.LightIntensityReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.LightIntensityValue);
                            memcpy(ocpp->CPGetConfigurationResponse.LightIntensityValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.LightIntensity = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.LightIntensity == 0)
                            {
                                ocpp->CPGetConfigurationResponse.LightIntensity = 100;
                                setNULL(ocpp->CPGetConfigurationResponse.LightIntensityValue);
                                memcpy(ocpp->CPGetConfigurationResponse.LightIntensityValue, "100", strlen("100"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MaxEnergyOnInvalidId", strlen("MaxEnergyOnInvalidId")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MaxEnergyOnInvalidIdReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MaxEnergyOnInvalidIdValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MaxEnergyOnInvalidIdValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MaxEnergyOnInvalidId = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MeterValuesAlignedDataMaxLength", strlen("MeterValuesAlignedDataMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLength > 5)
                            {
                                ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLength = 5;
                                setNULL(ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue, "5", strlen("5"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MeterValuesSampledDataMaxLength", strlen("MeterValuesSampledDataMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLength > 5)
                            {
                                ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLength = 5;
                                setNULL(ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue, "5", strlen("5"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MinimumStatusDuration", strlen("MinimumStatusDuration")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MinimumStatusDurationReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MinimumStatusDurationValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MinimumStatusDurationValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MinimumStatusDuration = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.MinimumStatusDuration == 0)
                            {
                                ocpp->CPGetConfigurationResponse.MinimumStatusDuration = 2;
                                setNULL(ocpp->CPGetConfigurationResponse.MinimumStatusDurationValue);
                                memcpy(ocpp->CPGetConfigurationResponse.MinimumStatusDurationValue, "2", strlen("2"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ConnectorPhaseRotationMaxLength", strlen("ConnectorPhaseRotationMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLength > 1)
                            {
                                ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLength = 1;
                                setNULL(ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue, "1", strlen("1"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "StopTxnAlignedDataMaxLength", strlen("StopTxnAlignedDataMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLength > 5)
                            {
                                ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLength = 5;
                                setNULL(ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue, "5", strlen("5"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "StopTxnSampledDataMaxLength", strlen("StopTxnSampledDataMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLength > 5)
                            {
                                ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLength = 5;
                                setNULL(ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue, "5", strlen("5"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "SupportedFeatureProfilesMaxLength", strlen("SupportedFeatureProfilesMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if (ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLength > 6)
                            {
                                ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLength = 6;
                                setNULL(ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue, "6", strlen("6"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "WebSocketPingInterval", strlen("WebSocketPingInterval")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.WebSocketPingIntervalReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.WebSocketPingIntervalValue);
                            memcpy(ocpp->CPGetConfigurationResponse.WebSocketPingIntervalValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.WebSocketPingInterval = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if ((ocpp->CPGetConfigurationResponse.WebSocketPingInterval < 10) || (ocpp->CPGetConfigurationResponse.WebSocketPingInterval > 180))
                            {
                                ocpp->CPGetConfigurationResponse.WebSocketPingInterval = 30;
                                setNULL(ocpp->CPGetConfigurationResponse.WebSocketPingIntervalValue);
                                memcpy(ocpp->CPGetConfigurationResponse.WebSocketPingIntervalValue, "30", strlen("30"));
                            }
                            // setWebsocketPingInterval();
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "LocalAuthListEnabled", strlen("LocalAuthListEnabled")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.LocalAuthListEnabledReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.LocalAuthListEnabledValue);
                            memcpy(ocpp->CPGetConfigurationResponse.LocalAuthListEnabledValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.LocalAuthListEnabled = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "LocalAuthListMaxLength", strlen("LocalAuthListMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.LocalAuthListMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.LocalAuthListMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.LocalAuthListMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.LocalAuthListMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if ((ocpp->CPGetConfigurationResponse.LocalAuthListMaxLength < 1) || (ocpp->CPGetConfigurationResponse.LocalAuthListMaxLength > 20))
                            {
                                ocpp->CPGetConfigurationResponse.LocalAuthListMaxLength = 10;
                                setNULL(ocpp->CPGetConfigurationResponse.LocalAuthListMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.LocalAuthListMaxLengthValue, "10", strlen("10"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "SendLocalListMaxLength", strlen("SendLocalListMaxLength")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.SendLocalListMaxLengthReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.SendLocalListMaxLengthValue);
                            memcpy(ocpp->CPGetConfigurationResponse.SendLocalListMaxLengthValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.SendLocalListMaxLength = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            if ((ocpp->CPGetConfigurationResponse.SendLocalListMaxLength < 1) || (ocpp->CPGetConfigurationResponse.SendLocalListMaxLength > 20))
                            {
                                ocpp->CPGetConfigurationResponse.SendLocalListMaxLength = 10;
                                setNULL(ocpp->CPGetConfigurationResponse.SendLocalListMaxLengthValue);
                                memcpy(ocpp->CPGetConfigurationResponse.SendLocalListMaxLengthValue, "10", strlen("10"));
                            }
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ReserveConnectorZeroSupported", strlen("ReserveConnectorZeroSupported")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ReserveConnectorZeroSupportedReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ReserveConnectorZeroSupportedValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ReserveConnectorZeroSupportedValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ReserveConnectorZeroSupported = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ChargeProfileMaxStackLevel", strlen("ChargeProfileMaxStackLevel")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ChargeProfileMaxStackLevelReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ChargeProfileMaxStackLevelValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ChargeProfileMaxStackLevelValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ChargeProfileMaxStackLevel = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ConnectorSwitch3to1PhaseSupported", strlen("ConnectorSwitch3to1PhaseSupported")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupported = (memcmp(ocpp->CMSChangeConfigurationRequest.value, "true", strlen("true")) == 0) ? true : false;
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "ChargingScheduleMaxPeriods", strlen("ChargingScheduleMaxPeriods")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.ChargingScheduleMaxPeriodsReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.ChargingScheduleMaxPeriodsValue);
                            memcpy(ocpp->CPGetConfigurationResponse.ChargingScheduleMaxPeriodsValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.ChargingScheduleMaxPeriods = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else if (memcmp(ocpp->CMSChangeConfigurationRequest.key, "MaxChargingProfilesInstalled", strlen("MaxChargingProfilesInstalled")) == 0)
                    {
                        if (ocpp->CPGetConfigurationResponse.MaxChargingProfilesInstalledReadOnly == false)
                        {
                            setNULL(ocpp->CPGetConfigurationResponse.MaxChargingProfilesInstalledValue);
                            memcpy(ocpp->CPGetConfigurationResponse.MaxChargingProfilesInstalledValue, ocpp->CMSChangeConfigurationRequest.value, strlen(ocpp->CMSChangeConfigurationRequest.value));
                            ocpp->CPGetConfigurationResponse.MaxChargingProfilesInstalled = atoi(ocpp->CMSChangeConfigurationRequest.value);
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Accepted, strlen(Accepted));
                        }
                        else
                        {
                            memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                        }
                    }
                    else
                    {
                        memcpy(ocpp->CPChangeConfigurationResponse.status, Rejected, strlen(Rejected));
                    }
                    ocpp->sendChangeConfigurationResponse();
                    ocpp->write_config_ocpp();
                }
                if (ocpp->CMSResetRequest.Received)
                {
                    if (memcmp(ocpp->CMSResetRequest.type, Soft, strlen(Soft)) == 0)
                    {
                        bool isAllConnectorsInDefaultState = true;
                        for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                        {
                            if ((connector->moduleStatus[i].stateMachineState != PLCModule::StateMachineState::Default) &&
                                isAllConnectorsInDefaultState)
                            {
                                isAllConnectorsInDefaultState = false;
                            }
                        }
                        if (isAllConnectorsInDefaultState)
                        {
                            setNULL(ocpp->CPResetResponse.status);
                            memcpy(ocpp->CPResetResponse.status, Accepted, strlen(Accepted));
                            ocpp->sendResetResponse();
                            ocpp->CMSResetRequest.Received = false;
                            vTaskDelay(2000 / portTICK_PERIOD_MS);
                            esp_restart();
                        }
                        else
                        {
                            for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                            {
                                if (connector->moduleStatus[i].stateMachineState != PLCModule::StateMachineState::Default)
                                {
                                    connector->moduleStatus[i].stopReason = OCPPModule::StopReasons::Stop_SoftReset;
                                    plc->Set_EVSEStatusCode(i, PLCModule::EVSEStatusCode::EVSE_Shutdown);
                                }
                            }
                        }
                    }
                    else if (memcmp(ocpp->CMSResetRequest.type, Hard, strlen(Hard)) == 0)
                    {
                        memcpy(ocpp->CPResetResponse.status, Accepted, strlen(Accepted));
                        ocpp->sendResetResponse();
                        for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                        {
                            if (connector->moduleStatus[i].stateMachineState == PLCModule::StateMachineState::Charge)
                            {
                                plc->Set_EVSEStatusCode(i, PLCModule::EVSEStatusCode::EVSE_EmergencyShutdown);
                                connector->moduleStatus[i].stopReason = OCPPModule::StopReasons::Stop_HardReset;
                                // check  contactor status and switch off power modules
                                connector->writeConnectorModuleStatus(i);
                            }
                        }
                        ocpp->CMSResetRequest.Received = false;
                        vTaskDelay(2000 / portTICK_PERIOD_MS);
                        esp_restart();
                    }
                }
                if (ocpp->CMSUpdateFirmwareRequest.Received)
                {
                    ocpp->sendUpdateFirmwareResponse();
                    ocpp->CMSUpdateFirmwareRequest.Received = false;
                    bool isAllConnectorsInDefaultState = true;
                    for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                    {
                        if ((connector->moduleStatus[i].stateMachineState != PLCModule::StateMachineState::Default) &&
                            isAllConnectorsInDefaultState)
                        {
                            isAllConnectorsInDefaultState = false;
                        }
                    }
                    if (config->OtaUrlFromCMSEnable)
                    {
                        if (isAllConnectorsInDefaultState)
                        {
                            // UpdateFirmware();
                        }
                        else
                            connector->updatingFirmwarePending = true;
                    }
                }
                if (ocpp->CMSClearChargingProfileRequest.Received)
                {
                    ocpp->sendClearChargingProfileResponse();
                    // custom_nvs_clear_chargingProfile(ocpp->CMSClearChargingProfileRequest.id);
                    ocpp->CMSClearChargingProfileRequest.Received = false;
                }
                if (ocpp->CMSGetCompositeScheduleRequest.Received)
                {
                    ocpp->sendGetCompositeScheduleResponse();
                    ocpp->CMSGetCompositeScheduleRequest.Received = false;
                }
                if (ocpp->CMSGetDiagnosticsRequest.Received)
                {
                    // updateDiagnosticsfileName();
                    ocpp->sendGetDiagnosticsResponse();
                    // sendLogsToCloud();
                    ocpp->CMSGetDiagnosticsRequest.Received = false;
                }
                if (ocpp->CMSSetChargingProfileRequest.Received)
                {
                    ocpp->sendSetChargingProfileResponse();
                    ocpp->CMSSetChargingProfileRequest.Received = false;
                }
                if (ocpp->CMSUnlockConnectorRequest.Received)
                {
                    ocpp->sendUnlockConnectorResponse();
                    ocpp->CMSUnlockConnectorRequest.Received = false;
                }
                if (connector->updatingFirmwarePending)
                {
                    bool isAllConnectorsInDefaultState = true;
                    for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                    {
                        if ((connector->moduleStatus[i].stateMachineState != PLCModule::StateMachineState::Default) &&
                            isAllConnectorsInDefaultState)
                        {
                            isAllConnectorsInDefaultState = false;
                        }
                    }
                    if (isAllConnectorsInDefaultState)
                    {
                        connector->updatingFirmwarePending = false;
                        // UpdateFirmware();
                    }
                }
            }
            loopCount++;

            if ((connector->isWebsocketConnected == false) &&
                (connector->isPoweredOn == false) &&
                (loopCount % 10 == 0))
            {
                SystemTime->writeCurrentTimeToFlash();
            }

            if (connector->FirmwareUpdateFailed && connector->FirmwareUpdateStarted)
            {
                ESP_LOGI(TAG, "Firmware Update Failed Reset");
                connector->FirmwareUpdateFailed = false;
                connector->FirmwareUpdateStarted = false;
                isWebsocketStateChanged = true;
            }
            for (uint8_t connId = 1; connId <= connector->NumberOfConnectors; connId++)
            {
                if ((connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::SNA) && (connector->FirmwareUpdateStarted == false))
                {
                    if (connector->moduleStatus[connId].isReserved & (loopCount % 10 == 0))
                    {
                        if (SystemTime->isTimeExpired(connector->moduleStatus[connId].ReservedData.expiryDate))
                        {
                            connector->moduleStatus[connId].isReserved = false;
                            connector->writeConnectorModuleStatus(connId);
                            resetOldVariables[connId] = true;
                        }
                    }
                    if (resetOldVariables[connId])
                    {
                        connector->emergency_old[connId] = false;
                        connector->earthDisconnect_old[connId] = false;
                        connector->gfci_old[connId] = false;
                        connector->relayWeld_old[connId] = false;
                        connector->overVoltage_old[connId] = false;
                        connector->underVoltage_old[connId] = false;
                        connector->overCurrent_old[connId] = false;
                        connector->overTemp_old[connId] = false;
                        connector->powerLoss_old[connId] = false;
                        connector->connectorFault_old[connId] = false;
                        resetOldVariables[connId] = false;
                    }

                    if (connector->bootedNow)
                    {
                        connector->emergency_old[connId] = false;
                        connector->earthDisconnect_old[connId] = false;
                        connector->gfci_old[connId] = false;
                        connector->relayWeld_old[connId] = false;
                        connector->overVoltage_old[connId] = false;
                        connector->underVoltage_old[connId] = false;
                        connector->overCurrent_old[connId] = false;
                        connector->overTemp_old[connId] = false;
                        connector->powerLoss_old[connId] = false;
                        connector->connectorFault_old[connId] = false;
                        connector->moduleStatus[connId].controlPilotState = PLCModule::ControlPilotState::SNA;
                    }

                    if (connector->emergency ||
                        connector->relayWeld[connId] ||
                        connector->earthDisconnect ||
                        connector->gfci[connId] ||
                        connector->powerLoss ||
                        connector->overTemp[connId] ||
                        connector->overVoltage[connId] ||
                        connector->underVoltage[connId] ||
                        connector->overCurrent[connId])
                    {
                        // connector->moduleStatus[connId].controlPilotState = FAULT;
                        connector->connectorFault[connId] = true;
                        if ((connector->powerLoss == false) && (connector->powerLoss != connector->powerLoss_old[connId]))
                        {
                            connector->powerLoss_old[connId] = 0;
                            connector->emergency_old[connId] = 0;
                            connector->gfci_old[connId] = 0;
                            connector->earthDisconnect_old[connId] = 0;
                            connector->overVoltage_old[connId] = 0;
                            connector->underVoltage_old[connId] = 0;
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->emergency == 0) && (connector->emergency != connector->emergency_old[connId]))
                        {
                            connector->emergency_old[connId] = 0;
                            connector->gfci_old[connId] = 0;
                            connector->earthDisconnect_old[connId] = 0;
                            connector->overVoltage_old[connId] = 0;
                            connector->underVoltage_old[connId] = 0;
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->gfci[connId] == 0) && (connector->gfci[connId] != connector->gfci_old[connId]))
                        {
                            connector->gfci_old[connId] = 0;
                            connector->earthDisconnect_old[connId] = 0;
                            connector->overVoltage_old[connId] = 0;
                            connector->underVoltage_old[connId] = 0;
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->earthDisconnect == 0) && (connector->earthDisconnect != connector->earthDisconnect_old[connId]))
                        {
                            connector->earthDisconnect_old[connId] = 0;
                            connector->overVoltage_old[connId] = 0;
                            connector->underVoltage_old[connId] = 0;
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->overVoltage[connId] == 0) && (connector->overVoltage[connId] != connector->overVoltage_old[connId]))
                        {
                            connector->overVoltage_old[connId] = 0;
                            connector->underVoltage_old[connId] = 0;
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->underVoltage[connId] == 0) && (connector->underVoltage[connId] != connector->underVoltage_old[connId]))
                        {
                            connector->underVoltage_old[connId] = 0;
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->overCurrent[connId] == 0) && (connector->overCurrent[connId] != connector->overCurrent_old[connId]))
                        {
                            connector->overCurrent_old[connId] = 0;
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->overTemp[connId] == 0) && (connector->overTemp[connId] != connector->overTemp_old[connId]))
                        {
                            connector->overTemp_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                        }
                        else if ((connector->relayWeld[connId] == 0) && (connector->relayWeld[connId] != connector->relayWeld_old[connId]))
                        {
                            connector->relayWeld_old[connId] = 0;
                        }

                        if (connector->powerLoss == 1)
                        {
                            connector->emergency_old[connId] = connector->emergency;
                            connector->gfci_old[connId] = connector->gfci[connId];
                            connector->earthDisconnect_old[connId] = connector->earthDisconnect;
                            connector->overVoltage_old[connId] = connector->overVoltage[connId];
                            connector->underVoltage_old[connId] = connector->underVoltage[connId];
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->emergency == 1)
                        {
                            connector->gfci_old[connId] = connector->gfci[connId];
                            connector->earthDisconnect_old[connId] = connector->earthDisconnect;
                            connector->overVoltage_old[connId] = connector->overVoltage[connId];
                            connector->underVoltage_old[connId] = connector->underVoltage[connId];
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->gfci[connId] == 1)
                        {
                            connector->earthDisconnect_old[connId] = connector->earthDisconnect;
                            connector->overVoltage_old[connId] = connector->overVoltage[connId];
                            connector->underVoltage_old[connId] = connector->underVoltage[connId];
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->earthDisconnect == 1)
                        {
                            connector->overVoltage_old[connId] = connector->overVoltage[connId];
                            connector->underVoltage_old[connId] = connector->underVoltage[connId];
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->overVoltage[connId] == 1)
                        {
                            connector->underVoltage_old[connId] = connector->underVoltage[connId];
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->underVoltage[connId] == 1)
                        {
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->overCurrent[connId] == 1)
                        {
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                        else if (connector->overTemp[connId] == 1)
                        {
                            connector->relayWeld_old[connId] = connector->relayWeld[connId];
                        }
                    }
                    else
                    {
                        connector->connectorFault[connId] = 0;
                    }

                    if ((connector->connectorFault[connId] != connector->connectorFault_old[connId]) && (connector->connectorFault[connId] == 0))
                    {
                        if (connector->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Charge)
                        {
                            isFaultRecovered[connId] = true;
                        }

                        connector->controlPilotState_old[connId] = PLCModule::ControlPilotState::SNA;
                        finishingStatusSent[connId] = false;
                        if (connector->moduleStatus[connId].CmsAvailable == false)
                        {
                            connector->moduleStatus[connId].CmsAvailableChanged = true;
                        }
                        else if (connector->moduleStatus[connId].isReserved)
                        {
                            isReserved_old[connId] = false;
                        }
                    }
                    else if ((connector->connectorFault[connId] != connector->connectorFault_old[connId]) && (connector->connectorFault[connId] == 1))
                    {

                        ESP_LOGE(TAG, "Connector %d FAULTED", connId);
                    }

                    isControlPilotStateChanged[connId] = (connector->moduleStatus[connId].stateMachineState != connector->stateMachineState_old[connId]) ? true : false;

                    if ((connector->powerLoss == 1) &&
                        ((connector->powerLoss != connector->powerLoss_old[connId]) ||
                         (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, PowerSwitchFailure, Faulted, "Input Power Failure", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d Power Loss Fault", connId);
                    }
                    else if ((connector->emergency == 1) && ((connector->emergency != connector->emergency_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, OtherError, Faulted, "EmergencyPressed", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::EMERGENCY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_EMERGENCY_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_EMERGENCY_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_EMERGENCY_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_EMERGENCY_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGW(TAG_STATUS, "Connector %d Emergency State", connId);
                    }
                    else if ((connector->gfci[connId] == 1) && ((connector->gfci[connId] != connector->gfci_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, GroundFailure, Faulted, GroundFailure, 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d GFCI Fault", connId);
                    }
                    else if ((connector->earthDisconnect == 1) && ((connector->earthDisconnect != connector->earthDisconnect_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, OtherError, Faulted, "EarthDisconnect", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d Earth Disconnect Fault", connId);
                    }
                    else if ((connector->overVoltage[connId] == 1) && ((connector->overVoltage[connId] != connector->overVoltage_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, OverVoltage, Faulted, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d Over Voltage Fault", connId);
                    }
                    else if ((connector->underVoltage[connId] == 1) && ((connector->underVoltage[connId] != connector->underVoltage_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, UnderVoltage, Faulted, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d Under Voltage Fault", connId);
                    }
                    else if ((connector->overCurrent[connId] == 1) && ((connector->overCurrent[connId] != connector->overCurrent_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, OverCurrentFailure, Faulted, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d Over Current Fault", connId);
                    }
                    else if ((connector->overTemp[connId] == 1) && ((connector->overTemp[connId] != connector->overTemp_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, HighTemperature, Faulted, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d High Temperature Fault", connId);
                    }
                    else if ((connector->relayWeld[connId] == 1) && ((connector->relayWeld[connId] != connector->relayWeld_old[connId]) || (isWebsocketStateChanged)))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, OtherError, Faulted, WeldDetect, 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FAULT_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FAULT_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FAULT_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FAULT_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);

                        // setChargerFaultBit(connId);
                        ESP_LOGE(TAG_STATUS, "Connector %d Relay Weld Detection Fault", connId);
                    }
                    else if ((connector->moduleStatus[connId].CmsAvailable == false) &&
                             ((isWebsocketStateChanged) ||
                              (connector->moduleStatus[connId].CmsAvailableChanged == true)))
                    {
                        connector->moduleStatus[connId].CmsAvailableChanged = false;
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, NoError, Unavailable, "None", 0);

                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::UNAVAILABLE_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_UNAVAILABLE_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE || config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_UNAVAILABLE_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_UNAVAILABLE_LED_STATE;
                        }
                        // SetChargerUnAvailableBit(connId);

                        // sendStatusNotificationToDS(connId);

                        ESP_LOGW(TAG_STATUS, "Connector %d Unavailable", connId);
                    }
                    else if (connector->moduleStatus[connId].isReserved &&
                             (isReserved_old[connId] == false) &&
                             (connector->moduleStatus[connId].CmsAvailable == true))
                    {
                        isReserved_old[connId] = true;
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, NoError, Reserved, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::RESERVATION_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_RESERVATION_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_RESERVATION_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_RESERVATION_LED_STATE;
                        }
                        // SetChargerReservedBit(connId);
                        ESP_LOGI(TAG_STATUS, "Connector %d Reserved", connId);

                        // sendStatusNotificationToDS(connId);
                    }
                    else if ((connector->moduleStatus[connId].controlPilotState == PLCModule::ControlPilotState::A) &&
                             (connector->moduleStatus[connId].isReserved == false) &&
                             (connector->moduleStatus[connId].CmsAvailable == true) &&
                             (connector->connectorFault[connId] == 0) &&
                             (connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::Charge) &&
                             (connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::StopCharge) &&
                             ((isControlPilotStateChanged[connId]) ||
                              ((finishingStatusSent[connId] == true) && (connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::StopCharge)) ||
                              (isWebsocketStateChanged) ||
                              (connector->moduleStatus[connId].CmsAvailableChanged == true)))
                    {
                        connectorChargingStatusSent[connId] = false;
                        finishingStatusSent[connId] = false;
                        finishingStatusPending[connId] = false;
                        connector->moduleStatus[connId].CmsAvailableChanged = false;
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, NoError, Available, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::AVAILABLE_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_AVAILABLE_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_AVAILABLE_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_AVAILABLE_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_AVAILABLE_LED_STATE;
                        }
                        if (((connector->isWebsocketConnected == false) || (connector->isChargerBooted == false)) && (config->networkMode == ConfigModule::NetworkMode::ONLINE))
                        {
                            connector->Set_UNAVAILABLE(connId);
                            ESP_LOGI(TAG_STATUS, "Connector %d Unavailable", connId);
                        }
                        else
                        {
                            connector->Set_AVAILABLE(connId);
                            ESP_LOGI(TAG_STATUS, "Connector %d Available", connId);
                        }

                        // sendStatusNotificationToDS(connId);
                    }
                    else if (((connector->moduleStatus[connId].controlPilotState == PLCModule::ControlPilotState::B)) &&
                             (connector->moduleStatus[connId].isReserved == false) &&
                             (connector->moduleStatus[connId].CmsAvailable == true) &&
                             (connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::Charge) &&
                             (connector->connectorFault[connId] == 0) &&
                             ((isControlPilotStateChanged[connId] && (connector->controlPilotState_old[connId] != PLCModule::ControlPilotState::E)) ||
                              (isWebsocketStateChanged) ||
                              (connector->moduleStatus[connId].CmsAvailableChanged == true)))
                    {
                        connectorChargingStatusSent[connId] = false;
                        connector->moduleStatus[connId].CmsAvailableChanged = false;
                        isReserved_old[connId] = false;
                        if ((finishingStatusPending[connId] == true) || (finishingStatusSent[connId] == true))
                        {
                            finishingStatusPending[connId] = false;
                            if (connector->isWebsocketConnected & connector->isChargerBooted)
                            {
                                connector->encodeAndSendStatusNotification(connId, NoError, Finishing, "None", 0);
                                if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                    connector->ledState[connId] = OCPPModule::LED_STATE::FINISHING_LED_STATE;
                                else
                                    connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_FINISHING_LED_STATE;
                            }
                            else
                            {
                                if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                    connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_FINISHING_LED_STATE;
                                else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                    connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_FINISHING_LED_STATE;
                                else
                                    connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_FINISHING_LED_STATE;
                            }
                            connector->Set_FINISHING(connId);
                            ESP_LOGI(TAG_STATUS, "Connector %d Finishing", connId);
                        }
                        else if (finishingStatusSent[connId] == false)
                        {
                            if (connector->isWebsocketConnected & connector->isChargerBooted)
                            {
                                connector->encodeAndSendStatusNotification(connId, NoError, Preparing, "None", 0);
                                if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                    connector->ledState[connId] = OCPPModule::LED_STATE::PREPARING_LED_STATE;
                                else
                                    connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_PREPARING_LED_STATE;
                            }
                            else
                            {
                                if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                    connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_PREPARING_LED_STATE;
                                else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                {
                                    connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_PREPARING_LED_STATE;
                                    if (config->networkMode == ConfigModule::NetworkMode::PLUGNPLAY)
                                    {
                                        ESP_LOGE(TAG, "Task switching to Plug & Play mode");
                                        connector->moduleStatus[connId].UnkownId = false;
                                        setNULL(connector->Rfid_tag);
                                        memcpy(connector->Rfid_tag, "PLUGNPLAY", strlen("PLUGNPLAY"));
                                        connector->writeConnectorModuleStatus(connId);
                                        connector->RfidAuthorized = true;
                                        ocpp->CPAuthorizeRequest[connId].Sent = false;
                                    }
                                }
                                else
                                    connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_PREPARING_LED_STATE;
                            }
                            if (connector->isRfidTappedFirst == false)
                            {
                                connector->Set_EV_PLUGGED_TAP_RFID(connId);
                            }
                            ESP_LOGI(TAG_STATUS, "Connector %d Preparing", connId);
                        }
                        // sendStatusNotificationToDS(connId);
                    }
                    else if (((connector->moduleStatus[connId].controlPilotState == PLCModule::ControlPilotState::C)) &&
                             ((isControlPilotStateChanged[connId] && (connector->controlPilotState_old[connId] != PLCModule::ControlPilotState::D)) ||
                              (isWebsocketStateChanged) ||
                              (connector->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Charge) ||
                              (isFaultRecovered[connId] == true)) &&
                             (connector->connectorFault[connId] == 0))
                    {
                        if (isFaultRecovered[connId] == true)
                        {
                            isFaultRecovered[connId] = false;
                        }
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, NoError, Charging, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::CHARGING_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_CHARGING_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_CHARGING_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_CHARGING_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_CHARGING_LED_STATE;
                        }

                        // sendStatusNotificationToDS(connId);
                        ESP_LOGI(TAG_STATUS, "Connector %d Charging", connId);
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        connectorChargingStatusSent[connId] = true;
                    }
                    else if (((connector->moduleStatus[connId].controlPilotState == PLCModule::ControlPilotState::D)) &&
                             ((isControlPilotStateChanged[connId] && (connector->controlPilotState_old[connId] != PLCModule::ControlPilotState::C)) ||
                              (isWebsocketStateChanged) ||
                              (connector->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Charge) ||
                              (isFaultRecovered[connId] == true)) &&
                             (connector->connectorFault[connId] == 0))
                    {
                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, NoError, Charging, "None", 0);
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::CHARGING_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_CHARGING_LED_STATE;
                        }
                        else
                        {
                            if (config->networkMode == ConfigModule::NetworkMode::ONLINE_OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_CHARGING_LED_STATE;
                            else if (config->networkMode == ConfigModule::NetworkMode::OFFLINE)
                                connector->ledState[connId] = OCPPModule::LED_STATE::OFFLINE_ONLY_CHARGING_LED_STATE;
                            else
                                connector->ledState[connId] = OCPPModule::LED_STATE::ONLINE_ONLY_OFFLINE_CHARGING_LED_STATE;
                        }
                        // sendStatusNotificationToDS(connId);
                        ESP_LOGI(TAG_STATUS, "Connector %d Charging", connId);
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        connectorChargingStatusSent[connId] = true;
                    }
                    else if (((connector->moduleStatus[connId].controlPilotState == PLCModule::ControlPilotState::E)) &&
                             (isControlPilotStateChanged[connId] ||
                              ((finishingStatusSent[connId] == true) && (connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::StopCharge)) ||
                              (isWebsocketStateChanged) ||
                              (connector->moduleStatus[connId].CmsAvailableChanged == true)) &&
                             (connector->moduleStatus[connId].isReserved == false) &&
                             (connector->moduleStatus[connId].CmsAvailable == true) &&
                             (connector->connectorFault[connId] == 0) &&
                             ((connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::Charge) && (connector->moduleStatus[connId].stateMachineState != PLCModule::StateMachineState::StopCharge)))
                    {
                        connector->moduleStatus[connId].CmsAvailableChanged = false;

                        if (connector->isWebsocketConnected & connector->isChargerBooted)
                        {
                            connector->encodeAndSendStatusNotification(connId, EVCommunicationError, Faulted, "None", 0);
                        }

                        // sendStatusNotificationToDS(connId);
                        ESP_LOGI(TAG_STATUS, "Connector %d CommunicationFailure", connId);
                    }
                    else if (((connector->moduleStatus[connId].controlPilotState == PLCModule::ControlPilotState::F)) && isControlPilotStateChanged[connId])
                    {
                        // sendStatusNotificationToDS(connId);
                    }
                    connector->gfci_old[connId] = connector->gfci[connId];
                    connector->earthDisconnect_old[connId] = connector->earthDisconnect;
                    connector->emergency_old[connId] = connector->emergency;
                    connector->relayWeld_old[connId] = connector->relayWeld[connId];
                    connector->moduleStatus[connId].controlPilotState = connector->controlPilotState_old[connId];

                    connector->connectorFault_old[connId] = connector->connectorFault[connId];
                    if (connector->isWebsocketConnected & connector->isChargerBooted)
                    {
                        if ((connector->connectorFault[connId] == 1) && (memcmp(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted)) != 0))
                        {
                            ESP_LOGE(TAG, "Connector %d Resetting fault old variables", connId);
                            connector->gfci_old[connId] = 0;
                            connector->earthDisconnect_old[connId] = 0;
                            connector->emergency_old[connId] = 0;
                            connector->relayWeld_old[connId] = 0;
                            connector->overVoltage_old[connId] = connector->overVoltage[connId];
                            connector->underVoltage_old[connId] = connector->underVoltage[connId];
                            connector->overCurrent_old[connId] = connector->overCurrent[connId];
                            connector->overTemp_old[connId] = connector->overTemp[connId];
                        }
                        else if ((connector->connectorFault[connId] == 0) && (memcmp(ocpp->CPStatusNotificationRequest[connId].status, Faulted, strlen(Faulted)) == 0))
                        {
                            connector->controlPilotState_old[connId] = PLCModule::ControlPilotState::SNA;
                        }
                    }
                }
            }
            connector->bootedNow = false;
            vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS));
        }
    }

}