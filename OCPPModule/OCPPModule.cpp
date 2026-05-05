#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "OCPPModule.hpp"
#include "NetworkModule.hpp"
#include <cstring>
#include <cmath>

#define TAG "OCPP"

OCPPModule::OCPPController *ocpp;
namespace OCPPModule
{

    // Constructor
    OCPPController::OCPPController()
    {
        Initialize();
        read_localist();
        read_LocalAuthorizationList();
        SetDefaultOcppConfig();
    }

    // Destructor
    OCPPController::~OCPPController()
    {
        // Cleanup, if necessary
    }

    bool OCPPController::SendData(std::string jsonData)
    {
        if (network)
        {
            ESP_LOGI("WEBSOCKET", "Sent: %s", jsonData.c_str());
            return network->SendData(jsonData);
        }
        return false; // or handle error
    }

    bool OCPPController::ReceiveData(std::string jsonData)
    {
        ocpp_response(jsonData.c_str());
        return false; // or handle error
    }

    void OCPPController::Initialize()
    {

        ESP_LOGV(TAG, "Size of CPHeartbeatRequest is : %d ", sizeof(CPHeartbeatRequest));
        ESP_LOGV(TAG, "Size of CPBootNotificationRequest is : %d ", sizeof(CPBootNotificationRequest));
        ESP_LOGV(TAG, "Size of CPHeartbeatRequest is : %d ", sizeof(CPHeartbeatRequest));
        ESP_LOGV(TAG, "Size of CPStatusNotificationRequest is : %d ", sizeof(CPStatusNotificationRequest));
        ESP_LOGV(TAG, "Size of CPAuthorizeRequest is : %d ", sizeof(CPAuthorizeRequest));
        ESP_LOGV(TAG, "Size of CPStartTransactionRequest is : %d ", sizeof(CPStartTransactionRequest));
        ESP_LOGV(TAG, "Size of CPMeterValuesRequest is : %d ", sizeof(CPMeterValuesRequest));
        ESP_LOGV(TAG, "Size of CPStopTransactionRequest is : %d ", sizeof(CPStopTransactionRequest));
        ESP_LOGV(TAG, "Size of CPFirmwareStatusNotificationRequest is : %d ", sizeof(CPFirmwareStatusNotificationRequest));
        ESP_LOGV(TAG, "Size of CPDiagnosticsStatusNotificationRequest is : %d ", sizeof(CPDiagnosticsStatusNotificationRequest));
        ESP_LOGV(TAG, "Size of CMSRemoteStartTransactionRequest is : %d ", sizeof(CMSRemoteStartTransactionRequest));
        ESP_LOGV(TAG, "Size of CMSRemoteStopTransactionRequest is : %d ", sizeof(CMSRemoteStopTransactionRequest));
        ESP_LOGV(TAG, "Size of CMSCancelReservationRequest is : %d ", sizeof(CMSCancelReservationRequest));
        ESP_LOGV(TAG, "Size of CMSChangeAvailabilityRequest is : %d ", sizeof(CMSChangeAvailabilityRequest));
        ESP_LOGV(TAG, "Size of CMSChangeConfigurationRequest is : %d ", sizeof(CMSChangeConfigurationRequest));
        ESP_LOGV(TAG, "Size of CMSTriggerMessageRequest is : %d ", sizeof(CMSTriggerMessageRequest));
        ESP_LOGV(TAG, "Size of CMSReserveNowRequest is : %d ", sizeof(CMSReserveNowRequest));
        ESP_LOGV(TAG, "Size of CMSResetRequest is : %d ", sizeof(CMSResetRequest));
        ESP_LOGV(TAG, "Size of CMSUpdateFirmwareRequest is : %d ", sizeof(CMSUpdateFirmwareRequest));
        ESP_LOGV(TAG, "Size of CMSSendLocalListRequest is : %d ", sizeof(CMSSendLocalListRequest));
        ESP_LOGV(TAG, "Size of CMSClearCacheRequest is : %d ", sizeof(CMSClearCacheRequest));
        ESP_LOGV(TAG, "Size of CMSGetLocalListVersionRequest is : %d ", sizeof(CMSGetLocalListVersionRequest));
        ESP_LOGV(TAG, "Size of CMSGetConfigurationRequest is : %d ", sizeof(CMSGetConfigurationRequest));
        ESP_LOGV(TAG, "Size of CMSClearChargingProfileRequest is : %d ", sizeof(CMSClearChargingProfileRequest));
        ESP_LOGV(TAG, "Size of CMSGetCompositeScheduleRequest is : %d ", sizeof(CMSGetCompositeScheduleRequest));
        ESP_LOGV(TAG, "Size of CMSGetDiagnosticsRequest is : %d ", sizeof(CMSGetDiagnosticsRequest));
        ESP_LOGV(TAG, "Size of CMSSetChargingProfileRequest is : %d ", sizeof(CMSSetChargingProfileRequest));
        ESP_LOGV(TAG, "Size of CMSUnlockConnectorRequest is : %d ", sizeof(CMSUnlockConnectorRequest));
        ESP_LOGV(TAG, "Size of CPRemoteStartTransactionResponse is : %d ", sizeof(CPRemoteStartTransactionResponse));
        ESP_LOGV(TAG, "Size of CPRemoteStopTransactionResponse is : %d ", sizeof(CPRemoteStopTransactionResponse));
        ESP_LOGV(TAG, "Size of CPReserveNowResponse is : %d ", sizeof(CPReserveNowResponse));
        ESP_LOGV(TAG, "Size of CPResetResponse is : %d ", sizeof(CPResetResponse));
        ESP_LOGV(TAG, "Size of CPSendLocalListResponse is : %d ", sizeof(CPSendLocalListResponse));
        ESP_LOGV(TAG, "Size of CPTriggerMessageResponse is : %d ", sizeof(CPTriggerMessageResponse));
        ESP_LOGV(TAG, "Size of CPCancelReservationResponse is : %d ", sizeof(CPCancelReservationResponse));
        ESP_LOGV(TAG, "Size of CPChangeAvailabilityResponse is : %d ", sizeof(CPChangeAvailabilityResponse));
        ESP_LOGV(TAG, "Size of CPChangeConfigurationResponse is : %d ", sizeof(CPChangeConfigurationResponse));
        ESP_LOGV(TAG, "Size of CPClearCacheResponse is : %d ", sizeof(CPClearCacheResponse));
        ESP_LOGV(TAG, "Size of CPGetLocalListVersionResponse is : %d ", sizeof(CPGetLocalListVersionResponse));
        ESP_LOGV(TAG, "Size of CPUpdateFirmwareResponse is : %d ", sizeof(CPUpdateFirmwareResponse));
        ESP_LOGV(TAG, "Size of CPGetDiagnosticsResponse is : %d ", sizeof(CPGetDiagnosticsResponse));
        ESP_LOGV(TAG, "Size of CPGetConfigurationResponse is : %d ", sizeof(CPGetConfigurationResponse));
        ESP_LOGV(TAG, "Size of CMSFirmwareStatusNotificationResponse is : %d ", sizeof(CMSFirmwareStatusNotificationResponse));
        ESP_LOGV(TAG, "Size of CMSDiagnosticsStatusNotificationResponse is : %d ", sizeof(CMSDiagnosticsStatusNotificationResponse));
        ESP_LOGV(TAG, "Size of CMSHeartbeatResponse is : %d ", sizeof(CMSHeartbeatResponse));
        ESP_LOGV(TAG, "Size of CMSBootNotificationResponse is : %d ", sizeof(CMSBootNotificationResponse));
        ESP_LOGV(TAG, "Size of CMSStatusNotificationResponse is : %d ", sizeof(CMSStatusNotificationResponse));
        ESP_LOGV(TAG, "Size of CMSMeterValuesResponse is : %d ", sizeof(CMSMeterValuesResponse));
        ESP_LOGV(TAG, "Size of CMSAuthorizeResponse is : %d ", sizeof(CMSAuthorizeResponse));
        ESP_LOGV(TAG, "Size of CMSStartTransactionResponse is : %d ", sizeof(CMSStartTransactionResponse));
        ESP_LOGV(TAG, "Size of CMSStopTransactionResponse is : %d ", sizeof(CMSStopTransactionResponse));
        ESP_LOGV(TAG, "Size of LocalAuthorizationList is : %d ", sizeof(LocalAuthorizationList));

        if (CPGetConfigurationResponse.HeartbeatInterval < 10)
            CPGetConfigurationResponse.HeartbeatInterval = 10;
        CPGetConfigurationResponse.HeartbeatInterval = 10;
        CPBootNotificationRequest.Sent = false;
        memset(&CPBootNotificationRequest, 0, sizeof(CPBootNotificationRequest));
        memset(&CMSHeartbeatResponse, 0, sizeof(CMSHeartbeatResponse));
        memset(CMSRemoteStartTransactionRequest, 0, sizeof(CMSRemoteStartTransactionRequest));
        memset(CMSReserveNowRequest, 0, sizeof(CMSReserveNowRequest));
        memset(CMSChangeAvailabilityRequest, 0, sizeof(CMSChangeAvailabilityRequest));
        memset(&CMSRemoteStopTransactionRequest, 0, sizeof(CMSRemoteStopTransactionRequest));
        memset(&CMSCancelReservationRequest, 0, sizeof(CMSCancelReservationRequest));
        memset(&CMSChangeConfigurationRequest, 0, sizeof(CMSChangeConfigurationRequest));
        memset(&CMSTriggerMessageRequest, 0, sizeof(CMSTriggerMessageRequest));
        memset(&CMSResetRequest, 0, sizeof(CMSResetRequest));
        memset(&CMSUpdateFirmwareRequest, 0, sizeof(CMSUpdateFirmwareRequest));
        memset(&CMSSendLocalListRequest, 0, sizeof(CMSSendLocalListRequest));
        memset(&CMSClearCacheRequest, 0, sizeof(CMSClearCacheRequest));
        memset(&CMSGetLocalListVersionRequest, 0, sizeof(CMSGetLocalListVersionRequest));
        memset(&CMSGetConfigurationRequest, 0, sizeof(CMSGetConfigurationRequest));
        memset(&CMSClearChargingProfileRequest, 0, sizeof(CMSClearChargingProfileRequest));
        memset(&CMSDataTransferRequest, 0, sizeof(CMSDataTransferRequest));
        memset(&CMSGetCompositeScheduleRequest, 0, sizeof(CMSGetCompositeScheduleRequest));
        memset(&CMSGetDiagnosticsRequest, 0, sizeof(CMSGetDiagnosticsRequest));
        memset(&CMSSetChargingProfileRequest, 0, sizeof(CMSSetChargingProfileRequest));
        memset(&CMSUnlockConnectorRequest, 0, sizeof(CMSUnlockConnectorRequest));

        char *last_occurence;
        last_occurence = strrchr(config->webSocketURL, '/');

        if (last_occurence != NULL)
        {
            last_occurence += 1;
            setNULL(CPBootNotificationRequest.chargePointSerialNumber);
            memcpy(CPBootNotificationRequest.chargePointSerialNumber, last_occurence, strlen(last_occurence));
        }
        memcpy(CPBootNotificationRequest.chargePointVendor, config->chargePointVendor, strlen(config->chargePointVendor));
        memcpy(CPBootNotificationRequest.chargePointModel, config->chargePointModel, strlen(config->chargePointModel));
        memcpy(CPBootNotificationRequest.chargeBoxSerialNumber, config->serialNumber, strlen(config->serialNumber));
        memcpy(CPBootNotificationRequest.firmwareVersion, config->firmwareVersion, strlen(config->firmwareVersion));
        for (uint8_t i = 0; i < NUM_OF_CONNECTORS; i++)
        {
            CPMeterValuesRequest[i].connectorId = i;
            CPMeterValuesRequest[i].transactionId = 123456;
            memcpy(CPMeterValuesRequest[i].meterValue.timestamp, "2024-02-01T15:30:00", strlen("2024-02-01T15:30:00"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].location, "Outlet", strlen("Outlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].measurand, "Energy.Active.Import.Register", strlen("Energy.Active.Import.Register"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].unit, "Wh", strlen("Wh"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[0].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].location, "Outlet", strlen("Outlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].measurand, "Power.Active.Import", strlen("Power.Active.Import"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].unit, "kW", strlen("kW"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[1].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].location, "Outlet", strlen("Outlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].measurand, "Voltage", strlen("Voltage"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].unit, "V", strlen("V"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[2].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].location, "Outlet", strlen("Outlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].measurand, "Current.Import", strlen("Current.Import"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].unit, "A", strlen("A"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[3].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].location, "Body", strlen("Body"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].measurand, "Temperature", strlen("Temperature"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].unit, "Celsius", strlen("Celsius"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[4].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].location, "EV", strlen("EV"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].measurand, "SoC", strlen("SoC"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].unit, "Percent", strlen("Percent"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[5].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].location, "Inlet", strlen("Inlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].measurand, "Voltage", strlen("Voltage"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].phase, "L2", strlen("L2"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].unit, "V", strlen("V"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[6].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].location, "Inlet", strlen("Inlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].measurand, "Current.Import", strlen("Current.Import"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].phase, "L2", strlen("L2"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].unit, "A", strlen("A"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[7].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].location, "Inlet", strlen("Inlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].measurand, "Voltage", strlen("Voltage"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].phase, "L3", strlen("L3"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].unit, "V", strlen("V"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[8].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].location, "Inlet", strlen("Inlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].measurand, "Current.Import", strlen("Current.Import"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].phase, "L3", strlen("L3"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].unit, "A", strlen("A"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[9].value, "0", strlen("0"));

            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].context, "Sample.Periodic", strlen("Sample.Periodic"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].format, "Raw", strlen("Raw"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].location, "Inlet", strlen("Inlet"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].measurand, "Power.Factor", strlen("Power.Factor"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].phase, "L1", strlen("L1"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].unit, "Percent", strlen("Percent"));
            memcpy(CPMeterValuesRequest[i].meterValue.sampledValue[10].value, "0", strlen("0"));
        }
    }

    void OCPPController::sendHeartbeatRequest(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification
        sprintf(CPHeartbeatRequest.UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPHeartbeatRequest.UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("Heartbeat"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_PrintUnformatted(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        CPHeartbeatRequest.Sent = true;
        // Delete the cJSON array
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendBootNotificationRequest(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPBootNotificationRequest.UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPBootNotificationRequest.UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("BootNotification"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "chargePointVendor", CPBootNotificationRequest.chargePointVendor);
        cJSON_AddStringToObject(object, "chargePointModel", CPBootNotificationRequest.chargePointModel);
        cJSON_AddStringToObject(object, "chargePointSerialNumber", CPBootNotificationRequest.chargePointSerialNumber);
        cJSON_AddStringToObject(object, "chargeBoxSerialNumber", CPBootNotificationRequest.chargeBoxSerialNumber);
        cJSON_AddStringToObject(object, "firmwareVersion", CPBootNotificationRequest.firmwareVersion);
        if (config->gsmEnable)
        {
            cJSON_AddStringToObject(object, "iccid", CPBootNotificationRequest.iccid);
            cJSON_AddStringToObject(object, "imsi", CPBootNotificationRequest.imsi);
        }
        // cJSON_AddStringToObject(object, "meterType", CPBootNotificationRequest.meterType);
        // cJSON_AddStringToObject(object, "meterSerialNumber", CPBootNotificationRequest.meterSerialNumber);
        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_Print(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        CPBootNotificationRequest.Sent = true;
        // Delete the cJSON array
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendStatusNotificationRequest(uint8_t connId)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPStatusNotificationRequest[connId].UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPStatusNotificationRequest[connId].UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("StatusNotification"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddNumberToObject(object, "connectorId", CPStatusNotificationRequest[connId].connectorId);
        cJSON_AddStringToObject(object, "errorCode", CPStatusNotificationRequest[connId].errorCode);
        cJSON_AddStringToObject(object, "status", CPStatusNotificationRequest[connId].status);
        cJSON_AddStringToObject(object, "timestamp", CPStatusNotificationRequest[connId].timestamp);
        cJSON_AddStringToObject(object, "vendorErrorCode", CPStatusNotificationRequest[connId].vendorErrorCode);
        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_Print(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }

        CPStatusNotificationRequest[connId].Sent = true;
        // Delete the cJSON array
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendAuthorizationRequest(uint8_t connId)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPAuthorizeRequest[connId].UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPAuthorizeRequest[connId].UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("Authorize"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "idTag", CPAuthorizeRequest[connId].idTag);
        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_PrintUnformatted(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }

        CPAuthorizeRequest[connId].Sent = true;
        // Delete the cJSON array
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendFirmwareStatusNotificationRequest(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPFirmwareStatusNotificationRequest.UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPFirmwareStatusNotificationRequest.UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("FirmwareStatusNotification"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPFirmwareStatusNotificationRequest.status);

        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_PrintUnformatted(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }

        CPFirmwareStatusNotificationRequest.Sent = true;
        // Delete the cJSON array
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendDiagnosticsStatusNotificationRequest(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPDiagnosticsStatusNotificationRequest.UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPDiagnosticsStatusNotificationRequest.UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("DiagnosticsStatusNotification"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPDiagnosticsStatusNotificationRequest.status);

        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_PrintUnformatted(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }

        CPDiagnosticsStatusNotificationRequest.Sent = true;
        // Delete the cJSON array
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendStartTransactionRequest(uint8_t connId)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPStartTransactionRequest[connId].UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPStartTransactionRequest[connId].UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("StartTransaction"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddNumberToObject(object, "connectorId", CPStartTransactionRequest[connId].connectorId);
        cJSON_AddStringToObject(object, "idTag", CPStartTransactionRequest[connId].idTag);
        cJSON_AddNumberToObject(object, "meterStart", CPStartTransactionRequest[connId].meterStart);
        if (CPStartTransactionRequest[connId].reservationIdPresent)
        {
            cJSON_AddNumberToObject(object, "reservationId", CPStartTransactionRequest[connId].reservationId);
        }
        cJSON_AddStringToObject(object, "timestamp", CPStartTransactionRequest[connId].timestamp);

        cJSON_AddItemToArray(jsonArray, object);

        // Convert cJSON array to a JSON-formatted string
        char *json_string = cJSON_Print(jsonArray);

        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }

        CPStartTransactionRequest[connId].Sent = true;
        CMSStartTransactionResponse[connId].Received = false;
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendStopTransactionRequest(uint8_t connId, TransactionContext context, bool Aligned, bool AC, bool offline)
    {
        char ContextValuestr[20];
        setNULL(ContextValuestr);

        switch (context)
        {
        case TransactionContext::TransactionBegin:
            memcpy(ContextValuestr, "Transaction.Begin", strlen("Transaction.Begin"));
            break;
        case TransactionContext::TransactionEnd:
            memcpy(ContextValuestr, "Transaction.End", strlen("Transaction.End"));
            break;
        case TransactionContext::SampleClock:
            memcpy(ContextValuestr, "Sample.Clock", strlen("Sample.Clock"));
            break;
        case TransactionContext::SamplePeriodic:
            memcpy(ContextValuestr, "Sample.Periodic", strlen("Sample.Periodic"));
            break;
        case TransactionContext::InterruptionBegin:
            memcpy(ContextValuestr, "Interruption.Begin", strlen("Interruption.Begin"));
            break;
        case TransactionContext::InterruptionEnd:
            memcpy(ContextValuestr, "Interruption.End", strlen("Interruption.End"));
            break;
        case TransactionContext::Trigger:
            memcpy(ContextValuestr, "Trigger", strlen("Trigger"));
            break;
        default:
            memcpy(ContextValuestr, "Other", strlen("Other"));
            break;
        }

        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification

        sprintf(CPStopTransactionRequest[connId].UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPStopTransactionRequest[connId].UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("StopTransaction"));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "idTag", CPStopTransactionRequest[connId].idTag);
        cJSON_AddNumberToObject(object, "meterStop", CPStopTransactionRequest[connId].meterStop);
        cJSON_AddStringToObject(object, "timestamp", CPStopTransactionRequest[connId].timestamp);
        cJSON_AddNumberToObject(object, "transactionId", CPStopTransactionRequest[connId].transactionId);
        cJSON_AddStringToObject(object, "reason", CPStopTransactionRequest[connId].reason);
        if (offline == false)
        {
            cJSON *transactionDataArray = cJSON_CreateArray();
            cJSON *transactionDataObject = cJSON_CreateObject();
            cJSON_AddStringToObject(transactionDataObject, "timestamp", CPStopTransactionRequest[connId].timestamp);

            cJSON *sampledValueArray = cJSON_CreateArray();
            if ((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.EnergyActiveImportRegister) ||
                ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.EnergyActiveImportRegister))
            {
                cJSON *sampledValueObject0 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject0, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[0].value);
                cJSON_AddStringToObject(sampledValueObject0, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject0, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[0].measurand);
                cJSON_AddStringToObject(sampledValueObject0, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[0].location);
                cJSON_AddStringToObject(sampledValueObject0, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[0].unit);
                // cJSON_AddStringToObject(sampledValueObject0, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[0].format);
                // cJSON_AddStringToObject(sampledValueObject0, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[0].phase);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject0);
            }
            if ((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.PowerActiveImport) ||
                ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.PowerActiveImport))
            {
                cJSON *sampledValueObject1 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject1, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[1].value);
                cJSON_AddStringToObject(sampledValueObject1, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject1, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[1].measurand);
                cJSON_AddStringToObject(sampledValueObject1, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[1].location);
                cJSON_AddStringToObject(sampledValueObject1, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[1].unit);
                // cJSON_AddStringToObject(sampledValueObject1, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[1].format);
                // cJSON_AddStringToObject(sampledValueObject1, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[1].phase);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject1);
            }
            if ((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.Voltage) ||
                ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.Voltage))
            {
                cJSON *sampledValueObject2 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject2, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[2].value);
                cJSON_AddStringToObject(sampledValueObject2, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject2, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[2].measurand);
                cJSON_AddStringToObject(sampledValueObject2, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[2].location);
                cJSON_AddStringToObject(sampledValueObject2, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[2].unit);
                if (AC)
                    cJSON_AddStringToObject(sampledValueObject2, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[2].phase);
                // cJSON_AddStringToObject(sampledValueObject2, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[2].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject2);
            }
            if ((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.CurrentImport) ||
                ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.CurrentImport))
            {
                cJSON *sampledValueObject3 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject3, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[3].value);
                cJSON_AddStringToObject(sampledValueObject3, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject3, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[3].measurand);
                cJSON_AddStringToObject(sampledValueObject3, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[3].location);
                cJSON_AddStringToObject(sampledValueObject3, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[3].unit);
                if (AC)
                    cJSON_AddStringToObject(sampledValueObject3, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[3].phase);
                // cJSON_AddStringToObject(sampledValueObject3, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[3].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject3);
            }
            if ((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.Temperature) ||
                ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.Temperature))
            {
                cJSON *sampledValueObject4 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject4, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[4].value);
                cJSON_AddStringToObject(sampledValueObject4, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject4, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[4].measurand);
                cJSON_AddStringToObject(sampledValueObject4, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[4].location);
                cJSON_AddStringToObject(sampledValueObject4, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[4].unit);
                // cJSON_AddStringToObject(sampledValueObject4, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[4].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject4);
            }
            if ((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.SoC) ||
                ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.SoC))
            {
                cJSON *sampledValueObject5 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject5, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[5].value);
                cJSON_AddStringToObject(sampledValueObject5, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject5, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[5].measurand);
                cJSON_AddStringToObject(sampledValueObject5, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[5].location);
                cJSON_AddStringToObject(sampledValueObject5, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[5].unit);
                // cJSON_AddStringToObject(sampledValueObject5, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[5].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject5);
            }
            if (((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.Voltage) ||
                 ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.Voltage)) &&
                AC)
            {
                cJSON *sampledValueObject6 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject6, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[6].value);
                cJSON_AddStringToObject(sampledValueObject6, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject6, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[6].measurand);
                cJSON_AddStringToObject(sampledValueObject6, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[6].location);
                cJSON_AddStringToObject(sampledValueObject6, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[6].unit);
                cJSON_AddStringToObject(sampledValueObject6, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[6].phase);
                // cJSON_AddStringToObject(sampledValueObject6, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[6].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject6);
            }
            if (((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.CurrentImport) ||
                 ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.CurrentImport)) &&
                AC)
            {
                cJSON *sampledValueObject7 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject7, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[7].value);
                cJSON_AddStringToObject(sampledValueObject7, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject7, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[7].measurand);
                cJSON_AddStringToObject(sampledValueObject7, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[7].location);
                cJSON_AddStringToObject(sampledValueObject7, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[7].unit);
                cJSON_AddStringToObject(sampledValueObject7, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[7].phase);
                // cJSON_AddStringToObject(sampledValueObject7, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[7].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject7);
            }
            if (((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.Voltage) ||
                 ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.Voltage)) &&
                AC)
            {
                cJSON *sampledValueObject8 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject8, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[8].value);
                cJSON_AddStringToObject(sampledValueObject8, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject8, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[8].measurand);
                cJSON_AddStringToObject(sampledValueObject8, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[8].location);
                cJSON_AddStringToObject(sampledValueObject8, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[8].unit);
                cJSON_AddStringToObject(sampledValueObject8, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[8].phase);
                // cJSON_AddStringToObject(sampledValueObject8, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[8].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject8);
            }
            if (((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.CurrentImport) ||
                 ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.CurrentImport)) &&
                AC)
            {
                cJSON *sampledValueObject9 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject9, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[9].value);
                cJSON_AddStringToObject(sampledValueObject9, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject9, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[9].measurand);
                cJSON_AddStringToObject(sampledValueObject9, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[9].location);
                cJSON_AddStringToObject(sampledValueObject9, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[9].unit);
                cJSON_AddStringToObject(sampledValueObject9, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[9].phase);
                // cJSON_AddStringToObject(sampledValueObject9, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[9].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject9);
            }
            if (((Aligned && CPGetConfigurationResponse.StopTxnAlignedData.PowerFactor) ||
                 ((Aligned == false) && CPGetConfigurationResponse.StopTxnSampledData.PowerFactor)) &&
                AC)
            {
                cJSON *sampledValueObject10 = cJSON_CreateObject();
                cJSON_AddStringToObject(sampledValueObject10, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[10].value);
                cJSON_AddStringToObject(sampledValueObject10, "context", ContextValuestr);
                cJSON_AddStringToObject(sampledValueObject10, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[10].measurand);
                cJSON_AddStringToObject(sampledValueObject10, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[10].location);
                // cJSON_AddStringToObject(sampledValueObject10, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[10].phase);
                // cJSON_AddStringToObject(sampledValueObject10, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[10].unit);
                // cJSON_AddStringToObject(sampledValueObject10, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[10].format);
                cJSON_AddItemToArray(sampledValueArray, sampledValueObject10);
            }

            cJSON_AddItemToObject(transactionDataObject, "sampledValue", sampledValueArray);
            cJSON_AddItemToArray(transactionDataArray, transactionDataObject);

            cJSON_AddItemToObject(object, "transactionData", transactionDataArray);
        }
        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_Print(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPStopTransactionRequest[connId].Sent = true;
        CMSStopTransactionResponse[connId].Received = false;
    }

    void OCPPController::sendMeterValuesRequest(uint8_t connId, TransactionContext context, bool Aligned, bool AC)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();
        // Generate Random unique Id for boot notification
        char ContextValuestr[20];
        setNULL(ContextValuestr);

        switch (context)
        {
        case TransactionContext::TransactionBegin:
            memcpy(ContextValuestr, "Transaction.Begin", strlen("Transaction.Begin"));
            break;
        case TransactionContext::TransactionEnd:
            memcpy(ContextValuestr, "Transaction.End", strlen("Transaction.End"));
            break;
        case TransactionContext::SampleClock:
            memcpy(ContextValuestr, "Sample.Clock", strlen("Sample.Clock"));
            break;
        case TransactionContext::SamplePeriodic:
            memcpy(ContextValuestr, "Sample.Periodic", strlen("Sample.Periodic"));
            break;
        case TransactionContext::InterruptionBegin:
            memcpy(ContextValuestr, "Interruption.Begin", strlen("Interruption.Begin"));
            break;
        case TransactionContext::InterruptionEnd:
            memcpy(ContextValuestr, "Interruption.End", strlen("Interruption.End"));
            break;
        case TransactionContext::Trigger:
            memcpy(ContextValuestr, "Trigger", strlen("Trigger"));
            break;
        default:
            memcpy(ContextValuestr, "Other", strlen("Other"));
            break;
        }

        sprintf(CPMeterValuesRequest[connId].UniqId, "%llu", randomNumber++);
        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_REQUEST));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CPMeterValuesRequest[connId].UniqId));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString("MeterValues"));

        // Create an object and add it to the cJSON array
        cJSON *meterValuesRequest = cJSON_CreateObject();

        cJSON_AddNumberToObject(meterValuesRequest, "connectorId", CPMeterValuesRequest[connId].connectorId);
        cJSON_AddNumberToObject(meterValuesRequest, "transactionId", CPMeterValuesRequest[connId].transactionId);

        cJSON *meterValueArray = cJSON_CreateArray();
        cJSON *meterValueObject = cJSON_CreateObject();
        cJSON_AddStringToObject(meterValueObject, "timestamp", CPMeterValuesRequest[connId].meterValue.timestamp);

        cJSON *sampledValueArray = cJSON_CreateArray();
        if ((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.EnergyActiveImportRegister) ||
            ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.EnergyActiveImportRegister))
        {
            cJSON *sampledValueObject0 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject0, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[0].value);
            cJSON_AddStringToObject(sampledValueObject0, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject0, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[0].measurand);
            cJSON_AddStringToObject(sampledValueObject0, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[0].location);
            cJSON_AddStringToObject(sampledValueObject0, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[0].unit);
            // cJSON_AddStringToObject(sampledValueObject0, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[0].format);
            // cJSON_AddStringToObject(sampledValueObject0, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[0].phase);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject0);
        }
        if ((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.PowerActiveImport) ||
            ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.PowerActiveImport))
        {
            cJSON *sampledValueObject1 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject1, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[1].value);
            cJSON_AddStringToObject(sampledValueObject1, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject1, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[1].measurand);
            cJSON_AddStringToObject(sampledValueObject1, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[1].location);
            cJSON_AddStringToObject(sampledValueObject1, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[1].unit);
            // cJSON_AddStringToObject(sampledValueObject1, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[1].format);
            // cJSON_AddStringToObject(sampledValueObject1, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[1].phase);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject1);
        }
        if ((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.Voltage) ||
            ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.Voltage))
        {
            cJSON *sampledValueObject2 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject2, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[2].value);
            cJSON_AddStringToObject(sampledValueObject2, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject2, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[2].measurand);
            cJSON_AddStringToObject(sampledValueObject2, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[2].location);
            cJSON_AddStringToObject(sampledValueObject2, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[2].unit);
            if (AC)
                cJSON_AddStringToObject(sampledValueObject2, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[2].phase);
            // cJSON_AddStringToObject(sampledValueObject2, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[2].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject2);
        }
        if ((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.CurrentImport) ||
            ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.CurrentImport))
        {
            cJSON *sampledValueObject3 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject3, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[3].value);
            cJSON_AddStringToObject(sampledValueObject3, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject3, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[3].measurand);
            cJSON_AddStringToObject(sampledValueObject3, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[3].location);
            cJSON_AddStringToObject(sampledValueObject3, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[3].unit);
            if (AC)
                cJSON_AddStringToObject(sampledValueObject3, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[3].phase);
            // cJSON_AddStringToObject(sampledValueObject3, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[3].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject3);
        }
        if ((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.Temperature) ||
            ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.Temperature))
        {
            cJSON *sampledValueObject4 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject4, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[4].value);
            cJSON_AddStringToObject(sampledValueObject4, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject4, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[4].measurand);
            cJSON_AddStringToObject(sampledValueObject4, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[4].location);
            cJSON_AddStringToObject(sampledValueObject4, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[4].unit);
            // cJSON_AddStringToObject(sampledValueObject4, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[4].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject4);
        }
        if ((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.SoC) ||
            ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.SoC))
        {
            cJSON *sampledValueObject5 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject5, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[5].value);
            cJSON_AddStringToObject(sampledValueObject5, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject5, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[5].measurand);
            cJSON_AddStringToObject(sampledValueObject5, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[5].location);
            cJSON_AddStringToObject(sampledValueObject5, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[5].unit);
            // cJSON_AddStringToObject(sampledValueObject5, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[5].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject5);
        }
        if (((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.Voltage) ||
             ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.Voltage)) &&
            AC)
        {
            cJSON *sampledValueObject6 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject6, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[6].value);
            cJSON_AddStringToObject(sampledValueObject6, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject6, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[6].measurand);
            cJSON_AddStringToObject(sampledValueObject6, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[6].location);
            cJSON_AddStringToObject(sampledValueObject6, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[6].unit);
            cJSON_AddStringToObject(sampledValueObject6, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[6].phase);
            // cJSON_AddStringToObject(sampledValueObject6, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[6].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject6);
        }
        if (((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.CurrentImport) ||
             ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.CurrentImport)) &&
            AC)
        {
            cJSON *sampledValueObject7 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject7, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[7].value);
            cJSON_AddStringToObject(sampledValueObject7, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject7, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[7].measurand);
            cJSON_AddStringToObject(sampledValueObject7, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[7].location);
            cJSON_AddStringToObject(sampledValueObject7, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[7].unit);
            cJSON_AddStringToObject(sampledValueObject7, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[7].phase);
            // cJSON_AddStringToObject(sampledValueObject7, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[7].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject7);
        }
        if (((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.Voltage) ||
             ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.Voltage)) &&
            AC)
        {
            cJSON *sampledValueObject8 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject8, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[8].value);
            cJSON_AddStringToObject(sampledValueObject8, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject8, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[8].measurand);
            cJSON_AddStringToObject(sampledValueObject8, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[8].location);
            cJSON_AddStringToObject(sampledValueObject8, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[8].unit);
            cJSON_AddStringToObject(sampledValueObject8, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[8].phase);
            // cJSON_AddStringToObject(sampledValueObject8, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[8].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject8);
        }
        if (((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.CurrentImport) ||
             ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.CurrentImport)) &&
            AC)
        {
            cJSON *sampledValueObject9 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject9, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[9].value);
            cJSON_AddStringToObject(sampledValueObject9, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject9, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[9].measurand);
            cJSON_AddStringToObject(sampledValueObject9, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[9].location);
            cJSON_AddStringToObject(sampledValueObject9, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[9].unit);
            cJSON_AddStringToObject(sampledValueObject9, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[9].phase);
            // cJSON_AddStringToObject(sampledValueObject9, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[9].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject9);
        }
        if (((Aligned && CPGetConfigurationResponse.MeterValuesAlignedData.PowerFactor) ||
             ((Aligned == false) && CPGetConfigurationResponse.MeterValuesSampledData.PowerFactor)) &&
            AC)
        {
            cJSON *sampledValueObject10 = cJSON_CreateObject();
            cJSON_AddStringToObject(sampledValueObject10, "value", CPMeterValuesRequest[connId].meterValue.sampledValue[10].value);
            cJSON_AddStringToObject(sampledValueObject10, "context", ContextValuestr);
            cJSON_AddStringToObject(sampledValueObject10, "measurand", CPMeterValuesRequest[connId].meterValue.sampledValue[10].measurand);
            cJSON_AddStringToObject(sampledValueObject10, "location", CPMeterValuesRequest[connId].meterValue.sampledValue[10].location);
            // cJSON_AddStringToObject(sampledValueObject10, "phase", CPMeterValuesRequest[connId].meterValue.sampledValue[10].phase);
            // cJSON_AddStringToObject(sampledValueObject10, "unit", CPMeterValuesRequest[connId].meterValue.sampledValue[10].unit);
            // cJSON_AddStringToObject(sampledValueObject10, "format", CPMeterValuesRequest[connId].meterValue.sampledValue[10].format);
            cJSON_AddItemToArray(sampledValueArray, sampledValueObject10);
        }

        cJSON_AddItemToObject(meterValueObject, "sampledValue", sampledValueArray);
        cJSON_AddItemToArray(meterValueArray, meterValueObject);
        cJSON_AddItemToObject(meterValuesRequest, "meterValue", meterValueArray);

        cJSON_AddItemToArray(jsonArray, meterValuesRequest);

        char *json_string = cJSON_Print(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPMeterValuesRequest[connId].Sent = true;
        CMSMeterValuesResponse[connId].Received = false;
    }

    void OCPPController::sendCancelReservationResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSCancelReservationRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPCancelReservationResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPCancelReservationResponse.Sent = true;
    }

    void OCPPController::sendClearChargingProfileResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSClearChargingProfileRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        // if (config->smartCharging)
        // cJSON_AddStringToObject(object, "status", "Accepted");
        // else
        cJSON_AddStringToObject(object, "status", "NotSupported");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendDataTransferResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSDataTransferRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", "Rejected");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendGetCompositeScheduleResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSGetCompositeScheduleRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", "Rejected");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendGetDiagnosticsResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSGetDiagnosticsRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();
        cJSON_AddStringToObject(object, "fileName", CPGetDiagnosticsResponse.fileName);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPGetDiagnosticsResponse.Sent = true;
    }

    void OCPPController::sendSetChargingProfileResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSSetChargingProfileRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        // if (config->smartCharging)
        //    cJSON_AddStringToObject(object, "status", "Accepted");
        // else
        cJSON_AddStringToObject(object, "status", "NotSupported");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendUnlockConnectorResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSUnlockConnectorRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", "NotSupported");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendChangeAvailabilityResponse(uint8_t connId)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSChangeAvailabilityRequest[connId].UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPChangeAvailabilityResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPChangeAvailabilityResponse.Sent = true;
    }

    void OCPPController::sendChangeConfigurationResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSChangeConfigurationRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPChangeConfigurationResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPChangeConfigurationResponse.Sent = true;
    }

    void OCPPController::sendClearCacheResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSClearCacheRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPClearCacheResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPGetLocalListVersionResponse.Sent = true;
    }

    void OCPPController::sendUpdateFirmwareResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSUpdateFirmwareRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPUpdateFirmwareResponse.Sent = true;
    }

    void OCPPController::sendGetLocalListVersionResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSGetLocalListVersionRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddNumberToObject(object, "listVersion", CMSSendLocalListRequest.listVersion);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPGetLocalListVersionResponse.Sent = true;
    }

    void OCPPController::sendRemoteStartTransactionResponse(uint8_t connId)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSRemoteStartTransactionRequest[connId].UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPRemoteStartTransactionResponse[connId].status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPRemoteStartTransactionResponse[connId].Sent = true;
    }

    void OCPPController::sendRemoteStopTransactionResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSRemoteStopTransactionRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", "Accepted");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPRemoteStopTransactionResponse.Sent = true;
    }

    void OCPPController::sendReserveNowResponse(uint8_t connId)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSReserveNowRequest[connId].UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPReserveNowResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPReserveNowResponse.Sent = true;
    }

    void OCPPController::sendResetResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSResetRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPResetResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPResetResponse.Sent = true;
    }

    void OCPPController::sendLocalListResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSSendLocalListRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", "Accepted");

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPSendLocalListResponse.Sent = true;
    }

    void OCPPController::sendTriggerMessageResponse(void)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSTriggerMessageRequest.UniqId));

        // Create an object and add it to the cJSON array
        cJSON *object = cJSON_CreateObject();

        cJSON_AddStringToObject(object, "status", CPTriggerMessageResponse.status);

        cJSON_AddItemToArray(jsonArray, object);

        char *json_string = cJSON_PrintUnformatted(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
        CPTriggerMessageResponse.Sent = true;
    }

    void OCPPController::sendGetConfigurationResponseKeys(uint8_t count)
    {
        // Create a cJSON array
        cJSON *jsonArray = cJSON_CreateArray();

        // Add elements to the cJSON array
        cJSON_AddItemToArray(jsonArray, cJSON_CreateNumber(OCPP_RESPONSE));
        cJSON_AddItemToArray(jsonArray, cJSON_CreateString(CMSGetConfigurationRequest.UniqId));

        cJSON *configurationJson = cJSON_CreateObject();
        cJSON *configurationKeyJsonArray = cJSON_CreateArray();
        if (count == 1)
        {
            //    Add AuthorizeRemoteTxRequests to configurationKeyJsonArray
            cJSON *AuthorizeRemoteTxRequestsJson = cJSON_CreateObject();
            cJSON_AddStringToObject(AuthorizeRemoteTxRequestsJson, "key", "AuthorizeRemoteTxRequests");
            cJSON_AddBoolToObject(AuthorizeRemoteTxRequestsJson, "readonly", CPGetConfigurationResponse.AuthorizeRemoteTxRequestsReadOnly ? true : false);
            cJSON_AddStringToObject(AuthorizeRemoteTxRequestsJson, "value", CPGetConfigurationResponse.AuthorizeRemoteTxRequestsValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, AuthorizeRemoteTxRequestsJson);

            //    Add ClockAlignedDataInterval to configurationKeyJsonArray
            cJSON *ClockAlignedDataIntervalJson = cJSON_CreateObject();
            cJSON_AddStringToObject(ClockAlignedDataIntervalJson, "key", "ClockAlignedDataInterval");
            cJSON_AddBoolToObject(ClockAlignedDataIntervalJson, "readonly", CPGetConfigurationResponse.ClockAlignedDataIntervalReadOnly ? true : false);
            cJSON_AddStringToObject(ClockAlignedDataIntervalJson, "value", CPGetConfigurationResponse.ClockAlignedDataIntervalValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, ClockAlignedDataIntervalJson);

            //    Add ConnectionTimeOut to configurationKeyJsonArray
            cJSON *ConnectionTimeOutJson = cJSON_CreateObject();
            cJSON_AddStringToObject(ConnectionTimeOutJson, "key", "ConnectionTimeOut");
            cJSON_AddBoolToObject(ConnectionTimeOutJson, "readonly", CPGetConfigurationResponse.ConnectionTimeOutReadOnly ? true : false);
            cJSON_AddStringToObject(ConnectionTimeOutJson, "value", CPGetConfigurationResponse.ConnectionTimeOutValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, ConnectionTimeOutJson);

            //    Add GetConfigurationMaxKeys to configurationKeyJsonArray
            cJSON *GetConfigurationMaxKeysJson = cJSON_CreateObject();
            cJSON_AddStringToObject(GetConfigurationMaxKeysJson, "key", "GetConfigurationMaxKeys");
            cJSON_AddBoolToObject(GetConfigurationMaxKeysJson, "readonly", CPGetConfigurationResponse.GetConfigurationMaxKeysReadOnly ? true : false);
            cJSON_AddStringToObject(GetConfigurationMaxKeysJson, "value", CPGetConfigurationResponse.GetConfigurationMaxKeysValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, GetConfigurationMaxKeysJson);

            //    Add HeartbeatInterval to configurationKeyJsonArray
            cJSON *HeartbeatIntervalJson = cJSON_CreateObject();
            cJSON_AddStringToObject(HeartbeatIntervalJson, "key", "HeartbeatInterval");
            cJSON_AddBoolToObject(HeartbeatIntervalJson, "readonly", CPGetConfigurationResponse.HeartbeatIntervalReadOnly ? true : false);
            cJSON_AddStringToObject(HeartbeatIntervalJson, "value", CPGetConfigurationResponse.HeartbeatIntervalValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, HeartbeatIntervalJson);

            //    Add LocalAuthorizeOffline to configurationKeyJsonArray
            cJSON *LocalAuthorizeOfflineJson = cJSON_CreateObject();
            cJSON_AddStringToObject(LocalAuthorizeOfflineJson, "key", "LocalAuthorizeOffline");
            cJSON_AddBoolToObject(LocalAuthorizeOfflineJson, "readonly", CPGetConfigurationResponse.LocalAuthorizeOfflineReadOnly ? true : false);
            cJSON_AddStringToObject(LocalAuthorizeOfflineJson, "value", CPGetConfigurationResponse.LocalAuthorizeOfflineValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, LocalAuthorizeOfflineJson);

            //    Add LocalPreAuthorize to configurationKeyJsonArray
            cJSON *LocalPreAuthorizeJson = cJSON_CreateObject();
            cJSON_AddStringToObject(LocalPreAuthorizeJson, "key", "LocalPreAuthorize");
            cJSON_AddBoolToObject(LocalPreAuthorizeJson, "readonly", CPGetConfigurationResponse.LocalPreAuthorizeReadOnly ? true : false);
            cJSON_AddStringToObject(LocalPreAuthorizeJson, "value", CPGetConfigurationResponse.LocalPreAuthorizeValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, LocalPreAuthorizeJson);

            //    Add MeterValuesAlignedData to configurationKeyJsonArray
            cJSON *MeterValuesAlignedDataJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MeterValuesAlignedDataJson, "key", "MeterValuesAlignedData");
            cJSON_AddBoolToObject(MeterValuesAlignedDataJson, "readonly", CPGetConfigurationResponse.MeterValuesAlignedDataReadOnly ? true : false);
            cJSON_AddStringToObject(MeterValuesAlignedDataJson, "value", CPGetConfigurationResponse.MeterValuesAlignedDataValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MeterValuesAlignedDataJson);

            //    Add MeterValuesSampledData to configurationKeyJsonArray
            cJSON *MeterValuesSampledDataJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MeterValuesSampledDataJson, "key", "MeterValuesSampledData");
            cJSON_AddBoolToObject(MeterValuesSampledDataJson, "readonly", CPGetConfigurationResponse.MeterValuesSampledDataReadOnly ? true : false);
            cJSON_AddStringToObject(MeterValuesSampledDataJson, "value", CPGetConfigurationResponse.MeterValuesSampledDataValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MeterValuesSampledDataJson);

            //    Add MeterValueSampleInterval to configurationKeyJsonArray
            cJSON *MeterValueSampleIntervalJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MeterValueSampleIntervalJson, "key", "MeterValueSampleInterval");
            cJSON_AddBoolToObject(MeterValueSampleIntervalJson, "readonly", CPGetConfigurationResponse.MeterValueSampleIntervalReadOnly ? true : false);
            cJSON_AddStringToObject(MeterValueSampleIntervalJson, "value", CPGetConfigurationResponse.MeterValueSampleIntervalValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MeterValueSampleIntervalJson);

            //    Add NumberOfConnectors to configurationKeyJsonArray
            cJSON *NumberOfConnectorsJson = cJSON_CreateObject();
            cJSON_AddStringToObject(NumberOfConnectorsJson, "key", "NumberOfConnectors");
            cJSON_AddBoolToObject(NumberOfConnectorsJson, "readonly", CPGetConfigurationResponse.NumberOfConnectorsReadOnly ? true : false);
            cJSON_AddStringToObject(NumberOfConnectorsJson, "value", CPGetConfigurationResponse.NumberOfConnectorsValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, NumberOfConnectorsJson);

            //    Add ResetRetries to configurationKeyJsonArray
            cJSON *ResetRetriesJson = cJSON_CreateObject();
            cJSON_AddStringToObject(ResetRetriesJson, "key", "ResetRetries");
            cJSON_AddBoolToObject(ResetRetriesJson, "readonly", CPGetConfigurationResponse.ResetRetriesReadOnly ? true : false);
            cJSON_AddStringToObject(ResetRetriesJson, "value", CPGetConfigurationResponse.ResetRetriesValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, ResetRetriesJson);

            //    Add ConnectorPhaseRotation to configurationKeyJsonArray
            cJSON *ConnectorPhaseRotationJson = cJSON_CreateObject();
            cJSON_AddStringToObject(ConnectorPhaseRotationJson, "key", "ConnectorPhaseRotation");
            cJSON_AddBoolToObject(ConnectorPhaseRotationJson, "readonly", CPGetConfigurationResponse.ConnectorPhaseRotationReadOnly ? true : false);
            cJSON_AddStringToObject(ConnectorPhaseRotationJson, "value", CPGetConfigurationResponse.ConnectorPhaseRotationValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, ConnectorPhaseRotationJson);

            //    Add StopTransactionOnEVSideDisconnect to configurationKeyJsonArray
            cJSON *StopTransactionOnEVSideDisconnectJson = cJSON_CreateObject();
            cJSON_AddStringToObject(StopTransactionOnEVSideDisconnectJson, "key", "StopTransactionOnEVSideDisconnect");
            cJSON_AddBoolToObject(StopTransactionOnEVSideDisconnectJson, "readonly", CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectReadOnly ? true : false);
            cJSON_AddStringToObject(StopTransactionOnEVSideDisconnectJson, "value", CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, StopTransactionOnEVSideDisconnectJson);

            //    Add StopTransactionOnInvalidId to configurationKeyJsonArray
            cJSON *StopTransactionOnInvalidIdJson = cJSON_CreateObject();
            cJSON_AddStringToObject(StopTransactionOnInvalidIdJson, "key", "StopTransactionOnInvalidId");
            cJSON_AddBoolToObject(StopTransactionOnInvalidIdJson, "readonly", CPGetConfigurationResponse.StopTransactionOnInvalidIdReadOnly ? true : false);
            cJSON_AddStringToObject(StopTransactionOnInvalidIdJson, "value", CPGetConfigurationResponse.StopTransactionOnInvalidIdValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, StopTransactionOnInvalidIdJson);
        }
        else if (count == 2)
        {
            //    Add StopTxnAlignedData to configurationKeyJsonArray
            cJSON *StopTxnAlignedDataJson = cJSON_CreateObject();
            cJSON_AddStringToObject(StopTxnAlignedDataJson, "key", "StopTxnAlignedData");
            cJSON_AddBoolToObject(StopTxnAlignedDataJson, "readonly", CPGetConfigurationResponse.StopTxnAlignedDataReadOnly ? true : false);
            cJSON_AddStringToObject(StopTxnAlignedDataJson, "value", CPGetConfigurationResponse.StopTxnAlignedDataValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, StopTxnAlignedDataJson);

            //    Add StopTxnSampledData to configurationKeyJsonArray
            cJSON *StopTxnSampledDataJson = cJSON_CreateObject();
            cJSON_AddStringToObject(StopTxnSampledDataJson, "key", "StopTxnSampledData");
            cJSON_AddBoolToObject(StopTxnSampledDataJson, "readonly", CPGetConfigurationResponse.StopTxnSampledDataReadOnly ? true : false);
            cJSON_AddStringToObject(StopTxnSampledDataJson, "value", CPGetConfigurationResponse.StopTxnSampledDataValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, StopTxnSampledDataJson);

            //    Add SupportedFeatureProfiles to configurationKeyJsonArray
            cJSON *SupportedFeatureProfilesJson = cJSON_CreateObject();
            cJSON_AddStringToObject(SupportedFeatureProfilesJson, "key", "SupportedFeatureProfiles");
            cJSON_AddBoolToObject(SupportedFeatureProfilesJson, "readonly", CPGetConfigurationResponse.SupportedFeatureProfilesReadOnly ? true : false);
            cJSON_AddStringToObject(SupportedFeatureProfilesJson, "value", CPGetConfigurationResponse.SupportedFeatureProfilesValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, SupportedFeatureProfilesJson);

            //    Add TransactionMessageAttempts to configurationKeyJsonArray
            cJSON *TransactionMessageAttemptsJson = cJSON_CreateObject();
            cJSON_AddStringToObject(TransactionMessageAttemptsJson, "key", "TransactionMessageAttempts");
            cJSON_AddBoolToObject(TransactionMessageAttemptsJson, "readonly", CPGetConfigurationResponse.TransactionMessageAttemptsReadOnly ? true : false);
            cJSON_AddStringToObject(TransactionMessageAttemptsJson, "value", CPGetConfigurationResponse.TransactionMessageAttemptsValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, TransactionMessageAttemptsJson);

            //    Add TransactionMessageRetryInterval to configurationKeyJsonArray
            cJSON *TransactionMessageRetryIntervalJson = cJSON_CreateObject();
            cJSON_AddStringToObject(TransactionMessageRetryIntervalJson, "key", "TransactionMessageRetryInterval");
            cJSON_AddBoolToObject(TransactionMessageRetryIntervalJson, "readonly", CPGetConfigurationResponse.TransactionMessageRetryIntervalReadOnly ? true : false);
            cJSON_AddStringToObject(TransactionMessageRetryIntervalJson, "value", CPGetConfigurationResponse.TransactionMessageRetryIntervalValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, TransactionMessageRetryIntervalJson);

            //    Add UnlockConnectorOnEVSideDisconnect to configurationKeyJsonArray
            cJSON *UnlockConnectorOnEVSideDisconnectJson = cJSON_CreateObject();
            cJSON_AddStringToObject(UnlockConnectorOnEVSideDisconnectJson, "key", "UnlockConnectorOnEVSideDisconnect");
            cJSON_AddBoolToObject(UnlockConnectorOnEVSideDisconnectJson, "readonly", CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectReadOnly ? true : false);
            cJSON_AddStringToObject(UnlockConnectorOnEVSideDisconnectJson, "value", CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, UnlockConnectorOnEVSideDisconnectJson);

            //    Add AllowOfflineTxForUnknownId to configurationKeyJsonArray
            cJSON *AllowOfflineTxForUnknownIdJson = cJSON_CreateObject();
            cJSON_AddStringToObject(AllowOfflineTxForUnknownIdJson, "key", "AllowOfflineTxForUnknownId");
            cJSON_AddBoolToObject(AllowOfflineTxForUnknownIdJson, "readonly", CPGetConfigurationResponse.AllowOfflineTxForUnknownIdReadOnly ? true : false);
            cJSON_AddStringToObject(AllowOfflineTxForUnknownIdJson, "value", CPGetConfigurationResponse.AllowOfflineTxForUnknownIdValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, AllowOfflineTxForUnknownIdJson);

            //    Add AuthorizationCacheEnabled to configurationKeyJsonArray
            cJSON *AuthorizationCacheEnabledJson = cJSON_CreateObject();
            cJSON_AddStringToObject(AuthorizationCacheEnabledJson, "key", "AuthorizationCacheEnabled");
            cJSON_AddBoolToObject(AuthorizationCacheEnabledJson, "readonly", CPGetConfigurationResponse.AuthorizationCacheEnabledReadOnly ? true : false);
            cJSON_AddStringToObject(AuthorizationCacheEnabledJson, "value", CPGetConfigurationResponse.AuthorizationCacheEnabledValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, AuthorizationCacheEnabledJson);

            //    Add BlinkRepeat to configurationKeyJsonArray
            cJSON *BlinkRepeatJson = cJSON_CreateObject();
            cJSON_AddStringToObject(BlinkRepeatJson, "key", "BlinkRepeat");
            cJSON_AddBoolToObject(BlinkRepeatJson, "readonly", CPGetConfigurationResponse.BlinkRepeatReadOnly ? true : false);
            cJSON_AddStringToObject(BlinkRepeatJson, "value", CPGetConfigurationResponse.BlinkRepeatValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, BlinkRepeatJson);

            //    Add LightIntensity to configurationKeyJsonArray
            cJSON *LightIntensityJson = cJSON_CreateObject();
            cJSON_AddStringToObject(LightIntensityJson, "key", "LightIntensity");
            cJSON_AddBoolToObject(LightIntensityJson, "readonly", CPGetConfigurationResponse.LightIntensityReadOnly ? true : false);
            cJSON_AddStringToObject(LightIntensityJson, "value", CPGetConfigurationResponse.LightIntensityValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, LightIntensityJson);

            //    Add MaxEnergyOnInvalidId to configurationKeyJsonArray
            cJSON *MaxEnergyOnInvalidIdJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MaxEnergyOnInvalidIdJson, "key", "MaxEnergyOnInvalidId");
            cJSON_AddBoolToObject(MaxEnergyOnInvalidIdJson, "readonly", CPGetConfigurationResponse.MaxEnergyOnInvalidIdReadOnly ? true : false);
            cJSON_AddStringToObject(MaxEnergyOnInvalidIdJson, "value", CPGetConfigurationResponse.MaxEnergyOnInvalidIdValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MaxEnergyOnInvalidIdJson);

            //    Add MeterValuesAlignedDataMaxLength to configurationKeyJsonArray
            cJSON *MeterValuesAlignedDataMaxLengthJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MeterValuesAlignedDataMaxLengthJson, "key", "MeterValuesAlignedDataMaxLength");
            cJSON_AddBoolToObject(MeterValuesAlignedDataMaxLengthJson, "readonly", CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthReadOnly ? true : false);
            cJSON_AddStringToObject(MeterValuesAlignedDataMaxLengthJson, "value", CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MeterValuesAlignedDataMaxLengthJson);

            //    Add MeterValuesSampledDataMaxLength to configurationKeyJsonArray
            cJSON *MeterValuesSampledDataMaxLengthJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MeterValuesSampledDataMaxLengthJson, "key", "MeterValuesSampledDataMaxLength");
            cJSON_AddBoolToObject(MeterValuesSampledDataMaxLengthJson, "readonly", CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthReadOnly ? true : false);
            cJSON_AddStringToObject(MeterValuesSampledDataMaxLengthJson, "value", CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MeterValuesSampledDataMaxLengthJson);

            //    Add MinimumStatusDuration to configurationKeyJsonArray
            cJSON *MinimumStatusDurationJson = cJSON_CreateObject();
            cJSON_AddStringToObject(MinimumStatusDurationJson, "key", "MinimumStatusDuration");
            cJSON_AddBoolToObject(MinimumStatusDurationJson, "readonly", CPGetConfigurationResponse.MinimumStatusDurationReadOnly ? true : false);
            cJSON_AddStringToObject(MinimumStatusDurationJson, "value", CPGetConfigurationResponse.MinimumStatusDurationValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, MinimumStatusDurationJson);

            //    Add ConnectorPhaseRotationMaxLength to configurationKeyJsonArray
            cJSON *ConnectorPhaseRotationMaxLengthJson = cJSON_CreateObject();
            cJSON_AddStringToObject(ConnectorPhaseRotationMaxLengthJson, "key", "ConnectorPhaseRotationMaxLength");
            cJSON_AddBoolToObject(ConnectorPhaseRotationMaxLengthJson, "readonly", CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthReadOnly ? true : false);
            cJSON_AddStringToObject(ConnectorPhaseRotationMaxLengthJson, "value", CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, ConnectorPhaseRotationMaxLengthJson);
        }
        else if (count == 3)
        {
            //    Add StopTxnAlignedDataMaxLength to configurationKeyJsonArray
            cJSON *StopTxnAlignedDataMaxLengthJson = cJSON_CreateObject();
            cJSON_AddStringToObject(StopTxnAlignedDataMaxLengthJson, "key", "StopTxnAlignedDataMaxLength");
            cJSON_AddBoolToObject(StopTxnAlignedDataMaxLengthJson, "readonly", CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthReadOnly ? true : false);
            cJSON_AddStringToObject(StopTxnAlignedDataMaxLengthJson, "value", CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, StopTxnAlignedDataMaxLengthJson);

            //    Add StopTxnSampledDataMaxLength to configurationKeyJsonArray
            cJSON *StopTxnSampledDataMaxLengthJson = cJSON_CreateObject();
            cJSON_AddStringToObject(StopTxnSampledDataMaxLengthJson, "key", "StopTxnSampledDataMaxLength");
            cJSON_AddBoolToObject(StopTxnSampledDataMaxLengthJson, "readonly", CPGetConfigurationResponse.StopTxnSampledDataMaxLengthReadOnly ? true : false);
            cJSON_AddStringToObject(StopTxnSampledDataMaxLengthJson, "value", CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, StopTxnSampledDataMaxLengthJson);

            //    Add SupportedFeatureProfilesMaxLength to configurationKeyJsonArray
            cJSON *SupportedFeatureProfilesMaxLengthJson = cJSON_CreateObject();
            cJSON_AddStringToObject(SupportedFeatureProfilesMaxLengthJson, "key", "SupportedFeatureProfilesMaxLength");
            cJSON_AddBoolToObject(SupportedFeatureProfilesMaxLengthJson, "readonly", CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthReadOnly ? true : false);
            cJSON_AddStringToObject(SupportedFeatureProfilesMaxLengthJson, "value", CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, SupportedFeatureProfilesMaxLengthJson);

            //    Add WebSocketPingInterval to configurationKeyJsonArray
            cJSON *WebSocketPingIntervalJson = cJSON_CreateObject();
            cJSON_AddStringToObject(WebSocketPingIntervalJson, "key", "WebSocketPingInterval");
            cJSON_AddBoolToObject(WebSocketPingIntervalJson, "readonly", CPGetConfigurationResponse.WebSocketPingIntervalReadOnly ? true : false);
            cJSON_AddStringToObject(WebSocketPingIntervalJson, "value", CPGetConfigurationResponse.WebSocketPingIntervalValue);
            cJSON_AddItemToArray(configurationKeyJsonArray, WebSocketPingIntervalJson);
            if (CPGetConfigurationResponse.SupportedFeatureProfiles.LocalAuthListManagement)
            {
                //    Add LocalAuthListEnabled to configurationKeyJsonArray
                cJSON *LocalAuthListEnabledJson = cJSON_CreateObject();
                cJSON_AddStringToObject(LocalAuthListEnabledJson, "key", "LocalAuthListEnabled");
                cJSON_AddBoolToObject(LocalAuthListEnabledJson, "readonly", CPGetConfigurationResponse.LocalAuthListEnabledReadOnly ? true : false);
                cJSON_AddStringToObject(LocalAuthListEnabledJson, "value", CPGetConfigurationResponse.LocalAuthListEnabledValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, LocalAuthListEnabledJson);

                //    Add LocalAuthListMaxLength to configurationKeyJsonArray
                cJSON *LocalAuthListMaxLengthJson = cJSON_CreateObject();
                cJSON_AddStringToObject(LocalAuthListMaxLengthJson, "key", "LocalAuthListMaxLength");
                cJSON_AddBoolToObject(LocalAuthListMaxLengthJson, "readonly", CPGetConfigurationResponse.LocalAuthListMaxLengthReadOnly ? true : false);
                cJSON_AddStringToObject(LocalAuthListMaxLengthJson, "value", CPGetConfigurationResponse.LocalAuthListMaxLengthValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, LocalAuthListMaxLengthJson);

                //    Add SendLocalListMaxLength to configurationKeyJsonArray
                cJSON *SendLocalListMaxLengthJson = cJSON_CreateObject();
                cJSON_AddStringToObject(SendLocalListMaxLengthJson, "key", "SendLocalListMaxLength");
                cJSON_AddBoolToObject(SendLocalListMaxLengthJson, "readonly", CPGetConfigurationResponse.SendLocalListMaxLengthReadOnly ? true : false);
                cJSON_AddStringToObject(SendLocalListMaxLengthJson, "value", CPGetConfigurationResponse.SendLocalListMaxLengthValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, SendLocalListMaxLengthJson);
            }
            if (CPGetConfigurationResponse.SupportedFeatureProfiles.Reservation)
            {
                //    Add ReserveConnectorZeroSupported to configurationKeyJsonArray
                cJSON *ReserveConnectorZeroSupportedJson = cJSON_CreateObject();
                cJSON_AddStringToObject(ReserveConnectorZeroSupportedJson, "key", "ReserveConnectorZeroSupported");
                cJSON_AddBoolToObject(ReserveConnectorZeroSupportedJson, "readonly", CPGetConfigurationResponse.ReserveConnectorZeroSupportedReadOnly ? true : false);
                cJSON_AddStringToObject(ReserveConnectorZeroSupportedJson, "value", CPGetConfigurationResponse.ReserveConnectorZeroSupportedValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, ReserveConnectorZeroSupportedJson);
            }
            if (CPGetConfigurationResponse.SupportedFeatureProfiles.SmartCharging)
            {
                //    Add ChargeProfileMaxStackLevel to configurationKeyJsonArray
                cJSON *ChargeProfileMaxStackLevelJson = cJSON_CreateObject();
                cJSON_AddStringToObject(ChargeProfileMaxStackLevelJson, "key", "ChargeProfileMaxStackLevel");
                cJSON_AddBoolToObject(ChargeProfileMaxStackLevelJson, "readonly", CPGetConfigurationResponse.ChargeProfileMaxStackLevelReadOnly ? true : false);
                cJSON_AddStringToObject(ChargeProfileMaxStackLevelJson, "value", CPGetConfigurationResponse.ChargeProfileMaxStackLevelValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, ChargeProfileMaxStackLevelJson);

                //    Add ChargingScheduleAllowedChargingRateUnit to configurationKeyJsonArray
                cJSON *ChargingScheduleAllowedChargingRateUnitJson = cJSON_CreateObject();
                cJSON_AddStringToObject(ChargingScheduleAllowedChargingRateUnitJson, "key", "ChargingScheduleAllowedChargingRateUnit");
                cJSON_AddBoolToObject(ChargingScheduleAllowedChargingRateUnitJson, "readonly", CPGetConfigurationResponse.ChargingScheduleAllowedChargingRateUnitReadOnly ? true : false);
                cJSON_AddStringToObject(ChargingScheduleAllowedChargingRateUnitJson, "value", CPGetConfigurationResponse.ChargingScheduleAllowedChargingRateUnitValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, ChargingScheduleAllowedChargingRateUnitJson);

                //    Add ChargingScheduleMaxPeriods to configurationKeyJsonArray
                cJSON *ChargingScheduleMaxPeriodsJson = cJSON_CreateObject();
                cJSON_AddStringToObject(ChargingScheduleMaxPeriodsJson, "key", "ChargingScheduleMaxPeriods");
                cJSON_AddBoolToObject(ChargingScheduleMaxPeriodsJson, "readonly", CPGetConfigurationResponse.ChargingScheduleMaxPeriodsReadOnly ? true : false);
                cJSON_AddStringToObject(ChargingScheduleMaxPeriodsJson, "value", CPGetConfigurationResponse.ChargingScheduleMaxPeriodsValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, ChargingScheduleMaxPeriodsJson);

                //    Add ConnectorSwitch3to1PhaseSupported to configurationKeyJsonArray
                cJSON *ConnectorSwitch3to1PhaseSupportedJson = cJSON_CreateObject();
                cJSON_AddStringToObject(ConnectorSwitch3to1PhaseSupportedJson, "key", "ConnectorSwitch3to1PhaseSupported");
                cJSON_AddBoolToObject(ConnectorSwitch3to1PhaseSupportedJson, "readonly", CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedReadOnly ? true : false);
                cJSON_AddStringToObject(ConnectorSwitch3to1PhaseSupportedJson, "value", CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, ConnectorSwitch3to1PhaseSupportedJson);

                //    Add MaxChargingProfilesInstalled to configurationKeyJsonArray
                cJSON *MaxChargingProfilesInstalledJson = cJSON_CreateObject();
                cJSON_AddStringToObject(MaxChargingProfilesInstalledJson, "key", "MaxChargingProfilesInstalled");
                cJSON_AddBoolToObject(MaxChargingProfilesInstalledJson, "readonly", CPGetConfigurationResponse.MaxChargingProfilesInstalledReadOnly ? true : false);
                cJSON_AddStringToObject(MaxChargingProfilesInstalledJson, "value", CPGetConfigurationResponse.MaxChargingProfilesInstalledValue);
                cJSON_AddItemToArray(configurationKeyJsonArray, MaxChargingProfilesInstalledJson);
            }
        }

        cJSON_AddItemToObject(configurationJson, "configurationKey", configurationKeyJsonArray);

        cJSON *unknownKeyJsonArray = cJSON_CreateArray();
        cJSON_AddItemToObject(configurationJson, "unknownKey", unknownKeyJsonArray);

        cJSON_AddItemToArray(jsonArray, configurationJson);

        char *json_string = cJSON_Print(jsonArray);
        if (json_string != NULL)
        {
            SendData(json_string);
            cJSON_free(json_string);
        }
        cJSON_Delete(jsonArray);
    }

    void OCPPController::sendGetConfigurationResponse(void)
    {
        sendGetConfigurationResponseKeys(1);
        sendGetConfigurationResponseKeys(2);
        sendGetConfigurationResponseKeys(3);
    }

    void OCPPController::BootNotificationResponseProcess(cJSON *bootResponse)
    {
        bool isBootStatusReceived = false;
        bool isBootcurrentTimeReceived = false;
        bool isBootintervalReceived = false;
        // bootNotificationResponseReceived = true;
        cJSON *status = cJSON_GetObjectItem(bootResponse, "status");
        if (cJSON_IsString(status))
        {
            isBootStatusReceived = true;
            setNULL(CMSBootNotificationResponse.status);
            memcpy(CMSBootNotificationResponse.status, status->valuestring, strlen(status->valuestring));
        }
        cJSON *currentTime = cJSON_GetObjectItem(bootResponse, "currentTime");
        if (cJSON_IsString(currentTime))
        {
            isBootcurrentTimeReceived = true;
            setNULL(CMSBootNotificationResponse.currentTime);
            memcpy(CMSBootNotificationResponse.currentTime, currentTime->valuestring, strlen(currentTime->valuestring));
        }
        cJSON *interval = cJSON_GetObjectItem(bootResponse, "interval");
        if (cJSON_IsNumber(interval))
        {
            isBootintervalReceived = true;
            CMSBootNotificationResponse.interval = interval->valueint;
        }

        CMSBootNotificationResponse.Received = isBootStatusReceived & isBootcurrentTimeReceived & isBootintervalReceived;
        if (CMSBootNotificationResponse.Received)
        {
            CPGetConfigurationResponse.HeartbeatInterval = CMSBootNotificationResponse.interval;
            setNULL(CPGetConfigurationResponse.HeartbeatIntervalValue);
            sprintf(CPGetConfigurationResponse.HeartbeatIntervalValue, "%ld", CPGetConfigurationResponse.HeartbeatInterval);
        }
    }

    void OCPPController::AuthorizationResponseProcess(cJSON *authorizationResponse, uint8_t connId)
    {
        bool isidTagInfoReceived = false;
        bool isidTagInfoStatusReceived = false;

        cJSON *idTagInfo = cJSON_GetObjectItem(authorizationResponse, "idTagInfo");
        if (cJSON_IsObject(idTagInfo))
        {
            isidTagInfoReceived = true;
        }
        cJSON *status = cJSON_GetObjectItem(idTagInfo, "status");
        if (cJSON_IsString(status))
        {
            isidTagInfoStatusReceived = true;
            setNULL(CMSAuthorizeResponse[connId].idtaginfo.status);
            memcpy(CMSAuthorizeResponse[connId].idtaginfo.status, status->valuestring, strlen(status->valuestring));
        }
        cJSON *expiryDate = cJSON_GetObjectItem(idTagInfo, "expiryDate");
        if (cJSON_IsString(expiryDate))
        {
            CMSAuthorizeResponse[connId].idtaginfo.expiryDateReceived = true;
            setNULL(CMSAuthorizeResponse[connId].idtaginfo.expiryDate);
            memcpy(CMSAuthorizeResponse[connId].idtaginfo.expiryDate, expiryDate->valuestring, strlen(expiryDate->valuestring));
        }
        else
        {
            CMSAuthorizeResponse[connId].idtaginfo.expiryDateReceived = false;
        }
        cJSON *parentidTag = cJSON_GetObjectItem(idTagInfo, "parentidTag");
        if (cJSON_IsString(parentidTag))
        {
            CMSAuthorizeResponse[connId].idtaginfo.parentidTagReceived = true;
            setNULL(CMSAuthorizeResponse[connId].idtaginfo.parentidTag);
            memcpy(CMSAuthorizeResponse[connId].idtaginfo.parentidTag, parentidTag->valuestring, strlen(parentidTag->valuestring));
        }
        else
        {
            CMSAuthorizeResponse[connId].idtaginfo.parentidTagReceived = false;
        }

        CMSAuthorizeResponse[connId].Received = isidTagInfoReceived & isidTagInfoStatusReceived;
    }

    void OCPPController::CancelReservationResponseProcess(cJSON *CancelReservationResponseV)
    {
        bool isCancelReservationStatusReceived = false;
        cJSON *status = cJSON_GetObjectItem(CancelReservationResponseV, "status");
        if (cJSON_IsString(status))
        {
            isCancelReservationStatusReceived = true;
            setNULL(CPCancelReservationResponse.status);
            memcpy(CPCancelReservationResponse.status, status->valuestring, strlen(status->valuestring));
        }

        CPCancelReservationResponse.Sent = isCancelReservationStatusReceived;
    }

    void OCPPController::ChangeConfigurationResponseProcess(cJSON *ChangeConfigurationResponseV)
    {
        bool isChangeConfigurationStatusReceived = false;

        cJSON *status = cJSON_GetObjectItem(ChangeConfigurationResponseV, "status");
        if (cJSON_IsString(status))
        {
            isChangeConfigurationStatusReceived = true;
            setNULL(CPChangeConfigurationResponse.status);
            memcpy(CPChangeConfigurationResponse.status, status->valuestring, strlen(status->valuestring));
        }
        CPChangeConfigurationResponse.Sent = isChangeConfigurationStatusReceived;
    }

    void OCPPController::ClearCacheResponseProcess(cJSON *ClearCacheResponseV)
    {
        bool isClearCacheResponseStatusReceived = false;

        cJSON *status = cJSON_GetObjectItem(ClearCacheResponseV, "status");
        if (cJSON_IsString(status))
        {
            isClearCacheResponseStatusReceived = true;
            setNULL(CPClearCacheResponse.status);
            memcpy(CPClearCacheResponse.status, status->valuestring, strlen(status->valuestring));
        }
        CPClearCacheResponse.Sent = isClearCacheResponseStatusReceived;
    }

    void OCPPController::HeartbeatResponseProcess(cJSON *HeartbeatResponseV)
    {
        bool isHeartbeatResponseStatusReceived = false;

        cJSON *currentTime = cJSON_GetObjectItem(HeartbeatResponseV, "currentTime");
        if (cJSON_IsString(currentTime))
        {
            isHeartbeatResponseStatusReceived = true;
            setNULL(CMSHeartbeatResponse.currentTime);
            memcpy(CMSHeartbeatResponse.currentTime, currentTime->valuestring, strlen(currentTime->valuestring));
        }
        CMSHeartbeatResponse.Received = isHeartbeatResponseStatusReceived;
    }

    void OCPPController::ResetResponseProcess(cJSON *ResetResponseV)
    {
        bool isResetResponseStatusReceived = false;

        cJSON *status = cJSON_GetObjectItem(ResetResponseV, "status");
        if (cJSON_IsString(status))
        {
            isResetResponseStatusReceived = true;
            setNULL(CPResetResponse.status);
            memcpy(CPResetResponse.status, status->valuestring, strlen(status->valuestring));
        }
        CPResetResponse.Sent = isResetResponseStatusReceived;
    }

    void OCPPController::StartTransactionResponseProcess(cJSON *StartTransactionResponseV, uint8_t connId)
    {
        bool isidTagInfoStartTransReceived = false;
        bool isidTagInfoStartTransStatusReceived = false;
        bool isStartTranstransactionIdReceived = false;

        cJSON *idTagInfo = cJSON_GetObjectItem(StartTransactionResponseV, "idTagInfo");
        if (cJSON_IsObject(idTagInfo))
        {
            isidTagInfoStartTransReceived = true;
        }

        cJSON *status = cJSON_GetObjectItem(idTagInfo, "status");
        if (cJSON_IsString(status))
        {
            isidTagInfoStartTransStatusReceived = true;
            setNULL(CMSStartTransactionResponse[connId].idtaginfo.status);
            memcpy(CMSStartTransactionResponse[connId].idtaginfo.status, status->valuestring, strlen(status->valuestring));
        }
        cJSON *expiryDate = cJSON_GetObjectItem(idTagInfo, "expiryDate");
        if (cJSON_IsString(expiryDate))
        {
            CMSStartTransactionResponse[connId].idtaginfo.expiryDateReceived = true;
            setNULL(CMSStartTransactionResponse[connId].idtaginfo.expiryDate);
            memcpy(CMSStartTransactionResponse[connId].idtaginfo.expiryDate, expiryDate->valuestring, strlen(expiryDate->valuestring));
        }
        else
        {
            CMSStartTransactionResponse[connId].idtaginfo.expiryDateReceived = false;
        }
        cJSON *parentidTag = cJSON_GetObjectItem(idTagInfo, "parentidTag");
        if (cJSON_IsString(parentidTag))
        {
            CMSStartTransactionResponse[connId].idtaginfo.parentidTagReceived = true;
            setNULL(CMSStartTransactionResponse[connId].idtaginfo.parentidTag);
            memcpy(CMSStartTransactionResponse[connId].idtaginfo.parentidTag, parentidTag->valuestring, strlen(parentidTag->valuestring));
        }
        else
        {
            CMSStartTransactionResponse[connId].idtaginfo.parentidTagReceived = false;
        }
        cJSON *transactionId = cJSON_GetObjectItem(StartTransactionResponseV, "transactionId");
        if (cJSON_IsNumber(transactionId))
        {
            isStartTranstransactionIdReceived = true;
            CMSStartTransactionResponse[connId].transactionId = transactionId->valueint;
        }

        CMSStartTransactionResponse[connId].Received = isidTagInfoStartTransReceived && isidTagInfoStartTransStatusReceived && isStartTranstransactionIdReceived;
    }

    void OCPPController::StopTransactionResponseProcess(cJSON *StopTransactionResponseV, uint8_t connId)
    {
        bool isidTagInfoStopTReceived = false;
        bool isidTagInfoStopTStatusReceived = false;

        cJSON *idTagInfo = cJSON_GetObjectItem(StopTransactionResponseV, "idTagInfo");
        if (cJSON_IsObject(idTagInfo))
        {
            isidTagInfoStopTReceived = true;
        }
        cJSON *status = cJSON_GetObjectItem(idTagInfo, "status");
        if (cJSON_IsString(status))
        {
            isidTagInfoStopTStatusReceived = true;
            setNULL(CMSStopTransactionResponse[connId].idtaginfo.status);
            memcpy(CMSStopTransactionResponse[connId].idtaginfo.status, status->valuestring, strlen(status->valuestring));
        }
        cJSON *expiryDate = cJSON_GetObjectItem(idTagInfo, "expiryDate");
        if (cJSON_IsString(expiryDate))
        {
            CMSStopTransactionResponse[connId].idtaginfo.expiryDateReceived = true;
            setNULL(CMSStopTransactionResponse[connId].idtaginfo.expiryDate);
            memcpy(CMSStopTransactionResponse[connId].idtaginfo.expiryDate, expiryDate->valuestring, strlen(expiryDate->valuestring));
        }
        else
        {
            CMSStopTransactionResponse[connId].idtaginfo.expiryDateReceived = false;
        }
        cJSON *parentidTag = cJSON_GetObjectItem(idTagInfo, "parentidTag");
        if (cJSON_IsString(parentidTag))
        {
            CMSStopTransactionResponse[connId].idtaginfo.parentidTagReceived = true;
            setNULL(CMSStopTransactionResponse[connId].idtaginfo.parentidTag);
            memcpy(CMSStopTransactionResponse[connId].idtaginfo.parentidTag, parentidTag->valuestring, strlen(parentidTag->valuestring));
        }
        else
        {
            CMSStopTransactionResponse[connId].idtaginfo.parentidTagReceived = false;
        }

        CMSStopTransactionResponse[connId].Received = isidTagInfoStopTReceived & isidTagInfoStopTStatusReceived;
    }

    void OCPPController::ocpp_response(const char *jsonString)
    {
        // Parse the JSON-formatted string
        cJSON *jsonArray = cJSON_Parse(jsonString);
        if (jsonArray != NULL)
        {
            cJSON *response = cJSON_GetArrayItem(jsonArray, 0);
            if (cJSON_IsNumber(response))
            {
                if (response->valueint == OCPP_REQUEST)
                {
                    cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
                    cJSON *ResponseMsg = cJSON_GetArrayItem(jsonArray, 2);
                    // cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);

                    if (ResponseMsg != NULL && cJSON_IsString(ResponseMsg))
                    {
                        const char *ResponseMsgStr = cJSON_GetStringValue(ResponseMsg);
                        if (strcmp(ResponseMsgStr, "CancelReservation") == 0)
                        {
                            CMSCancelReservationRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "ChangeAvailability") == 0)
                        {
                            CMSChangeAvailabilityRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "ChangeConfiguration") == 0)
                        {
                            CMSChangeConfigurationRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "ClearCache") == 0)
                        {
                            setNULL(CMSClearCacheRequest.UniqId);
                            memcpy(CMSClearCacheRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
                            CMSClearCacheRequest.Received = true;
                        }
                        else if (strcmp(ResponseMsgStr, "GetConfiguration") == 0)
                        {
                            setNULL(CMSGetConfigurationRequest.UniqId);
                            memcpy(CMSGetConfigurationRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
                            CMSGetConfigurationRequest.Received = true;
                        }
                        else if (strcmp(ResponseMsgStr, "RemoteStartTransaction") == 0)
                        {
                            CMSRemoteStartTransactionRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "RemoteStopTransaction") == 0)
                        {
                            CMSRemoteStopTransactionRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "Reset") == 0)
                        {
                            CMSResetRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "TriggerMessage") == 0)
                        {
                            CMSTriggerMessageRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "GetLocalListVersion") == 0)
                        {
                            setNULL(CMSGetLocalListVersionRequest.UniqId);
                            memcpy(CMSGetLocalListVersionRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
                            CMSGetLocalListVersionRequest.Received = true;
                        }
                        else if (strcmp(ResponseMsgStr, "SendLocalList") == 0)
                        {
                            CMSSendLocalListRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "ReserveNow") == 0)
                        {
                            CMSReserveNowRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "UpdateFirmware") == 0)
                        {
                            CMSUpdateFirmwareRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "ClearChargingProfile") == 0)
                        {
                            CMSClearChargingProfileRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "DataTransfer") == 0)
                        {
                            setNULL(CMSDataTransferRequest.UniqId);
                            memcpy(CMSDataTransferRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
                            CMSDataTransferRequest.Received = true;
                        }
                        else if (strcmp(ResponseMsgStr, "GetCompositeSchedule") == 0)
                        {
                            setNULL(CMSGetCompositeScheduleRequest.UniqId);
                            memcpy(CMSGetCompositeScheduleRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
                            CMSGetCompositeScheduleRequest.Received = true;
                        }
                        else if (strcmp(ResponseMsgStr, "GetDiagnostics") == 0)
                        {
                            CMSGetDiagnosticsRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "SetChargingProfile") == 0)
                        {
                            CMSSetChargingProfileRequestProcess(jsonArray);
                        }
                        else if (strcmp(ResponseMsgStr, "UnlockConnector") == 0)
                        {
                            setNULL(CMSUnlockConnectorRequest.UniqId);
                            memcpy(CMSUnlockConnectorRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
                            CMSUnlockConnectorRequest.Received = true;
                        }
                    }
                }
                else if (response->valueint == OCPP_RESPONSE)
                {
                    // memcpy(bootNotificationReqUniqId,"0123456789ABCDEF",16);
                    cJSON *ResponseId = cJSON_GetArrayItem(jsonArray, 1);
                    cJSON *Payload = cJSON_GetArrayItem(jsonArray, 2);
                    if (strcmp(ResponseId->valuestring, CPHeartbeatRequest.UniqId) == 0)
                    {
                        HeartbeatResponseProcess(Payload);
                    }
                    if (strcmp(ResponseId->valuestring, CPBootNotificationRequest.UniqId) == 0)
                    {
                        BootNotificationResponseProcess(Payload);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStatusNotificationRequest[0].UniqId) == 0)
                    {
                        CMSStatusNotificationResponse[0].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPStatusNotificationRequest[1].UniqId) == 0)
                    {
                        CMSStatusNotificationResponse[1].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPStatusNotificationRequest[2].UniqId) == 0)
                    {
                        CMSStatusNotificationResponse[2].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPStatusNotificationRequest[3].UniqId) == 0)
                    {
                        CMSStatusNotificationResponse[3].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPMeterValuesRequest[0].UniqId) == 0)
                    {
                        CMSMeterValuesResponse[0].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPMeterValuesRequest[1].UniqId) == 0)
                    {
                        CMSMeterValuesResponse[1].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPMeterValuesRequest[2].UniqId) == 0)
                    {
                        CMSMeterValuesResponse[2].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPMeterValuesRequest[3].UniqId) == 0)
                    {
                        CMSMeterValuesResponse[3].Received = true;
                    }
                    else if (strcmp(ResponseId->valuestring, CPAuthorizeRequest[0].UniqId) == 0)
                    {
                        AuthorizationResponseProcess(Payload, 0);
                    }
                    else if (strcmp(ResponseId->valuestring, CPAuthorizeRequest[1].UniqId) == 0)
                    {
                        AuthorizationResponseProcess(Payload, 1);
                    }
                    else if (strcmp(ResponseId->valuestring, CPAuthorizeRequest[2].UniqId) == 0)
                    {
                        AuthorizationResponseProcess(Payload, 2);
                    }
                    else if (strcmp(ResponseId->valuestring, CPAuthorizeRequest[3].UniqId) == 0)
                    {
                        AuthorizationResponseProcess(Payload, 3);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStartTransactionRequest[0].UniqId) == 0)
                    {
                        StartTransactionResponseProcess(Payload, 0);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStartTransactionRequest[1].UniqId) == 0)
                    {
                        StartTransactionResponseProcess(Payload, 1);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStartTransactionRequest[2].UniqId) == 0)
                    {
                        StartTransactionResponseProcess(Payload, 2);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStartTransactionRequest[3].UniqId) == 0)
                    {
                        StartTransactionResponseProcess(Payload, 3);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStopTransactionRequest[0].UniqId) == 0)
                    {
                        StopTransactionResponseProcess(Payload, 0);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStopTransactionRequest[1].UniqId) == 0)
                    {
                        StopTransactionResponseProcess(Payload, 1);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStopTransactionRequest[2].UniqId) == 0)
                    {
                        StopTransactionResponseProcess(Payload, 2);
                    }
                    else if (strcmp(ResponseId->valuestring, CPStopTransactionRequest[3].UniqId) == 0)
                    {
                        StopTransactionResponseProcess(Payload, 3);
                    }
                    else if (strcmp(ResponseId->valuestring, CPFirmwareStatusNotificationRequest.UniqId) == 0)
                    {
                        CMSFirmwareStatusNotificationResponse.Received = true;
                    }
                }
                else if (response->valueint == OCPP_ERROR)
                {
                }
                else
                {
                }
            }
        }
        else
        {
            ESP_LOGW(TAG, "Error string Cant parse json");
        }

        cJSON_Delete(jsonArray);
    }

    void OCPPController::CMSChangeAvailabilityRequestProcess(cJSON *jsonArray)
    {
        uint8_t connId = 0;
        bool isTypeReceived = false;
        bool isConnectorIdReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *connectorID = cJSON_GetObjectItem(Payload, "connectorId");
        cJSON *type = cJSON_GetObjectItem(Payload, "type");
        if (cJSON_IsNumber(connectorID))
        {
            isConnectorIdReceived = true;
            connId = (uint8_t)connectorID->valueint;
            setNULL(CMSChangeAvailabilityRequest[connId].UniqId);
            memcpy(CMSChangeAvailabilityRequest[connId].UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            if (cJSON_IsString(type))
            {
                isTypeReceived = true;
                setNULL(CMSChangeAvailabilityRequest[connId].type);
                memcpy(CMSChangeAvailabilityRequest[connId].type, type->valuestring, strlen(type->valuestring));
                CMSChangeAvailabilityRequest[connId].Received = isConnectorIdReceived & isTypeReceived;
            }
            else
            {
                CMSChangeAvailabilityRequest[connId].Received = false;
            }
        }
    }

    void OCPPController::CMSChangeConfigurationRequestProcess(cJSON *jsonArray)
    {
        bool iskeyReceived = false;
        bool isvalueReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *key = cJSON_GetObjectItem(Payload, "key");
        cJSON *value = cJSON_GetObjectItem(Payload, "value");
        if (cJSON_IsString(key))
        {
            iskeyReceived = true;
            setNULL(CMSChangeConfigurationRequest.UniqId);
            memcpy(CMSChangeConfigurationRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            setNULL(CMSChangeConfigurationRequest.key);
            memcpy(CMSChangeConfigurationRequest.key, key->valuestring, strlen(key->valuestring));
            if (cJSON_IsString(value))
            {
                isvalueReceived = true;
                setNULL(CMSChangeConfigurationRequest.value);
                memcpy(CMSChangeConfigurationRequest.value, value->valuestring, strlen(value->valuestring));
                CMSChangeConfigurationRequest.Received = iskeyReceived & isvalueReceived;
            }
            else
            {
                CMSChangeConfigurationRequest.Received = false;
            }
        }
    }

    void OCPPController::CMSCancelReservationRequestProcess(cJSON *jsonArray)
    {
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *reservationId = cJSON_GetObjectItem(Payload, "reservationId");
        if (cJSON_IsNumber(reservationId))
        {
            setNULL(CMSCancelReservationRequest.UniqId);
            memcpy(CMSCancelReservationRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            CMSCancelReservationRequest.reservationId = reservationId->valueint;
            CMSCancelReservationRequest.Received = true;
        }
        else
        {
            CMSCancelReservationRequest.Received = false;
        }
    }

    void OCPPController::CMSTriggerMessageRequestProcess(cJSON *jsonArray)
    {
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *requestedMessage = cJSON_GetObjectItem(Payload, "requestedMessage");
        cJSON *connectorId = cJSON_GetObjectItem(Payload, "connectorId");

        if (cJSON_IsString(requestedMessage))
        {
            setNULL(CMSTriggerMessageRequest.UniqId);
            memcpy(CMSTriggerMessageRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            setNULL(CMSTriggerMessageRequest.requestedMessage);
            memcpy(CMSTriggerMessageRequest.requestedMessage, requestedMessage->valuestring, strlen(requestedMessage->valuestring));
            if (strcmp(requestedMessage->valuestring, "BootNotification") == 0 ||
                strcmp(requestedMessage->valuestring, "DiagnosticsStatusNotification") == 0 ||
                strcmp(requestedMessage->valuestring, "FirmwareStatusNotification") == 0 ||
                strcmp(requestedMessage->valuestring, "Heartbeat") == 0)
            {
                CMSTriggerMessageRequest.Received = true;
            }
            else if (strcmp(requestedMessage->valuestring, "MeterValues") == 0 || strcmp(requestedMessage->valuestring, "StatusNotification") == 0)
            {
                if (cJSON_IsNumber(connectorId))
                {
                    CMSTriggerMessageRequest.Received = true;
                    CMSTriggerMessageRequest.connectorId = (uint8_t)connectorId->valueint;
                }
                else
                {
                    CMSTriggerMessageRequest.Received = false;
                }
            }
            else
            {
                CMSTriggerMessageRequest.Received = false;
            }
        }
    }

    void OCPPController::CMSSendLocalListRequestProcess(cJSON *jsonArray)
    {
        bool isSendLocalListMessageReceived = false;
        bool isListVersionReceived = false;
        bool isUpdateTypeReceived = false;
        bool isidTagInfoidTagReceived = true;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *listVersionJson = cJSON_GetObjectItem(Payload, "listVersion");
        cJSON *localAuthorizationListJson = cJSON_GetObjectItem(Payload, "localAuthorizationList");
        cJSON *updateTypeJson = cJSON_GetObjectItem(Payload, "updateType");
        ESP_LOGI(TAG, "Local list reuqest received");
        if (cJSON_IsString(ResponseMsgId))
        {
            ESP_LOGI(TAG, "Received UniqId from SendLocalListRequest: %s", ResponseMsgId->valuestring);
            setNULL(CMSSendLocalListRequest.UniqId);
            memcpy(CMSSendLocalListRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
        }
        if (cJSON_IsNumber(listVersionJson))
        {
            ESP_LOGI(TAG, "Received listVersion from SendLocalListRequest: %d", listVersionJson->valueint);
            isListVersionReceived = true;
            CMSSendLocalListRequest.listVersion = listVersionJson->valueint;
        }
        if (cJSON_IsString(updateTypeJson))
        {
            ESP_LOGI(TAG, "Received updateType from SendLocalListRequest: %s", updateTypeJson->valuestring);
            isUpdateTypeReceived = true;
            setNULL(CMSSendLocalListRequest.updateType);
            memcpy(CMSSendLocalListRequest.updateType, updateTypeJson->valuestring, strlen(updateTypeJson->valuestring));
        }
        if (memcmp(CMSSendLocalListRequest.updateType, "Full", strlen("Full")) == 0)
        {
            ESP_LOGI(TAG, "Received updateType as Full from SendLocalListRequest");
            if (cJSON_IsArray(localAuthorizationListJson))
            {
                ESP_LOGI(TAG, "Received localAuthorizationList as array from SendLocalListRequest");
                int numberOfItems = cJSON_GetArraySize(localAuthorizationListJson);
                if (numberOfItems > LOCAL_LIST_COUNT)
                {
                    ESP_LOGW(TAG, "Number of items in localAuthorizationList is more than the maximum supported. Truncating the items to %d", LOCAL_LIST_COUNT);
                    numberOfItems = LOCAL_LIST_COUNT;
                }
                ESP_LOGI(TAG, "Number of items in localAuthorizationList: %d", numberOfItems);
                int i = 0;
                cJSON *item;
                cJSON_ArrayForEach(item, localAuthorizationListJson)
                {
                    ESP_LOGI(TAG, "Item number: %d", i);
                    cJSON *idTag = cJSON_GetObjectItem(item, "idTag");
                    cJSON *idTagInfo = cJSON_GetObjectItem(item, "idTagInfo");
                    cJSON *expiryDate = cJSON_GetObjectItem(idTagInfo, "expiryDate");
                    cJSON *parentidTag = cJSON_GetObjectItem(idTagInfo, "parentidTag");
                    cJSON *status = cJSON_GetObjectItem(idTagInfo, "status");

                    if (i < LOCAL_LIST_COUNT)
                    {
                        if (cJSON_IsString(idTag))
                        {
                            isidTagInfoidTagReceived = isidTagInfoidTagReceived && true;
                            CMSSendLocalListRequest.localAuthorizationList[i].idTagPresent = true;
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTag);
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.expiryDate);
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.parentidTag);
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.status);
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTag, idTag->valuestring, strlen(idTag->valuestring));
                            ESP_LOGI(TAG, "Received idTag[%d] from SendLocalListRequest: %s", i, CMSSendLocalListRequest.localAuthorizationList[i].idTag);
                        }
                        else
                        {
                            isidTagInfoidTagReceived = false;
                            ESP_LOGW(TAG, "Received idTag[%d] from SendLocalListRequest is not a string", i);
                        }
                        if (cJSON_IsString(expiryDate))
                        {
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.expiryDate, expiryDate->valuestring, strlen(expiryDate->valuestring));
                            ESP_LOGI(TAG, "Received expiryDate[%d] from SendLocalListRequest: %s", i, CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.expiryDate);
                        }
                        if (cJSON_IsString(parentidTag))
                        {
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.parentidTag, parentidTag->valuestring, strlen(parentidTag->valuestring));
                            ESP_LOGI(TAG, "Received parentidTag[%d] from SendLocalListRequest: %s", i, CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.parentidTag);
                        }
                        if (cJSON_IsString(status))
                        {
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.status, status->valuestring, strlen(status->valuestring));
                            ESP_LOGI(TAG, "Received status[%d] from SendLocalListRequest: %s", i, CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.status);
                        }
                    }
                    i++;
                }
                ESP_LOGI(TAG, "localAuthorizationList size = %d", i);
                for (int k = i; k < LOCAL_LIST_COUNT; k++)
                {
                    CMSSendLocalListRequest.localAuthorizationList[k].idTagPresent = false;
                }
                isSendLocalListMessageReceived = isListVersionReceived & isUpdateTypeReceived & isidTagInfoidTagReceived;
            }
            else
            {
                ESP_LOGI(TAG, "Received localAuthorizationList is not a JSON Array");
                isSendLocalListMessageReceived = isListVersionReceived & isUpdateTypeReceived;
                if (isSendLocalListMessageReceived)
                {
                    for (uint8_t index = 0; index < LOCAL_LIST_COUNT; index++)
                    {
                        CMSSendLocalListRequest.localAuthorizationList[index].idTagPresent = false;
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTag);
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTagInfo.expiryDate);
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTagInfo.parentidTag);
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTagInfo.status);
                    }
                    CMSSendLocalListRequest.Received = true;
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to receive SendLocalListRequest, isListVersionReceived = %d, isUpdateTypeReceived = %d", isListVersionReceived, isUpdateTypeReceived);
                }
            }
        }
        else if (memcmp(CMSSendLocalListRequest.updateType, "Differential", strlen("Differential")) == 0)
        {
            if (cJSON_IsArray(localAuthorizationListJson))
            {
                int numberOfItems = cJSON_GetArraySize(localAuthorizationListJson);
                int existingItems = 0;
                for (int j = 0; j < LOCAL_LIST_COUNT; j++)
                {
                    if (CMSSendLocalListRequest.localAuthorizationList[j].idTagPresent)
                    {
                        existingItems++;
                    }
                }
                if (numberOfItems > (LOCAL_LIST_COUNT - existingItems))
                {
                    numberOfItems = (LOCAL_LIST_COUNT - existingItems);
                }
                int i = existingItems;
                cJSON *item;
                cJSON_ArrayForEach(item, localAuthorizationListJson)
                {
                    cJSON *idTag = cJSON_GetObjectItem(item, "idTag");
                    cJSON *idTagInfo = cJSON_GetObjectItem(item, "idTagInfo");
                    cJSON *expiryDate = cJSON_GetObjectItem(idTagInfo, "expiryDate");
                    cJSON *parentidTag = cJSON_GetObjectItem(idTagInfo, "parentidTag");
                    cJSON *status = cJSON_GetObjectItem(idTagInfo, "status");

                    if (i < (LOCAL_LIST_COUNT - existingItems))
                    {
                        if (cJSON_IsString(idTag))
                        {
                            isidTagInfoidTagReceived = isidTagInfoidTagReceived && true;
                            CMSSendLocalListRequest.localAuthorizationList[i].idTagPresent = true;
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTag);
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.expiryDate);
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.parentidTag);
                            setNULL(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.status);
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTag, idTag->valuestring, strlen(idTag->valuestring));
                        }
                        else
                        {
                            isidTagInfoidTagReceived = false;
                        }
                        if (cJSON_IsString(expiryDate))
                        {
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.expiryDate, expiryDate->valuestring, strlen(expiryDate->valuestring));
                        }
                        if (cJSON_IsString(parentidTag))
                        {
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.parentidTag, parentidTag->valuestring, strlen(parentidTag->valuestring));
                        }
                        if (cJSON_IsString(status))
                        {
                            memcpy(CMSSendLocalListRequest.localAuthorizationList[i].idTagInfo.status, status->valuestring, strlen(status->valuestring));
                        }
                    }
                    i++;
                }
                for (int k = i; k < LOCAL_LIST_COUNT; k++)
                {
                    CMSSendLocalListRequest.localAuthorizationList[k].idTagPresent = false;
                }
                isSendLocalListMessageReceived = isListVersionReceived & isUpdateTypeReceived & isidTagInfoidTagReceived;
            }
            else
            {
                isSendLocalListMessageReceived = isListVersionReceived & isUpdateTypeReceived;
                if (isSendLocalListMessageReceived)
                {
                    for (uint8_t index = 0; index < LOCAL_LIST_COUNT; index++)
                    {
                        CMSSendLocalListRequest.localAuthorizationList[index].idTagPresent = false;
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTag);
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTagInfo.expiryDate);
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTagInfo.parentidTag);
                        setNULL(CMSSendLocalListRequest.localAuthorizationList[index].idTagInfo.status);
                    }
                    CMSSendLocalListRequest.Received = true;
                }
            }
        }
        CMSSendLocalListRequest.Received = isSendLocalListMessageReceived;
        ESP_LOGI(TAG, "Send Local List Response : %d", CMSSendLocalListRequest.Received);
    }

    void OCPPController::CMSRemoteStartTransactionRequestProcess(cJSON *jsonArray)
    {
        uint8_t connId = 0;
        bool isidTagReceived = false;
        bool iscsChargingProfileReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *connectorID = cJSON_GetObjectItem(Payload, "connectorId");
        cJSON *idTag = cJSON_GetObjectItem(Payload, "idTag");
        cJSON *csChargingProfiles = cJSON_GetObjectItem(Payload, "chargingProfile");

        if (cJSON_IsNumber(connectorID))
        {
            if (connectorID->valueint < OCPPModule::Constants::MAX_MODULES)
            {
                connId = (uint8_t)connectorID->valueint;
                CMSRemoteStartTransactionRequest[connectorID->valueint].isConnectorIdReceived = true;
                CMSRemoteStartTransactionRequest[connectorID->valueint].connectorId = connectorID->valueint;
                if (cJSON_IsObject(csChargingProfiles))
                {
                    ChargingProfiles_t ChargingProfile;
                    memset(&ChargingProfile, 0, sizeof(ChargingProfile));
                    ChargingProfile.connectorId = connectorID->valueint;
                    // iscsChargingProfileReceived = WriteChargingProfileToFlash(ChargingProfile, csChargingProfiles);
                }
                else
                {
                    iscsChargingProfileReceived = true;
                }
            }
        }
        else
        {
            connId = 0;
            CMSRemoteStartTransactionRequest[0].isConnectorIdReceived = true;
        }
        setNULL(CMSRemoteStartTransactionRequest[connId].UniqId);
        if (cJSON_IsString(ResponseMsgId))
        {
            memcpy(CMSRemoteStartTransactionRequest[connId].UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            if (cJSON_IsString(idTag))
            {
                isidTagReceived = true;
                setNULL(CMSRemoteStartTransactionRequest[connId].idTag);
                memcpy(CMSRemoteStartTransactionRequest[connId].idTag, idTag->valuestring, strlen(idTag->valuestring));
                CMSRemoteStartTransactionRequest[connId].Received = isidTagReceived && iscsChargingProfileReceived;
            }
            else
            {
                CMSRemoteStartTransactionRequest[connId].Received = false;
            }
        }
    }

    void OCPPController::CMSRemoteStopTransactionRequestProcess(cJSON *jsonArray)
    {
        bool istransactionIdReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *transactionId = cJSON_GetObjectItem(Payload, "transactionId");
        if (cJSON_IsNumber(transactionId))
        {
            istransactionIdReceived = true;
            setNULL(CMSRemoteStopTransactionRequest.UniqId);
            memcpy(CMSRemoteStopTransactionRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            CMSRemoteStopTransactionRequest.transactionId = transactionId->valueint;
            CMSRemoteStopTransactionRequest.Received = istransactionIdReceived;
        }
        else
        {
            CMSRemoteStopTransactionRequest.Received = false;
        }
    }

    void OCPPController::CMSReserveNowRequestProcess(cJSON *jsonArray)
    {
        uint8_t connId = 0;
        bool isidTagReceived = false;
        bool isConnectorIdReceived = false;
        bool isexpiryDateReceived = false;
        bool isreservationIdReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *connectorID = cJSON_GetObjectItem(Payload, "connectorId");
        cJSON *expiryDate = cJSON_GetObjectItem(Payload, "expiryDate");
        cJSON *idTag = cJSON_GetObjectItem(Payload, "idTag");
        cJSON *parentidTag = cJSON_GetObjectItem(Payload, "parentidTag");
        cJSON *reservationId = cJSON_GetObjectItem(Payload, "reservationId");
        if (cJSON_IsNumber(connectorID))
        {
            isConnectorIdReceived = true;
            connId = (uint8_t)connectorID->valueint;
            CMSReserveNowRequest[connId].connectorId = connectorID->valueint;
            setNULL(CMSReserveNowRequest[connId].UniqId);
            memcpy(CMSReserveNowRequest[connId].UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            if (cJSON_IsString(expiryDate))
            {
                isexpiryDateReceived = true;
                setNULL(CMSReserveNowRequest[connId].expiryDate);
                memcpy(CMSReserveNowRequest[connId].expiryDate, expiryDate->valuestring, strlen(expiryDate->valuestring));
            }
            if (cJSON_IsString(idTag))
            {
                isidTagReceived = true;
                setNULL(CMSReserveNowRequest[connId].idTag);
                memcpy(CMSReserveNowRequest[connId].idTag, idTag->valuestring, strlen(idTag->valuestring));
            }
            if (cJSON_IsString(parentidTag))
            {
                CMSReserveNowRequest[connId].isparentidTagReceived = true;
                setNULL(CMSReserveNowRequest[connId].parentidTag);
                memcpy(CMSReserveNowRequest[connId].parentidTag, parentidTag->valuestring, strlen(parentidTag->valuestring));
            }
            else
            {
                CMSReserveNowRequest[connId].isparentidTagReceived = false;
                setNULL(CMSReserveNowRequest[connId].parentidTag);
            }
            if (cJSON_IsNumber(reservationId))
            {
                isreservationIdReceived = true;
                CMSReserveNowRequest[connId].reservationId = reservationId->valueint;
            }
            CMSReserveNowRequest[connId].Received = isConnectorIdReceived & isexpiryDateReceived & isidTagReceived & isreservationIdReceived;
        }
    }

    void OCPPController::CMSResetRequestProcess(cJSON *jsonArray)
    {
        bool istypeReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *type = cJSON_GetObjectItem(Payload, "type");
        if (cJSON_IsString(type))
        {
            istypeReceived = true;
            setNULL(CMSResetRequest.UniqId);
            memcpy(CMSResetRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            setNULL(CMSResetRequest.type);
            memcpy(CMSResetRequest.type, type->valuestring, strlen(type->valuestring));
            CMSResetRequest.Received = istypeReceived;
        }
        else
        {
            CMSResetRequest.Received = false;
        }
    }

    void OCPPController::CMSUpdateFirmwareRequestProcess(cJSON *jsonArray)
    {
        bool islocationReceived = false;
        bool isretrieveDateReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *location = cJSON_GetObjectItem(Payload, "location");
        cJSON *retrieveDate = cJSON_GetObjectItem(Payload, "retrieveDate");
        if (cJSON_IsString(location))
        {
            islocationReceived = true;
        }
        if (cJSON_IsString(retrieveDate))
        {
            isretrieveDateReceived = true;
        }
        if (cJSON_IsString(ResponseMsgId))
        {
            setNULL(CMSUpdateFirmwareRequest.UniqId);
            memcpy(CMSUpdateFirmwareRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            CMSUpdateFirmwareRequest.Received = islocationReceived & isretrieveDateReceived;
        }
        else
        {
            CMSUpdateFirmwareRequest.Received = false;
        }
    }

    void OCPPController::CMSGetDiagnosticsRequestProcess(cJSON *jsonArray)
    {
        bool islocationReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *location = cJSON_GetObjectItem(Payload, "location");
        if (cJSON_IsString(location))
        {
            islocationReceived = true;
            setNULL(CMSGetDiagnosticsRequest.location);
            memcpy(CMSGetDiagnosticsRequest.location, location->valuestring, strlen(location->valuestring));
        }
        if (cJSON_IsString(ResponseMsgId))
        {
            setNULL(CMSGetDiagnosticsRequest.UniqId);
            memcpy(CMSGetDiagnosticsRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            CMSGetDiagnosticsRequest.Received = islocationReceived;
        }
    }

    void OCPPController::CMSClearChargingProfileRequestProcess(cJSON *jsonArray)
    {
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *id = cJSON_GetObjectItem(Payload, "id");
        cJSON *connectorId = cJSON_GetObjectItem(Payload, "connectorId");
        cJSON *chargingProfilePurpose = cJSON_GetObjectItem(Payload, "chargingProfilePurpose");
        cJSON *stackLevel = cJSON_GetObjectItem(Payload, "stackLevel");

        if (cJSON_IsNumber(id))
        {
            CMSClearChargingProfileRequest.id = id->valueint;
        }
        if (cJSON_IsNumber(connectorId))
        {
            CMSClearChargingProfileRequest.connectorId = connectorId->valueint;
        }
        if (cJSON_IsString(chargingProfilePurpose))
        {
            if (memcmp(chargingProfilePurpose->valuestring, "ChargePointMaxProfile", strlen(chargingProfilePurpose->valuestring)) == 0)
            {
                CMSClearChargingProfileRequest.chargingProfilePurpose = static_cast<uint8_t>(OCPPModule::ProfileType::ChargePointMaxProfile);
            }
            else if (memcmp(chargingProfilePurpose->valuestring, "TxProfile", strlen(chargingProfilePurpose->valuestring)) == 0)
            {
                CMSClearChargingProfileRequest.chargingProfilePurpose = static_cast<uint8_t>(OCPPModule::ProfileType::TxProfile);
            }
            else if (memcmp(chargingProfilePurpose->valuestring, "TxDefaultProfile", strlen(chargingProfilePurpose->valuestring)) == 0)
            {
                CMSClearChargingProfileRequest.chargingProfilePurpose = static_cast<uint8_t>(OCPPModule::ProfileType::TxDefaultProfile);
            }
            else
            {
                CMSClearChargingProfileRequest.chargingProfilePurpose = 0;
            }
        }
        if (cJSON_IsNumber(stackLevel))
        {
            CMSClearChargingProfileRequest.stackLevel = stackLevel->valueint;
        }

        if (cJSON_IsString(ResponseMsgId))
        {
            setNULL(CMSClearChargingProfileRequest.UniqId);
            memcpy(CMSClearChargingProfileRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            CMSClearChargingProfileRequest.Received = true;
        }
    }

    void OCPPController::CMSSetChargingProfileRequestProcess(cJSON *jsonArray)
    {
        ChargingProfiles_t ChargingProfile;
        memset(&ChargingProfile, 0, sizeof(ChargingProfile));
        bool isconnectorIdReceived = false;
        bool iscsChargingProfilesReceived = false;
        cJSON *ResponseMsgId = cJSON_GetArrayItem(jsonArray, 1);
        cJSON *Payload = cJSON_GetArrayItem(jsonArray, 3);
        cJSON *connectorId = cJSON_GetObjectItem(Payload, "connectorId");
        // cJSON *csChargingProfiles = cJSON_GetObjectItem(Payload, "csChargingProfiles");

        if (cJSON_IsString(ResponseMsgId))
        {
            setNULL(CMSSetChargingProfileRequest.UniqId);
            memcpy(CMSSetChargingProfileRequest.UniqId, ResponseMsgId->valuestring, strlen(ResponseMsgId->valuestring));
            if (cJSON_IsNumber(connectorId))
            {
                isconnectorIdReceived = true;
                ChargingProfile.connectorId = connectorId->valueint;
                // iscsChargingProfilesReceived = WriteChargingProfileToFlash(ChargingProfile, csChargingProfiles);
            }
            CMSSetChargingProfileRequest.Received = isconnectorIdReceived && iscsChargingProfilesReceived;
        }
    }

    bool OCPPController::rfidMatchingLocalList(char *idTag)
    {
        bool matching = false;
        uint8_t i = 0;
        for (i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            if ((strncmp(idTag, CMSSendLocalListRequest.localAuthorizationList[i].idTag, strlen(idTag)) == 0) && CMSSendLocalListRequest.localAuthorizationList[i].idTagPresent)
            {
                i = LOCAL_LIST_COUNT;
                matching = true;
            }
        }
        return matching;
    }
    bool OCPPController::rfidMatchingAuthorizationCache(char *idTag)
    {
        bool matching = false;
        uint8_t i = 0;
        for (i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            if ((strncmp(idTag, LocalAuthorizationList.idTag[i], strlen(idTag)) == 0) && (LocalAuthorizationList.idTagPresent[i]))
            {
                i = LOCAL_LIST_COUNT;
                matching = true;
            }
        }
        return matching;
    }
    bool OCPPController::saveTagToLocalAuthenticationList(char *idTag)
    {
        uint32_t i = 0;
        bool isTagAlreadyPresent = false;
        for (i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            if ((strcmp(LocalAuthorizationList.idTag[i], idTag) == 0) && (LocalAuthorizationList.idTagPresent[i] == true))
            {
                isTagAlreadyPresent = true;
                i = LOCAL_LIST_COUNT;
            }
        }
        if (isTagAlreadyPresent == false)
        {
            for (i = 0; i < LOCAL_LIST_COUNT; i++)
            {
                if (LocalAuthorizationList.idTagPresent[i] == false)
                {
                    setNULL(LocalAuthorizationList.idTag[i]);
                    memcpy(LocalAuthorizationList.idTag[i], idTag, strlen(idTag));
                    LocalAuthorizationList.idTagPresent[i] = true;
                    i = LOCAL_LIST_COUNT;
                }
            }
        }
        write_LocalAuthorizationList();
        ESP_LOGI(TAG, "Printing Local Authentication list");
        // log localAuthentication list
        std::memset(&LocalAuthorizationList, 0, sizeof(LocalAuthorizationList));
        for (i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            if (LocalAuthorizationList.idTagPresent[i])
            {
                ESP_LOGI(TAG, "Local Authentication Tag%ld : %s", i, LocalAuthorizationList.idTag[i]);
            }
        }
        return true;
    }

    esp_err_t OCPPController::read_config_ocpp(void)
    {
        size_t bufferSize = 8192;
        char *data = (char *)malloc(bufferSize);
        if (data == NULL)
            return ESP_FAIL;

        memset(data, 0, bufferSize);

        esp_err_t err = storage->read_config_ocpp(data, bufferSize);
        if (err != ESP_OK)
        {
            free(data);
            return err;
        }

        cJSON *json = cJSON_Parse(data);
        free(data);

        if (json == NULL)
            return ESP_FAIL;

        auto &cfg = CPGetConfigurationResponse;

        cfg.AuthorizeRemoteTxRequestsReadOnly = cJSON_GetObjectItem(json, "AuthorizeRemoteTxRequestsReadOnly")->valueint;
        cfg.ClockAlignedDataIntervalReadOnly = cJSON_GetObjectItem(json, "ClockAlignedDataIntervalReadOnly")->valueint;
        cfg.ConnectionTimeOutReadOnly = cJSON_GetObjectItem(json, "ConnectionTimeOutReadOnly")->valueint;
        cfg.GetConfigurationMaxKeysReadOnly = cJSON_GetObjectItem(json, "GetConfigurationMaxKeysReadOnly")->valueint;
        cfg.HeartbeatIntervalReadOnly = cJSON_GetObjectItem(json, "HeartbeatIntervalReadOnly")->valueint;
        cfg.LocalAuthorizeOfflineReadOnly = cJSON_GetObjectItem(json, "LocalAuthorizeOfflineReadOnly")->valueint;
        cfg.LocalPreAuthorizeReadOnly = cJSON_GetObjectItem(json, "LocalPreAuthorizeReadOnly")->valueint;
        cfg.MeterValuesAlignedDataReadOnly = cJSON_GetObjectItem(json, "MeterValuesAlignedDataReadOnly")->valueint;
        cfg.MeterValuesSampledDataReadOnly = cJSON_GetObjectItem(json, "MeterValuesSampledDataReadOnly")->valueint;
        cfg.MeterValueSampleIntervalReadOnly = cJSON_GetObjectItem(json, "MeterValueSampleIntervalReadOnly")->valueint;
        cfg.NumberOfConnectorsReadOnly = cJSON_GetObjectItem(json, "NumberOfConnectorsReadOnly")->valueint;
        cfg.ResetRetriesReadOnly = cJSON_GetObjectItem(json, "ResetRetriesReadOnly")->valueint;
        cfg.ConnectorPhaseRotationReadOnly = cJSON_GetObjectItem(json, "ConnectorPhaseRotationReadOnly")->valueint;
        cfg.StopTransactionOnEVSideDisconnectReadOnly = cJSON_GetObjectItem(json, "StopTransactionOnEVSideDisconnectReadOnly")->valueint;
        cfg.StopTransactionOnInvalidIdReadOnly = cJSON_GetObjectItem(json, "StopTransactionOnInvalidIdReadOnly")->valueint;
        cfg.StopTxnAlignedDataReadOnly = cJSON_GetObjectItem(json, "StopTxnAlignedDataReadOnly")->valueint;
        cfg.StopTxnSampledDataReadOnly = cJSON_GetObjectItem(json, "StopTxnSampledDataReadOnly")->valueint;
        cfg.SupportedFeatureProfilesReadOnly = cJSON_GetObjectItem(json, "SupportedFeatureProfilesReadOnly")->valueint;
        cfg.TransactionMessageAttemptsReadOnly = cJSON_GetObjectItem(json, "TransactionMessageAttemptsReadOnly")->valueint;
        cfg.TransactionMessageRetryIntervalReadOnly = cJSON_GetObjectItem(json, "TransactionMessageRetryIntervalReadOnly")->valueint;
        cfg.UnlockConnectorOnEVSideDisconnectReadOnly = cJSON_GetObjectItem(json, "UnlockConnectorOnEVSideDisconnectReadOnly")->valueint;
        cfg.AllowOfflineTxForUnknownIdReadOnly = cJSON_GetObjectItem(json, "AllowOfflineTxForUnknownIdReadOnly")->valueint;
        cfg.AuthorizationCacheEnabledReadOnly = cJSON_GetObjectItem(json, "AuthorizationCacheEnabledReadOnly")->valueint;
        cfg.BlinkRepeatReadOnly = cJSON_GetObjectItem(json, "BlinkRepeatReadOnly")->valueint;
        cfg.LightIntensityReadOnly = cJSON_GetObjectItem(json, "LightIntensityReadOnly")->valueint;
        cfg.MaxEnergyOnInvalidIdReadOnly = cJSON_GetObjectItem(json, "MaxEnergyOnInvalidIdReadOnly")->valueint;
        cfg.MeterValuesAlignedDataMaxLengthReadOnly = cJSON_GetObjectItem(json, "MeterValuesAlignedDataMaxLengthReadOnly")->valueint;
        cfg.MeterValuesSampledDataMaxLengthReadOnly = cJSON_GetObjectItem(json, "MeterValuesSampledDataMaxLengthReadOnly")->valueint;
        cfg.MinimumStatusDurationReadOnly = cJSON_GetObjectItem(json, "MinimumStatusDurationReadOnly")->valueint;
        cfg.ConnectorPhaseRotationMaxLengthReadOnly = cJSON_GetObjectItem(json, "ConnectorPhaseRotationMaxLengthReadOnly")->valueint;
        cfg.StopTxnAlignedDataMaxLengthReadOnly = cJSON_GetObjectItem(json, "StopTxnAlignedDataMaxLengthReadOnly")->valueint;
        cfg.StopTxnSampledDataMaxLengthReadOnly = cJSON_GetObjectItem(json, "StopTxnSampledDataMaxLengthReadOnly")->valueint;
        cfg.SupportedFeatureProfilesMaxLengthReadOnly = cJSON_GetObjectItem(json, "SupportedFeatureProfilesMaxLengthReadOnly")->valueint;
        cfg.WebSocketPingIntervalReadOnly = cJSON_GetObjectItem(json, "WebSocketPingIntervalReadOnly")->valueint;
        cfg.LocalAuthListEnabledReadOnly = cJSON_GetObjectItem(json, "LocalAuthListEnabledReadOnly")->valueint;
        cfg.LocalAuthListMaxLengthReadOnly = cJSON_GetObjectItem(json, "LocalAuthListMaxLengthReadOnly")->valueint;
        cfg.SendLocalListMaxLengthReadOnly = cJSON_GetObjectItem(json, "SendLocalListMaxLengthReadOnly")->valueint;
        cfg.ReserveConnectorZeroSupportedReadOnly = cJSON_GetObjectItem(json, "ReserveConnectorZeroSupportedReadOnly")->valueint;
        cfg.ChargeProfileMaxStackLevelReadOnly = cJSON_GetObjectItem(json, "ChargeProfileMaxStackLevelReadOnly")->valueint;
        cfg.ChargingScheduleAllowedChargingRateUnitReadOnly = cJSON_GetObjectItem(json, "ChargingScheduleAllowedChargingRateUnitReadOnly")->valueint;
        cfg.ChargingScheduleMaxPeriodsReadOnly = cJSON_GetObjectItem(json, "ChargingScheduleMaxPeriodsReadOnly")->valueint;
        cfg.ConnectorSwitch3to1PhaseSupportedReadOnly = cJSON_GetObjectItem(json, "ConnectorSwitch3to1PhaseSupportedReadOnly")->valueint;
        cfg.MaxChargingProfilesInstalledReadOnly = cJSON_GetObjectItem(json, "MaxChargingProfilesInstalledReadOnly")->valueint;
        strcpy(cfg.AuthorizeRemoteTxRequestsValue, cJSON_GetObjectItem(json, "AuthorizeRemoteTxRequestsValue")->valuestring);
        strcpy(cfg.ClockAlignedDataIntervalValue, cJSON_GetObjectItem(json, "ClockAlignedDataIntervalValue")->valuestring);
        strcpy(cfg.ConnectionTimeOutValue, cJSON_GetObjectItem(json, "ConnectionTimeOutValue")->valuestring);
        strcpy(cfg.GetConfigurationMaxKeysValue, cJSON_GetObjectItem(json, "GetConfigurationMaxKeysValue")->valuestring);
        strcpy(cfg.HeartbeatIntervalValue, cJSON_GetObjectItem(json, "HeartbeatIntervalValue")->valuestring);
        strcpy(cfg.LocalAuthorizeOfflineValue, cJSON_GetObjectItem(json, "LocalAuthorizeOfflineValue")->valuestring);
        strcpy(cfg.LocalPreAuthorizeValue, cJSON_GetObjectItem(json, "LocalPreAuthorizeValue")->valuestring);
        strcpy(cfg.MeterValuesAlignedDataValue, cJSON_GetObjectItem(json, "MeterValuesAlignedDataValue")->valuestring);
        strcpy(cfg.MeterValuesSampledDataValue, cJSON_GetObjectItem(json, "MeterValuesSampledDataValue")->valuestring);
        strcpy(cfg.MeterValueSampleIntervalValue, cJSON_GetObjectItem(json, "MeterValueSampleIntervalValue")->valuestring);
        strcpy(cfg.NumberOfConnectorsValue, cJSON_GetObjectItem(json, "NumberOfConnectorsValue")->valuestring);
        strcpy(cfg.ResetRetriesValue, cJSON_GetObjectItem(json, "ResetRetriesValue")->valuestring);
        strcpy(cfg.ConnectorPhaseRotationValue, cJSON_GetObjectItem(json, "ConnectorPhaseRotationValue")->valuestring);
        strcpy(cfg.StopTransactionOnEVSideDisconnectValue, cJSON_GetObjectItem(json, "StopTransactionOnEVSideDisconnectValue")->valuestring);
        strcpy(cfg.StopTransactionOnInvalidIdValue, cJSON_GetObjectItem(json, "StopTransactionOnInvalidIdValue")->valuestring);
        strcpy(cfg.StopTxnAlignedDataValue, cJSON_GetObjectItem(json, "StopTxnAlignedDataValue")->valuestring);
        strcpy(cfg.StopTxnSampledDataValue, cJSON_GetObjectItem(json, "StopTxnSampledDataValue")->valuestring);
        strcpy(cfg.SupportedFeatureProfilesValue, cJSON_GetObjectItem(json, "SupportedFeatureProfilesValue")->valuestring);
        strcpy(cfg.TransactionMessageAttemptsValue, cJSON_GetObjectItem(json, "TransactionMessageAttemptsValue")->valuestring);
        strcpy(cfg.TransactionMessageRetryIntervalValue, cJSON_GetObjectItem(json, "TransactionMessageRetryIntervalValue")->valuestring);
        strcpy(cfg.UnlockConnectorOnEVSideDisconnectValue, cJSON_GetObjectItem(json, "UnlockConnectorOnEVSideDisconnectValue")->valuestring);
        strcpy(cfg.AllowOfflineTxForUnknownIdValue, cJSON_GetObjectItem(json, "AllowOfflineTxForUnknownIdValue")->valuestring);
        strcpy(cfg.AuthorizationCacheEnabledValue, cJSON_GetObjectItem(json, "AuthorizationCacheEnabledValue")->valuestring);
        strcpy(cfg.BlinkRepeatValue, cJSON_GetObjectItem(json, "BlinkRepeatValue")->valuestring);
        strcpy(cfg.LightIntensityValue, cJSON_GetObjectItem(json, "LightIntensityValue")->valuestring);
        strcpy(cfg.MaxEnergyOnInvalidIdValue, cJSON_GetObjectItem(json, "MaxEnergyOnInvalidIdValue")->valuestring);
        strcpy(cfg.MeterValuesAlignedDataMaxLengthValue, cJSON_GetObjectItem(json, "MeterValuesAlignedDataMaxLengthValue")->valuestring);
        strcpy(cfg.MeterValuesSampledDataMaxLengthValue, cJSON_GetObjectItem(json, "MeterValuesSampledDataMaxLengthValue")->valuestring);
        strcpy(cfg.MinimumStatusDurationValue, cJSON_GetObjectItem(json, "MinimumStatusDurationValue")->valuestring);
        strcpy(cfg.ConnectorPhaseRotationMaxLengthValue, cJSON_GetObjectItem(json, "ConnectorPhaseRotationMaxLengthValue")->valuestring);
        strcpy(cfg.StopTxnAlignedDataMaxLengthValue, cJSON_GetObjectItem(json, "StopTxnAlignedDataMaxLengthValue")->valuestring);
        strcpy(cfg.StopTxnSampledDataMaxLengthValue, cJSON_GetObjectItem(json, "StopTxnSampledDataMaxLengthValue")->valuestring);
        strcpy(cfg.SupportedFeatureProfilesMaxLengthValue, cJSON_GetObjectItem(json, "SupportedFeatureProfilesMaxLengthValue")->valuestring);
        strcpy(cfg.WebSocketPingIntervalValue, cJSON_GetObjectItem(json, "WebSocketPingIntervalValue")->valuestring);
        strcpy(cfg.LocalAuthListEnabledValue, cJSON_GetObjectItem(json, "LocalAuthListEnabledValue")->valuestring);
        strcpy(cfg.LocalAuthListMaxLengthValue, cJSON_GetObjectItem(json, "LocalAuthListMaxLengthValue")->valuestring);
        strcpy(cfg.SendLocalListMaxLengthValue, cJSON_GetObjectItem(json, "SendLocalListMaxLengthValue")->valuestring);
        strcpy(cfg.ReserveConnectorZeroSupportedValue, cJSON_GetObjectItem(json, "ReserveConnectorZeroSupportedValue")->valuestring);
        strcpy(cfg.ChargeProfileMaxStackLevelValue, cJSON_GetObjectItem(json, "ChargeProfileMaxStackLevelValue")->valuestring);
        strcpy(cfg.ChargingScheduleAllowedChargingRateUnitValue, cJSON_GetObjectItem(json, "ChargingScheduleAllowedChargingRateUnitValue")->valuestring);
        strcpy(cfg.ChargingScheduleMaxPeriodsValue, cJSON_GetObjectItem(json, "ChargingScheduleMaxPeriodsValue")->valuestring);
        strcpy(cfg.ConnectorSwitch3to1PhaseSupportedValue, cJSON_GetObjectItem(json, "ConnectorSwitch3to1PhaseSupportedValue")->valuestring);
        strcpy(cfg.MaxChargingProfilesInstalledValue, cJSON_GetObjectItem(json, "MaxChargingProfilesInstalledValue")->valuestring);
        cfg.AuthorizeRemoteTxRequests = cJSON_GetObjectItem(json, "AuthorizeRemoteTxRequests")->valueint;
        cfg.ClockAlignedDataInterval = cJSON_GetObjectItem(json, "ClockAlignedDataInterval")->valueint;
        cfg.ConnectionTimeOut = cJSON_GetObjectItem(json, "ConnectionTimeOut")->valueint;
        cfg.GetConfigurationMaxKeys = cJSON_GetObjectItem(json, "GetConfigurationMaxKeys")->valueint;
        cfg.HeartbeatInterval = cJSON_GetObjectItem(json, "HeartbeatInterval")->valueint;
        cfg.LocalAuthorizeOffline = cJSON_GetObjectItem(json, "LocalAuthorizeOffline")->valueint;
        cfg.LocalPreAuthorize = cJSON_GetObjectItem(json, "LocalPreAuthorize")->valueint;
        cfg.MeterValueSampleInterval = cJSON_GetObjectItem(json, "MeterValueSampleInterval")->valueint;
        cfg.NumberOfConnectors = cJSON_GetObjectItem(json, "NumberOfConnectors")->valueint;
        cfg.ResetRetries = cJSON_GetObjectItem(json, "ResetRetries")->valueint;
        cfg.StopTransactionOnEVSideDisconnect = cJSON_GetObjectItem(json, "StopTransactionOnEVSideDisconnect")->valueint;
        cfg.StopTransactionOnInvalidId = cJSON_GetObjectItem(json, "StopTransactionOnInvalidId")->valueint;
        cfg.TransactionMessageAttempts = cJSON_GetObjectItem(json, "TransactionMessageAttempts")->valueint;
        cfg.TransactionMessageRetryInterval = cJSON_GetObjectItem(json, "TransactionMessageRetryInterval")->valueint;
        cfg.UnlockConnectorOnEVSideDisconnect = cJSON_GetObjectItem(json, "UnlockConnectorOnEVSideDisconnect")->valueint;
        cfg.AllowOfflineTxForUnknownId = cJSON_GetObjectItem(json, "AllowOfflineTxForUnknownId")->valueint;
        cfg.AuthorizationCacheEnabled = cJSON_GetObjectItem(json, "AuthorizationCacheEnabled")->valueint;
        cfg.BlinkRepeat = cJSON_GetObjectItem(json, "BlinkRepeat")->valueint;
        cfg.LightIntensity = cJSON_GetObjectItem(json, "LightIntensity")->valueint;
        cfg.MaxEnergyOnInvalidId = cJSON_GetObjectItem(json, "MaxEnergyOnInvalidId")->valueint;
        cfg.MeterValuesAlignedDataMaxLength = cJSON_GetObjectItem(json, "MeterValuesAlignedDataMaxLength")->valueint;
        cfg.MeterValuesSampledDataMaxLength = cJSON_GetObjectItem(json, "MeterValuesSampledDataMaxLength")->valueint;
        cfg.MinimumStatusDuration = cJSON_GetObjectItem(json, "MinimumStatusDuration")->valueint;
        cfg.ConnectorPhaseRotationMaxLength = cJSON_GetObjectItem(json, "ConnectorPhaseRotationMaxLength")->valueint;
        cfg.StopTxnAlignedDataMaxLength = cJSON_GetObjectItem(json, "StopTxnAlignedDataMaxLength")->valueint;
        cfg.StopTxnSampledDataMaxLength = cJSON_GetObjectItem(json, "StopTxnSampledDataMaxLength")->valueint;
        cfg.SupportedFeatureProfilesMaxLength = cJSON_GetObjectItem(json, "SupportedFeatureProfilesMaxLength")->valueint;
        cfg.WebSocketPingInterval = cJSON_GetObjectItem(json, "WebSocketPingInterval")->valueint;
        cfg.LocalAuthListEnabled = cJSON_GetObjectItem(json, "LocalAuthListEnabled")->valueint;
        cfg.LocalAuthListMaxLength = cJSON_GetObjectItem(json, "LocalAuthListMaxLength")->valueint;
        cfg.SendLocalListMaxLength = cJSON_GetObjectItem(json, "SendLocalListMaxLength")->valueint;
        cfg.ReserveConnectorZeroSupported = cJSON_GetObjectItem(json, "ReserveConnectorZeroSupported")->valueint;
        cfg.ChargeProfileMaxStackLevel = cJSON_GetObjectItem(json, "ChargeProfileMaxStackLevel")->valueint;
        cfg.ChargingScheduleMaxPeriods = cJSON_GetObjectItem(json, "ChargingScheduleMaxPeriods")->valueint;
        cfg.ConnectorSwitch3to1PhaseSupported = cJSON_GetObjectItem(json, "ConnectorSwitch3to1PhaseSupported")->valueint;
        cfg.MaxChargingProfilesInstalled = cJSON_GetObjectItem(json, "MaxChargingProfilesInstalled")->valueint;

        cJSON *mva = cJSON_GetObjectItem(json, "MeterValuesAlignedData");
        cfg.MeterValuesAlignedData.EnergyActiveExportRegister = cJSON_GetObjectItem(mva, "EnergyActiveExportRegister")->valueint;
        cfg.MeterValuesAlignedData.EnergyActiveImportRegister = cJSON_GetObjectItem(mva, "EnergyActiveImportRegister")->valueint;
        cfg.MeterValuesAlignedData.EnergyReactiveExportRegister = cJSON_GetObjectItem(mva, "EnergyReactiveExportRegister")->valueint;
        cfg.MeterValuesAlignedData.EnergyReactiveImportRegister = cJSON_GetObjectItem(mva, "EnergyReactiveImportRegister")->valueint;
        cfg.MeterValuesAlignedData.EnergyActiveExportInterval = cJSON_GetObjectItem(mva, "EnergyActiveExportInterval")->valueint;
        cfg.MeterValuesAlignedData.EnergyActiveImportInterval = cJSON_GetObjectItem(mva, "EnergyActiveImportInterval")->valueint;
        cfg.MeterValuesAlignedData.EnergyReactiveExportInterval = cJSON_GetObjectItem(mva, "EnergyReactiveExportInterval")->valueint;
        cfg.MeterValuesAlignedData.EnergyReactiveImportInterval = cJSON_GetObjectItem(mva, "EnergyReactiveImportInterval")->valueint;
        cfg.MeterValuesAlignedData.PowerActiveExport = cJSON_GetObjectItem(mva, "PowerActiveExport")->valueint;
        cfg.MeterValuesAlignedData.PowerActiveImport = cJSON_GetObjectItem(mva, "PowerActiveImport")->valueint;
        cfg.MeterValuesAlignedData.PowerOffered = cJSON_GetObjectItem(mva, "PowerOffered")->valueint;
        cfg.MeterValuesAlignedData.PowerReactiveExport = cJSON_GetObjectItem(mva, "PowerReactiveExport")->valueint;
        cfg.MeterValuesAlignedData.PowerReactiveImport = cJSON_GetObjectItem(mva, "PowerReactiveImport")->valueint;
        cfg.MeterValuesAlignedData.PowerFactor = cJSON_GetObjectItem(mva, "PowerFactor")->valueint;
        cfg.MeterValuesAlignedData.CurrentImport = cJSON_GetObjectItem(mva, "CurrentImport")->valueint;
        cfg.MeterValuesAlignedData.CurrentExport = cJSON_GetObjectItem(mva, "CurrentExport")->valueint;
        cfg.MeterValuesAlignedData.CurrentOffered = cJSON_GetObjectItem(mva, "CurrentOffered")->valueint;
        cfg.MeterValuesAlignedData.Voltage = cJSON_GetObjectItem(mva, "Voltage")->valueint;
        cfg.MeterValuesAlignedData.Frequency = cJSON_GetObjectItem(mva, "Frequency")->valueint;
        cfg.MeterValuesAlignedData.Temperature = cJSON_GetObjectItem(mva, "Temperature")->valueint;
        cfg.MeterValuesAlignedData.SoC = cJSON_GetObjectItem(mva, "SoC")->valueint;
        cfg.MeterValuesAlignedData.RPM = cJSON_GetObjectItem(mva, "RPM")->valueint;

        cJSON *mvs = cJSON_GetObjectItem(json, "MeterValuesSampledData");
        cfg.MeterValuesSampledData.EnergyActiveExportRegister = cJSON_GetObjectItem(mvs, "EnergyActiveExportRegister")->valueint;
        cfg.MeterValuesSampledData.EnergyActiveImportRegister = cJSON_GetObjectItem(mvs, "EnergyActiveImportRegister")->valueint;
        cfg.MeterValuesSampledData.EnergyReactiveExportRegister = cJSON_GetObjectItem(mvs, "EnergyReactiveExportRegister")->valueint;
        cfg.MeterValuesSampledData.EnergyReactiveImportRegister = cJSON_GetObjectItem(mvs, "EnergyReactiveImportRegister")->valueint;
        cfg.MeterValuesSampledData.EnergyActiveExportInterval = cJSON_GetObjectItem(mvs, "EnergyActiveExportInterval")->valueint;
        cfg.MeterValuesSampledData.EnergyActiveImportInterval = cJSON_GetObjectItem(mvs, "EnergyActiveImportInterval")->valueint;
        cfg.MeterValuesSampledData.EnergyReactiveExportInterval = cJSON_GetObjectItem(mvs, "EnergyReactiveExportInterval")->valueint;
        cfg.MeterValuesSampledData.EnergyReactiveImportInterval = cJSON_GetObjectItem(mvs, "EnergyReactiveImportInterval")->valueint;
        cfg.MeterValuesSampledData.PowerActiveExport = cJSON_GetObjectItem(mvs, "PowerActiveExport")->valueint;
        cfg.MeterValuesSampledData.PowerActiveImport = cJSON_GetObjectItem(mvs, "PowerActiveImport")->valueint;
        cfg.MeterValuesSampledData.PowerOffered = cJSON_GetObjectItem(mvs, "PowerOffered")->valueint;
        cfg.MeterValuesSampledData.PowerReactiveExport = cJSON_GetObjectItem(mvs, "PowerReactiveExport")->valueint;
        cfg.MeterValuesSampledData.PowerReactiveImport = cJSON_GetObjectItem(mvs, "PowerReactiveImport")->valueint;
        cfg.MeterValuesSampledData.PowerFactor = cJSON_GetObjectItem(mvs, "PowerFactor")->valueint;
        cfg.MeterValuesSampledData.CurrentImport = cJSON_GetObjectItem(mvs, "CurrentImport")->valueint;
        cfg.MeterValuesSampledData.CurrentExport = cJSON_GetObjectItem(mvs, "CurrentExport")->valueint;
        cfg.MeterValuesSampledData.CurrentOffered = cJSON_GetObjectItem(mvs, "CurrentOffered")->valueint;
        cfg.MeterValuesSampledData.Voltage = cJSON_GetObjectItem(mvs, "Voltage")->valueint;
        cfg.MeterValuesSampledData.Frequency = cJSON_GetObjectItem(mvs, "Frequency")->valueint;
        cfg.MeterValuesSampledData.Temperature = cJSON_GetObjectItem(mvs, "Temperature")->valueint;
        cfg.MeterValuesSampledData.SoC = cJSON_GetObjectItem(mvs, "SoC")->valueint;
        cfg.MeterValuesSampledData.RPM = cJSON_GetObjectItem(mvs, "RPM")->valueint;

        CJSON *sta = cJSON_GetObjectItem(json, "StopTxnAlignedData");
        cfg.StopTxnAlignedData.EnergyActiveExportRegister = cJSON_GetObjectItem(sta, "EnergyActiveExportRegister")->valueint;
        cfg.StopTxnAlignedData.EnergyActiveImportRegister = cJSON_GetObjectItem(sta, "EnergyActiveImportRegister")->valueint;
        cfg.StopTxnAlignedData.EnergyReactiveExportRegister = cJSON_GetObjectItem(sta, "EnergyReactiveExportRegister")->valueint;
        cfg.StopTxnAlignedData.EnergyReactiveImportRegister = cJSON_GetObjectItem(sta, "EnergyReactiveImportRegister")->valueint;
        cfg.StopTxnAlignedData.EnergyActiveExportInterval = cJSON_GetObjectItem(sta, "EnergyActiveExportInterval")->valueint;
        cfg.StopTxnAlignedData.EnergyActiveImportInterval = cJSON_GetObjectItem(sta, "EnergyActiveImportInterval")->valueint;
        cfg.StopTxnAlignedData.EnergyReactiveExportInterval = cJSON_GetObjectItem(sta, "EnergyReactiveExportInterval")->valueint;
        cfg.StopTxnAlignedData.EnergyReactiveImportInterval = cJSON_GetObjectItem(sta, "EnergyReactiveImportInterval")->valueint;
        cfg.StopTxnAlignedData.PowerActiveExport = cJSON_GetObjectItem(sta, "PowerActiveExport")->valueint;
        cfg.StopTxnAlignedData.PowerActiveImport = cJSON_GetObjectItem(sta, "PowerActiveImport")->valueint;
        cfg.StopTxnAlignedData.PowerOffered = cJSON_GetObjectItem(sta, "PowerOffered")->valueint;
        cfg.StopTxnAlignedData.PowerReactiveExport = cJSON_GetObjectItem(sta, "PowerReactiveExport")->valueint;
        cfg.StopTxnAlignedData.PowerReactiveImport = cJSON_GetObjectItem(sta, "PowerReactiveImport")->valueint;
        cfg.StopTxnAlignedData.PowerFactor = cJSON_GetObjectItem(sta, "PowerFactor")->valueint;
        cfg.StopTxnAlignedData.CurrentImport = cJSON_GetObjectItem(sta, "CurrentImport")->valueint;
        cfg.StopTxnAlignedData.CurrentExport = cJSON_GetObjectItem(sta, "CurrentExport")->valueint;
        cfg.StopTxnAlignedData.CurrentOffered = cJSON_GetObjectItem(sta, "CurrentOffered")->valueint;
        cfg.StopTxnAlignedData.Voltage = cJSON_GetObjectItem(sta, "Voltage")->valueint;
        cfg.StopTxnAlignedData.Frequency = cJSON_GetObjectItem(sta, "Frequency")->valueint;
        cfg.StopTxnAlignedData.Temperature = cJSON_GetObjectItem(sta, "Temperature")->valueint;
        cfg.StopTxnAlignedData.SoC = cJSON_GetObjectItem(sta, "SoC")->valueint;
        cfg.StopTxnAlignedData.RPM = cJSON_GetObjectItem(sta, "RPM")->valueint;

        CJSON *sts = cJSON_GetObjectItem(json, "StopTxnSampledData");
        cfg.StopTxnSampledData.EnergyActiveExportRegister = cJSON_GetObjectItem(sts, "EnergyActiveExportRegister")->valueint;
        cfg.StopTxnSampledData.EnergyActiveImportRegister = cJSON_GetObjectItem(sts, "EnergyActiveImportRegister")->valueint;
        cfg.StopTxnSampledData.EnergyReactiveExportRegister = cJSON_GetObjectItem(sts, "EnergyReactiveExportRegister")->valueint;
        cfg.StopTxnSampledData.EnergyReactiveImportRegister = cJSON_GetObjectItem(sts, "EnergyReactiveImportRegister")->valueint;
        cfg.StopTxnSampledData.EnergyActiveExportInterval = cJSON_GetObjectItem(sts, "EnergyActiveExportInterval")->valueint;
        cfg.StopTxnSampledData.EnergyActiveImportInterval = cJSON_GetObjectItem(sts, "EnergyActiveImportInterval")->valueint;
        cfg.StopTxnSampledData.EnergyReactiveExportInterval = cJSON_GetObjectItem(sts, "EnergyReactiveExportInterval")->valueint;
        cfg.StopTxnSampledData.EnergyReactiveImportInterval = cJSON_GetObjectItem(sts, "EnergyReactiveImportInterval")->valueint;
        cfg.StopTxnSampledData.PowerActiveExport = cJSON_GetObjectItem(sts, "PowerActiveExport")->valueint;
        cfg.StopTxnSampledData.PowerActiveImport = cJSON_GetObjectItem(sts, "PowerActiveImport")->valueint;
        cfg.StopTxnSampledData.PowerOffered = cJSON_GetObjectItem(sts, "PowerOffered")->valueint;
        cfg.StopTxnSampledData.PowerReactiveExport = cJSON_GetObjectItem(sts, "PowerReactiveExport")->valueint;
        cfg.StopTxnSampledData.PowerReactiveImport = cJSON_GetObjectItem(sts, "PowerReactiveImport")->valueint;
        cfg.StopTxnSampledData.PowerFactor = cJSON_GetObjectItem(sts, "PowerFactor")->valueint;
        cfg.StopTxnSampledData.CurrentImport = cJSON_GetObjectItem(sts, "CurrentImport")->valueint;
        cfg.StopTxnSampledData.CurrentExport = cJSON_GetObjectItem(sts, "CurrentExport")->valueint;
        cfg.StopTxnSampledData.CurrentOffered = cJSON_GetObjectItem(sts, "CurrentOffered")->valueint;
        cfg.StopTxnSampledData.Voltage = cJSON_GetObjectItem(sts, "Voltage")->valueint;
        cfg.StopTxnSampledData.Frequency = cJSON_GetObjectItem(sts, "Frequency")->valueint;
        cfg.StopTxnSampledData.Temperature = cJSON_GetObjectItem(sts, "Temperature")->valueint;
        cfg.StopTxnSampledData.SoC = cJSON_GetObjectItem(sts, "SoC")->valueint;
        cfg.StopTxnSampledData.RPM = cJSON_GetObjectItem(sts, "RPM")->valueint;

        cJSON *profiles = cJSON_GetObjectItem(json, "SupportedFeatureProfiles");
        cfg.SupportedFeatureProfiles.Core = cJSON_GetObjectItem(profiles, "Core")->valueint;
        cfg.SupportedFeatureProfiles.LocalAuthListManagement = cJSON_GetObjectItem(profiles, "LocalAuthListManagement")->valueint;
        cfg.SupportedFeatureProfiles.Reservation = cJSON_GetObjectItem(profiles, "Reservation")->valueint;
        cfg.SupportedFeatureProfiles.RemoteTrigger = cJSON_GetObjectItem(profiles, "RemoteTrigger")->valueint;
        cfg.SupportedFeatureProfiles.FirmwareManagement = cJSON_GetObjectItem(profiles, "FirmwareManagement")->valueint;
        cfg.SupportedFeatureProfiles.SmartCharging = cJSON_GetObjectItem(profiles, "SmartCharging")->valueint;

        cJSON_Delete(json);
        return ESP_OK;
    }

    esp_err_t OCPPController::write_config_ocpp(void)
    {
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        auto &cfg = CPGetConfigurationResponse;

        cJSON_AddBoolToObject(json, "AuthorizeRemoteTxRequestsReadOnly", cfg.AuthorizeRemoteTxRequestsReadOnly);
        cJSON_AddBoolToObject(json, "ClockAlignedDataIntervalReadOnly", cfg.ClockAlignedDataIntervalReadOnly);
        cJSON_AddBoolToObject(json, "ConnectionTimeOutReadOnly", cfg.ConnectionTimeOutReadOnly);
        cJSON_AddBoolToObject(json, "GetConfigurationMaxKeysReadOnly", cfg.GetConfigurationMaxKeysReadOnly);
        cJSON_AddBoolToObject(json, "HeartbeatIntervalReadOnly", cfg.HeartbeatIntervalReadOnly);
        cJSON_AddBoolToObject(json, "LocalAuthorizeOfflineReadOnly", cfg.LocalAuthorizeOfflineReadOnly);
        cJSON_AddBoolToObject(json, "LocalPreAuthorizeReadOnly", cfg.LocalPreAuthorizeReadOnly);
        cJSON_AddBoolToObject(json, "MeterValuesAlignedDataReadOnly", cfg.MeterValuesAlignedDataReadOnly);
        cJSON_AddBoolToObject(json, "MeterValuesSampledDataReadOnly", cfg.MeterValuesSampledDataReadOnly);
        cJSON_AddBoolToObject(json, "MeterValueSampleIntervalReadOnly", cfg.MeterValueSampleIntervalReadOnly);
        cJSON_AddBoolToObject(json, "NumberOfConnectorsReadOnly", cfg.NumberOfConnectorsReadOnly);
        cJSON_AddBoolToObject(json, "ResetRetriesReadOnly", cfg.ResetRetriesReadOnly);
        cJSON_AddBoolToObject(json, "ConnectorPhaseRotationReadOnly", cfg.ConnectorPhaseRotationReadOnly);
        cJSON_AddBoolToObject(json, "StopTransactionOnEVSideDisconnectReadOnly", cfg.StopTransactionOnEVSideDisconnectReadOnly);
        cJSON_AddBoolToObject(json, "StopTransactionOnInvalidIdReadOnly", cfg.StopTransactionOnInvalidIdReadOnly);
        cJSON_AddBoolToObject(json, "StopTxnAlignedDataReadOnly", cfg.StopTxnAlignedDataReadOnly);
        cJSON_AddBoolToObject(json, "StopTxnSampledDataReadOnly", cfg.StopTxnSampledDataReadOnly);
        cJSON_AddBoolToObject(json, "SupportedFeatureProfilesReadOnly", cfg.SupportedFeatureProfilesReadOnly);
        cJSON_AddBoolToObject(json, "TransactionMessageAttemptsReadOnly", cfg.TransactionMessageAttemptsReadOnly);
        cJSON_AddBoolToObject(json, "TransactionMessageRetryIntervalReadOnly", cfg.TransactionMessageRetryIntervalReadOnly);
        cJSON_AddBoolToObject(json, "UnlockConnectorOnEVSideDisconnectReadOnly", cfg.UnlockConnectorOnEVSideDisconnectReadOnly);
        cJSON_AddBoolToObject(json, "AllowOfflineTxForUnknownIdReadOnly", cfg.AllowOfflineTxForUnknownIdReadOnly);
        cJSON_AddBoolToObject(json, "AuthorizationCacheEnabledReadOnly", cfg.AuthorizationCacheEnabledReadOnly);
        cJSON_AddBoolToObject(json, "BlinkRepeatReadOnly", cfg.BlinkRepeatReadOnly);
        cJSON_AddBoolToObject(json, "LightIntensityReadOnly", cfg.LightIntensityReadOnly);
        cJSON_AddBoolToObject(json, "MaxEnergyOnInvalidIdReadOnly", cfg.MaxEnergyOnInvalidIdReadOnly);
        cJSON_AddBoolToObject(json, "MeterValuesAlignedDataMaxLengthReadOnly", cfg.MeterValuesAlignedDataMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "MeterValuesSampledDataMaxLengthReadOnly", cfg.MeterValuesSampledDataMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "MinimumStatusDurationReadOnly", cfg.MinimumStatusDurationReadOnly);
        cJSON_AddBoolToObject(json, "ConnectorPhaseRotationMaxLengthReadOnly", cfg.ConnectorPhaseRotationMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "StopTxnAlignedDataMaxLengthReadOnly", cfg.StopTxnAlignedDataMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "StopTxnSampledDataMaxLengthReadOnly", cfg.StopTxnSampledDataMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "SupportedFeatureProfilesMaxLengthReadOnly", cfg.SupportedFeatureProfilesMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "WebSocketPingIntervalReadOnly", cfg.WebSocketPingIntervalReadOnly);
        cJSON_AddBoolToObject(json, "LocalAuthListEnabledReadOnly", cfg.LocalAuthListEnabledReadOnly);
        cJSON_AddBoolToObject(json, "LocalAuthListMaxLengthReadOnly", cfg.LocalAuthListMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "SendLocalListMaxLengthReadOnly", cfg.SendLocalListMaxLengthReadOnly);
        cJSON_AddBoolToObject(json, "ReserveConnectorZeroSupportedReadOnly", cfg.ReserveConnectorZeroSupportedReadOnly);
        cJSON_AddBoolToObject(json, "ChargeProfileMaxStackLevelReadOnly", cfg.ChargeProfileMaxStackLevelReadOnly);
        cJSON_AddBoolToObject(json, "ChargingScheduleAllowedChargingRateUnitReadOnly", cfg.ChargingScheduleAllowedChargingRateUnitReadOnly);
        cJSON_AddBoolToObject(json, "ChargingScheduleMaxPeriodsReadOnly", cfg.ChargingScheduleMaxPeriodsReadOnly);
        cJSON_AddBoolToObject(json, "ConnectorSwitch3to1PhaseSupportedReadOnly", cfg.ConnectorSwitch3to1PhaseSupportedReadOnly);
        cJSON_AddBoolToObject(json, "MaxChargingProfilesInstalledReadOnly", cfg.MaxChargingProfilesInstalledReadOnly);
        cJSON_AddStringToObject(json, "AuthorizeRemoteTxRequestsValue", cfg.AuthorizeRemoteTxRequestsValue);
        cJSON_AddStringToObject(json, "ClockAlignedDataIntervalValue", cfg.ClockAlignedDataIntervalValue);
        cJSON_AddStringToObject(json, "ConnectionTimeOutValue", cfg.ConnectionTimeOutValue);
        cJSON_AddStringToObject(json, "GetConfigurationMaxKeysValue", cfg.GetConfigurationMaxKeysValue);
        cJSON_AddStringToObject(json, "HeartbeatIntervalValue", cfg.HeartbeatIntervalValue);
        cJSON_AddStringToObject(json, "LocalAuthorizeOfflineValue", cfg.LocalAuthorizeOfflineValue);
        cJSON_AddStringToObject(json, "LocalPreAuthorizeValue", cfg.LocalPreAuthorizeValue);
        cJSON_AddStringToObject(json, "MeterValuesAlignedDataValue", cfg.MeterValuesAlignedDataValue);
        cJSON_AddStringToObject(json, "MeterValuesSampledDataValue", cfg.MeterValuesSampledDataValue);
        cJSON_AddStringToObject(json, "MeterValueSampleIntervalValue", cfg.MeterValueSampleIntervalValue);
        cJSON_AddStringToObject(json, "NumberOfConnectorsValue", cfg.NumberOfConnectorsValue);
        cJSON_AddStringToObject(json, "ResetRetriesValue", cfg.ResetRetriesValue);
        cJSON_AddStringToObject(json, "ConnectorPhaseRotationValue", cfg.ConnectorPhaseRotationValue);
        cJSON_AddStringToObject(json, "StopTransactionOnEVSideDisconnectValue", cfg.StopTransactionOnEVSideDisconnectValue);
        cJSON_AddStringToObject(json, "StopTransactionOnInvalidIdValue", cfg.StopTransactionOnInvalidIdValue);
        cJSON_AddStringToObject(json, "StopTxnAlignedDataValue", cfg.StopTxnAlignedDataValue);
        cJSON_AddStringToObject(json, "StopTxnSampledDataValue", cfg.StopTxnSampledDataValue);
        cJSON_AddStringToObject(json, "SupportedFeatureProfilesValue", cfg.SupportedFeatureProfilesValue);
        cJSON_AddStringToObject(json, "TransactionMessageAttemptsValue", cfg.TransactionMessageAttemptsValue);
        cJSON_AddStringToObject(json, "TransactionMessageRetryIntervalValue", cfg.TransactionMessageRetryIntervalValue);
        cJSON_AddStringToObject(json, "UnlockConnectorOnEVSideDisconnectValue", cfg.UnlockConnectorOnEVSideDisconnectValue);
        cJSON_AddStringToObject(json, "AllowOfflineTxForUnknownIdValue", cfg.AllowOfflineTxForUnknownIdValue);
        cJSON_AddStringToObject(json, "AuthorizationCacheEnabledValue", cfg.AuthorizationCacheEnabledValue);
        cJSON_AddStringToObject(json, "BlinkRepeatValue", cfg.BlinkRepeatValue);
        cJSON_AddStringToObject(json, "LightIntensityValue", cfg.LightIntensityValue);
        cJSON_AddStringToObject(json, "MaxEnergyOnInvalidIdValue", cfg.MaxEnergyOnInvalidIdValue);
        cJSON_AddStringToObject(json, "MeterValuesAlignedDataMaxLengthValue", cfg.MeterValuesAlignedDataMaxLengthValue);
        cJSON_AddStringToObject(json, "MeterValuesSampledDataMaxLengthValue", cfg.MeterValuesSampledDataMaxLengthValue);
        cJSON_AddStringToObject(json, "MinimumStatusDurationValue", cfg.MinimumStatusDurationValue);
        cJSON_AddStringToObject(json, "ConnectorPhaseRotationMaxLengthValue", cfg.ConnectorPhaseRotationMaxLengthValue);
        cJSON_AddStringToObject(json, "StopTxnAlignedDataMaxLengthValue", cfg.StopTxnAlignedDataMaxLengthValue);
        cJSON_AddStringToObject(json, "StopTxnSampledDataMaxLengthValue", cfg.StopTxnSampledDataMaxLengthValue);
        cJSON_AddStringToObject(json, "SupportedFeatureProfilesMaxLengthValue", cfg.SupportedFeatureProfilesMaxLengthValue);
        cJSON_AddStringToObject(json, "WebSocketPingIntervalValue", cfg.WebSocketPingIntervalValue);
        cJSON_AddStringToObject(json, "LocalAuthListEnabledValue", cfg.LocalAuthListEnabledValue);
        cJSON_AddStringToObject(json, "LocalAuthListMaxLengthValue", cfg.LocalAuthListMaxLengthValue);
        cJSON_AddStringToObject(json, "SendLocalListMaxLengthValue", cfg.SendLocalListMaxLengthValue);
        cJSON_AddStringToObject(json, "ReserveConnectorZeroSupportedValue", cfg.ReserveConnectorZeroSupportedValue);
        cJSON_AddStringToObject(json, "ChargeProfileMaxStackLevelValue", cfg.ChargeProfileMaxStackLevelValue);
        cJSON_AddStringToObject(json, "ChargingScheduleAllowedChargingRateUnitValue", cfg.ChargingScheduleAllowedChargingRateUnitValue);
        cJSON_AddStringToObject(json, "ChargingScheduleMaxPeriodsValue", cfg.ChargingScheduleMaxPeriodsValue);
        cJSON_AddStringToObject(json, "ConnectorSwitch3to1PhaseSupportedValue", cfg.ConnectorSwitch3to1PhaseSupportedValue);
        cJSON_AddStringToObject(json, "MaxChargingProfilesInstalledValue", cfg.MaxChargingProfilesInstalledValue);
        cJSON_AddBoolToObject(json, "AuthorizeRemoteTxRequests", cfg.AuthorizeRemoteTxRequests);
        cJSON_AddNumberToObject(json, "ClockAlignedDataInterval", cfg.ClockAlignedDataInterval);
        cJSON_AddNumberToObject(json, "ConnectionTimeOut", cfg.ConnectionTimeOut);
        cJSON_AddNumberToObject(json, "GetConfigurationMaxKeys", cfg.GetConfigurationMaxKeys);
        cJSON_AddNumberToObject(json, "HeartbeatInterval", cfg.HeartbeatInterval);
        cJSON_AddBoolToObject(json, "LocalAuthorizeOffline", cfg.LocalAuthorizeOffline);
        cJSON_AddBoolToObject(json, "LocalPreAuthorize", cfg.LocalPreAuthorize);
        cJSON_AddNumberToObject(json, "MeterValueSampleInterval", cfg.MeterValueSampleInterval);
        cJSON_AddNumberToObject(json, "NumberOfConnectors", cfg.NumberOfConnectors);
        cJSON_AddNumberToObject(json, "ResetRetries", cfg.ResetRetries);
        cJSON_AddBoolToObject(json, "StopTransactionOnEVSideDisconnect", cfg.StopTransactionOnEVSideDisconnect);
        cJSON_AddBoolToObject(json, "StopTransactionOnInvalidId", cfg.StopTransactionOnInvalidId);
        cJSON_AddNumberToObject(json, "TransactionMessageAttempts", cfg.TransactionMessageAttempts);
        cJSON_AddNumberToObject(json, "TransactionMessageRetryInterval", cfg.TransactionMessageRetryInterval);
        cJSON_AddBoolToObject(json, "UnlockConnectorOnEVSideDisconnect", cfg.UnlockConnectorOnEVSideDisconnect);
        cJSON_AddBoolToObject(json, "AllowOfflineTxForUnknownId", cfg.AllowOfflineTxForUnknownId);
        cJSON_AddBoolToObject(json, "AuthorizationCacheEnabled", cfg.AuthorizationCacheEnabled);
        cJSON_AddNumberToObject(json, "BlinkRepeat", cfg.BlinkRepeat);
        cJSON_AddNumberToObject(json, "LightIntensity", cfg.LightIntensity);
        cJSON_AddNumberToObject(json, "MaxEnergyOnInvalidId", cfg.MaxEnergyOnInvalidId);
        cJSON_AddNumberToObject(json, "MeterValuesAlignedDataMaxLength", cfg.MeterValuesAlignedDataMaxLength);
        cJSON_AddNumberToObject(json, "MeterValuesSampledDataMaxLength", cfg.MeterValuesSampledDataMaxLength);
        cJSON_AddNumberToObject(json, "MinimumStatusDuration", cfg.MinimumStatusDuration);
        cJSON_AddNumberToObject(json, "ConnectorPhaseRotationMaxLength", cfg.ConnectorPhaseRotationMaxLength);
        cJSON_AddNumberToObject(json, "StopTxnAlignedDataMaxLength", cfg.StopTxnAlignedDataMaxLength);
        cJSON_AddNumberToObject(json, "StopTxnSampledDataMaxLength", cfg.StopTxnSampledDataMaxLength);
        cJSON_AddNumberToObject(json, "SupportedFeatureProfilesMaxLength", cfg.SupportedFeatureProfilesMaxLength);
        cJSON_AddBoolToObject(json, "LocalAuthListEnabled", cfg.LocalAuthListEnabled);
        cJSON_AddNumberToObject(json, "LocalAuthListMaxLength", cfg.LocalAuthListMaxLength);
        cJSON_AddNumberToObject(json, "SendLocalListMaxLength", cfg.SendLocalListMaxLength);
        cJSON_AddBoolToObject(json, "ReserveConnectorZeroSupported", cfg.ReserveConnectorZeroSupported);
        cJSON_AddNumberToObject(json, "ChargeProfileMaxStackLevel", cfg.ChargeProfileMaxStackLevel);
        cJSON_AddNumberToObject(json, "ChargingScheduleMaxPeriods", cfg.ChargingScheduleMaxPeriods);
        cJSON_AddBoolToObject(json, "ConnectorSwitch3to1PhaseSupported", cfg.ConnectorSwitch3to1PhaseSupported);
        cJSON_AddNumberToObject(json, "MaxChargingProfilesInstalled", cfg.MaxChargingProfilesInstalled);

        cJSON *mva = cJSON_CreateObject();
        cJSON_AddBoolToObject(mva, "EnergyActiveExportRegister", cfg.MeterValuesAlignedData.EnergyActiveExportRegister);
        cJSON_AddBoolToObject(mva, "EnergyActiveImportRegister", cfg.MeterValuesAlignedData.EnergyActiveImportRegister);
        cJSON_AddBoolToObject(mva, "EnergyReactiveExportRegister", cfg.MeterValuesAlignedData.EnergyReactiveExportRegister);
        cJSON_AddBoolToObject(mva, "EnergyReactiveImportRegister", cfg.MeterValuesAlignedData.EnergyReactiveImportRegister);
        cJSON_AddBoolToObject(mva, "EnergyActiveExportInterval", cfg.MeterValuesAlignedData.EnergyActiveExportInterval);
        cJSON_AddBoolToObject(mva, "EnergyActiveImportInterval", cfg.MeterValuesAlignedData.EnergyActiveImportInterval);
        cJSON_AddBoolToObject(mva, "EnergyReactiveExportInterval", cfg.MeterValuesAlignedData.EnergyReactiveExportInterval);
        cJSON_AddBoolToObject(mva, "EnergyReactiveImportInterval", cfg.MeterValuesAlignedData.EnergyReactiveImportInterval);
        cJSON_AddBoolToObject(mva, "PowerActiveExport", cfg.MeterValuesAlignedData.PowerActiveExport);
        cJSON_AddBoolToObject(mva, "PowerActiveImport", cfg.MeterValuesAlignedData.PowerActiveImport);
        cJSON_AddBoolToObject(mva, "PowerOffered", cfg.MeterValuesAlignedData.PowerOffered);
        cJSON_AddBoolToObject(mva, "PowerReactiveExport", cfg.MeterValuesAlignedData.PowerReactiveExport);
        cJSON_AddBoolToObject(mva, "PowerReactiveImport", cfg.MeterValuesAlignedData.PowerReactiveImport);
        cJSON_AddBoolToObject(mva, "PowerFactor", cfg.MeterValuesAlignedData.PowerFactor);
        cJSON_AddBoolToObject(mva, "CurrentImport", cfg.MeterValuesAlignedData.CurrentImport);
        cJSON_AddBoolToObject(mva, "CurrentExport", cfg.MeterValuesAlignedData.CurrentExport);
        cJSON_AddBoolToObject(mva, "CurrentOffered", cfg.MeterValuesAlignedData.CurrentOffered);
        cJSON_AddBoolToObject(mva, "Voltage", cfg.MeterValuesAlignedData.Voltage);
        cJSON_AddBoolToObject(mva, "Frequency", cfg.MeterValuesAlignedData.Frequency);
        cJSON_AddBoolToObject(mva, "Temperature", cfg.MeterValuesAlignedData.Temperature);
        cJSON_AddBoolToObject(mva, "SoC", cfg.MeterValuesAlignedData.SoC);
        cJSON_AddBoolToObject(mva, "RPM", cfg.MeterValuesAlignedData.RPM);
        cJSON_AddItemToObject(json, "MeterValuesAlignedData", mva);

        cJSON *mvs = cJSON_CreateObject();
        cJSON_AddBoolToObject(mvs, "EnergyActiveExportRegister", cfg.MeterValuesSampledData.EnergyActiveExportRegister);
        cJSON_AddBoolToObject(mvs, "EnergyActiveImportRegister", cfg.MeterValuesSampledData.EnergyActiveImportRegister);
        cJSON_AddBoolToObject(mvs, "EnergyReactiveExportRegister", cfg.MeterValuesSampledData.EnergyReactiveExportRegister);
        cJSON_AddBoolToObject(mvs, "EnergyReactiveImportRegister", cfg.MeterValuesSampledData.EnergyReactiveImportRegister);
        cJSON_AddBoolToObject(mvs, "EnergyActiveExportInterval", cfg.MeterValuesSampledData.EnergyActiveExportInterval);
        cJSON_AddBoolToObject(mvs, "EnergyActiveImportInterval", cfg.MeterValuesSampledData.EnergyActiveImportInterval);
        cJSON_AddBoolToObject(mvs, "EnergyReactiveExportInterval", cfg.MeterValuesSampledData.EnergyReactiveExportInterval);
        cJSON_AddBoolToObject(mvs, "EnergyReactiveImportInterval", cfg.MeterValuesSampledData.EnergyReactiveImportInterval);
        cJSON_AddBoolToObject(mvs, "PowerActiveExport", cfg.MeterValuesSampledData.PowerActiveExport);
        cJSON_AddBoolToObject(mvs, "PowerActiveImport", cfg.MeterValuesSampledData.PowerActiveImport);
        cJSON_AddBoolToObject(mvs, "PowerOffered", cfg.MeterValuesSampledData.PowerOffered);
        cJSON_AddBoolToObject(mvs, "PowerReactiveExport", cfg.MeterValuesSampledData.PowerReactiveExport);
        cJSON_AddBoolToObject(mvs, "PowerReactiveImport", cfg.MeterValuesSampledData.PowerReactiveImport);
        cJSON_AddBoolToObject(mvs, "PowerFactor", cfg.MeterValuesSampledData.PowerFactor);
        cJSON_AddBoolToObject(mvs, "CurrentImport", cfg.MeterValuesSampledData.CurrentImport);
        cJSON_AddBoolToObject(mvs, "CurrentExport", cfg.MeterValuesSampledData.CurrentExport);
        cJSON_AddBoolToObject(mvs, "CurrentOffered", cfg.MeterValuesSampledData.CurrentOffered);
        cJSON_AddBoolToObject(mvs, "Voltage", cfg.MeterValuesSampledData.Voltage);
        cJSON_AddBoolToObject(mvs, "Frequency", cfg.MeterValuesSampledData.Frequency);
        cJSON_AddBoolToObject(mvs, "Temperature", cfg.MeterValuesSampledData.Temperature);
        cJSON_AddBoolToObject(mvs, "SoC", cfg.MeterValuesSampledData.SoC);
        cJSON_AddBoolToObject(mvs, "RPM", cfg.MeterValuesSampledData.RPM);
        cJSON_AddItemToObject(json, "MeterValuesSampledData", mvs);

        cJSON *sta = cJSON_CreateObject();
        cJSON_AddBoolToObject(sta, "EnergyActiveExportRegister", cfg.StopTxnAlignedData.EnergyActiveExportRegister);
        cJSON_AddBoolToObject(sta, "EnergyActiveImportRegister", cfg.StopTxnAlignedData.EnergyActiveImportRegister);
        cJSON_AddBoolToObject(sta, "EnergyReactiveExportRegister", cfg.StopTxnAlignedData.EnergyReactiveExportRegister);
        cJSON_AddBoolToObject(sta, "EnergyReactiveImportRegister", cfg.StopTxnAlignedData.EnergyReactiveImportRegister);
        cJSON_AddBoolToObject(sta, "EnergyActiveExportInterval", cfg.StopTxnAlignedData.EnergyActiveExportInterval);
        cJSON_AddBoolToObject(sta, "EnergyActiveImportInterval", cfg.StopTxnAlignedData.EnergyActiveImportInterval);
        cJSON_AddBoolToObject(sta, "EnergyReactiveExportInterval", cfg.StopTxnAlignedData.EnergyReactiveExportInterval);
        cJSON_AddBoolToObject(sta, "EnergyReactiveImportInterval", cfg.StopTxnAlignedData.EnergyReactiveImportInterval);
        cJSON_AddBoolToObject(sta, "PowerActiveExport", cfg.StopTxnAlignedData.PowerActiveExport);
        cJSON_AddBoolToObject(sta, "PowerActiveImport", cfg.StopTxnAlignedData.PowerActiveImport);
        cJSON_AddBoolToObject(sta, "PowerOffered", cfg.StopTxnAlignedData.PowerOffered);
        cJSON_AddBoolToObject(sta, "PowerReactiveExport", cfg.StopTxnAlignedData.PowerReactiveExport);
        cJSON_AddBoolToObject(sta, "PowerReactiveImport", cfg.StopTxnAlignedData.PowerReactiveImport);
        cJSON_AddBoolToObject(sta, "PowerFactor", cfg.StopTxnAlignedData.PowerFactor);
        cJSON_AddBoolToObject(sta, "CurrentImport", cfg.StopTxnAlignedData.CurrentImport);
        cJSON_AddBoolToObject(sta, "CurrentExport", cfg.StopTxnAlignedData.CurrentExport);
        cJSON_AddBoolToObject(sta, "CurrentOffered", cfg.StopTxnAlignedData.CurrentOffered);
        cJSON_AddBoolToObject(sta, "Voltage", cfg.StopTxnAlignedData.Voltage);
        cJSON_AddBoolToObject(sta, "Frequency", cfg.StopTxnAlignedData.Frequency);
        cJSON_AddBoolToObject(sta, "Temperature", cfg.StopTxnAlignedData.Temperature);
        cJSON_AddBoolToObject(sta, "SoC", cfg.StopTxnAlignedData.SoC);
        cJSON_AddBoolToObject(sta, "RPM", cfg.StopTxnAlignedData.RPM);
        cJSON_AddItemToObject(json, "StopTxnAlignedData", sta);

        cJSON *sts = cJSON_CreateObject();
        cJSON_AddBoolToObject(sts, "EnergyActiveExportRegister", cfg.StopTxnSampledData.EnergyActiveExportRegister);
        cJSON_AddBoolToObject(sts, "EnergyActiveImportRegister", cfg.StopTxnSampledData.EnergyActiveImportRegister);
        cJSON_AddBoolToObject(sts, "EnergyReactiveExportRegister", cfg.StopTxnSampledData.EnergyReactiveExportRegister);
        cJSON_AddBoolToObject(sts, "EnergyReactiveImportRegister", cfg.StopTxnSampledData.EnergyReactiveImportRegister);
        cJSON_AddBoolToObject(sts, "EnergyActiveExportInterval", cfg.StopTxnSampledData.EnergyActiveExportInterval);
        cJSON_AddBoolToObject(sts, "EnergyActiveImportInterval", cfg.StopTxnSampledData.EnergyActiveImportInterval);
        cJSON_AddBoolToObject(sts, "EnergyReactiveExportInterval", cfg.StopTxnSampledData.EnergyReactiveExportInterval);
        cJSON_AddBoolToObject(sts, "EnergyReactiveImportInterval", cfg.StopTxnSampledData.EnergyReactiveImportInterval);
        cJSON_AddBoolToObject(sts, "PowerActiveExport", cfg.StopTxnSampledData.PowerActiveExport);
        cJSON_AddBoolToObject(sts, "PowerActiveImport", cfg.StopTxnSampledData.PowerActiveImport);
        cJSON_AddBoolToObject(sts, "PowerOffered", cfg.StopTxnSampledData.PowerOffered);
        cJSON_AddBoolToObject(sts, "PowerReactiveExport", cfg.StopTxnSampledData.PowerReactiveExport);
        cJSON_AddBoolToObject(sts, "PowerReactiveImport", cfg.StopTxnSampledData.PowerReactiveImport);
        cJSON_AddBoolToObject(sts, "PowerFactor", cfg.StopTxnSampledData.PowerFactor);
        cJSON_AddBoolToObject(sts, "CurrentImport", cfg.StopTxnSampledData.CurrentImport);
        cJSON_AddBoolToObject(sts, "CurrentExport", cfg.StopTxnSampledData.CurrentExport);
        cJSON_AddBoolToObject(sts, "CurrentOffered", cfg.StopTxnSampledData.CurrentOffered);
        cJSON_AddBoolToObject(sts, "Voltage", cfg.StopTxnSampledData.Voltage);
        cJSON_AddBoolToObject(sts, "Frequency", cfg.StopTxnSampledData.Frequency);
        cJSON_AddBoolToObject(sts, "Temperature", cfg.StopTxnSampledData.Temperature);
        cJSON_AddBoolToObject(sts, "SoC", cfg.StopTxnSampledData.SoC);
        cJSON_AddBoolToObject(sts, "RPM", cfg.StopTxnSampledData.RPM);
        cJSON_AddItemToObject(json, "StopTxnSampledData", sts);

        cJSON *profiles = cJSON_CreateObject();
        cJSON_AddBoolToObject(profiles, "Core", cfg.SupportedFeatureProfiles.Core);
        cJSON_AddBoolToObject(profiles, "LocalAuthListManagement", cfg.SupportedFeatureProfiles.LocalAuthListManagement);
        cJSON_AddBoolToObject(profiles, "Reservation", cfg.SupportedFeatureProfiles.Reservation);
        cJSON_AddBoolToObject(profiles, "RemoteTrigger", cfg.SupportedFeatureProfiles.RemoteTrigger);
        cJSON_AddBoolToObject(profiles, "FirmwareManagement", cfg.SupportedFeatureProfiles.FirmwareManagement);
        cJSON_AddBoolToObject(profiles, "SmartCharging", cfg.SupportedFeatureProfiles.SmartCharging);
        cJSON_AddItemToObject(json, "SupportedFeatureProfiles", profiles);

        char *data = cJSON_PrintUnformatted(json);
        if (data == NULL)
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        esp_err_t err = storage->write_config_ocpp(data);

        free(data);
        cJSON_Delete(json);

        return err;
    }

    esp_err_t OCPPController::read_localist(void)
    {
        size_t bufferSize = 4096;
        char *data = (char *)malloc(bufferSize);
        if (data == NULL)
            return ESP_FAIL;

        memset(data, 0, bufferSize);

        esp_err_t err = storage->read_localist(data, bufferSize);
        if (err != ESP_OK)
        {
            free(data);
            return err;
        }

        cJSON *json = cJSON_Parse(data);
        free(data);

        if (json == NULL)
            return ESP_FAIL;

        auto &cfg = CMSSendLocalListRequest;

        cfg.listVersion = cJSON_GetObjectItem(json, "listVersion")->valueint;
        strcpy(cfg.updateType, cJSON_GetObjectItem(json, "updateType")->valuestring);
        strcpy(cfg.UniqId, cJSON_GetObjectItem(json, "UniqId")->valuestring);
        cfg.Received = cJSON_GetObjectItem(json, "Received")->valueint;

        cJSON *listArray = cJSON_GetObjectItem(json, "localAuthorizationList");

        for (int i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            cJSON *item = cJSON_GetArrayItem(listArray, i);

            cfg.localAuthorizationList[i].idTagPresent =
                cJSON_GetObjectItem(item, "idTagPresent")->valueint;

            strcpy(cfg.localAuthorizationList[i].idTag,
                   cJSON_GetObjectItem(item, "idTag")->valuestring);

            cJSON *info = cJSON_GetObjectItem(item, "idTagInfo");

            strcpy(cfg.localAuthorizationList[i].idTagInfo.expiryDate,
                   cJSON_GetObjectItem(info, "expiryDate")->valuestring);

            strcpy(cfg.localAuthorizationList[i].idTagInfo.parentidTag,
                   cJSON_GetObjectItem(info, "parentidTag")->valuestring);

            strcpy(cfg.localAuthorizationList[i].idTagInfo.status,
                   cJSON_GetObjectItem(info, "status")->valuestring);
        }

        cJSON_Delete(json);
        return ESP_OK;
    }

    esp_err_t OCPPController::write_localist(void)
    {
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        auto &cfg = CMSSendLocalListRequest;

        cJSON_AddNumberToObject(json, "listVersion", cfg.listVersion);
        cJSON_AddStringToObject(json, "updateType", cfg.updateType);
        cJSON_AddStringToObject(json, "UniqId", cfg.UniqId);
        cJSON_AddBoolToObject(json, "Received", cfg.Received);

        cJSON *listArray = cJSON_CreateArray();

        for (int i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            cJSON *item = cJSON_CreateObject();
            cJSON_AddBoolToObject(item, "idTagPresent", cfg.localAuthorizationList[i].idTagPresent);
            cJSON_AddStringToObject(item, "idTag", cfg.localAuthorizationList[i].idTag);

            cJSON *info = cJSON_CreateObject();
            cJSON_AddStringToObject(info, "expiryDate", cfg.localAuthorizationList[i].idTagInfo.expiryDate);
            cJSON_AddStringToObject(info, "parentidTag", cfg.localAuthorizationList[i].idTagInfo.parentidTag);
            cJSON_AddStringToObject(info, "status", cfg.localAuthorizationList[i].idTagInfo.status);
            cJSON_AddItemToObject(item, "idTagInfo", info);
            cJSON_AddItemToArray(listArray, item);
        }

        cJSON_AddItemToObject(json, "localAuthorizationList", listArray);

        char *data = cJSON_PrintUnformatted(json);
        if (data == NULL)
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        esp_err_t err = storage->write_localist(data);

        free(data);
        cJSON_Delete(json);

        return err;
    }

    esp_err_t OCPPController::read_LocalAuthorizationList(void)
    {
        size_t bufferSize = 2048;
        char *data = (char *)malloc(bufferSize);
        if (data == NULL)
            return ESP_FAIL;

        memset(data, 0, bufferSize);

        esp_err_t err = storage->read_LocalAuthorizationList(data, bufferSize);
        if (err != ESP_OK)
        {
            free(data);
            return err;
        }

        cJSON *json = cJSON_Parse(data);
        free(data);

        if (json == NULL)
            return ESP_FAIL;

        auto &cfg = LocalAuthorizationList;

        cJSON *listArray = cJSON_GetObjectItem(json, "LocalAuthorizationList");

        for (int i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            cJSON *item = cJSON_GetArrayItem(listArray, i);

            strcpy(cfg.idTag[i],
                   cJSON_GetObjectItem(item, "idTag")->valuestring);

            cfg.idTagPresent[i] =
                cJSON_GetObjectItem(item, "idTagPresent")->valueint;
        }

        cJSON_Delete(json);
        return ESP_OK;
    }

    esp_err_t OCPPController::write_LocalAuthorizationList(void)
    {
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        auto &cfg = LocalAuthorizationList;

        cJSON *listArray = cJSON_CreateArray();

        for (int i = 0; i < LOCAL_LIST_COUNT; i++)
        {
            cJSON *item = cJSON_CreateObject();

            cJSON_AddStringToObject(item, "idTag", cfg.idTag[i]);
            cJSON_AddBoolToObject(item, "idTagPresent", cfg.idTagPresent[i]);

            cJSON_AddItemToArray(listArray, item);
        }

        cJSON_AddItemToObject(json, "LocalAuthorizationList", listArray);

        char *data = cJSON_PrintUnformatted(json);
        if (data == NULL)
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        esp_err_t err = storage->write_LocalAuthorizationList(data);

        free(data);
        cJSON_Delete(json);

        return err;
    }

    esp_err_t OCPPController::SetDefaultOcppConfig(void)
    {
        esp_err_t result = read_config_ocpp();
        switch (result)
        {
        case ESP_ERR_NOT_FOUND:
        case ESP_ERR_NVS_NOT_FOUND:
            memset(&CPGetConfigurationResponse, 0, sizeof(CPGetConfigurationResponse_t));
            setNULL(CPGetConfigurationResponse.AuthorizeRemoteTxRequestsValue);
            setNULL(CPGetConfigurationResponse.ClockAlignedDataIntervalValue);
            setNULL(CPGetConfigurationResponse.ConnectionTimeOutValue);
            setNULL(CPGetConfigurationResponse.GetConfigurationMaxKeysValue);
            setNULL(CPGetConfigurationResponse.HeartbeatIntervalValue);
            setNULL(CPGetConfigurationResponse.LocalAuthorizeOfflineValue);
            setNULL(CPGetConfigurationResponse.LocalPreAuthorizeValue);
            setNULL(CPGetConfigurationResponse.MeterValuesAlignedDataValue);
            setNULL(CPGetConfigurationResponse.MeterValuesSampledDataValue);
            setNULL(CPGetConfigurationResponse.MeterValueSampleIntervalValue);
            setNULL(CPGetConfigurationResponse.NumberOfConnectorsValue);
            setNULL(CPGetConfigurationResponse.ResetRetriesValue);
            setNULL(CPGetConfigurationResponse.ConnectorPhaseRotationValue);
            setNULL(CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectValue);
            setNULL(CPGetConfigurationResponse.StopTransactionOnInvalidIdValue);
            setNULL(CPGetConfigurationResponse.StopTxnAlignedDataValue);
            setNULL(CPGetConfigurationResponse.StopTxnSampledDataValue);
            setNULL(CPGetConfigurationResponse.SupportedFeatureProfilesValue);
            setNULL(CPGetConfigurationResponse.TransactionMessageAttemptsValue);
            setNULL(CPGetConfigurationResponse.TransactionMessageRetryIntervalValue);
            setNULL(CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectValue);
            setNULL(CPGetConfigurationResponse.AllowOfflineTxForUnknownIdValue);
            setNULL(CPGetConfigurationResponse.AuthorizationCacheEnabledValue);
            setNULL(CPGetConfigurationResponse.BlinkRepeatValue);
            setNULL(CPGetConfigurationResponse.LightIntensityValue);
            setNULL(CPGetConfigurationResponse.MaxEnergyOnInvalidIdValue);
            setNULL(CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue);
            setNULL(CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue);
            setNULL(CPGetConfigurationResponse.MinimumStatusDurationValue);
            setNULL(CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue);
            setNULL(CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue);
            setNULL(CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue);
            setNULL(CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue);
            setNULL(CPGetConfigurationResponse.WebSocketPingIntervalValue);
            setNULL(CPGetConfigurationResponse.LocalAuthListEnabledValue);
            setNULL(CPGetConfigurationResponse.LocalAuthListMaxLengthValue);
            setNULL(CPGetConfigurationResponse.SendLocalListMaxLengthValue);
            setNULL(CPGetConfigurationResponse.ReserveConnectorZeroSupportedValue);
            setNULL(CPGetConfigurationResponse.ChargeProfileMaxStackLevelValue);
            setNULL(CPGetConfigurationResponse.ChargingScheduleAllowedChargingRateUnitValue);
            setNULL(CPGetConfigurationResponse.ChargingScheduleMaxPeriodsValue);
            setNULL(CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedValue);
            setNULL(CPGetConfigurationResponse.MaxChargingProfilesInstalledValue);

            CPGetConfigurationResponse.AuthorizeRemoteTxRequestsReadOnly = false;
            CPGetConfigurationResponse.ClockAlignedDataIntervalReadOnly = false;
            CPGetConfigurationResponse.ConnectionTimeOutReadOnly = false;
            CPGetConfigurationResponse.GetConfigurationMaxKeysReadOnly = true;
            CPGetConfigurationResponse.HeartbeatIntervalReadOnly = false;
            CPGetConfigurationResponse.LocalAuthorizeOfflineReadOnly = false;
            CPGetConfigurationResponse.LocalPreAuthorizeReadOnly = false;
            CPGetConfigurationResponse.MeterValuesAlignedDataReadOnly = false;
            CPGetConfigurationResponse.MeterValuesSampledDataReadOnly = false;
            CPGetConfigurationResponse.MeterValueSampleIntervalReadOnly = false;
            CPGetConfigurationResponse.NumberOfConnectorsReadOnly = true;
            CPGetConfigurationResponse.ResetRetriesReadOnly = false;
            CPGetConfigurationResponse.ConnectorPhaseRotationReadOnly = false;
            CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectReadOnly = false;
            CPGetConfigurationResponse.StopTransactionOnInvalidIdReadOnly = false;
            CPGetConfigurationResponse.StopTxnAlignedDataReadOnly = false;
            CPGetConfigurationResponse.StopTxnSampledDataReadOnly = false;
            CPGetConfigurationResponse.SupportedFeatureProfilesReadOnly = true;
            CPGetConfigurationResponse.TransactionMessageAttemptsReadOnly = false;
            CPGetConfigurationResponse.TransactionMessageRetryIntervalReadOnly = false;
            CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectReadOnly = false;
            CPGetConfigurationResponse.AllowOfflineTxForUnknownIdReadOnly = false;
            CPGetConfigurationResponse.AuthorizationCacheEnabledReadOnly = false;
            CPGetConfigurationResponse.BlinkRepeatReadOnly = false;
            CPGetConfigurationResponse.LightIntensityReadOnly = false;
            CPGetConfigurationResponse.MaxEnergyOnInvalidIdReadOnly = false;
            CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthReadOnly = true;
            CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthReadOnly = true;
            CPGetConfigurationResponse.MinimumStatusDurationReadOnly = false;
            CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthReadOnly = true;
            CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthReadOnly = true;
            CPGetConfigurationResponse.StopTxnSampledDataMaxLengthReadOnly = true;
            CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthReadOnly = true;
            CPGetConfigurationResponse.WebSocketPingIntervalReadOnly = false;
            CPGetConfigurationResponse.LocalAuthListEnabledReadOnly = false;
            CPGetConfigurationResponse.LocalAuthListMaxLengthReadOnly = true;
            CPGetConfigurationResponse.SendLocalListMaxLengthReadOnly = true;
            CPGetConfigurationResponse.ReserveConnectorZeroSupportedReadOnly = true;
            CPGetConfigurationResponse.ChargeProfileMaxStackLevelReadOnly = true;
            CPGetConfigurationResponse.ChargingScheduleAllowedChargingRateUnitReadOnly = false;
            CPGetConfigurationResponse.ChargingScheduleMaxPeriodsReadOnly = true;
            CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedReadOnly = true;
            CPGetConfigurationResponse.MaxChargingProfilesInstalledReadOnly = true;

            memcpy(CPGetConfigurationResponse.AuthorizeRemoteTxRequestsValue, "false", strlen("false"));
            memcpy(CPGetConfigurationResponse.ClockAlignedDataIntervalValue, "600", strlen("600"));
            memcpy(CPGetConfigurationResponse.ConnectionTimeOutValue, "120", strlen("120"));
            memcpy(CPGetConfigurationResponse.GetConfigurationMaxKeysValue, "50", strlen("50"));
            memcpy(CPGetConfigurationResponse.HeartbeatIntervalValue, "30", strlen("30"));
            memcpy(CPGetConfigurationResponse.LocalAuthorizeOfflineValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.LocalPreAuthorizeValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.MeterValuesAlignedDataValue,
                   "Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature",
                   strlen("Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature"));
            memcpy(CPGetConfigurationResponse.MeterValuesSampledDataValue,
                   "Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature",
                   strlen("Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature"));
            memcpy(CPGetConfigurationResponse.MeterValueSampleIntervalValue, "30", strlen("30"));
            if (config->NumberOfConnectors == 1)
            {
                memcpy(CPGetConfigurationResponse.NumberOfConnectorsValue, "1", strlen("1"));
            }
            else if (config->NumberOfConnectors == 2)
            {
                memcpy(CPGetConfigurationResponse.NumberOfConnectorsValue, "2", strlen("2"));
            }
            else
            {
                memcpy(CPGetConfigurationResponse.NumberOfConnectorsValue, "3", strlen("3"));
            }
            memcpy(CPGetConfigurationResponse.ResetRetriesValue, "3", strlen("3"));
            memcpy(CPGetConfigurationResponse.ConnectorPhaseRotationValue, "NotApplicable", strlen("NotApplicable"));
            memcpy(CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.StopTransactionOnInvalidIdValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.StopTxnAlignedDataValue,
                   "Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature",
                   strlen("Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature"));
            memcpy(CPGetConfigurationResponse.StopTxnSampledDataValue,
                   "Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature",
                   strlen("Energy.Active.Import.Register, Power.Active.Import, Voltage, Current.Import, Temperature"));
            memcpy(CPGetConfigurationResponse.SupportedFeatureProfilesValue,
                   "Core,LocalAuthListManagement,Reservation,RemoteTrigger,FirmwareManagement,SmartCharging",
                   strlen("Core,LocalAuthListManagement,Reservation,RemoteTrigger,FirmwareManagement,SmartCharging"));
            memcpy(CPGetConfigurationResponse.TransactionMessageAttemptsValue, "2", strlen("2"));
            memcpy(CPGetConfigurationResponse.TransactionMessageRetryIntervalValue, "60", strlen("60"));
            memcpy(CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.AllowOfflineTxForUnknownIdValue, "false", strlen("false"));
            memcpy(CPGetConfigurationResponse.AuthorizationCacheEnabledValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.BlinkRepeatValue, "500", strlen("500"));
            memcpy(CPGetConfigurationResponse.LightIntensityValue, "100", strlen("100"));
            memcpy(CPGetConfigurationResponse.MaxEnergyOnInvalidIdValue, "0", strlen("0"));
            memcpy(CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue, "5", strlen("5"));
            memcpy(CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue, "5", strlen("5"));
            memcpy(CPGetConfigurationResponse.MinimumStatusDurationValue, "900", strlen("900"));
            memcpy(CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue, "1", strlen("1"));
            memcpy(CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue, "5", strlen("5"));
            memcpy(CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue, "5", strlen("5"));
            memcpy(CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue, "6", strlen("6"));
            memcpy(CPGetConfigurationResponse.WebSocketPingIntervalValue, "30", strlen("30"));
            memcpy(CPGetConfigurationResponse.LocalAuthListEnabledValue, "true", strlen("true"));
            memcpy(CPGetConfigurationResponse.LocalAuthListMaxLengthValue, "10", strlen("10"));
            memcpy(CPGetConfigurationResponse.SendLocalListMaxLengthValue, "10", strlen("10"));
            memcpy(CPGetConfigurationResponse.ReserveConnectorZeroSupportedValue, "false", strlen("false"));
            memcpy(CPGetConfigurationResponse.ChargeProfileMaxStackLevelValue, "100", strlen("100"));
            memcpy(CPGetConfigurationResponse.ChargingScheduleAllowedChargingRateUnitValue, "A", strlen("A"));
            memcpy(CPGetConfigurationResponse.ChargingScheduleMaxPeriodsValue, "20", strlen("20"));
            memcpy(CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedValue, "false", strlen("false"));
            memcpy(CPGetConfigurationResponse.MaxChargingProfilesInstalledValue, "100", strlen("100"));

            CPGetConfigurationResponse.AuthorizeRemoteTxRequests = false;
            CPGetConfigurationResponse.ClockAlignedDataInterval = 600;
            CPGetConfigurationResponse.ConnectionTimeOut = 120;
            CPGetConfigurationResponse.GetConfigurationMaxKeys = 50;
            CPGetConfigurationResponse.HeartbeatInterval = 30;
            CPGetConfigurationResponse.LocalAuthorizeOffline = true;
            CPGetConfigurationResponse.LocalPreAuthorize = true;
            CPGetConfigurationResponse.MeterValuesAlignedData.EnergyActiveImportRegister = true;
            CPGetConfigurationResponse.MeterValuesAlignedData.PowerActiveImport = true;
            CPGetConfigurationResponse.MeterValuesAlignedData.Voltage = true;
            CPGetConfigurationResponse.MeterValuesAlignedData.CurrentImport = true;
            CPGetConfigurationResponse.MeterValuesAlignedData.Temperature = true;
            CPGetConfigurationResponse.MeterValuesSampledData.EnergyActiveImportRegister = true;
            CPGetConfigurationResponse.MeterValuesSampledData.PowerActiveImport = true;
            CPGetConfigurationResponse.MeterValuesSampledData.Voltage = true;
            CPGetConfigurationResponse.MeterValuesSampledData.CurrentImport = true;
            CPGetConfigurationResponse.MeterValuesSampledData.Temperature = true;
            CPGetConfigurationResponse.MeterValueSampleInterval = 30;
            CPGetConfigurationResponse.NumberOfConnectors = config->NumberOfConnectors;
            CPGetConfigurationResponse.ResetRetries = 3;
            CPGetConfigurationResponse.StopTransactionOnEVSideDisconnect = true;
            CPGetConfigurationResponse.StopTransactionOnInvalidId = true;
            CPGetConfigurationResponse.StopTxnAlignedData.EnergyActiveImportRegister = true;
            CPGetConfigurationResponse.StopTxnAlignedData.PowerActiveImport = true;
            CPGetConfigurationResponse.StopTxnAlignedData.Voltage = true;
            CPGetConfigurationResponse.StopTxnAlignedData.CurrentImport = true;
            CPGetConfigurationResponse.StopTxnAlignedData.Temperature = true;
            CPGetConfigurationResponse.StopTxnSampledData.EnergyActiveImportRegister = true;
            CPGetConfigurationResponse.StopTxnSampledData.PowerActiveImport = true;
            CPGetConfigurationResponse.StopTxnSampledData.Voltage = true;
            CPGetConfigurationResponse.StopTxnSampledData.CurrentImport = true;
            CPGetConfigurationResponse.StopTxnSampledData.Temperature = true;
            CPGetConfigurationResponse.SupportedFeatureProfiles.Core = true;
            CPGetConfigurationResponse.SupportedFeatureProfiles.LocalAuthListManagement = true;
            CPGetConfigurationResponse.SupportedFeatureProfiles.Reservation = true;
            CPGetConfigurationResponse.SupportedFeatureProfiles.RemoteTrigger = true;
            CPGetConfigurationResponse.SupportedFeatureProfiles.FirmwareManagement = true;
            CPGetConfigurationResponse.SupportedFeatureProfiles.SmartCharging = false;
            CPGetConfigurationResponse.TransactionMessageAttempts = 2;
            CPGetConfigurationResponse.TransactionMessageRetryInterval = 60;
            CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnect = true;
            CPGetConfigurationResponse.AllowOfflineTxForUnknownId = false;
            CPGetConfigurationResponse.AuthorizationCacheEnabled = true;
            CPGetConfigurationResponse.BlinkRepeat = 500;
            CPGetConfigurationResponse.LightIntensity = 100;
            CPGetConfigurationResponse.MaxEnergyOnInvalidId = 0;
            CPGetConfigurationResponse.MeterValuesAlignedDataMaxLength = 5;
            CPGetConfigurationResponse.MeterValuesSampledDataMaxLength = 5;
            CPGetConfigurationResponse.MinimumStatusDuration = 900;
            CPGetConfigurationResponse.ConnectorPhaseRotationMaxLength = 1;
            CPGetConfigurationResponse.StopTxnAlignedDataMaxLength = 5;
            CPGetConfigurationResponse.StopTxnSampledDataMaxLength = 5;
            CPGetConfigurationResponse.SupportedFeatureProfilesMaxLength = 6;
            CPGetConfigurationResponse.WebSocketPingInterval = 30;
            CPGetConfigurationResponse.LocalAuthListEnabled = true;
            CPGetConfigurationResponse.LocalAuthListMaxLength = 10;
            CPGetConfigurationResponse.SendLocalListMaxLength = 10;
            CPGetConfigurationResponse.ReserveConnectorZeroSupported = false;
            CPGetConfigurationResponse.ChargeProfileMaxStackLevel = 100;
            CPGetConfigurationResponse.ChargingScheduleMaxPeriods = 20;
            CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupported = false;
            CPGetConfigurationResponse.MaxChargingProfilesInstalled = 100;

            write_config_ocpp();
            break;
        case ESP_OK:
            if (config->NumberOfConnectors == 1)
            {
                memcpy(CPGetConfigurationResponse.NumberOfConnectorsValue, "1", strlen("1"));
            }
            else if (config->NumberOfConnectors == 2)
            {
                memcpy(CPGetConfigurationResponse.NumberOfConnectorsValue, "2", strlen("2"));
            }
            else
            {
                memcpy(CPGetConfigurationResponse.NumberOfConnectorsValue, "3", strlen("3"));
            }

            break;
        default:
            ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
            break;
        }
        ESP_LOGI(TAG, "----------------------------------------------------------------------");
        ESP_LOGI(TAG, "OCPP Configuration Parameters");
        ESP_LOGI(TAG, "----------------------------------------------------------------------");

        ESP_LOGI(TAG, "AuthorizeRemoteTxRequestsValue : %s", CPGetConfigurationResponse.AuthorizeRemoteTxRequestsValue);
        ESP_LOGI(TAG, "ClockAlignedDataIntervalValue : %s", CPGetConfigurationResponse.ClockAlignedDataIntervalValue);
        ESP_LOGI(TAG, "ConnectionTimeOutValue : %s", CPGetConfigurationResponse.ConnectionTimeOutValue);
        ESP_LOGI(TAG, "GetConfigurationMaxKeysValue : %s", CPGetConfigurationResponse.GetConfigurationMaxKeysValue);
        ESP_LOGI(TAG, "HeartbeatIntervalValue : %s", CPGetConfigurationResponse.HeartbeatIntervalValue);
        ESP_LOGI(TAG, "LocalAuthorizeOfflineValue : %s", CPGetConfigurationResponse.LocalAuthorizeOfflineValue);
        ESP_LOGI(TAG, "LocalPreAuthorizeValue : %s", CPGetConfigurationResponse.LocalPreAuthorizeValue);
        ESP_LOGI(TAG, "MeterValuesAlignedDataValue : %s", CPGetConfigurationResponse.MeterValuesAlignedDataValue);
        ESP_LOGI(TAG, "MeterValuesSampledDataValue : %s", CPGetConfigurationResponse.MeterValuesSampledDataValue);
        ESP_LOGI(TAG, "MeterValueSampleIntervalValue : %s", CPGetConfigurationResponse.MeterValueSampleIntervalValue);
        ESP_LOGI(TAG, "NumberOfConnectorsValue : %s", CPGetConfigurationResponse.NumberOfConnectorsValue);
        ESP_LOGI(TAG, "ResetRetriesValue : %s", CPGetConfigurationResponse.ResetRetriesValue);
        ESP_LOGI(TAG, "ConnectorPhaseRotationValue : %s", CPGetConfigurationResponse.ConnectorPhaseRotationValue);
        ESP_LOGI(TAG, "StopTransactionOnEVSideDisconnectValue : %s", CPGetConfigurationResponse.StopTransactionOnEVSideDisconnectValue);
        ESP_LOGI(TAG, "StopTransactionOnInvalidIdValue : %s", CPGetConfigurationResponse.StopTransactionOnInvalidIdValue);
        ESP_LOGI(TAG, "StopTxnAlignedDataValue : %s", CPGetConfigurationResponse.StopTxnAlignedDataValue);
        ESP_LOGI(TAG, "StopTxnSampledDataValue : %s", CPGetConfigurationResponse.StopTxnSampledDataValue);
        ESP_LOGI(TAG, "SupportedFeatureProfilesValue : %s", CPGetConfigurationResponse.SupportedFeatureProfilesValue);
        ESP_LOGI(TAG, "TransactionMessageAttemptsValue : %s", CPGetConfigurationResponse.TransactionMessageAttemptsValue);
        ESP_LOGI(TAG, "TransactionMessageRetryIntervalValue : %s", CPGetConfigurationResponse.TransactionMessageRetryIntervalValue);
        ESP_LOGI(TAG, "UnlockConnectorOnEVSideDisconnectValue : %s", CPGetConfigurationResponse.UnlockConnectorOnEVSideDisconnectValue);
        ESP_LOGI(TAG, "AllowOfflineTxForUnknownIdValue : %s", CPGetConfigurationResponse.AllowOfflineTxForUnknownIdValue);
        ESP_LOGI(TAG, "AuthorizationCacheEnabledValue : %s", CPGetConfigurationResponse.AuthorizationCacheEnabledValue);
        ESP_LOGI(TAG, "BlinkRepeatValue : %s", CPGetConfigurationResponse.BlinkRepeatValue);
        ESP_LOGI(TAG, "LightIntensityValue : %s", CPGetConfigurationResponse.LightIntensityValue);
        ESP_LOGI(TAG, "MaxEnergyOnInvalidIdValue : %s", CPGetConfigurationResponse.MaxEnergyOnInvalidIdValue);
        ESP_LOGI(TAG, "MeterValuesAlignedDataMaxLengthValue : %s", CPGetConfigurationResponse.MeterValuesAlignedDataMaxLengthValue);
        ESP_LOGI(TAG, "MeterValuesSampledDataMaxLengthValue : %s", CPGetConfigurationResponse.MeterValuesSampledDataMaxLengthValue);
        ESP_LOGI(TAG, "MinimumStatusDurationValue : %s", CPGetConfigurationResponse.MinimumStatusDurationValue);
        ESP_LOGI(TAG, "ConnectorPhaseRotationMaxLengthValue : %s", CPGetConfigurationResponse.ConnectorPhaseRotationMaxLengthValue);
        ESP_LOGI(TAG, "StopTxnAlignedDataMaxLengthValue : %s", CPGetConfigurationResponse.StopTxnAlignedDataMaxLengthValue);
        ESP_LOGI(TAG, "StopTxnSampledDataMaxLengthValue : %s", CPGetConfigurationResponse.StopTxnSampledDataMaxLengthValue);
        ESP_LOGI(TAG, "SupportedFeatureProfilesMaxLengthValue : %s", CPGetConfigurationResponse.SupportedFeatureProfilesMaxLengthValue);
        ESP_LOGI(TAG, "WebSocketPingIntervalValue : %s", CPGetConfigurationResponse.WebSocketPingIntervalValue);
        ESP_LOGI(TAG, "LocalAuthListEnabledValue : %s", CPGetConfigurationResponse.LocalAuthListEnabledValue);
        ESP_LOGI(TAG, "LocalAuthListMaxLengthValue : %s", CPGetConfigurationResponse.LocalAuthListMaxLengthValue);
        ESP_LOGI(TAG, "SendLocalListMaxLengthValue : %s", CPGetConfigurationResponse.SendLocalListMaxLengthValue);
        ESP_LOGI(TAG, "ReserveConnectorZeroSupportedValue : %s", CPGetConfigurationResponse.ReserveConnectorZeroSupportedValue);
        ESP_LOGI(TAG, "ChargeProfileMaxStackLevelValue : %s", CPGetConfigurationResponse.ChargeProfileMaxStackLevelValue);
        ESP_LOGI(TAG, "ChargingScheduleAllowedChargingRateUnitValue : %s", CPGetConfigurationResponse.ChargingScheduleAllowedChargingRateUnitValue);
        ESP_LOGI(TAG, "ChargingScheduleMaxPeriodsValue : %s", CPGetConfigurationResponse.ChargingScheduleMaxPeriodsValue);
        ESP_LOGI(TAG, "ConnectorSwitch3to1PhaseSupportedValue : %s", CPGetConfigurationResponse.ConnectorSwitch3to1PhaseSupportedValue);
        ESP_LOGI(TAG, "MaxChargingProfilesInstalledValue : %s", CPGetConfigurationResponse.MaxChargingProfilesInstalledValue);
        ESP_LOGI(TAG, "----------------------------------------------------------------------\r\n");
        return ESP_OK;
    }

} // namespace OCPPModule