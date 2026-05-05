#include <cstring>
#include <cmath>
#include "NVSModule.hpp"
#include "esp_spiffs.h"
#include "cJSON.h"

#define TAG "NVS"

#define PARTITION_NAME "Mynvs"

#define CONFIG_NAMESPACE "config"
#define CONFIG_DATA "configData"

#define CONNECTOR_NAMESPACE "Connector"
#define CONNECTOR_STATUS "ConnStatus"

#define TIME_BUFFER "timeBuffer"

#define LOG_FILENO "LOG_FNO"

#define OCPP_NAMESPACE "OCPP"
#define CONFIG_DATA_OCPP "configDataOcpp"
#define LOCAL_LIST_OCPP "localListOcpp"
#define LOCAL_AUTH_LIST_OCPP "LocalAuthList"

#define OFFLINE_DATA_NAMESPACE "offlineData"
#define OFFLINE_DATA_COUNT "ODatacount"

#define CHARGING_PROGILE_NAMESPACE "chargProfile"
#define CHARGING_PROGILE_COUNT "cpcount"

NVSModule::NVSStorage *storage;

namespace NVSModule
{

    // Constructor
    NVSStorage::NVSStorage()
    {
        nvs_flash_init_partition(PARTITION_NAME);
        if (esp_spiffs_mounted("MyStorage"))
        {
            ESP_LOGW(TAG, "SPIFFS already mounted, skipping registration");
        }
        else
        {
            esp_vfs_spiffs_conf_t conf = {
                .base_path = "/MyStorage",
                .partition_label = "MyStorage",
                .max_files = 20,
                .format_if_mount_failed = true,
            };

            esp_err_t ret = esp_vfs_spiffs_register(&conf);
            if (ret != ESP_OK)
            {
                if (ret == ESP_FAIL)
                    ESP_LOGE(TAG, "Failed to mount or format filesystem");
                else if (ret == ESP_ERR_NOT_FOUND)
                    ESP_LOGE(TAG, "Failed to find SPIFFS partition");
                else
                    ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
                return;
            }
            size_t total = 0, used = 0;
            ret = esp_spiffs_info(conf.partition_label, &total, &used);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
                esp_spiffs_format(conf.partition_label);
                return;
            }
            else
            {
                ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
            }
        }
    }

    // Destructor
    NVSStorage::~NVSStorage()
    {
        // Cleanup, if necessary
    }

    esp_err_t NVSStorage::readFile(char *filename, char *data)
    {
        FILE *file = fopen(filename, "r");
        if (file == NULL)
            return ESP_FAIL;

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        size_t bytesRead = fread(data, 1, size, file);
        fclose(file);

        if (bytesRead == 0)
            return ESP_FAIL;

        return ESP_OK;
    }

    esp_err_t NVSStorage::readFile_(char *filename, char **outData)
    {
        FILE *file = fopen(filename, "r");
        if (file == NULL)
            return ESP_FAIL;

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        *outData = (char *)malloc(size + 1);
        if (*outData == NULL)
        {
            fclose(file);
            return ESP_ERR_NO_MEM;
        }

        size_t bytesRead = fread(*outData, 1, size, file);
        (*outData)[bytesRead] = '\0';
        fclose(file);

        if (bytesRead == 0)
        {
            free(*outData);
            *outData = NULL;
            return ESP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t NVSStorage::writeFile(char *filename, char *data)
    {
        FILE *file = fopen(filename, "w");
        if (file == NULL)
        {
            return ESP_FAIL;
        }

        size_t len = strlen(data);
        size_t written = fwrite(data, 1, len, file);
        fclose(file);

        if (written != len)
            return ESP_FAIL;
        return ESP_OK;
    }

    esp_err_t NVSStorage::ReadTimeJson(char *data, uint16_t dataBufferLen)
    {
        FILE *file = fopen("/MyStorage/time.txt", "r");
        if (file == NULL)
            return ESP_FAIL;

        size_t bytesRead = fread(data, 1, dataBufferLen, file);
        fclose(file);

        if (bytesRead == 0)
            return ESP_FAIL;

        return ESP_OK;
    }

    esp_err_t NVSStorage::WriteTimeJson(char *data)
    {
        FILE *file = fopen("/MyStorage/time.txt", "w");
        if (file == NULL)
            return ESP_FAIL;

        size_t len = strlen(data);
        size_t written = fwrite(data, 1, len, file);
        fclose(file);

        if (written != len)
            return ESP_FAIL;

        return ESP_OK;
    }

    esp_err_t NVSStorage::ReadConfigData(char **data)
    {
        return readFile_("/MyStorage/config.txt", data);
    }

    esp_err_t NVSStorage::WriteConfigData(char *data)
    {
        return writeFile("/MyStorage/config.txt", data);
    }

    esp_err_t NVSStorage::ReadConnectorModuleStatus(uint8_t ConnId, char **data)
    {
        char fileName[30];
        memset(fileName, '\0', sizeof(fileName));
        snprintf(fileName, sizeof(fileName), "/MyStorage/connector_%hhu.txt", ConnId);

        return readFile_(fileName, data);
    }

    esp_err_t NVSStorage::WriteConnectorModuleStatus(uint8_t ConnId, char *data)
    {
        char fileName[30];
        memset(fileName, '\0', sizeof(fileName));
        snprintf(fileName, sizeof(fileName), "/MyStorage/connector_%hhu.txt", ConnId);

        return writeFile(fileName, data);
    }

    esp_err_t NVSStorage::read_connectors_offline_data_count(uint8_t ConnId, uint32_t *offlineCount)
    {
        char filename[50];
        memset(filename, '\0', sizeof(filename));
        snprintf(filename, sizeof(filename), "/MyStorage/ConOffDataCount%d.txt", ConnId);
        char data[100];
        esp_err_t err = readFile(filename, data);
        if (err != ESP_OK)
        {
            return err;
        }

        cJSON *json = cJSON_Parse(data);
        if (json == NULL)
            return ESP_FAIL;

        cJSON *countItem = cJSON_GetObjectItem(json, "count");
        if (!cJSON_IsNumber(countItem))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        *offlineCount = (uint32_t)countItem->valuedouble;

        cJSON_Delete(json);

        return ESP_OK;
    }

    esp_err_t NVSStorage::write_connectors_offline_data_count(uint8_t ConnId, uint32_t offlineCount)
    {
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        // uint64_t can exceed double precision, but Unix timestamps fit safely
        if (!cJSON_AddNumberToObject(json, "count", (double)offlineCount))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        char *data = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);

        if (data == NULL)
            return ESP_FAIL;

        char filename[50];
        memset(filename, '\0', sizeof(filename));
        snprintf(filename, sizeof(filename), "/MyStorage/ConOffDataCount%d.txt", ConnId);

        esp_err_t err = writeFile(filename, data);
        if (err != ESP_OK)
        {
            free(data);
            return err;
        }
        return ESP_OK;
    }

    esp_err_t NVSStorage::read_connectors_offline_data(uint8_t ConnId, char *data, size_t size)
    {
        char fileName[50];
        memset(fileName, '\0', sizeof(fileName));
        snprintf(fileName, sizeof(fileName), "/MyStorage/connector_%hhu.txt", ConnId);

        FILE *file = fopen(fileName, "r");
        if (file == NULL)
        {
            ESP_LOGD(TAG, "Failed to open connector status file for reading");
            return ESP_FAIL;
        }

        size_t bytesRead = fread(data, 1, size, file);
        fclose(file);

        if (bytesRead == 0)
        {
            ESP_LOGD(TAG, "Failed to read connector status data from file");
            return ESP_FAIL;
        }
        // TODO: check data is valid Json string
        return ESP_OK;
    }

    esp_err_t NVSStorage::write_connectors_offline_data(uint8_t ConnId, char data)
    {
        char fileName[50];
        memset(fileName, '\0', sizeof(fileName));
        snprintf(fileName, sizeof(fileName), "/MyStorage/connector_%hhu.txt", ConnId);

        FILE file = fopen(fileName, "r");

        char buffer = NULL;
        size_t oldSize = 0;
        size_t newSize = 0;

        if (file != NULL)
        {
            fseek(file, 0, SEEK_END);
            oldSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            newSize = oldSize + strlen(data) + 2; // adding ',' and '\0'

            buffer = (char)malloc(newSize);
            if (buffer == NULL)
            {
                fclose(file);
                return ESP_ERR_NO_MEM;
            }

            fread(buffer, 1, oldSize, file);
            fclose(file);

            Append new data
                buffer[oldSize - 1] = ',';
            memcpy(buffer + oldSize, data, strlen(data));
            buffer[newSize - 2] = ']';
            buffer[newSize - 1] = '\0';
        }
        else
        {

            size_t dataLen = strlen(data);

            size_t newSize = dataLen + 2;

            buffer = (char *)malloc(newSize);
            if (buffer == NULL)
            {
                return ESP_ERR_NO_MEM;

                buffer[newSize - 2] = ']';
                buffer[newSize - 1] = '\0';
            }

            buffer[0] = '[';

            memcpy(buffer + 1, data, dataLen);

            buffer[1 + dataLen] = ']';
        }

        return writeFile(filename, buffer);
    }

    esp_err_t NVSStorage::read_logFileNo(uint32_t *fileNumber)
    {
        char data[100];
        esp_err_t err = readFile("/MyStorage/logfileNumber.txt", data);
        if (err != ESP_OK)
        {
            return err;
        }

        cJSON *json = cJSON_Parse(data);
        if (json == NULL)
            return ESP_FAIL;

        cJSON *countItem = cJSON_GetObjectItem(json, "fileNumber");
        if (!cJSON_IsNumber(countItem))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        *fileNumber = (uint32_t)countItem->valuedouble;

        cJSON_Delete(json);

        return ESP_OK;
    }

    esp_err_t NVSStorage::write_logFileNo(uint32_t fileNumber)
    {
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        // uint64_t can exceed double precision, but Unix timestamps fit safely
        if (!cJSON_AddNumberToObject(json, "fileNumber", (double)fileNumber))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        char *data = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);

        if (data == NULL)
            return ESP_FAIL;

        esp_err_t err = writeFile("/MyStorage/logfileNumber.txt", data);
        if (err != ESP_OK)
        {
            free(data);
            return err;
        }
        return ESP_OK;
    }

    esp_err_t NVSStorage::read_config_ocpp(char *data)
    {
        return readFile("/MyStorage/ocppconfig.txt", data);
    }

    esp_err_t NVSStorage::write_config_ocpp(char *data)
    {
        return writeFile("/MyStorage/ocppconfig.txt", data);
    }

    esp_err_t NVSStorage::read_localist(char *data)
    {
        return readFile("/MyStorage/locallist.txt", data);
    }

    esp_err_t NVSStorage::write_localist(char *data)
    {
        return writeFile("/MyStorage/locallist.txt", data);
    }

    esp_err_t NVSStorage::read_LocalAuthorizationList(char *data)
    {
        return readFile("/MyStorage/localAuthlist.txt", data);
    }

    esp_err_t NVSStorage::write_LocalAuthorizationList(char *data)
    {
        return writeFile("/MyStorage/localAuthlist.txt", data);
    }

    esp_err_t NVSStorage::read_chargingProfileCount(uint32_t *profileCount)
    {
        char data[100];
        esp_err_t err = readFile("/MyStorage/chargingProfileCount.txt", data);
        if (err != ESP_OK)
        {
            return err;
        }

        cJSON *json = cJSON_Parse(data);
        if (json == NULL)
            return ESP_FAIL;

        cJSON *countItem = cJSON_GetObjectItem(json, "profileCount");
        if (!cJSON_IsNumber(countItem))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        *profileCount = (uint32_t)countItem->valuedouble;

        cJSON_Delete(json);

        return ESP_OK;
    }

    esp_err_t NVSStorage::write_chargingProfileCount(uint32_t profileCount)
    {
        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        // uint64_t can exceed double precision, but Unix timestamps fit safely
        if (!cJSON_AddNumberToObject(json, "profileCount", (double)profileCount))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        char *data = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);

        if (data == NULL)
            return ESP_FAIL;

        esp_err_t err = writeFile("/MyStorage/chargingProfileCount.txt", data);
        if (err != ESP_OK)
        {
            free(data);
            return err;
        }
        return ESP_OK;
    }

    esp_err_t NVSStorage::read_chargingProgile(char *data)
    {
        return readFile("/MyStorage/chargingProfile.txt", data);
    }

    esp_err_t NVSStorage::write_chargingProgile(char *data)
    {
        return writeFile("/MyStorage/chargingProfile.txt", data);
    }
}
