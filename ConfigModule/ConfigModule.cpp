#include "ConfigModule.hpp"
#include "cJSON.h"
#include <cstring>
#include <cmath>

#define TAG "CONFIG"

ConfigModule::Configuration *config;
namespace ConfigModule
{
   // Constructor
    Configuration::Configuration()
    {
        //storage->Readconfig((uint8_t *)&config, sizeof(Config_t));
        // if (this->defaultConfig == false)`
        {
            memset(this->serialNumber, '\0', sizeof(this->serialNumber));
            memset(this->chargerName, '\0', sizeof(this->chargerName));
            memset(this->chargePointVendor, '\0', sizeof(this->chargePointVendor));
            memset(this->chargePointModel, '\0', sizeof(this->chargePointModel));
            memset(this->commissionedBy, '\0', sizeof(this->commissionedBy));
            memset(this->commissionedDate, '\0', sizeof(this->commissionedDate));
            memset(this->simIMEINumber, '\0', sizeof(this->simIMEINumber));
            memset(this->simIMSINumber, '\0', sizeof(this->simIMSINumber));
            memset(this->webSocketURL, '\0', sizeof(this->webSocketURL));
            memset(this->firmwareVersion, '\0', sizeof(this->firmwareVersion));
            memset(this->slavefirmwareVersion, '\0', sizeof(this->slavefirmwareVersion));
            memset(this->adminpassword, '\0', sizeof(this->adminpassword));
            memset(this->factorypassword, '\0', sizeof(this->factorypassword));
            memset(this->servicepassword, '\0', sizeof(this->servicepassword));
            memset(this->customerpassword, '\0', sizeof(this->customerpassword));
            memset(this->wifiSSID, '\0', sizeof(this->wifiSSID));
            memset(this->wifiPassword, '\0', sizeof(this->wifiPassword));
            memset(this->gsmAPN, '\0', sizeof(this->gsmAPN));
            memset(this->ipAddress, '\0', sizeof(this->ipAddress));
            memset(this->gatewayAddress, '\0', sizeof(this->gatewayAddress));
            memset(this->dnsAddress, '\0', sizeof(this->dnsAddress));
            memset(this->subnetMask, '\0', sizeof(this->subnetMask));
            memset(this->macAddress, '\0', sizeof(this->macAddress));
            memset(this->OtaURLConfig, '\0', sizeof(this->OtaURLConfig));
            memset(this->DiagnosticServerUrl, '\0', sizeof(this->DiagnosticServerUrl));

            memset(NumberOfSubSets, 0, sizeof(NumberOfSubSets));

            this->defaultConfig = false;
            this->chargerType = ChargerType::STANDALONE;
            memcpy(this->serialNumber, "Amplify", strlen("Amplify"));
            memcpy(this->chargerName, "Amplify", strlen("Amplify"));
            memcpy(this->chargePointVendor, "EMEDGE", strlen("EMEDGE"));
            memcpy(this->chargePointModel, "HALO", strlen("HALO"));
            memcpy(this->commissionedBy, "Kishore", strlen("Kishore"));
            memcpy(this->commissionedDate, "2025-06-02", strlen("2025-06-02"));
            memcpy(this->simIMEINumber, "864180056160042", strlen("864180056160042"));
            memcpy(this->simIMSINumber, "404920406446951", strlen("404920406446951"));
            memcpy(this->webSocketURL, "ws://emedge.pulseenergy.io/ws/OCPP16J/1000/CPK8IR6MV6", strlen("ws://emedge.pulseenergy.io/ws/OCPP16J/1000/CPK8IR6MV6"));
            memcpy(this->firmwareVersion, "1.0", strlen("1.0"));
            memcpy(this->slavefirmwareVersion, "1.0", strlen("1.0"));
            memcpy(this->adminpassword, "root", strlen("root"));
            memcpy(this->factorypassword, "root", strlen("root"));
            memcpy(this->servicepassword, "root", strlen("root"));
            memcpy(this->customerpassword, "root", strlen("root"));

            this->wifiEnable = true;
            this->wifiPriority = 3;

            memcpy(this->wifiSSID, "EMEDGE", strlen("EMEDGE"));
            memcpy(this->wifiPassword, "9490848535", strlen("9490848535"));

            this->gsmEnable = false;
            this->gsmPriority = 2;
            memcpy(this->gsmAPN, "airtelgprs.com", strlen("airtelgprs.com"));

            this->ethernetEnable = false;
            this->ethernetPriority = 1;
            this->ethernetConfig = EthernetConfigType::DHCP;
            memcpy(this->ipAddress, "192.168.0.1", strlen("192.168.0.1"));
            memcpy(this->gatewayAddress, "192.168.0.1", strlen("192.168.0.1"));
            memcpy(this->dnsAddress, "8.8.8.8", strlen("8.8.8.8"));
            memcpy(this->subnetMask, "255.255.255.0", strlen("255.255.255.0"));
            memcpy(this->macAddress, "10:97:bd:22:7e:00", strlen("10:97:bd:22:7e:00"));

            this->networkMode = NetworkMode::ONLINE_OFFLINE;
            this->chargingMode = ChargingMode::DC;

            this->ResumeSessionAfterPowerLoss = true;
            this->OtaUrlFromCMSEnable = false;
            memcpy(this->OtaURLConfig, "http://34.100.138.28:8080/ota/", strlen("http://34.100.138.28:8080/ota/"));

            this->CurrentGain = 1.0;
            this->CurrentOffset = 0.0;
            this->CurrentGain1 = 1.0;
            this->CurrentGain2 = 1.0;

            this->restoreSessionFromFault = true;
            this->restoreSessionFromFaultTime = 60;
            this->chargerModel = ChargerModel::DC30S;
            memset(&this->SuperSetMux, 0, sizeof(this->SuperSetMux));
            memset(&this->StackMux, 0, sizeof(this->StackMux));

            this->DCoverVoltageThreshold = 500.0;
            this->ACoverVoltageThreshold = 250.0;
            this->DCunderVoltageThreshold = 200.0;
            this->ACunderVoltageThreshold = 100.0;
            for (size_t i = 0; i < MAX_NUM_OF_CONNECTORS + 1; i++)
            {
                this->DCoverCurrentThreshold[i] = 100.0;
            }
            for (size_t i = 0; i < MAX_NUM_OF_CONNECTORS + 1; i++)
            {
                this->ACoverCurrentThreshold[i] = 33.0;
            }
            this->overTemperatureThreshold = 60.0;
            this->smartCharging = false;
            this->BatteryBackup = false;

            this->DiagnosticServer = false;
            memcpy(this->DiagnosticServerUrl, "http://34.100.138.28:8080/diagnostic/", strlen("http://34.100.138.28:8080/diagnostic/"));
            this->StopTransactionInSuspendedState = false;
            this->StopTransactionInSuspendedStateTime = 60 * 8;
            this->ALPREnable = false;
            this->NumOfAlprDevices = 0;
            memset(&this->macAddressofAlprDevice, 0, sizeof(this->macAddressofAlprDevice));
            memset(&this->serverSetCpDuty, 0, sizeof(this->serverSetCpDuty));
            this->NumberOfConnectors = NUM_OF_CONNECTORS;
            this->NumberOfDisplays = 1 ;

            for(uint8_t i = 0; i < MAX_NUM_OF_SUBSETS; i++)
            {
                for (size_t j = 0; j < 3; j++)
                {
                this->SubSetRelay[i][j] = true;
                }
            }

            //memset(this->ByteMacAddress, 0, sizeof(this->ByteMacAddress));
            for (size_t i = 0; i < 6; i++)
            {
                this->ByteMacAddress[i] = 00;
            }

            for(uint8_t i = 0; i < MAX_NUM_OF_SUBSETS; i++)
            {
                this->NumberOfSubSets[i].NumberOfConnectors = 2;
                for(uint8_t j = 0; j < 4; j++)
                {
                for (uint8_t k = 0; k < 2; k++)
                {
                    this->NumberOfSubSets[i].PowerModule[j][k].isActive = false;
                    this->NumberOfSubSets[i].PowerModule[j][k].moduleAddress = ((i*8)+(j*2)+k) + 1;
                    this->NumberOfSubSets[i].PowerModule[j][k].MaxVoltage = 1000;
                    this->NumberOfSubSets[i].PowerModule[j][k].MaxCurrent = 120;
                    this->NumberOfSubSets[i].PowerModule[j][k].MinVoltage = 50;
                    this->NumberOfSubSets[i].PowerModule[j][k].MinCurrent = 0.5;
                    this->NumberOfSubSets[i].PowerModule[j][k].MaxPower = 30;
                    this->NumberOfSubSets[i].PowerModule[j][k].MinPower = 0.025f;
                    this->NumberOfSubSets[i].PowerModule[j][k].MaxTemperature = 75;
                    this->NumberOfSubSets[i].PowerModule[j][k].MinTemperature = -30;
                }
                }
            }

        }
    }

