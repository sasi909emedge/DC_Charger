#include <stdio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <cstring>
#include "esp_random.h"
#include "nvs_flash.h"
#include "TonhePowerModule.hpp"
#include "PLCModule.hpp"
#include "SerialModule.hpp"
#include "CCSConnector.hpp"
#include "OCPPModule.hpp"
#include "NVSModule.hpp"
#include "ConfigModule.hpp"
#include "EnergyModule.hpp"
#include "NetworkModule.hpp"
#include "PowerMuxModule.hpp"
#define TAG "MAIN"

bool SerialReceive(const SerialModule::PacketId id, uint8_t *data, uint16_t length)
{
    uint8_t receivedData[length];
    std::memcpy(receivedData, data, length);
    ESP_LOGD("SerialReceive", "Received data: ");
    uint32_t CanId = 0;
    uint64_t CanData = 0;

    std::memcpy(&CanId, data, sizeof(CanId));
    std::memcpy(&CanData, &data[4], sizeof(CanData));

    if (pmc && id == SerialModule::PacketId::CAN2)
    {
        pmc->ReceiveData(CanId, CanData);
    }
    else if (connector && id == SerialModule::PacketId::CAN1)
    {
        plc->ReceiveData(CanId, CanData);
    }
    else if (id == SerialModule::PacketId::ENERGY)
    {
        // Energy->ReceiveData(data, length);
    }
    // else if (id == SerialModule::PacketId::RELAY)
    // {
    //     gpio->ReceiveData(CanId, CanData);
    // }
    // else if (id == SerialModule::PacketId::DISPLAY)
    // {
    //     // Handle display data
    // }
    // plc->ReceiveData(CanId, CanData);
    for (uint16_t i = 0; i < length; ++i)
    {
        ESP_LOGD("SerialReceive", "%02X ", receivedData[i]);
    }
    return true; // Placeholder for actual receive function
}

bool SendPowerModuleDataToSerial(const uint32_t id, const uint64_t data)
{
    uint8_t dataBuffer[12];
    std::memcpy(dataBuffer, &id, sizeof(id));
    std::memcpy(&dataBuffer[4], &data, sizeof(data));

    ESP_LOGD("Sending Power Module DataToSerial", "ID: %lu, Data: %llu", id, data);
    return serial->SendData(SerialModule::PacketId::CAN2, dataBuffer, sizeof(dataBuffer));
}

bool SendPLCModuleDataToSerial(const uint32_t id, const uint64_t data)
{
    uint8_t dataBuffer[12];
    std::memcpy(dataBuffer, &id, sizeof(id));
    std::memcpy(&dataBuffer[4], &data, sizeof(data));
    ESP_LOGD("Sending PLC Module DataToSerial", "ID: %lu, Data: %llu", id, data);
    return serial->SendData(SerialModule::PacketId::CAN1, dataBuffer, sizeof(dataBuffer));
}

bool SendConnectorDataToPowerMuxModule(const uint32_t id, const uint64_t data)
{

    return true;
}

bool SendPowerMuxModuleDataToConnector(const uint32_t id, const uint64_t data)
{

    return true;
}

bool ReceiveOCPPModuleData(const std::string jsonData)
{
    return ocpp->ReceiveData(jsonData);
}

bool SendEnergyModuleData(uint8_t *data, uint16_t length)
{
    return serial->SendData(SerialModule::PacketId::ENERGY, data, length);
}

bool SendGpioData(uint8_t id, uint16_t gpio, uint8_t state)
{
    uint8_t dataBuffer[4];

    dataBuffer[0] = id;

    std::memcpy(&dataBuffer[1], &gpio, sizeof(gpio));

    dataBuffer[3] = state;

    return serial->SendData(
        SerialModule::PacketId::RELAY,
        dataBuffer,
        sizeof(dataBuffer));
}

void TimerTask(void)
{
    ESP_LOGI(TAG, "Free Heap: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());
    // serial->SendHeartBeat();
}

extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    storage = new NVSModule::NVSStorage();
    SystemTime = new TimeModule::SystemTime();
    config = new ConfigModule::Configuration();
    serial = new SerialModule::SerialController(SerialReceive);
    gpio = new GpioModule::GpioController(SendGpioData);
    plc = new PLCModule::PLCController(SendPLCModuleDataToSerial);
    energy = new EnergyModule::EnergyController(SendEnergyModuleData);
    ocpp = new OCPPModule::OCPPController();
    pmc = new PowerModule::TonhePMController(SendPowerModuleDataToSerial);
    connector = new CCSConnector::CCSConnectorController(SendConnectorDataToPowerMuxModule);
    pmm = new PowerMuxModule::PowerMuxController(SendPowerMuxModuleDataToConnector);
    network = new NetworkModule::NetworkController(ReceiveOCPPModuleData);
    network->WebSocketPingInterval = ocpp->CPGetConfigurationResponse.WebSocketPingInterval;

    char *configData = NULL;
    if (storage->ReadConfigData(&configData) == ESP_OK)
    {
        ESP_LOGI(TAG, "Config JSON string: %s", configData);
        cJSON *json = cJSON_Parse(configData);
        (void)json;
        free(configData); // free immediately after parsing, cJSON has its own copy
        configData = NULL;
    }

    while (true)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        TimerTask();
    }
}
