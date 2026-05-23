#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/uart.h>
#include <cstring>
#include "PowerMuxModule.hpp"

#define TAG "POWERMUX"
const uart_port_t SLAVE_UART = UART_NUM_2;
#define SLAVE_TX_PIN 17
#define SLAVE_RX_PIN 16
#define SLAVE_BUFF_SIZE 1024
#define MAX_PM_PER_CONNECTOR 20

PowerMuxModule::PowerMuxController *pmm;
// PowerMuxModule::ConnectorPowerModules connectorPowerModules[NUM_OF_CONNECTORS + 1]; // TODO : populate this in PowerMuxControllerTask or from can

namespace PowerMuxModule
{
    // bool PowerMuxController::StopPowerModule(uint8_t PmId)
    // {
    //     while ((pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::ON) &&
    //            pmc->moduleStatus[PmId].isAlive &&
    //            pmc->moduleStatus[PmId].isAvailable)
    //     {
    //         // pmc->moduleStatus[PmId].Connector = PowerModule::ConnectorType::DEFAULT; moved to before return true
    //         pmc->ModuleStop(pmc->moduleStatus[PmId].moduleAddress, 0.0f, 0.0f);
    //         vTaskDelay(pdMS_TO_TICKS(100));
    //     }
    //     if ((pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::NORMAL_OFF) ||
    //         (pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::FAULT_OFF))
    //     {
    //         pmc->moduleStatus[PmId].Connector = PowerModule::ConnectorType::DEFAULT;
    //         return true;
    //     }
    //     return false;
    // }

    bool PowerMuxController::StartPowerModule(uint8_t PmId)
    {
        while ((pmc->moduleStatus[PmId].state != PowerModule::ChargingModuleState::ON) &&
               pmc->moduleStatus[PmId].isAlive &&
               pmc->moduleStatus[PmId].isAvailable) // TODO : check avtive status here
        {
            pmc->ModuleStart(pmc->moduleStatus[PmId].moduleAddress, 0.0f, 0.0f);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::ON)
        {
            return true;
        }
        return false;
    }

    bool PowerMuxController::DcMergerOff(uint8_t mergerId)
    {
        if (gpio->GpioStateGet(gpio->DcMerger[mergerId]) == GpioModule::GpioState::WELD)
        {
            ESP_LOGE(TAG, "State: Weld . Failed to OFF DcMerger %hu", mergerId);
            return false;
        }

        if (gpio->GpioStateGet(gpio->DcMerger[mergerId]) == GpioModule::GpioState::OFF)
        {
            ESP_LOGD(TAG, "DcMerger %hu is already OFF", mergerId);
            return true;
        }

        uint8_t i = 0;
        while (gpio->GpioStateGet(gpio->DcMerger[mergerId]) == GpioModule::GpioState::ON)
        {
            gpio->GpioStateSet(gpio->DcMerger[mergerId], GpioModule::GpioState::OFF);
            vTaskDelay(pdMS_TO_TICKS(100));
            i++;
            if (i > 10)
            {
                ESP_LOGE(TAG, "Timeout: Failed to OFF DcMerger %hu", mergerId);
                return false;
            }
        }
        return true;
    }

    bool PowerMuxController::DcMergerOn(uint8_t mergerId)
    {
        if (gpio->GpioStateGet(gpio->DcMerger[mergerId]) == GpioModule::GpioState::WELD || gpio->GpioStateGet(gpio->DcMerger[mergerId]) == GpioModule::GpioState::INVALID)
        {
            ESP_LOGE(TAG, "State: Weld or Invalid. Failed to ON DcMerger %hu", mergerId);
            return false;
        }

        uint8_t i = 0;
        while (gpio->GpioStateGet(gpio->DcMerger[mergerId]) == GpioModule::GpioState::OFF)
        {
            gpio->GpioStateSet(gpio->DcMerger[mergerId], GpioModule::GpioState::ON);
            vTaskDelay(pdMS_TO_TICKS(100));
            i++;
            if (i > 10)
            {
                ESP_LOGE(TAG, "Timeout : Failed to ON DcMerger %hu", mergerId);
                return false;
            }
        }
        return true;
    }

