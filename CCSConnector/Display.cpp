#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "CCSConnector.hpp"

namespace CCSConnector
{
    void Display::Set_INITIALIZING(void)
    {
        if (INITIALIZING == false)
        {
            INITIALIZING = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_RFID_TAPPED(void)
    {
        if (RFID_TAPPED == false)
        {
            RFID_TAPPED = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_AUTH_SUCCESS_PLUG_EV(void)
    {
        if (AUTH_SUCCESS_PLUG_EV == false)
        {
            AUTH_SUCCESS_PLUG_EV = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_AUTH_FAILED(void)
    {
        if (AUTH_FAILED == false)
        {
            AUTH_FAILED = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_AUTH_SUCCESS(void)
    {
        if (AUTH_SUCCESS == false)
        {
            AUTH_SUCCESS = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_FIRMWARE_UPDATE(void)
    {
        if (FIRMWARE_UPDATE == false)
        {
            FIRMWARE_UPDATE = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_COMMISSIONING(void)
    {
        if (COMMISSIONING == false)
        {
            COMMISSIONING = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_NETWORKSWITCH(void)
    {
        if (NETWORKSWITCH == false)
        {
            NETWORKSWITCH = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_AVAILABLE(uint8_t ConnID)
    {
        if (AVAILABLE[ConnID] == false)
        {
            AVAILABLE[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_UNAVAILABLE(uint8_t ConnID)
    {
        if (UNAVAILABLE[ConnID] == false)
        {
            UNAVAILABLE[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_CHARGING(uint8_t ConnID)
    {
        if (CHARGING[ConnID] == false)
        {
            CHARGING[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_FINISHING(uint8_t ConnID)
    {
        if (FINISHING[ConnID] == false)
        {
            FINISHING[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_EV_PLUGGED_TAP_RFID(uint8_t ConnID)
    {
        if (EV_PLUGGED_TAP_RFID[ConnID] == false)
        {
            EV_PLUGGED_TAP_RFID[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_FAULT(uint8_t ConnID)
    {
        if (FAULT[ConnID] == false)
        {
            FAULT[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_RESERVED(uint8_t ConnID)
    {
        if (RESERVED[ConnID] == false)
        {
            RESERVED[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

    void Display::Set_SUSPENDED(uint8_t ConnID)
    {
        if (SUSPENDED[ConnID] == false)
        {
            SUSPENDED[ConnID] = true;
            xSemaphoreGive(counting_semaphore);
        }
    }

}