#include <cstring>
#include <cmath>
#include "TimeModule.hpp"
#include "cJSON.h"

#define TAG "TIME"

TimeModule::SystemTime *SystemTime;

namespace TimeModule
{
    // Constructor
    SystemTime::SystemTime()
    {
        setenv("TZ", "UTC", 1);
        tzset();
        time_t time;
        if (getTimeFromFlash(&time) != ESP_OK)
        {
            time = static_cast<time_t>(1773037672);
        }
        setTime(time);
    }

    // Destructor
    SystemTime::~SystemTime()
    {
        // Cleanup, if necessary
    }

    esp_err_t SystemTime::setTime(time_t time)
    {
        struct timeval tv;
        tv.tv_sec = time;
        tv.tv_usec = 0;

        if (settimeofday(&tv, NULL) != 0)
        {
            ESP_LOGE(TAG, "Failed to set system time");
            return ESP_FAIL;
        }
        ESP_LOGD(TAG, "Set Time: %lld", tv.tv_sec);

        return ESP_OK;
    }

    esp_err_t SystemTime::writeCurrentTimeToFlash(void)
    {
        time_t time;
        esp_err_t ret = getTime(&time);
        if (ret != ESP_OK)
            return ret;

        cJSON *json = cJSON_CreateObject();
        if (json == NULL)
            return ESP_FAIL;

        // uint64_t can exceed double precision, but Unix timestamps fit safely
        if (!cJSON_AddNumberToObject(json, "time", (double)time))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        char *jsonString = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);

        if (jsonString == NULL)
            return ESP_FAIL;

        ret = storage->WriteTimeJson(jsonString);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to write current time to flash");
            free(jsonString);
            return ret;
        }
        free(jsonString);

        ESP_LOGD(TAG, "Time written to flash: %lld", (uint64_t)time);
        currentTime = time;
        return ESP_OK;
    }

    esp_err_t SystemTime::getTime(time_t *time)
    {
        if (time == NULL)
            return ESP_ERR_INVALID_ARG;

        struct timeval tv;
        if (gettimeofday(&tv, NULL) != 0)
        {
            ESP_LOGE(TAG, "Failed to get system time");
            return ESP_FAIL;
        }

        *time = static_cast<time_t>(tv.tv_sec);
        return ESP_OK;
    }

    /*
    Time Storing Format in json
    {"time" : 1773037571}
    */
    esp_err_t SystemTime::getTimeFromFlash(time_t *time)
    {
        if (time == NULL)
            return ESP_ERR_INVALID_ARG;

        char *data = NULL;
        esp_err_t ret = storage->ReadTimeJson(&data);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to read time from flash");
            return ret;
        }

        cJSON *json = cJSON_Parse(data);
        free(data);
        if (json == NULL)
            return ESP_FAIL;

        // Extract the "time" field
        cJSON *timeItem = cJSON_GetObjectItem(json, "time");
        if (!cJSON_IsNumber(timeItem))
        {
            cJSON_Delete(json);
            return ESP_FAIL;
        }

        uint64_t time64 = (uint64_t)timeItem->valuedouble; // use valuedouble for large integers

        cJSON_Delete(json); // Free the JSON object

        *time = static_cast<time_t>(time64);
        ESP_LOGD(TAG, "Time read from flash: %lld", time64);
        return ESP_OK;
    }

    esp_err_t SystemTime::getTimeInOcppFormat(char *timeString)
    {
        struct timeval tv;
        struct tm timeinfo;
        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &timeinfo);
        std::memset(timeString, 0, 50);
        snprintf(timeString, 50,
                 "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
                 (timeinfo.tm_year + 1900),
                 timeinfo.tm_mon + 1,
                 timeinfo.tm_mday,
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec,
                 tv.tv_usec / 1000);
        return ESP_OK;
    }

    esp_err_t SystemTime::setTimeFromOcppFormat(const char *timeString)
    {
        ESP_LOGD(TAG, "Set Time in OCPP Format %s", timeString);
        struct tm tm_time = {};
        time_t unix_time;
        sscanf(timeString, "%d-%d-%dT%d:%d:%d.%*3sZ",
               &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
               &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec);

        tm_time.tm_year -= 1900; // Adjust year
        tm_time.tm_mon -= 1;     // Adjust month

        unix_time = mktime(&tm_time);
        setTime(unix_time);
        return ESP_OK;
    }

    uint64_t SystemTime::GetNumofSecondsFromOcppTimeString(char *timeString)
    {
        struct tm tm_time = {};
        time_t unix_time;
        sscanf(timeString, "%d-%d-%dT%d:%d:%d.%*3sZ",
               &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
               &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec);

        tm_time.tm_year -= 1900; // Adjust year
        tm_time.tm_mon -= 1;     // Adjust month

        unix_time = mktime(&tm_time);
        return static_cast<uint64_t>(unix_time);
    }

    uint64_t SystemTime::Get_NumofSecondsFromCurrentTime(void)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return static_cast<uint64_t>(tv.tv_sec);
    }

    bool SystemTime::isTimeExpired(char *timestring)
    {
        if (Get_NumofSecondsFromCurrentTime() > GetNumofSecondsFromOcppTimeString(timestring))
            return true;
        return false;
    }

}
