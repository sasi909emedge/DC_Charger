#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "DisplayModule.hpp"
#include "freertos/semphr.h"
#include <cstring>

namespace DisplayModule
{
    void DisplayController::SendToDisplay(uint8_t ConnId, PacketType packet)
    {
        uint8_t command[12];
        switch (packet)
        {
        case PacketType::COMMAND:
            command[0] = ConnId;
            std::memcpy(&command[2], dwin_buf, sizeof(dwin_buf));
            SendData(command, sizeof(dwin_buf) + 2);
            break;
        case PacketType::PAGE:
            command[0] = ConnId;
            std::memcpy(&command[2], dwin_page_buf, sizeof(dwin_page_buf));
            SendData(command, sizeof(dwin_page_buf) + 2);
            break;
        default:
            break;
        }
    }

    void DisplayController::Set_INITIALIZING(void)
    {
    }
    void DisplayController::Set_COMMISSIONING(void)
    {
    }
    void DisplayController::Set_FIRMWARE_UPDATE(void)
    {
    }
    void DisplayController::Set_RFID_TAPPED(void)
    {
    }
    void DisplayController::Set_AUTH_FAILED(void)
    {
    }
    void DisplayController::Set_AUTH_SUCCESS_PLUG_EV(void)
    {
    }
    void DisplayController::Set_AUTH_SUCCESS(void)
    {
    }
    void DisplayController::Set_NETWORKSWITCH(void)
    {
    }

    void DisplayController::Set_RESERVED(uint8_t ConnId)
    {
    }
    void DisplayController::Set_AVAILABLE(uint8_t ConnId)
    {
    }
    void DisplayController::Set_SUSPENDED(uint8_t ConnId)
    {
    }
    void DisplayController::Set_UNAVAILABLE(uint8_t ConnId)
    {
    }
    void DisplayController::Set_EV_PLUGGED_TAP_RFID(uint8_t ConnId)
    {
    }
    void DisplayController::Set_FINISHING(uint8_t ConnId)
    {
    }
    void DisplayController::Set_CHARGING(uint8_t ConnId)
    {
    }
    void DisplayController::Set_FAULT(uint8_t ConnId)
    {
    }

    void DisplayController::displayTask(void *pvParameters)
    {
        DisplayController *display = static_cast<DisplayController *>(pvParameters);
        connector->Set_INITIALIZING();
        bool connectorDataAvailable = false;
        while (1)
        {
            xSemaphoreTake(connector->counting_semaphore, portMAX_DELAY);
            for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
            {
                if ((connectorDataAvailable == false) &&
                    (connector->AVAILABLE[i] ||
                     connector->UNAVAILABLE[i] ||
                     connector->CHARGING[i] ||
                     connector->FINISHING[i] ||
                     connector->EV_PLUGGED_TAP_RFID[i] ||
                     connector->FAULT[i] ||
                     connector->RESERVED[i] ||
                     connector->SUSPENDED[i]))
                {
                    connectorDataAvailable = true;
                }
            }

            if (connector->INITIALIZING)
            {
                connector->INITIALIZING = false;
                display->Set_INITIALIZING();
            }
            else if (connector->COMMISSIONING)
            {
                connector->COMMISSIONING = false;
                display->Set_COMMISSIONING();
            }
            else if (connector->FIRMWARE_UPDATE)
            {
                connector->FIRMWARE_UPDATE = false;
                display->Set_FIRMWARE_UPDATE();
            }
            else if (connector->RFID_TAPPED)
            {
                connector->RFID_TAPPED = false;
                display->Set_RFID_TAPPED();
            }
            else if (connector->AUTH_FAILED)
            {
                connector->AUTH_FAILED = false;
                display->Set_AUTH_FAILED();
            }
            else if (connector->AUTH_SUCCESS_PLUG_EV)
            {
                connector->AUTH_SUCCESS_PLUG_EV = false;
                display->Set_AUTH_SUCCESS_PLUG_EV();
            }
            else if (connector->AUTH_SUCCESS)
            {
                connector->AUTH_SUCCESS = false;
                display->Set_AUTH_SUCCESS();
            }
            else if (connectorDataAvailable)
            {
                for (uint8_t i = 1; i <= connector->NumberOfConnectors; i++)
                {
                    if (connectorDataAvailable)
                    {
                        if (connector->RESERVED[i])
                        {
                            connector->RESERVED[i] = false;
                            display->Set_RESERVED(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->AVAILABLE[i])
                        {
                            connector->AVAILABLE[i] = false;
                            display->Set_AVAILABLE(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->SUSPENDED[i])
                        {
                            connector->SUSPENDED[i] = false;
                            display->Set_SUSPENDED(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->UNAVAILABLE[i])
                        {
                            connector->UNAVAILABLE[i] = false;
                            display->Set_UNAVAILABLE(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->EV_PLUGGED_TAP_RFID[i])
                        {
                            connector->EV_PLUGGED_TAP_RFID[i] = false;
                            display->Set_EV_PLUGGED_TAP_RFID(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->FINISHING[i])
                        {
                            connector->FINISHING[i] = false;
                            display->Set_FINISHING(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->CHARGING[i])
                        {
                            connector->CHARGING[i] = false;
                            display->Set_CHARGING(i);
                            connectorDataAvailable = false;
                        }
                        else if (connector->FAULT[i])
                        {
                            connector->FAULT[i] = false;
                            display->Set_FAULT(i);
                            connectorDataAvailable = false;
                        }
                    }
                }
            }
            else if (connector->NETWORKSWITCH)
            {
                connector->NETWORKSWITCH = false;
                display->Set_NETWORKSWITCH();
            }
        }
    }

    // Constructor
    DisplayController::DisplayController(SendDataFunc func)
    {
        xTaskCreate(&displayTask, "displayTask", 4096, this, 2, NULL);
        sendFunc = func;
    }

    // Destructor
    DisplayController::~DisplayController()
    {
        // Cleanup, if necessary
    }

    void DisplayController::SetSendFunction(SendDataFunc func)
    {
        sendFunc = func;
    }

    bool DisplayController::SendData(uint8_t *data, uint16_t length)
    {
        if (sendFunc)
        {
            return sendFunc(data, length);
        }
        return true; // or handle error
    }

    bool DisplayController::ReceiveData(uint8_t *data, uint16_t length)
    {

        return true; // or handle error
    }

} // namespace DisplayModule
