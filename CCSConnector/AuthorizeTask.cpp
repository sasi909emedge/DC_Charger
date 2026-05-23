#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "CCSConnector.hpp"

namespace CCSConnector
{
    void CCSConnectorController::AuthorizeTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay
        CCSConnectorController *connector = static_cast<CCSConnectorController *>(pvParameters);
        uint32_t RfidSentTimeout = 0;
        uint32_t RfidAuthTimeout = 0;
        uint8_t ConnID = 0;
        while (true)
        {

            // first time rfid received tasks
            if ((connector->Rfid != connector->Rfid_old) && connector->Rfid)
            {
                connector->RfidTappedLed = true;
                connector->Set_RFID_TAPPED();

                // check if any connector charging with this idTag
                for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                {
                    if (memcmp(connector->Rfid_tag, connector->moduleStatus[i].idTag, sizeof(connector->Rfid_tag) == 0) &&
                        (connector->moduleStatus[i].stateMachineState == PLCModule::StateMachineState::Charge))
                    {
                        connector->moduleStatus[i].stopReason = OCPPModule::StopReasons::Stop_Local;
                        plc->Set_EVSEStatusCode(i, PLCModule::EVSEStatusCode::EVSE_Shutdown);
                        connector->Rfid = false;
                    }
                }
                if (connector->Rfid)
                {
                    connector->isRfidTappedFirst = true;
                    for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                    {
                        if ((connector->moduleStatus[i].controlPilotState == PLCModule::ControlPilotState::B) &&
                            (connector->isRfidTappedFirst))
                            connector->isRfidTappedFirst = false;
                    }
                }

                if (((ocpp->rfidMatchingLocalList(connector->Rfid_tag) && ocpp->CPGetConfigurationResponse.LocalAuthListEnabled) ||
                     (ocpp->rfidMatchingAuthorizationCache(connector->Rfid_tag) && ocpp->CPGetConfigurationResponse.AuthorizationCacheEnabled)) &&
                    ocpp->CPGetConfigurationResponse.LocalPreAuthorize)
                {
                    connector->Rfid = false;
                    connector->RfidAuthorized = true;

                    if (connector->isRfidTappedFirst)
                        connector->Set_AUTH_SUCCESS_PLUG_EV();
                    else
                        connector->Set_AUTH_SUCCESS();
                }
                else if (connector->isChargerBooted && connector->isWebsocketConnected)
                {
                    setNULL(ocpp->CPAuthorizeRequest[ConnID].idTag);
                    setNULL(connector->moduleStatus[ConnID].idTag);
                    memcpy(ocpp->CPAuthorizeRequest[ConnID].idTag, connector->Rfid_tag, strlen(connector->Rfid_tag));
                    memcpy(connector->moduleStatus[ConnID].idTag, connector->Rfid_tag, strlen(connector->Rfid_tag));
                    connector->writeConnectorModuleStatus(ConnID);
                    ocpp->sendAuthorizationRequest(ConnID);
                }
                else
                {
                    connector->Rfid = false;
                    connector->Set_AUTH_FAILED();
                }
            }

            if (connector->RfidAuthorized)
            {
                RfidAuthTimeout++;
                if (RfidAuthTimeout > 120)
                {
                    RfidAuthTimeout = 0;
                    connector->RfidAuthorized = false;
                }
            }
            else
            {
                RfidAuthTimeout = 0;
            }

            if (ocpp->CPAuthorizeRequest[0].Sent)
            {
                RfidSentTimeout++;
                if (RfidSentTimeout > 60)
                {
                    if (connector->isChargerBooted && connector->isWebsocketConnected)
                    {
                        ocpp->sendAuthorizationRequest(ConnID);
                        RfidSentTimeout = 0;
                    }
                }
            }
            else
            {
                RfidSentTimeout = 0;
            }

            for (uint8_t i = 0; i <= connector->NumberOfConnectors; i++)
            {
                if (ocpp->CMSAuthorizeResponse[ConnID].Received)
                {
                    ocpp->CMSAuthorizeResponse[ConnID].Received = false;
                    ocpp->CPAuthorizeRequest[ConnID].Sent = false;
                    if (memcmp(ocpp->CMSAuthorizeResponse[ConnID].idtaginfo.status, Accepted, strlen(Accepted)) == 0)
                    {
                        connector->moduleStatus[ConnID].UnkownId = false;
                        if (ocpp->CPGetConfigurationResponse.AuthorizationCacheEnabled &&
                            (memcmp(ocpp->CPAuthorizeRequest[ConnID].idTag, connector->Rfid_tag, strlen(connector->Rfid_tag)) == 0))
                            ocpp->saveTagToLocalAuthenticationList(connector->Rfid_tag);

                        if (memcmp(ocpp->CPAuthorizeRequest[ConnID].idTag, connector->Rfid_tag, strlen(connector->Rfid_tag)) == 0)
                            connector->RfidAuthorized = true;
                        else if (memcmp(ocpp->CPAuthorizeRequest[ConnID].idTag, connector->Alpr_tag, strlen(connector->Alpr_tag)) == 0)
                            connector->ALPRAuthorized[ConnID] = true;
                        else if (memcmp(ocpp->CPAuthorizeRequest[ConnID].idTag, connector->Remote_tag, strlen(connector->Remote_tag)) == 0)
                            connector->RemoteAuthorized[ConnID] = true;

                        if (connector->isRfidTappedFirst)
                            connector->Set_AUTH_SUCCESS_PLUG_EV();
                        else
                            connector->Set_AUTH_SUCCESS();
                    }
                }
            }

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

}