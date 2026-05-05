/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef INC_NVS_MODULE_H_
#define INC_NVS_MODULE_H_

#include <cstdint>
#include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

namespace NVSModule
{
    namespace Constants
    {

    } // namespace Constants

    class NVSStorage
    {
    public:
        NVSStorage();
        ~NVSStorage();
        esp_err_t ReadTimeJson(char *data, uint16_t dataBufferLen);
        esp_err_t WriteTimeJson(char *data);
        esp_err_t ReadConfigData(char **data);
        esp_err_t WriteConfigData(char *data);
        esp_err_t ReadConnectorModuleStatus(uint8_t ConnId, char **data);
        esp_err_t WriteConnectorModuleStatus(uint8_t ConnId, char *data);
        esp_err_t read_connectors_offline_data_count(uint8_t ConnId, uint32_t *offlineCount);
        esp_err_t write_connectors_offline_data_count(uint8_t ConnId, uint32_t offlineCount);
        esp_err_t read_connectors_offline_data(uint8_t ConnId, char *data, size_t size);
        esp_err_t write_connectors_offline_data(uint8_t ConnId, char *data);
        esp_err_t read_logFileNo(uint32_t *fileNumber);
        esp_err_t write_logFileNo(uint32_t fileNumber);
        esp_err_t read_config_ocpp(char *data);
        esp_err_t write_config_ocpp(char *data);
        esp_err_t read_localist(char *data);
        esp_err_t write_localist(char *data);
        esp_err_t read_LocalAuthorizationList(char *data);
        esp_err_t write_LocalAuthorizationList(char *data);

        esp_err_t read_chargingProfileCount(uint32_t *profileCount);
        esp_err_t write_chargingProfileCount(uint32_t profileCount);
        esp_err_t read_chargingProgile(char *data);
        esp_err_t write_chargingProgile(char *data);

    private:
        esp_err_t readFile(char *filename, char *data);
        esp_err_t readFile_(char *filename, char **data);
        esp_err_t writeFile(char *filename, char *data);
    };
} // namespace NVSModule

extern NVSModule::NVSStorage *storage;

#endif // __cplusplus
#endif /* INC_NVS_MODULE_H_ */