#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "GpioModule.hpp"
#include <driver/gpio.h>
#include <cstring>

GpioModule::GpioController *gpio;

#define TAG "GPIO"

namespace GpioModule
{

    static bool IsValidEspOutputGPIO(uint16_t pin)
    {
        if (pin > 39)
        {
            ESP_LOGE(TAG, "Invalid GPIO number: %d", pin);
            return false;
        }

        if (pin >= 34 && pin <= 39)
        {
            ESP_LOGE(TAG, "GPIO %d is input only, cannot configure OUTPUT", pin);
            return false;
        }

        return true;
    }

    static bool IsValidEspInputGPIO(uint16_t pin)
    {
        if (pin > 39)
        {
            ESP_LOGE(TAG, "Invalid GPIO number: %d", pin);
            return false;
        }

        return true;
    }
    // Constructor
    GpioController::GpioController(SendDataFunc func)
    {
        this->sendFunc = func;
        gpioList[0] = &StmReset;
        gpioList[1] = &ModemReset;
        gpioList[2] = &DcGun[1];
        gpioList[3] = &DcGun[2];
        gpioList[4] = &DcMerger[1];
        gpioList[5] = &DcMerger[2];
        gpioList[6] = &DcMerger[3];
        gpioList[7] = &AcPm[1];
        gpioList[8] = &AcPm[2];
        gpioList[9] = &AcPm[3];
        gpioList[10] = &AcPm[4];
        gpioList[11] = &V12Control;
        gpioList[12] = &BatteryControl;
        gpioList[13] = &VentilationFan[1];
        gpioList[14] = &VentilationFan[2];
        gpioList[15] = &EmergencyBtn;
        gpioList[16] = &EarthFault;
        gpioList[17] = &Door;
        gpioList[18] = &SpdFault;
        gpioList[19] = &SmokeFault;
        gpioList[20] = &DcGunNegFb[1];
        gpioList[21] = &DcGunPosFb[1];
        gpioList[22] = &DcGunIsoFb[1];
        gpioList[23] = &DcGunNegFb[2];
        gpioList[24] = &DcGunPosFb[2];
        gpioList[25] = &DcGunIsoFb[2];
        gpioList[26] = &AcPmFb[1];
        gpioList[27] = &AcPmFb[2];
        gpioList[28] = &AcPmFb[3];
        gpioList[29] = &AcPmFb[4];
        gpioList[30] = &DcMergerPosFb[1];
        gpioList[31] = &DcMergerNegFb[1];
        gpioList[32] = &DcMergerPosFb[2];
        gpioList[33] = &DcMergerNegFb[2];
        gpioList[34] = &DcMergerPosFb[3];
        gpioList[35] = &DcMergerNegFb[3];
        gpioList[36] = &DcGunFuseFb[1];
        gpioList[37] = &DcGunFuseFb[2];

        for (uint8_t i = 0; i < 38; i++)
        {
            gpioList[i]->isAvailable = true;
            gpioList[i]->isInput = true;
            gpioList[i]->isDirectGpio = false;
            gpioList[i]->isActiveLow = false;
            gpioList[i]->state = GpioState::INVALID;
            gpioList[i]->gpioNum = 0;
            gpioList[i]->gpioPort = 0;
            memset(gpioList[i]->name, 0, sizeof(gpioList[i]->name));
        }

        StmReset.isInput = false;
        StmReset.isDirectGpio = true;
        StmReset.gpioNum = StmReset_Pin;
        strcpy(StmReset.name, "StmReset");

        ModemReset.isInput = false;
        ModemReset.isDirectGpio = true;
        ModemReset.gpioNum = ModemReset_Pin;
        strcpy(ModemReset.name, "ModemReset");

        DcGun[0].isAvailable = false;

        DcGun[1].isInput = false;
        DcGun[1].gpioNum = DcGun1_Pin;
        DcGun[1].gpioPort = DcGun1_GPIO_Port;
        strcpy(DcGun[1].name, "DC Gun1 Contactor");

        DcGun[2].isInput = false;
        DcGun[2].gpioNum = DcGun2_Pin;
        DcGun[2].gpioPort = DcGun2_GPIO_Port;
        strcpy(DcGun[2].name, "DC Gun2 Contactor");

        DcMerger[0].isAvailable = false;

        DcMerger[1].isInput = false;
        DcMerger[1].gpioNum = DcMerger1_Pin;
        DcMerger[1].gpioPort = DcMerger1_GPIO_Port;
        strcpy(DcMerger[1].name, "DC Merger1 Contactor");

        DcMerger[2].isInput = false;
        DcMerger[2].gpioNum = DcMerger2_Pin;
        DcMerger[2].gpioPort = DcMerger2_GPIO_Port;
        strcpy(DcMerger[2].name, "DC Merger2 Contactor");

        DcMerger[3].isInput = false;
        DcMerger[3].gpioNum = DcMerger3_Pin;
        DcMerger[3].gpioPort = DcMerger3_GPIO_Port;
        strcpy(DcMerger[3].name, "DC Merger3 Contactor");

        AcPm[0].isAvailable = false;

        AcPm[1].isInput = false;
        AcPm[1].gpioNum = AcPm1_Pin;
        AcPm[1].gpioPort = AcPm1_GPIO_Port;
        strcpy(AcPm[1].name, "AC PM1 Contactor");

        AcPm[2].isInput = false;
        AcPm[2].gpioNum = AcPm2_Pin;
        AcPm[2].gpioPort = AcPm2_GPIO_Port;
        strcpy(AcPm[2].name, "AC PM2 Contactor");

        AcPm[3].isInput = false;
        AcPm[3].gpioNum = AcPm3_Pin;
        AcPm[3].gpioPort = AcPm3_GPIO_Port;
        strcpy(AcPm[3].name, "AC PM3 Contactor");

        AcPm[4].isInput = false;
        AcPm[4].gpioNum = AcPm4_Pin;
        AcPm[4].gpioPort = AcPm4_GPIO_Port;
        strcpy(AcPm[4].name, "AC PM4 Contactor");

        V12Control.isInput = false;
        V12Control.gpioNum = V12Control_Pin;
        V12Control.gpioPort = V12Control_GPIO_Port;
        strcpy(V12Control.name, "V12 Control");

        BatteryControl.isInput = false;
        BatteryControl.gpioNum = BatteryControl_Pin;
        BatteryControl.gpioPort = BatteryControl_GPIO_Port;
        strcpy(BatteryControl.name, "Battery Control");

        VentilationFan[0].isAvailable = false;

        VentilationFan[1].isInput = false;
        VentilationFan[1].gpioNum = VentilationFan1_Pin;
        VentilationFan[1].gpioPort = VentilationFan1_GPIO_Port;
        strcpy(VentilationFan[1].name, "Ventilation Fan 1");

        VentilationFan[2].isInput = false;
        VentilationFan[2].gpioNum = VentilationFan2_Pin;
        VentilationFan[2].gpioPort = VentilationFan2_GPIO_Port;
        strcpy(VentilationFan[2].name, "Ventilation Fan 2");

        EmergencyBtn.isDirectGpio = true;
        EmergencyBtn.gpioNum = EmergencyBtn_Pin;
        strcpy(EmergencyBtn.name, "Emergency Button");

        EarthFault.isDirectGpio = true;
        EarthFault.gpioNum = EarthFault_Pin;
        strcpy(EarthFault.name, "Earth Fault");

        Door.gpioNum = Door_Pin;
        Door.gpioPort = Door_GPIO_Port;
        strcpy(Door.name, "Door");

        SpdFault.isDirectGpio = true;
        SpdFault.gpioNum = SpdFault_Pin;
        strcpy(SpdFault.name, "SPD Fault");

        SmokeFault.isDirectGpio = true;
        SmokeFault.gpioNum = SmokeFault_Pin;
        strcpy(SmokeFault.name, "Smoke Fault");

        DcGunNegFb[0].isAvailable = false;
        DcGunNegFb[1].gpioNum = DcGun1NegFb_Pin;
        DcGunNegFb[1].gpioPort = DcGun1NegFb_GPIO_Port;
        strcpy(DcGunNegFb[1].name, "DC Gun 1 Negative Feedback");
        DcGunNegFb[2].gpioNum = DcGun2NegFb_Pin;
        DcGunNegFb[2].gpioPort = DcGun2NegFb_GPIO_Port;
        strcpy(DcGunNegFb[2].name, "DC Gun 2 Negative Feedback");

        DcGunPosFb[0].isAvailable = false;
        DcGunPosFb[1].gpioNum = DcGun1PosFb_Pin;
        DcGunPosFb[1].gpioPort = DcGun1PosFb_GPIO_Port;
        strcpy(DcGunPosFb[1].name, "DC Gun 1 Positive Feedback");
        DcGunPosFb[2].gpioNum = DcGun2PosFb_Pin;
        DcGunPosFb[2].gpioPort = DcGun2PosFb_GPIO_Port;
        strcpy(DcGunPosFb[2].name, "DC Gun 2 Positive Feedback");

        DcGunIsoFb[0].isAvailable = false;
        DcGunIsoFb[1].gpioNum = DcGun1IsoFb_Pin;
        DcGunIsoFb[1].gpioPort = DcGun1IsoFb_GPIO_Port;
        strcpy(DcGunIsoFb[1].name, "DC Gun 1 Isolation Feedback");
        DcGunIsoFb[2].gpioNum = DcGun2IsoFb_Pin;
        DcGunIsoFb[2].gpioPort = DcGun2IsoFb_GPIO_Port;
        strcpy(DcGunIsoFb[2].name, "DC Gun 2 Isolation Feedback");

        AcPmFb[0].isAvailable = false;
        AcPmFb[1].gpioNum = AcPm1Fb_Pin;
        AcPmFb[1].gpioPort = AcPm1Fb_GPIO_Port;
        strcpy(AcPmFb[1].name, "AC PM1 Contactor Feedback");
        AcPmFb[2].gpioNum = AcPm2Fb_Pin;
        AcPmFb[2].gpioPort = AcPm2Fb_GPIO_Port;
        strcpy(AcPmFb[2].name, "AC PM2 Contactor Feedback");
        AcPmFb[3].gpioNum = AcPm3Fb_Pin;
        AcPmFb[3].gpioPort = AcPm3Fb_GPIO_Port;
        strcpy(AcPmFb[3].name, "AC PM3 Contactor Feedback");
        AcPmFb[4].gpioNum = AcPm4Fb_Pin;
        AcPmFb[4].gpioPort = AcPm4Fb_GPIO_Port;
        strcpy(AcPmFb[4].name, "AC PM4 Contactor Feedback");

        DcMergerPosFb[0].isAvailable = false;
        DcMergerPosFb[1].gpioNum = DcMerger1PosFb_Pin;
        DcMergerPosFb[1].gpioPort = DcMerger1PosFb_GPIO_Port;
        strcpy(DcMergerPosFb[1].name, "DC Merger 1 Positive Feedback");
        DcMergerPosFb[2].gpioNum = DcMerger2PosFb_Pin;
        DcMergerPosFb[2].gpioPort = DcMerger2PosFb_GPIO_Port;
        strcpy(DcMergerPosFb[2].name, "DC Merger 2 Positive Feedback");
        DcMergerPosFb[3].gpioNum = DcMerger3PosFb_Pin;
        DcMergerPosFb[3].gpioPort = DcMerger3PosFb_GPIO_Port;
        strcpy(DcMergerPosFb[3].name, "DC Merger 3 Positive Feedback");

        DcMergerNegFb[0].isAvailable = false;
        DcMergerNegFb[1].gpioNum = DcMerger1NegFb_Pin;
        DcMergerNegFb[1].gpioPort = DcMerger1NegFb_GPIO_Port;
        strcpy(DcMergerNegFb[1].name, "DC Merger 1 Negative Feedback");
        DcMergerNegFb[2].gpioNum = DcMerger2NegFb_Pin;
        DcMergerNegFb[2].gpioPort = DcMerger2NegFb_GPIO_Port;
        strcpy(DcMergerNegFb[2].name, "DC Merger 2 Negative Feedback");
        DcMergerNegFb[3].gpioNum = DcMerger3NegFb_Pin;
        DcMergerNegFb[3].gpioPort = DcMerger3NegFb_GPIO_Port;
        strcpy(DcMergerNegFb[3].name, "DC Merger 3 Negative Feedback");

        DcGunFuseFb[0].isAvailable = false;
        DcGunFuseFb[1].gpioNum = DcGun1FuseFb_Pin;
        DcGunFuseFb[1].gpioPort = DcGun1FuseFb_GPIO_Port;
        strcpy(DcGunFuseFb[1].name, "DC Gun 1 Fuse Feedback");
        DcGunFuseFb[2].gpioNum = DcGun2FuseFb_Pin;
        DcGunFuseFb[2].gpioPort = DcGun2FuseFb_GPIO_Port;
        strcpy(DcGunFuseFb[2].name, "DC Gun 2 Fuse Feedback");

        for (uint8_t i = 0; i < 38; i++)
        {
            if (gpioList[i]->isAvailable)
            {
                if (gpioList[i]->isDirectGpio)
                {
                    if (gpioList[i]->isInput)
                    {
                        if (IsValidEspInputGPIO(gpioList[i]->gpioNum))
                        {
                            gpio_set_direction(static_cast<gpio_num_t>(gpioList[i]->gpioNum), GPIO_MODE_INPUT);
                            gpio_pulldown_dis(static_cast<gpio_num_t>(gpioList[i]->gpioNum));
                            gpio_pullup_dis(static_cast<gpio_num_t>(gpioList[i]->gpioNum));
                            ESP_LOGD(TAG, "Configured GPIO %d INPUT %s", gpioList[i]->gpioNum, gpioList[i]->name);
                        }
                    }
                    else
                    {
                        if (IsValidEspOutputGPIO(gpioList[i]->gpioNum))
                        {
                            gpio_reset_pin(static_cast<gpio_num_t>(gpioList[i]->gpioNum));
                            gpio_set_direction(static_cast<gpio_num_t>(gpioList[i]->gpioNum), GPIO_MODE_OUTPUT);
                            gpio_pulldown_dis(static_cast<gpio_num_t>(gpioList[i]->gpioNum));
                            gpio_pullup_dis(static_cast<gpio_num_t>(gpioList[i]->gpioNum));
                            gpio_set_level(static_cast<gpio_num_t>(gpioList[i]->gpioNum), 0);
                            ESP_LOGD(TAG, "Configured GPIO %d OUTPUT %s", gpioList[i]->gpioNum, gpioList[i]->name);
                        }
                    }
                }
            }
        }
        xTaskCreate(&GpioTask, "GpioTask", 4 * 1024, this, 2, NULL);
    }