    // Destructor
    Configuration::~Configuration()
    {
        // Cleanup, if necessary
    }

    esp_err_t Configuration::WriteConfigurationToFlash(void)
    {
        // write connnfig in jsonn and write to flash
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        // uint64_t can exceed double precision, but Unix timestamps fit safely
        if (!cJSON_AddNumberToObject(json, "defaultConfig", (double)defaultConfig))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "ChargerType", (int)chargerType))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "serialNumber", serialNumber))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "chargerName", chargerName))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "chargePointVendor", chargePointVendor))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "chargePointModel", chargePointModel))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "commissionedBy", commissionedBy))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "commissionedDate", commissionedDate))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "simIMEINumber", simIMEINumber))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "simIMSINumber", simIMSINumber))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "webSocketURL", webSocketURL))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "firmwareVersion", firmwareVersion))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "slavefirmwareVersion", slavefirmwareVersion))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "adminpassword", adminpassword))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "factorypassword", factorypassword))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "servicepassword", servicepassword))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "customerpassword", customerpassword))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "wifiEnable", wifiEnable))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "wifiPriority", wifiPriority))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "wifiSSID", wifiSSID))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "wifiPassword", wifiPassword))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "gsmEnable", gsmEnable))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "gsmPriority", gsmPriority))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "gsmAPN", gsmAPN))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "ethernetEnable", ethernetEnable))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "ethernetPriority", ethernetPriority))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "ethernetConfig", (int)ethernetConfig))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "ipAddress", ipAddress))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "gatewayAddress", gatewayAddress))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "dnsAddress", dnsAddress))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "subnetMask", subnetMask))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "macAddress", macAddress))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "networkMode", (int)networkMode))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "chargingMode", (int)chargingMode))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "ResumeSessionAfterPowerLoss", ResumeSessionAfterPowerLoss))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "OtaUrlFromCMSEnable", OtaUrlFromCMSEnable))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "OtaURLConfig", OtaURLConfig))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "CurrentGain", CurrentGain))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "CurrentOffset", CurrentOffset))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "CurrentGain1", CurrentGain1))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "CurrentGain2", CurrentGain2))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }            

        if (!cJSON_AddBoolToObject(json, "restoreSessionFromFault", restoreSessionFromFault))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "restoreSessionFromFaultTime", restoreSessionFromFaultTime))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "ChargerModel", (int)chargerModel))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "boardModel", (int)boardModel))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if(!cJSON_AddNumberToObject(json, "NumberOfDisplay", (int)NumberOfDisplays))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if(!cJSON_AddNumberToObject(json, "NumberOfConnectors", (int)NumberOfConnectors))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        cJSON *subsetRelayArray = cJSON_AddArrayToObject(json, "SubSetRelay");
        for (int i = 0; i < MAX_NUM_OF_SUBSETS; i++)
        {
            cJSON *row = cJSON_CreateArray();
            for (int j = 0; j < 3; j++)
            {
                cJSON_AddItemToArray(row, cJSON_CreateBool(SubSetRelay[i][j]));
            }
            cJSON_AddItemToArray(subsetRelayArray, row);
        }

        cJSON *superSetMuxArray = cJSON_AddArrayToObject(json, "SuperSetMux");
        for (int i = 0; i < MAX_NUM_OF_SUPERSETS; i++)
        {
            cJSON *row = cJSON_CreateArray();
            for (int j = 0; j < 4; j++)
            {
                cJSON_AddItemToArray(row, cJSON_CreateBool(SuperSetMux[i][j]));
            }
            cJSON_AddItemToArray(superSetMuxArray, row);
        }

        cJSON *stackMuxArray = cJSON_AddArrayToObject(json, "StackMux");
        for (int i = 0; i < 4; i++)
        {
            cJSON_AddItemToArray(stackMuxArray, cJSON_CreateBool(StackMux[i]));
        }

        if (!cJSON_AddNumberToObject(json, "DCoverVoltageThreshold", DCoverVoltageThreshold))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "ACoverVoltageThreshold", ACoverVoltageThreshold))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "DCunderVoltageThreshold", DCunderVoltageThreshold))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "ACunderVoltageThreshold", ACunderVoltageThreshold))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        cJSON *dcOverCurrentArray = cJSON_AddArrayToObject(json, "DCoverCurrentThreshold");
        for (int i = 0; i < MAX_NUM_OF_CONNECTORS + 1 ; i++)
        {
            cJSON_AddItemToArray(dcOverCurrentArray, cJSON_CreateNumber(DCoverCurrentThreshold[i]));
        }

        cJSON *acOverCurrentArray = cJSON_AddArrayToObject(json, "ACoverCurrentThreshold");
        for (int i = 0; i < MAX_NUM_OF_CONNECTORS + 1; i++)
        {
            cJSON_AddItemToArray(acOverCurrentArray, cJSON_CreateNumber(ACoverCurrentThreshold[i]));
        }

        if (!cJSON_AddNumberToObject(json, "overTemperatureThreshold", overTemperatureThreshold))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "smartCharging", smartCharging))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "BatteryBackup", BatteryBackup))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "DiagnosticServer", DiagnosticServer))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddStringToObject(json, "DiagnosticServerUrl", DiagnosticServerUrl))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        cJSON *byteMacArray = cJSON_AddArrayToObject(json, "ByteMacAddress");
        for (int i = 0; i < 6; i++)
        {
            cJSON_AddItemToArray(byteMacArray, cJSON_CreateNumber(ByteMacAddress[i]));
        }

        if (!cJSON_AddBoolToObject(json, "StopTransactionInSuspendedState", StopTransactionInSuspendedState))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddNumberToObject(json, "StopTransactionInSuspendedStateTime", StopTransactionInSuspendedStateTime))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        if (!cJSON_AddBoolToObject(json, "ALPREnable", ALPREnable))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }           

        if (!cJSON_AddNumberToObject(json, "NumOfAlprDevices", NumOfAlprDevices))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        cJSON *macAddrAlprArray = cJSON_AddArrayToObject(json, "macAddressofAlprDevice");
        for (int i = 0; i < 4; i++)
        {
            cJSON *device = cJSON_CreateArray();
            for (int j = 0; j < 6; j++)
            {
                cJSON_AddItemToArray(device, cJSON_CreateNumber(macAddressofAlprDevice[i][j]));
            }
            cJSON_AddItemToArray(macAddrAlprArray, device);
        }

        cJSON *serverCpDutyArray = cJSON_AddArrayToObject(json, "serverSetCpDuty");
        for (int i = 0; i < 4; i++)
        {
            cJSON_AddItemToArray(serverCpDutyArray, cJSON_CreateNumber(serverSetCpDuty[i]));
        }            

        // NumberOfSubsets - start
        cJSON *subsetsArray = cJSON_AddArrayToObject(json, "NumberOfSubSets");
        if (subsetsArray == NULL)
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        for (int i = 0; i < MAX_NUM_OF_SUBSETS; i++)
        {
            cJSON *subsetObj = cJSON_CreateObject();
            if (subsetObj == NULL)
            {
                cJSON_Delete(json);
                return ESP_FAIL;
            }

            cJSON_AddItemToArray(subsetsArray, subsetObj);

            if (!cJSON_AddNumberToObject(subsetObj, "NumberOfConnectors", NumberOfSubSets[i].NumberOfConnectors))
            {
                cJSON_Delete(json);
                return ESP_FAIL;
            }

            cJSON *powerModuleArray = cJSON_AddArrayToObject(subsetObj, "PowerModule");
            if (powerModuleArray == NULL)
            {
                cJSON_Delete(json);
                return ESP_FAIL;
            }

            for (int j = 0; j < 4; j++)
            {
                cJSON *moduleRow = cJSON_CreateArray();
                if (moduleRow == NULL)
                {
                cJSON_Delete(json);
                return ESP_FAIL;
                }

                cJSON_AddItemToArray(powerModuleArray, moduleRow);

                for (int k = 0; k < 2; k++)
                {
                cJSON *moduleObj = cJSON_CreateObject();
                if (moduleObj == NULL)
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                cJSON_AddItemToArray(moduleRow, moduleObj);

                PowerModuleConfig &pm = NumberOfSubSets[i].PowerModule[j][k];

                if (!cJSON_AddBoolToObject(moduleObj, "isActive", pm.isActive))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "moduleAddress", pm.moduleAddress))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MaxVoltage", pm.MaxVoltage))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MaxCurrent", pm.MaxCurrent))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MinVoltage", pm.MinVoltage))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MinCurrent", pm.MinCurrent))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MaxPower", pm.MaxPower))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MinPower", pm.MinPower))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MaxTemperature", pm.MaxTemperature))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }

                if (!cJSON_AddNumberToObject(moduleObj, "MinTemperature", pm.MinTemperature))
                {
                    cJSON_Delete(json);
                    return ESP_FAIL;
                }
                }
            }
        }

        char *jsonString = cJSON_PrintUnformatted(json);
        //print
        ESP_LOGI("TEST - CONFIG", "JSON: %s", jsonString);

        cJSON_Delete(json);

        storage->WriteConfigData(jsonString);

        if (jsonString == NULL)
            return ESP_FAIL;

        return ESP_OK;
    }

    esp_err_t Configuration::ReadConfigurationFromFlash(void)
    {
        char *configData = NULL;
        esp_err_t err = storage->ReadConfigData(&configData);

        if (err != ESP_OK)
        {
            free(configData);
            return err;
        }

        //TODO :remove later
        ESP_LOGI(TAG, "READ :: Config JSON string: %s", configData);

        cJSON *json = cJSON_Parse(configData);
        free(configData);
        configData = NULL;
        
        if (json == NULL)
        {
            ESP_LOGI(TAG," Invalid config loaded from Flash");
            return ESP_FAIL;
        }

        this->chargerType = (ChargerType)cJSON_GetObjectItem(json, "ChargerType")->valueint;
        strcpy(this->serialNumber, cJSON_GetObjectItem(json, "serialNumber")->valuestring);
        strcpy(this->chargerName, cJSON_GetObjectItem(json, "chargerName")->valuestring);
        strcpy(this->chargePointVendor, cJSON_GetObjectItem(json, "chargePointVendor")->valuestring);
        strcpy(this->chargePointModel, cJSON_GetObjectItem(json, "chargePointModel")->valuestring);
        strcpy(this->commissionedBy, cJSON_GetObjectItem(json, "commissionedBy")->valuestring);
        strcpy(this->commissionedDate, cJSON_GetObjectItem(json, "commissionedDate")->valuestring);
        strcpy(this->simIMEINumber, cJSON_GetObjectItem(json, "simIMEINumber")->valuestring);
        strcpy(this->simIMSINumber, cJSON_GetObjectItem(json, "simIMSINumber")->valuestring);
        strcpy(this->webSocketURL, cJSON_GetObjectItem(json, "webSocketURL")->valuestring);
        strcpy(this->firmwareVersion, cJSON_GetObjectItem(json, "firmwareVersion")->valuestring);
        strcpy(this->slavefirmwareVersion, cJSON_GetObjectItem(json, "slavefirmwareVersion")->valuestring);
        strcpy(this->adminpassword, cJSON_GetObjectItem(json, "adminpassword")->valuestring);
        strcpy(this->factorypassword, cJSON_GetObjectItem(json, "factorypassword")->valuestring);
        strcpy(this->servicepassword, cJSON_GetObjectItem(json, "servicepassword")->valuestring);
        strcpy(this->customerpassword, cJSON_GetObjectItem(json, "customerpassword")->valuestring);
        this->wifiEnable = cJSON_GetObjectItem(json, "wifiEnable")->valueint;
        this->wifiPriority = cJSON_GetObjectItem(json, "wifiPriority")->valueint;
        strcpy(this->wifiSSID, cJSON_GetObjectItem(json, "wifiSSID")->valuestring);
        strcpy(this->wifiPassword, cJSON_GetObjectItem(json, "wifiPassword")->valuestring);
        this->gsmEnable = cJSON_GetObjectItem(json, "gsmEnable")->valueint;
        this->gsmPriority = cJSON_GetObjectItem(json, "gsmPriority")->valueint;
        strcpy(this->gsmAPN, cJSON_GetObjectItem(json, "gsmAPN")->valuestring);
        this->ethernetEnable = cJSON_GetObjectItem(json, "ethernetEnable")->valueint;
        this->ethernetPriority = cJSON_GetObjectItem(json, "ethernetPriority")->valueint;
        this->ethernetConfig = (EthernetConfigType)cJSON_GetObjectItem(json, "ethernetConfig")->valueint;
        strcpy(this->ipAddress, cJSON_GetObjectItem(json, "ipAddress")->valuestring);
        strcpy(this->gatewayAddress, cJSON_GetObjectItem(json, "gatewayAddress")->valuestring);
        strcpy(this->dnsAddress, cJSON_GetObjectItem(json, "dnsAddress")->valuestring);
        strcpy(this->subnetMask, cJSON_GetObjectItem(json, "subnetMask")->valuestring);
        strcpy(this->macAddress, cJSON_GetObjectItem(json, "macAddress")->valuestring);
        this->networkMode = (NetworkMode)cJSON_GetObjectItem(json, "networkMode")->valueint;
        this->chargingMode = (ChargingMode)cJSON_GetObjectItem(json, "chargingMode")->valueint;
        this->ResumeSessionAfterPowerLoss = cJSON_GetObjectItem(json, "ResumeSessionAfterPowerLoss")->valueint;
        this->OtaUrlFromCMSEnable = cJSON_GetObjectItem(json, "OtaUrlFromCMSEnable")->valueint;
        strcpy(this->OtaURLConfig, cJSON_GetObjectItem(json, "OtaURLConfig")->valuestring);
        this->CurrentGain = cJSON_GetObjectItem(json, "CurrentGain")->valuedouble;
        this->CurrentOffset = cJSON_GetObjectItem(json, "CurrentOffset")->valuedouble;
        this->CurrentGain1 = cJSON_GetObjectItem(json, "CurrentGain1")->valuedouble;
        this->CurrentGain2 = cJSON_GetObjectItem(json, "CurrentGain2")->valuedouble;
        this->restoreSessionFromFault = cJSON_GetObjectItem(json, "restoreSessionFromFault")->valueint;
        this->restoreSessionFromFaultTime = cJSON_GetObjectItem(json, "restoreSessionFromFaultTime")->valueint;
        this->chargerModel = (ChargerModel)cJSON_GetObjectItem(json, "ChargerModel")->valueint;
        this->boardModel = (BoardModel)cJSON_GetObjectItem(json, "boardModel")->valueint;
        this->NumberOfDisplays = cJSON_GetObjectItem(json, "NumberOfDisplay")->valueint;
        this->NumberOfConnectors = cJSON_GetObjectItem(json, "NumberOfConnectors")->valueint;

        cJSON *subsetArray = cJSON_GetObjectItem(json, "NumberOfSubSets");
        for (int i = 0; i < MAX_NUM_OF_SUBSETS; i++)
        {
            cJSON *subsetObj = cJSON_GetArrayItem(subsetArray, i);
            if (subsetObj == NULL)
                continue;

            this->NumberOfSubSets[i].NumberOfConnectors = cJSON_GetObjectItem(subsetObj, "NumberOfConnectors")->valueint;

            cJSON *pmArray = cJSON_GetObjectItem(subsetObj, "PowerModule");

            for (int j = 0; j < 4; j++)
            {
                cJSON *row = cJSON_GetArrayItem(pmArray, j);
                if (row == NULL)
                continue;

                for (int k = 0; k < 2; k++)
                {
                cJSON *pm = cJSON_GetArrayItem(row, k);
                if (pm == NULL)
                    continue;

                this->NumberOfSubSets[i].PowerModule[j][k].isActive = cJSON_GetObjectItem(pm, "isActive")->valueint;
                this->NumberOfSubSets[i].PowerModule[j][k].moduleAddress = cJSON_GetObjectItem(pm, "moduleAddress")->valueint;
                this->NumberOfSubSets[i].PowerModule[j][k].MaxVoltage = cJSON_GetObjectItem(pm, "MaxVoltage")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MaxCurrent = cJSON_GetObjectItem(pm, "MaxCurrent")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MinVoltage = cJSON_GetObjectItem(pm, "MinVoltage")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MinCurrent = cJSON_GetObjectItem(pm, "MinCurrent")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MaxPower = cJSON_GetObjectItem(pm, "MaxPower")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MinPower = cJSON_GetObjectItem(pm, "MinPower")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MaxTemperature = cJSON_GetObjectItem(pm, "MaxTemperature")->valuedouble;
                this->NumberOfSubSets[i].PowerModule[j][k].MinTemperature = cJSON_GetObjectItem(pm, "MinTemperature")->valuedouble;
                }
            }
        }

        cJSON *relayArray = cJSON_GetObjectItem(json, "SubSetRelay");
        for (int i = 0; i < MAX_NUM_OF_SUBSETS; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                this->SubSetRelay[i][j] = cJSON_GetArrayItem(cJSON_GetArrayItem(relayArray, i), j)->valueint;
            }
        }

        cJSON *muxArray = cJSON_GetObjectItem(json, "SuperSetMux");
        for (int i = 0; i < MAX_NUM_OF_SUPERSETS; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                this->SuperSetMux[i][j] = cJSON_GetArrayItem(cJSON_GetArrayItem(muxArray, i), j)->valueint;
            }
        }

        cJSON *stackArray = cJSON_GetObjectItem(json, "StackMux");
        for (int i = 0; i < 4; i++)
        {
            this->StackMux[i] = cJSON_GetArrayItem(stackArray, i)->valueint;
        }

        this->DCoverVoltageThreshold = cJSON_GetObjectItem(json, "DCoverVoltageThreshold")->valuedouble;
        this->ACoverVoltageThreshold = cJSON_GetObjectItem(json, "ACoverVoltageThreshold")->valuedouble;
        this->DCunderVoltageThreshold = cJSON_GetObjectItem(json, "DCunderVoltageThreshold")->valuedouble;
        this->ACunderVoltageThreshold = cJSON_GetObjectItem(json, "ACunderVoltageThreshold")->valuedouble;

        cJSON *dcArray = cJSON_GetObjectItem(json, "DCoverCurrentThreshold");
        for (int i = 0; i < MAX_NUM_OF_CONNECTORS + 1; i++)
        {
            this->DCoverCurrentThreshold[i] = cJSON_GetArrayItem(dcArray, i)->valuedouble;
        }

        cJSON *acArray = cJSON_GetObjectItem(json, "ACoverCurrentThreshold");
        for (int i = 0; i < MAX_NUM_OF_CONNECTORS + 1; i++)
        {
            this->ACoverCurrentThreshold[i] = cJSON_GetArrayItem(acArray, i)->valuedouble;
        }

        this->overTemperatureThreshold = cJSON_GetObjectItem(json, "overTemperatureThreshold")->valuedouble;
        this->smartCharging = cJSON_GetObjectItem(json, "smartCharging")->valueint;
        this->BatteryBackup = cJSON_GetObjectItem(json, "BatteryBackup")->valueint;
        this->DiagnosticServer = cJSON_GetObjectItem(json, "DiagnosticServer")->valueint;
        strcpy(this->DiagnosticServerUrl, cJSON_GetObjectItem(json, "DiagnosticServerUrl")->valuestring);

        cJSON *macArray = cJSON_GetObjectItem(json, "ByteMacAddress");
        for (int i = 0; i < 6; i++)
        {
            this->ByteMacAddress[i] = cJSON_GetArrayItem(macArray, i)->valueint;
        }

        this->StopTransactionInSuspendedState = cJSON_GetObjectItem(json, "StopTransactionInSuspendedState")->valueint;
        this->StopTransactionInSuspendedStateTime = cJSON_GetObjectItem(json, "StopTransactionInSuspendedStateTime")->valueint;
        this->ALPREnable = cJSON_GetObjectItem(json, "ALPREnable")->valueint;
        this->NumOfAlprDevices = cJSON_GetObjectItem(json, "NumOfAlprDevices")->valueint;

        cJSON *alprArray = cJSON_GetObjectItem(json, "macAddressofAlprDevice");
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                this->macAddressofAlprDevice[i][j] = cJSON_GetArrayItem(cJSON_GetArrayItem(alprArray, i), j)->valueint;
            }
        }

        cJSON *cpArray = cJSON_GetObjectItem(json, "serverSetCpDuty");
        for (int i = 0; i < 4; i++)
        {
            this->serverSetCpDuty[i] = cJSON_GetArrayItem(cpArray, i)->valueint;
        }

        cJSON_Delete(json);
        return ESP_OK;
    }

}