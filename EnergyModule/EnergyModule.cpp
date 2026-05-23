#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "EnergyModule.hpp"
#include <cstring>
#include <esp_log.h>

#define TAG "Energy"

EnergyModule::EnergyController *energy;

namespace EnergyModule
{

    void EnergyController::energyTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay
        EnergyController *energy = static_cast<EnergyController *>(pvParameters);
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    // Constructor
    EnergyController::EnergyController(SendDataFunc func)
    {
        xTaskCreate(&energyTask, "energyTask", 4096, this, 2, NULL);
        this->sendFunc = func;
        for (uint8_t i = 0; i < NUM_OF_CONNECTORS; i++)
        {
            memset(ACMeterValues, 0, sizeof(ACMeterValues));
            memset(ACMeterFaultValues, 0, sizeof(ACMeterFaultValues));
            memset(DCMeterValues, 0, sizeof(DCMeterValues));
            memset(DCMeterFaultValues, 0, sizeof(DCMeterFaultValues));
        }
        for (uint8_t i = 1; i < NUM_OF_CONNECTORS; i++)
        {
            SendEnergyConfig(i, MeterEnable::ENABLE, EnergyPacket::DC, 1000);
        }
    }

    // Destructor
    EnergyController::~EnergyController()
    {
    }

    void EnergyController::SetSendFunction(SendDataFunc func)
    {
        sendFunc = func;
    }

    bool EnergyController::SendData(uint8_t *data, uint16_t length)
    {
        if (sendFunc)
        {
            return sendFunc(data, length);
        }
        return true; // or handle error
    }

    bool EnergyController::ReceiveData(uint8_t *data, uint16_t length)
    {
        uint8_t ReceivedData[length];
        uint8_t ConnID = 0;
        std::memcpy(ReceivedData, data, sizeof(ReceivedData));
        if (ReceivedData[0] == static_cast<uint8_t>(EnergyPacket::AC))
        {
            ConnID = ReceivedData[1];
            if ((ConnID > 0) && (ConnID <= NUM_OF_CONNECTORS))
            {
                std::memcpy(&ACMeterValues[ConnID], &ReceivedData[2], sizeof(MeterValueAC));
            }
            else
            {
                return false;
            }
        }
        else if (ReceivedData[0] == static_cast<uint8_t>(EnergyPacket::DC))
        {
            ConnID = ReceivedData[1];
            if ((ConnID > 0) && (ConnID <= NUM_OF_CONNECTORS))
            {
                std::memcpy(&DCMeterValues[ConnID], &ReceivedData[2], sizeof(MeterValueDC));
                DCMeterValues[ConnID].voltage = (ReceivedData[2] * 256 + ReceivedData[3]) / 10;
                ESP_LOGI(TAG, "Received DC Meter Values for Connector ID: %d", ConnID);
                ESP_LOGI(TAG, "Voltage: %.2f, Current: %.2f, Power: %.2f, Temperature: %.2f",
                         DCMeterValues[ConnID].voltage,
                         DCMeterValues[ConnID].current,
                         DCMeterValues[ConnID].power,
                         DCMeterValues[ConnID].temperature);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        return true; // or handle error
    }

    bool EnergyController::SendEnergyConfig(uint8_t ConnID, MeterEnable Enable, EnergyPacket mode, uint16_t sampletimeinMillis)
    {
        // byte0 connectorId,
        // byte1 modbusId,
        // byte2 Enable/Disable
        // byte3 DC/AC
        // byte4&5 sample time
        // byte6&7 voltage register
        // byte8&9 voltage length
        // byte10&11 current register
        // byte12&13 current length
        // byte14&15 power register
        // byte16&17 power length

        uint8_t data[18];
        uint16_t voltageRegister = 40000 + 0x0131;
        uint16_t voltageRegisterLength = 1;
        uint16_t currentRegister = 40000 + 0x0139;
        uint16_t currentRegisterLength = 2;
        uint16_t powerRegister = 40000 + 0x0141;
        uint16_t powerRegisterLength = 2;

        data[0] = ConnID;
        data[1] = (ConnID == 0) ? 20 : ConnID;
        data[2] = static_cast<uint8_t>(Enable);
        data[3] = static_cast<uint8_t>(mode);
        data[4] = sampletimeinMillis & 0xFF;
        data[5] = (sampletimeinMillis >> 8) & 0xFF;
        data[6] = voltageRegister & 0xFF;
        data[7] = (voltageRegister >> 8) & 0xFF;
        data[8] = voltageRegisterLength & 0xFF;
        data[9] = (voltageRegisterLength >> 8) & 0xFF;
        data[10] = currentRegister & 0xFF;
        data[11] = (currentRegister >> 8) & 0xFF;
        data[12] = currentRegisterLength & 0xFF;
        data[13] = (currentRegisterLength >> 8) & 0xFF;
        data[14] = powerRegister & 0xFF;
        data[15] = (powerRegister >> 8) & 0xFF;
        data[16] = powerRegisterLength & 0xFF;
        data[17] = (powerRegisterLength >> 8) & 0xFF;

        return SendData(data, sizeof(data));
    }

    MeterValueAC EnergyController::GetACEnergyMeterValue(uint8_t ConnID)
    {
        MeterValueAC MeterValue;
        std::memcpy(&MeterValue, &ACMeterValues[ConnID], sizeof(MeterValue));
        return MeterValue;
    }
    MeterValueDC EnergyController::GetDCEnergyMeterValue(uint8_t ConnID)
    {
        MeterValueDC MeterValue;
        std::memcpy(&MeterValue, &DCMeterValues[ConnID], sizeof(MeterValue));
        return MeterValue;
    }

} // namespace EnergyModule