    // Destructor
    GpioController::~GpioController()
    {
        // Cleanup, if necessary
    }

    const char *GpioController::GpioStateStringify(GpioState state)
    {
        switch (state)
        {
        case GpioState::OFF:
            return "OFF";
        case GpioState::ON:
            return "ON";
        case GpioState::WELD:
            return "WELD";
        default:
            return "INVALID";
        }
    }

    void GpioController::GpioTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay
        ESP_LOGD(TAG, "GPIO Task Started");
        while (true)
        {
            for (uint8_t i = 0; i < 38; i++)
            {
                if (gpio->gpioList[i]->isAvailable && gpio->gpioList[i]->isDirectGpio)
                {
                    if (gpio->gpioList[i]->isInput)
                    {
                        int level = gpio_get_level(static_cast<gpio_num_t>(gpio->gpioList[i]->gpioNum));
                        GpioState newState = level ? GpioState::ON : GpioState::OFF;
                        if (newState != gpio->gpioList[i]->state)
                        {
                            gpio->gpioList[i]->state = newState;
                            ESP_LOGD(TAG, "%s, State: %s", gpio->gpioList[i]->name, gpio->GpioStateStringify(newState));
                        }
                    }
                }
            }
            vTaskDelay(pdMS_TO_TICKS(100)); // Example delay
        }
    }

    void GpioController::SetSendFunction(SendDataFunc func)
    {
        sendFunc = func;
    }

    bool GpioController::SendData(uint8_t port, uint16_t gpio, uint8_t state)
    {
        if (sendFunc)
        {
            return sendFunc(port, gpio, state);
        }
        return true; // or handle error
    }

    bool GpioController::ReceiveData(uint8_t port, uint16_t gpio, uint8_t state)
    {
        ESP_LOGD(TAG, "Receiving data for GPIO %d on port %d", gpio, port);
        for (uint8_t i = 0; i < 38; i++)
        {
            if (gpioList[i]->isAvailable && gpioList[i]->isDirectGpio == false)
            {
                if (gpioList[i]->isInput && gpioList[i]->gpioPort == port && gpioList[i]->gpioNum == gpio)
                {
                    gpioList[i]->state = static_cast<GpioState>(state);
                    ESP_LOGD(TAG, "Received - %s, State: %s", gpioList[i]->name, GpioStateStringify(static_cast<GpioState>(state)));
                    break;
                }
            }
        }
        return true; // or handle error
    }

    GpioState GpioController::GpioStateGet(ChargerGpio_t gpio)
    {
        return gpio.state;
    }

    bool GpioController::GpioStateSet(ChargerGpio_t gpio, GpioState state)
    {
        bool status = true;
        if (gpio.isAvailable && !gpio.isInput && state != gpio.state)
        {
            if (gpio.isDirectGpio)
            {
                if (!IsValidEspOutputGPIO(gpio.gpioNum))
                {
                    ESP_LOGE(TAG, "Blocked invalid GPIO write: %d", gpio.gpioNum);
                    return false;
                }

                if (gpio_set_level(static_cast<gpio_num_t>(gpio.gpioNum), static_cast<uint8_t>(state)) != ESP_OK)
                {
                    ESP_LOGE(TAG, "Failed to set GPIO level %d for GPIO Num: %d", static_cast<uint8_t>(state), gpio.gpioNum);
                    status = false;
                }
            }
            else
            {
                status = SendData(gpio.gpioPort, gpio.gpioNum, static_cast<uint8_t>(state));
            }
            if (status)
            {
                gpio.state = state;
                ESP_LOGD(TAG, "%s, State: %s", gpio.name, GpioStateStringify(state));
            }
        }
        else if (state == gpio.state)
        {
            ESP_LOGD(TAG, "%s is already in state %s", gpio.name, GpioStateStringify(state));
            return true;
        }
        else
        {
            status = false;
        }
        return status;
    }

} // namespace GpioModule