    void PowerMuxController::PowerMuxControllerTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay
        PowerMuxController *PowerMux = static_cast<PowerMuxController *>(pvParameters);
        uint32_t sleepTime = 100; // 1 second
        while (true)
        {
            for (uint8_t connId = 1; connId <= NUM_OF_CONNECTORS; connId++)
            {
                if (PowerMux->moduleStatus[connId].stateMachineState < PLCModule::StateMachineState::Parameter)
                {
                    for (uint8_t i = 1; i <= NUM_OF_POWER_MODULES; i++)
                    {
                        if (pmc->moduleStatus[i].isAvailable)
                        {
                            if (pmc->moduleStatus[i].state != PowerModule::ChargingModuleState::ON &&
                                gpio->GpioStateGet(*pmc->moduleStatus[i].AcPm) == GpioModule::GpioState::ON)
                            {
                                gpio->GpioStateSet(*pmc->moduleStatus[i].AcPm, GpioModule::GpioState::OFF);
                            }
                            else if (pmc->moduleStatus[i].state == PowerModule::ChargingModuleState::ON)
                            {
                                if (pmm->StopPowerModule(pmc->moduleStatus[i].moduleAddress))
                                    gpio->GpioStateSet(*pmc->moduleStatus[i].AcPm, GpioModule::GpioState::OFF);
                            }
                        }
                    }
                }

                if (PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Authentication)
                {
                    pmm->moduleStatus[connId].isParameterStateFinished = false;
                }

                else if (PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Parameter)
                {
                    if (pmm->IsolateDefaultPowerModules(connId) && pmm->AssignDefaultPowerModules(connId))
                        pmm->moduleStatus[connId].isParameterStateFinished = true;
                }

                else if (PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Isolation)
                {
                    pmm->moduleStatus[connId].isParameterStateFinished = false;
                }

                else if (PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::PreCharge)
                {
                    uint8_t NumOfPowerModuleAssigned = 0;
                    for (uint16_t pm = 1; pm < PowerModule::Constants::MAX_MODULES; pm++)
                    {
                        if (pmc->moduleStatus[pm].Connector == static_cast<PowerModule::ConnectorType>(connId))
                        {
                            NumOfPowerModuleAssigned++;
                        }
                    }
                    for (uint16_t pm = 1; pm < PowerModule::Constants::MAX_MODULES; pm++)
                    {
                        float current = 0;
                        current = PowerMux->moduleStatus[connId].EVTargetCurrent;
                        if (PowerMux->moduleStatus[connId].EVMaxCurrent < PowerMux->moduleStatus[connId].EVTargetCurrent)
                        {
                            current = PowerMux->moduleStatus[connId].EVMaxCurrent;
                        }
                        if (pmc->moduleStatus[pm].Connector == static_cast<PowerModule::ConnectorType>(connId))
                        {
                            if (pmc->moduleStatus[pm].state == PowerModule::ChargingModuleState::NORMAL_OFF)
                            {
                                pmc->ModuleStart(
                                    static_cast<uint8_t>(pmc->moduleStatus[pm].moduleAddress),
                                    PowerMux->moduleStatus[connId].EVTargetVoltage,
                                    current / NumOfPowerModuleAssigned);
                            }
                            else if (pmc->moduleStatus[pm].state == PowerModule::ChargingModuleState::ON)
                            {
                                pmc->ModuleParameters(static_cast<uint8_t>(pmc->moduleStatus[pm].moduleAddress),
                                                      PowerMux->moduleStatus[connId].EVTargetVoltage,
                                                      current / NumOfPowerModuleAssigned);
                            }
                        }
                    }
                }

                else if (PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Charge)
                {
                    // ONly handling single pm
                    // pmm->AssignAvailablePowerModules(connId);

                    uint8_t NumOfPowerModuleAssigned = 0;
                    for (uint16_t pm = 1; pm < PowerModule::Constants::MAX_MODULES; pm++)
                    {
                        if (pmc->moduleStatus[pm].Connector == static_cast<PowerModule::ConnectorType>(connId))
                        {
                            NumOfPowerModuleAssigned++;
                        }
                    }
                    for (uint16_t pm = 1; pm < PowerModule::Constants::MAX_MODULES; pm++)
                    {
                        float current = 0;
                        current = PowerMux->moduleStatus[connId].EVTargetCurrent;
                        if (PowerMux->moduleStatus[connId].EVMaxCurrent < PowerMux->moduleStatus[connId].EVTargetCurrent)
                        {
                            current = PowerMux->moduleStatus[connId].EVMaxCurrent;
                        }
                        if (pmc->moduleStatus[pm].Connector == static_cast<PowerModule::ConnectorType>(connId))
                        {
                            if (pmc->moduleStatus[pm].state != PowerModule::ChargingModuleState::ON)
                            {
                                pmc->ModuleStart(
                                    static_cast<uint8_t>(pmc->moduleStatus[pm].moduleAddress),
                                    PowerMux->moduleStatus[connId].EVTargetVoltage,
                                    current / NumOfPowerModuleAssigned);
                            }
                            else if (pmc->moduleStatus[pm].state == PowerModule::ChargingModuleState::ON)
                            {
                                pmc->ModuleParameters(static_cast<uint8_t>(pmc->moduleStatus[pm].moduleAddress),
                                                      PowerMux->moduleStatus[connId].EVTargetVoltage,
                                                      current / NumOfPowerModuleAssigned);
                            }
                        }
                    }

                    // Not needed for single pm test
                    // connectorPowerModules[connId].EVSEPower = PowerMux->moduleStatus[connId].EVSEPresentCurrent * PowerMux->moduleStatus[connId].EVSEPresentVoltage;
                    // connectorPowerModules[connId].EVPower = PowerMux->moduleStatus[connId].EVTargetCurrent * PowerMux->moduleStatus[connId].EVTargetVoltage;
                }
                pmm->SendEVSEMaxData(connId);
            }
            bool isTimingCommandRequired = false;
            for (uint8_t pmId = 1; pmId <= NUM_OF_POWER_MODULES; pmId++)
            {
                if (pmc->moduleStatus[pmId].AliveCounter > 5000)
                {
                    pmc->moduleStatus[pmId].isAlive = false;
                }
                else
                {
                    isTimingCommandRequired = true;
                    pmc->moduleStatus[pmId].AliveCounter += sleepTime;
                }
            }
            if (isTimingCommandRequired)
                pmc->ModuleTimingCommand();
            vTaskDelay(pdMS_TO_TICKS(sleepTime));
        }
    }

    // Constructor
    PowerMuxController::PowerMuxController(SendDataFunc func)
    {
        this->sendFunc = func;
        for (uint8_t pmId = 0; pmId <= NUM_OF_POWER_MODULES; pmId++)
        {
            pmc->moduleStatus[pmId].isAvailable = false; // change to true after SINGLE PM TEST
            pmc->moduleStatus[pmId].isAlive = false;
            pmc->moduleStatus[pmId].AliveCounter = 5000;
            pmc->moduleStatus[pmId].Connector = PowerModule::ConnectorType::DEFAULT;
            pmc->moduleStatus[pmId].state = PowerModule::ChargingModuleState::NORMAL_OFF;
            pmc->moduleStatus[pmId].moduleAddress = pmId;
            pmc->moduleStatus[pmId].MaxVoltage = 1000.0f;
            pmc->moduleStatus[pmId].MaxCurrent = 80.0f;
            pmc->moduleStatus[pmId].MinVoltage = 50.0f;
            pmc->moduleStatus[pmId].MinCurrent = 0.0f;
            pmc->moduleStatus[pmId].MaxPower = 30000.0f;
            pmc->moduleStatus[pmId].MinPower = 0.0f;
            pmc->moduleStatus[pmId].MaxTemperature = 80.0f;
            pmc->moduleStatus[pmId].MinTemperature = 0.0f;
            pmc->moduleStatus[pmId].isDefaultPm = false;
            pmc->moduleStatus[pmId].defaultConnector = PowerModule::ConnectorType::DEFAULT;
            pmc->moduleStatus[pmId].AcPm = NULL;
            pmc->moduleStatus[pmId].AcPmFb = NULL;
            pmc->moduleStatus[pmId].leftMerger = NULL;
            pmc->moduleStatus[pmId].rightMerger = NULL;
        }

        pmc->moduleStatus[0].isAvailable = false;

        pmc->moduleStatus[1].isAvailable = true; // remove after SINGLE PM TEST
        pmc->moduleStatus[1].isDefaultPm = true;
        pmc->moduleStatus[1].defaultConnector = PowerModule::ConnectorType::Connector1;
        pmc->moduleStatus[1].AcPm = &gpio->AcPm[1];
        pmc->moduleStatus[1].AcPmFb = &gpio->AcPmFb[1];
        pmc->moduleStatus[1].rightMerger = &gpio->DcMerger[1];

        pmc->moduleStatus[2].isDefaultPm = true;
        pmc->moduleStatus[2].defaultConnector = PowerModule::ConnectorType::Connector1;
        pmc->moduleStatus[2].AcPm = &gpio->AcPm[1];
        pmc->moduleStatus[2].AcPmFb = &gpio->AcPmFb[1];
        pmc->moduleStatus[2].rightMerger = &gpio->DcMerger[1];

        pmc->moduleStatus[3].AcPm = &gpio->AcPm[2];
        pmc->moduleStatus[3].AcPmFb = &gpio->AcPmFb[2];
        pmc->moduleStatus[3].leftMerger = &gpio->DcMerger[1];
        pmc->moduleStatus[3].rightMerger = &gpio->DcMerger[2];

        pmc->moduleStatus[4].AcPm = &gpio->AcPm[2];
        pmc->moduleStatus[4].AcPmFb = &gpio->AcPmFb[2];
        pmc->moduleStatus[4].leftMerger = &gpio->DcMerger[1];
        pmc->moduleStatus[4].rightMerger = &gpio->DcMerger[2];

        pmc->moduleStatus[5].AcPm = &gpio->AcPm[3];
        pmc->moduleStatus[5].AcPmFb = &gpio->AcPmFb[3];
        pmc->moduleStatus[5].leftMerger = &gpio->DcMerger[2];
        pmc->moduleStatus[5].rightMerger = &gpio->DcMerger[3];

        pmc->moduleStatus[6].AcPm = &gpio->AcPm[3];
        pmc->moduleStatus[6].AcPmFb = &gpio->AcPmFb[3];
        pmc->moduleStatus[6].leftMerger = &gpio->DcMerger[2];
        pmc->moduleStatus[6].rightMerger = &gpio->DcMerger[3];

        pmc->moduleStatus[7].isDefaultPm = true;
        pmc->moduleStatus[7].defaultConnector = PowerModule::ConnectorType::Connector2;
        pmc->moduleStatus[7].AcPm = &gpio->AcPm[4];
        pmc->moduleStatus[7].AcPmFb = &gpio->AcPmFb[4];
        pmc->moduleStatus[7].leftMerger = &gpio->DcMerger[3];

        pmc->moduleStatus[8].isDefaultPm = true;
        pmc->moduleStatus[8].defaultConnector = PowerModule::ConnectorType::Connector2;
        pmc->moduleStatus[8].AcPm = &gpio->AcPm[4];
        pmc->moduleStatus[8].AcPmFb = &gpio->AcPmFb[4];
        pmc->moduleStatus[8].leftMerger = &gpio->DcMerger[3];

        for (uint8_t i = 0; i < PLCModule::Constants::MAX_MODULES; i++)
        {
            controlPilotState_old[i] = PLCModule::ControlPilotState::SNA;
            stateMachineState_old[i] = PLCModule::StateMachineState::SNA;
            moduleStatus[i].EVSEMaxCurrent = pmc->moduleStatus[i].MaxCurrent;
            moduleStatus[i].EVSEMaxVoltage = pmc->moduleStatus[i].MaxVoltage;
            moduleStatus[i].EVSEMinCurrent = pmc->moduleStatus[i].MinCurrent;
            moduleStatus[i].EVSEMinVoltage = pmc->moduleStatus[i].MinVoltage;
            moduleStatus[i].EVSEPresentCurrent = 0;
            moduleStatus[i].EVSEPresentVoltage = 0;
            moduleStatus[i].EVSEMaxPower = pmc->moduleStatus[i].MaxPower;
            moduleStatus[i].isParameterStateFinished = false;
        }

        xTaskCreate(&PowerMuxControllerTask, "PowerMuxControllerTask", 4096, this, 2, NULL);
    }

    // Destructor
    PowerMuxController::~PowerMuxController()
    {
        // Cleanup, if necessary
    }

    void PowerMuxController::SetSendFunction(SendDataFunc func)
    {
        this->sendFunc = func;
    }

    bool PowerMuxController::SendData(const uint32_t id, const uint64_t data)
    {
        if (this->sendFunc)
        {
            return this->sendFunc(id, data);
        }
        return true; // or handle error
    }

    bool PowerMuxController::SendEVSEMaxData(uint8_t connId)
    {
        CCSConnector::EVSEMAX EVSEMax;
        uint64_t data = 0;
        std::memset(&EVSEMax, 0, sizeof(EVSEMax));
        EVSEMax.ConnectorId = static_cast<uint8_t>(connId);
        EVSEMax.EVSEMaxCurrent = static_cast<uint16_t>(moduleStatus[connId].EVSEMaxCurrent / PLCModule::Constants::CURRENT_SCALE);
        EVSEMax.EVSEMaxVoltage = static_cast<uint16_t>(moduleStatus[connId].EVSEMaxVoltage / PLCModule::Constants::VOLTAGE_SCALE);
        EVSEMax.isParameterStateFinished = static_cast<uint16_t>(moduleStatus[connId].isParameterStateFinished);
        std::memcpy(&data, &EVSEMax, sizeof(data));
        return SendData(static_cast<uint32_t>(CCSConnector::PowerMuxCanId::EVSEMAX), data);
    }

    bool PowerMuxController::ReceiveData(const uint32_t id, const uint64_t data)
    {
        uint8_t ConnID = 0;
        switch (id)
        {
        case static_cast<uint32_t>(CCSConnector::PowerMuxCanId::EVMAX):
            CCSConnector::EVMAX EVMax;
            std::memcpy((void *)&EVMax, (const void *)&data, sizeof(data));
            ConnID = EVMax.ConnectorId;
            moduleStatus[ConnID].EVMaxVoltage = static_cast<float>(EVMax.EVMaxVoltage * PLCModule::Constants::VOLTAGE_SCALE);
            moduleStatus[ConnID].EVMaxCurrent = static_cast<float>(EVMax.EVMaxCurrent * PLCModule::Constants::CURRENT_SCALE);
            moduleStatus[ConnID].controlPilotState = static_cast<PLCModule::ControlPilotState>(EVMax.controlPilotState);
            moduleStatus[ConnID].stateMachineState = static_cast<PLCModule::StateMachineState>(EVMax.stateMachineState);
            break;
        case static_cast<uint32_t>(CCSConnector::PowerMuxCanId::EVTARTGET):
            CCSConnector::EVTARTGET EVTarget;
            std::memcpy((void *)&EVTarget, (const void *)&data, sizeof(data));
            ConnID = EVTarget.ConnectorId;
            moduleStatus[ConnID].EVTargetVoltage = static_cast<float>(EVTarget.EVTargetVoltage * PLCModule::Constants::VOLTAGE_SCALE);
            moduleStatus[ConnID].EVTargetCurrent = static_cast<float>(EVTarget.EVTargetCurrent * PLCModule::Constants::CURRENT_SCALE);
            moduleStatus[ConnID].controlPilotState = static_cast<PLCModule::ControlPilotState>(EVTarget.controlPilotState);
            moduleStatus[ConnID].stateMachineState = static_cast<PLCModule::StateMachineState>(EVTarget.stateMachineState);
            break;
        default:
            break;
        }
        return true; // or handle error
    }

    bool PowerMuxController::sufficientPower(uint8_t connId)
    {
        float currentPower = pmm->moduleStatus[connId].EVSEPresentCurrent * pmm->moduleStatus[connId].EVSEPresentVoltage;
        float targetPower = pmm->moduleStatus[connId].EVTargetCurrent * pmm->moduleStatus[connId].EVTargetVoltage;

        return (currentPower >= targetPower);
    }

    float PowerMuxController::AssignedPower(uint8_t connId)
    {
        // TODO : Rework. retuns NoOfAssignedPMs * PM_MaxPower
        // return connectorPowerModules[connId].PmCount * pmc->moduleStatus[1].MaxPower;
        return 0;
    }

    uint8_t PowerMuxController::PriorityConnector(uint8_t ConnId1, uint8_t ConnId2)
    {
        // if (connectorPowerModules[ConnId1].StartTime <= connectorPowerModules[ConnId2].StartTime)
        //     return ConnId1;
        // else
        //     return ConnId2;

        return 0;
    }

    bool PowerMuxController::AssignDefaultPowerModules(uint8_t connId) // NOTE:  Assign only assigns START done in pmm task
    {
        PowerModule::ModuleStatus *pm = &pmc->moduleStatus[0];
        // PowerModule::ModuleStatus *pmTwin = &pmc->moduleStatus[0];
        // GpioModule::ChargerGpio_t *merger = NULL;
        for (uint8_t i = 1; i < PowerModule::Constants::MAX_MODULES; i++)
        {
            PowerModule::ModuleStatus *pmTemp = &pmc->moduleStatus[i];
            if (pmTemp->isDefaultPm && pmTemp->defaultConnector == static_cast<PowerModule::ConnectorType>(connId))
            {
                pm = pmTemp;
                // pmTwin = &pmc->moduleStatus[i + 1];
                // if (pm->leftMerger != NULL)
                // {
                //     merger = pm->leftMerger;
                // }
                // else if (pm->rightMerger != NULL)
                // {
                //     merger = pm->rightMerger;
                // }
                break;
            }
        }
        if (pm != NULL)
        {
            pm->Connector = pm->defaultConnector;
            return 1;
        }
        return 0;
    }

    void PowerMuxController::AssignAvailablePowerModules(uint8_t connId)
    {
        // Requirement : Connector and default power modules are already assigned and ON.
        // 1. Check if target power is achieved or not with default power modules, if yes return
        // 2. If not, assign other power modules one by one until target is achieved or no more power module is available.

        // if (sufficientPower(connId))
        //     return;

        // if (connId == 1)
        // {
        //     for (uint8_t pmId = 2; pmId < 8; pmId = pmId + 2)
        //     {
        //         if ((pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::ON) ||
        //             (pmc->moduleStatus[pmId + 1].state == PowerModule::ChargingModuleState::ON))
        //             break;

        //         pmc->moduleStatus[pmId].Connector = static_cast<PowerModule::ConnectorType>(connId);
        //         pmc->moduleStatus[pmId + 1].Connector = static_cast<PowerModule::ConnectorType>(connId);

        //         gpio->GpioStateSet(*pmc->moduleStatus[pmId].rightMerger, GpioModule::GpioState::ON);
        //         if (sufficientPower(connId))
        //             return;
        //     }
        // }
        // else
        // {
        //     for (uint8_t pmId = 4; pmId >= 0; pmId = pmId - 2)
        //     {
        //         if ((pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::ON) ||
        //             (pmc->moduleStatus[pmId + 1].state == PowerModule::ChargingModuleState::ON))
        //             break;
        //         pmc->moduleStatus[pmId].Connector = static_cast<PowerModule::ConnectorType>(connId);
        //         pmc->moduleStatus[pmId + 1].Connector = static_cast<PowerModule::ConnectorType>(connId);

        //         gpio->GpioStateSet(*pmc->moduleStatus[pmId].leftMerger, GpioModule::GpioState::ON);
        //         if (sufficientPower(connId))
        //             return;
        //     }
        // }

        // UNCOMMENT AFTER SINGLE PM TEST
    }

    bool PowerMuxController::StopPowerModule(uint8_t pmId)
    {
        ESP_LOGI(TAG, "Stopping Power Module %d", pmId);

        if (pmId == 0 || pmId > PowerModule::Constants::MAX_MODULES)
        {
            ESP_LOGE(TAG, "Invalid PM ID");
            return false;
        }

        if (pmc->moduleStatus[pmId].isAlive == false)
        {
            ESP_LOGW(TAG, "PM %d is not alive", pmId);
            return false;
        }

        uint8_t connectorId =
            static_cast<uint8_t>(pmc->moduleStatus[pmId].Connector);

        ESP_LOGI(TAG, "PM %d currently assigned to Connector %d", pmId, connectorId);

        for (uint8_t retry = 0; retry < 5; retry++)
        {
            pmc->ModuleStop(static_cast<uint8_t>(pmc->moduleStatus[pmId].moduleAddress), 0, 0);

            vTaskDelay(pdMS_TO_TICKS(500));

            if ((pmc->moduleStatus[pmId].state ==
                 PowerModule::ChargingModuleState::NORMAL_OFF) ||

                (pmc->moduleStatus[pmId].state ==
                 PowerModule::ChargingModuleState::FAULT_OFF))
            {
                ESP_LOGI(TAG, "PM %d stopped successfully", pmId);

                break;
            }
        }

        if ((pmc->moduleStatus[pmId].state !=
             PowerModule::ChargingModuleState::NORMAL_OFF) &&

            (pmc->moduleStatus[pmId].state !=
             PowerModule::ChargingModuleState::FAULT_OFF))
        {
            ESP_LOGE(TAG, "PM %d failed to stop", pmId);
            return false;
        }

        // gpio->SetLeftDcOutputMerger(pmId, GpioModule::GpioStatus::OFF);
        // gpio->SetRightDcOutputMerger(pmId, GpioModule::GpioStatus::OFF);
        ESP_LOGI(TAG, "Disabled DC mergers for PM %d", pmId);
        pmc->moduleStatus[pmId].Connector = PowerModule::ConnectorType::DEFAULT;
        ESP_LOGI(TAG, "Released PM %d ownership", pmId);
        // gpio->SetAcContactor(pmId, GpioModule::GpioStatus::OFF);

        ESP_LOGI(TAG, "Disabled AC contactor for PM %d", pmId);
        ESP_LOGI(TAG, "PM %d safely isolated and ready for reassignment", pmId);

        return true;
    }

    bool PowerMuxController::StopDefaultPowerModules(uint8_t connId)
    {
        PowerModule::ModuleStatus *pm = &pmc->moduleStatus[0];
        for (uint8_t i = 1; i < PowerModule::Constants::MAX_MODULES; i++)
        {
            PowerModule::ModuleStatus *pmTemp = &pmc->moduleStatus[i];
            if (pmTemp->isDefaultPm && pmTemp->defaultConnector == static_cast<PowerModule::ConnectorType>(connId))
            {
                if (pmTemp->Connector == static_cast<PowerModule::ConnectorType>(connId))
                {
                    pm = pmTemp;
                    break;
                }
            }
        }

        if (StopPowerModule(pm->moduleAddress))
        {
            pm->Connector = PowerModule::ConnectorType::DEFAULT;
            return true;
        }
        return false;
    }

    bool PowerMuxController::IsolatePowerModules(uint8_t pmId)
    {
        // uint8_t pmPair[2] = {static_cast<uint8_t>((pmId % 2 == 0) ? pmId : pmId - 1),
        //                      static_cast<uint8_t>((pmId % 2 == 0) ? pmId + 1 : pmId)};

        // uint8_t acContactor = (pmId / 2) + 1;

        // if (pmc->moduleStatus[pmPair[0]].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[pmPair[1]].state == PowerModule::ChargingModuleState::ON)
        // {
        //     StopPowerModule(pmPair[0]);
        //     StopPowerModule(pmPair[1]);

        //     if (pmc->moduleStatus[pmPair[0]].state == PowerModule::ChargingModuleState::NORMAL_OFF && pmc->moduleStatus[pmPair[1]].state == PowerModule::ChargingModuleState::NORMAL_OFF)
        //     {
        //         gpio->GpioStateSet(gpio->AcPm[acContactor], GpioModule::GpioState::ON);
        //         gpio->GpioStateSet(*pmc->moduleStatus[pmId].AcPm, GpioModule::GpioState::OFF); // TODO : Add null check if needed
        //         gpio->GpioStateSet(*pmc->moduleStatus[pmId].leftMerger, GpioModule::GpioState::OFF);
        //         gpio->GpioStateSet(*pmc->moduleStatus[pmId].rightMerger, GpioModule::GpioState::OFF);
        //         gpio->GpioStateSet(gpio->AcPm[acContactor], GpioModule::GpioState::OFF);
        //     }
        // }

        // UNCOMMENT AFTER SINGLE PM TEST
        return true;
    }

    bool PowerMuxController::IsolateDefaultPowerModules(uint8_t connId)
    {

        PowerModule::ModuleStatus *pm = &pmc->moduleStatus[0];
        // PowerModule::ModuleStatus *pmTwin = &pmc->moduleStatus[0];
        // GpioModule::ChargerGpio_t *merger = NULL;
        // for (uint8_t i = 1; i < PowerModule::Constants::MAX_MODULES; i++)
        // {
        //     PowerModule::ModuleStatus *pmTemp = &pmc->moduleStatus[i];
        //     if (pmTemp->isDefaultPm && pmTemp->defaultConnector == static_cast<PowerModule::ConnectorType>(connId))
        //     {
        //         pm = pmTemp;
        //         pmTwin = &pmc->moduleStatus[i + 1];
        //         if (pm->leftMerger != NULL)
        //         {
        //             merger = pm->leftMerger;
        //         }
        //         else if (pm->rightMerger != NULL)
        //         {
        //             merger = pm->rightMerger;
        //         }
        //         break;
        //     }
        // }

        if (gpio->GpioStateGet(*pm->AcPm) != GpioModule::GpioState::ON)
        {
            gpio->GpioStateSet(*pm->AcPm, GpioModule::GpioState::ON);
        }

        if (pm->state != PowerModule::ChargingModuleState::ON) /* && // TODO: One of them can be Fault_OFF.
             pmTwin->state != PowerModule::ChargingModuleState::ON &&
             gpio->GpioStateGet(*merger) == GpioModule::GpioState::OFF)*/
        {
            return true;
        }

        StopPowerModule(pm->moduleAddress);
        // StopPowerModule(pmTwin->moduleAddress);

        // if (pm->state != PowerModule::ChargingModuleState::ON && pmTwin->state != PowerModule::ChargingModuleState::ON)
        // {

        //     if (gpio->GpioStateGet(*merger) == GpioModule::GpioState::WELD)
        //     {
        //         ESP_LOGE(TAG, "State: Weld . Failed to OFF DcMerger %hu", (connId == 1) ? uint8_t{1} : uint8_t{3});
        //         return false;
        //     }
        //     gpio->GpioStateSet(*merger, GpioModule::GpioState::OFF);
        // }

        bool isIsolated = pm->state != PowerModule::ChargingModuleState::ON /*&&
                          pmTwin->state != PowerModule::ChargingModuleState::ON &&
                          gpio->GpioStateGet(*merger) == GpioModule::GpioState::OFF*/
            ;

        return isIsolated;
    }

    void PowerMuxController::OptimizePowerModulesForStandAlone()
    {
        // TODO : decide periodicity and implement timer or counter based mechanism to call optimizePowerModules function

        if (gpio->GpioStateGet(gpio->DcGun[1] /*ToDo Set Actual Gpio*/) == GpioModule::GpioState::OFF && gpio->GpioStateGet(gpio->DcGun[1] /*ToDo Set Actual Gpio*/) == GpioModule::GpioState::OFF)
        {
            // both connectors are  NOT active. no optimization possible
            return;
        }

        for (uint8_t pmId = 0; pmId < 8; pmId = pmId + 2)
        {
            if (pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[pmId + 1].state == PowerModule::ChargingModuleState::ON)
            {
                continue; // Already working
            }

            if (pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::FAULT_OFF && pmc->moduleStatus[pmId + 1].state == PowerModule::ChargingModuleState::FAULT_OFF)
            {
                continue; // Faulty modules, cant be optimized
                // TODO :  but mergers to be on for power to transfer from neext modules
            }

            // check if default modules
            if (pmId == 0 || pmId == 1)
            {
                // check if contactor is on
                if (gpio->GpioStateGet(gpio->DcGun[1] /*ToDo Set Actual Gpio*/) == GpioModule::GpioState::ON)
                {
                    continue; // TODO : CHECK ONLY : rarely/never comes here. only when contactor on and def powermodules off due to some reason.
                }
                else // TODO : remove this else and make if direct to reduce nesting
                {
                    if (pmc->moduleStatus[2].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[3].state == PowerModule::ChargingModuleState::ON)
                    {
                        // if connector 2 not have sufficient power
                        DcMergerOn(1);
                        StartPowerModule(0);
                        StartPowerModule(1);
                    }
                }
            }
            else if (pmId == 6 || pmId == 7)
            {
                if (gpio->GpioStateGet(gpio->DcGun[1] /*ToDo Set Actual Gpio*/) == GpioModule::GpioState::ON)
                {
                    continue;
                }
                else
                {
                    if (pmc->moduleStatus[4].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[5].state == PowerModule::ChargingModuleState::ON)
                    {
                        // if connector 1 not have sufficient power
                        DcMergerOn(3);
                        StartPowerModule(6);
                        StartPowerModule(7);
                    }
                }
            }
            else
            {
                uint8_t connector = PriorityConnector((uint8_t)pmc->moduleStatus[pmId - 2].Connector, (uint8_t)pmc->moduleStatus[pmId + 2].Connector);
                uint8_t mergerId = GetMergerId(pmId, connector);
                // ON mergerId
                if (DcMergerOn(mergerId))
                {
                    StartPowerModule(pmId);
                    StartPowerModule(pmId + 1);
                }
            }
        }
    }

} // namespace PowerMuxModule
