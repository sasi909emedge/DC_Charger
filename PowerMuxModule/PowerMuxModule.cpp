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
PowerMuxModule::ConnectorPowerModules connectorPowerModules[NUM_OF_CONNECTORS + 1]; //TODO : populate this in PowerMuxControllerTask or from can

namespace PowerMuxModule
{
    ConnectorMuxesRelays PowerMuxController::GetConnectorMuxesRelays(uint8_t connId)
    {
        bool OddNumberConnector = (connId % 2) ? true : false;
        uint8_t SubSet = (connId - 1) / 2;
        uint8_t SuperSetNum = (connId - 1) / 4;
        uint8_t SuperSetConnectorNum = (connId - 1) % 4;
        
        ConnectorMuxesRelays ConnMuxRelay;
        memset(&ConnMuxRelay, 0, sizeof(ConnectorMuxesRelays));
        ConnMuxRelay.RelayNum = static_cast<uint16_t>(GpioModule::Contactor::SubSet_RelayBase) + (SubSet * 3) + ((OddNumberConnector) ? 1 : 3);
        switch (SuperSetConnectorNum)
        {
        case 0:
            if (SuperSetMux[SuperSetNum][0]) // Conection Between 1 & 3
                ConnMuxRelay.SuperSetInternalMux1Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 1;
            if (SuperSetMux[SuperSetNum][1]) // Conection Between 1 & 4
                ConnMuxRelay.SuperSetInternalMux2Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 2;
            if (SuperSetNum == 0) // StackMux for Connector1
            {
                if (StackMux[0]) // Conection Between 1 & 5
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 1;
                if (StackMux[1]) // Conection Between 1 & 8
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 2;
            }
            else // StackMux for Connector5
            {
                if (StackMux[0]) // Conection Between 1 & 5
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 1;
                if (StackMux[2]) // Conection Between 4 & 5
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 3;
            }
            break;
        case 1:
            if (SuperSetMux[SuperSetNum][2]) // Conection Between 2 & 3
                ConnMuxRelay.SuperSetInternalMux1Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 3;
            if (SuperSetMux[SuperSetNum][3]) // Conection Between 2 & 4
                ConnMuxRelay.SuperSetInternalMux2Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 4;
            break;
        case 2:
            if (SuperSetMux[SuperSetNum][0]) // Conection Between 1 & 3
                ConnMuxRelay.SuperSetInternalMux1Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 1;
            if (SuperSetMux[SuperSetNum][2]) // Conection Between 2 & 3
                ConnMuxRelay.SuperSetInternalMux2Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 3;
            break;
        case 3:
            if (SuperSetMux[SuperSetNum][1]) // Conection Between 1 & 4
                ConnMuxRelay.SuperSetInternalMux1Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 2;
            if (SuperSetMux[SuperSetNum][3]) // Conection Between 2 & 4
                ConnMuxRelay.SuperSetInternalMux2Num = static_cast<uint16_t>(GpioModule::Contactor::SuperSet_InternalMuxBase) + SuperSetNum * 4 + 4;
            if (SuperSetNum == 0) // StackMux for Connector4
            {
                if (StackMux[2]) // Conection Between 4 & 5
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 3;
                if (StackMux[3]) // Conection Between 4 & 8
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 4;
            }
            else // StackMux for Connector8
            {
                if (StackMux[1]) // Conection Between 1 & 8
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 2;
                if (StackMux[3]) // Conection Between 4 & 8
                    ConnMuxRelay.StackMux1Num = static_cast<uint16_t>(GpioModule::Contactor::StackMuxBase) + 4;
            }
            break;

        default:
            break;
        }
        if (ConnMuxRelay.RelayNum != 0)
            ConnMuxRelay.RelayNumStatus = gpio->GetRelayState(ConnMuxRelay.RelayNum);
        else
            ConnMuxRelay.RelayNumStatus = GpioModule::GpioStatus::INVALID;
        if (ConnMuxRelay.SuperSetInternalMux1Num != 0)
            ConnMuxRelay.SuperSetInternalMux1NumStatus = gpio->GetMuxContactorState(ConnMuxRelay.SuperSetInternalMux1Num);
        else
            ConnMuxRelay.SuperSetInternalMux1NumStatus = GpioModule::GpioStatus::INVALID;
        if (ConnMuxRelay.SuperSetInternalMux2Num != 0)
            ConnMuxRelay.SuperSetInternalMux2NumStatus = gpio->GetMuxContactorState(ConnMuxRelay. SuperSetInternalMux2Num);
        else
            ConnMuxRelay.SuperSetInternalMux2NumStatus = GpioModule::GpioStatus::INVALID;
        if (ConnMuxRelay.StackMux1Num != 0)
            ConnMuxRelay.StackMux1NumStatus = gpio->GetMuxContactorState(ConnMuxRelay.StackMux1Num);
        else
            ConnMuxRelay.StackMux1NumStatus = GpioModule::GpioStatus::INVALID;
        if (ConnMuxRelay.StackMux2Num != 0)
            ConnMuxRelay.StackMux2NumStatus = gpio->GetMuxContactorState(ConnMuxRelay.StackMux2Num);
        else
            ConnMuxRelay.StackMux2NumStatus = GpioModule::GpioStatus::INVALID;
        return ConnMuxRelay;
    }

    RelayPowerFlowDirection PowerMuxController::GetRelayPowerFlowDirection(uint8_t connId,
                                                                           ConnectorMuxesRelays *ConnMuxRelay,
                                                                           PowerModuleConnections *PmConnections,
                                                                           PowerModule::ModuleStatus *PmStatus)
    {
        bool OddNumberConnector = (connId % 2) ? true : false;
        uint8_t AdjuscentConnectorNumber = connId;
        uint16_t AdjuscentRelayNum1 = ConnMuxRelay->RelayNum;
        uint16_t AdjuscentRelayNum2 = ConnMuxRelay->RelayNum;
        if (OddNumberConnector)
        {
            AdjuscentRelayNum1 += 1;
            AdjuscentRelayNum2 += 2;
            AdjuscentConnectorNumber += 1;
        }
        else
        {
            AdjuscentRelayNum1 -= 1;
            AdjuscentRelayNum2 -= 2;
            AdjuscentConnectorNumber -= 1;
        }
        if (gpio->GetRelayState(AdjuscentRelayNum1) == GpioModule::GpioStatus::OFF)
            return RelayPowerFlowDirection::RelayIsNotConnectedToAdjuscentRelay;
        else if ((gpio->GetRelayState(AdjuscentRelayNum1) == GpioModule::GpioStatus::ON) &&
                 (gpio->GetRelayState(AdjuscentRelayNum2) == GpioModule::GpioStatus::OFF))
            return RelayPowerFlowDirection::DrawingPowerFromAdjuscent1Relay;
        else
        {
            // if ((PmStatus->Connector == static_cast<PowerModule::ConnectorType>(AdjuscentConnectorNumber)) &&
            if ((ConnMuxRelay->StackMux1NumStatus != GpioModule::GpioStatus::ON) &&
                (ConnMuxRelay->StackMux2NumStatus != GpioModule::GpioStatus::ON) &&
                (ConnMuxRelay->SuperSetInternalMux1NumStatus != GpioModule::GpioStatus::ON) &&
                (ConnMuxRelay->SuperSetInternalMux2NumStatus != GpioModule::GpioStatus::ON))
                return RelayPowerFlowDirection::SourcingPowerToAdjuscentRelays;
            if ((ConnMuxRelay->StackMux1NumStatus == GpioModule::GpioStatus::ON) ||
                (ConnMuxRelay->StackMux2NumStatus == GpioModule::GpioStatus::ON) ||
                (ConnMuxRelay->SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::ON) ||
                (ConnMuxRelay->SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::ON))
                return RelayPowerFlowDirection::DrawingPowerFromAdjuscent2Relays;
        }
        return RelayPowerFlowDirection::Unknown;
    }

    PowerModule::ModuleStatus PowerMuxController::GetDefaultPowerModule(uint8_t connId, PowerModuleConnections *PmConnections)
    {
        bool OddNumberConnector = (connId % 2) ? true : false;
        uint8_t SubSet = (connId - 1) / 2;
        uint8_t PowerModuleId = SubSet * 8 + ((OddNumberConnector) ? 1 : 7);
        PmConnections->leftRelayNum = (OddNumberConnector) ? 0 : (static_cast<uint16_t>(GpioModule::Contactor::SubSet_RelayBase) + (SubSet * 3) + 3);
        PmConnections->rightRelayNum = (OddNumberConnector) ? (static_cast<uint16_t>(GpioModule::Contactor::SubSet_RelayBase) + (SubSet * 3) + 1) : 0;
        PmConnections->subSetNum = SubSet;
        PmConnections->subSetAcContactorNum = static_cast<uint16_t>(GpioModule::Contactor::SubSet_AcContactorBase) + SubSet + 1;
        PmConnections->PmId = PowerModuleId;
        return pmc->moduleStatus[PowerModuleId];
    }

    bool PowerMuxController::StopPowerModule(uint8_t PmId)
    {
        while ((pmc->moduleStatus[PmId].state != PowerModule::ChargingModuleState::ON) &&
               pmc->moduleStatus[PmId].isAlive &&
               pmc->moduleStatus[PmId].isActive)
        {
            // pmc->moduleStatus[PmId].Connector = PowerModule::ConnectorType::DEFAULT; moved to before return true
            pmc->ModuleStop(pmc->moduleStatus[PmId].moduleAddress, 0.0f, 0.0f);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if ((pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::NORMAL_OFF) &&
            (pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::FAULT_OFF))
        {
            pmc->moduleStatus[PmId].Connector = PowerModule::ConnectorType::DEFAULT;
            return true;
        }
        return false;
    }

    bool PowerMuxController::StartPowerModule(uint8_t PmId)
    {
        while((pmc->moduleStatus[PmId].state != PowerModule::ChargingModuleState::NORMAL_OFF) &&
              pmc->moduleStatus[PmId].isAlive &&
              pmc->moduleStatus[PmId].isActive)
        {
            // TODO : Assign to list if stored elsewhere
            pmc->ModuleStart(pmc->moduleStatus[PmId].moduleAddress, 0.0f, 0.0f);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if(pmc->moduleStatus[PmId].state == PowerModule::ChargingModuleState::ON)
        {
            return true;
        }
        return false;
    }

    bool PowerMuxController::RelayOff(uint16_t relayId)
    {
        uint8_t i = 0;
        while (gpio->GetRelayState(relayId) == GpioModule::GpioStatus::ON)
        {
            gpio->RelayOff(relayId);
            vTaskDelay(pdMS_TO_TICKS(100));
            i++;
            if (i > 10)
            {
                ESP_LOGE(TAG, "Failed to Stop Relay %hu", relayId);
                return false;
            }
        }
        return true;
    }

    bool PowerMuxController::isolateConnectorAndAssignDefaultPowerModule(uint8_t connId)
    {
        if(isolateConnector(static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT), connId))
        {
            //assignDefaultPowerModules(static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT), connId);
            //TODO : Add separate Assign function as above
            bool OddNumberConnector = (connId % 2) ? true : false;
            uint8_t pm[2];
            if(OddNumberConnector)
            {
                pm[0] = (connId - 1) * 4;
                pm[1] = (connId - 1) * 4 + 1;               
            }
            else
            {
                pm[0] = (connId - 1) * 4 - 2;
                pm[1] = (connId - 1) * 4 - 1;
            }

            bool pmStartResult[2] = { StartPowerModule(pm[0]), StartPowerModule(pm[1]) };
            return pmStartResult[0] || pmStartResult[1]; // TODO : return true if any of the power modules started successfully
        }
        return false;
    }

    uint8_t PowerMuxController::GetDestinationConnectorIdFromSuperSetInternalMuxId(uint8_t connId, uint16_t SuperSetInternalMuxId)
    {
        return connId;
    }

    uint8_t PowerMuxController::GetDestinationConnectorIdFromStackMuxId(uint8_t connId, uint16_t StackMuxId)
    {
        return connId;
    }

    bool PowerMuxController::isolateConnector(uint8_t SourceConnId, uint8_t DestinationConnId)
    {
        uint8_t connId = DestinationConnId;
        bool OddNumberConnector = (connId % 2) ? true : false;
        ConnectorMuxesRelays ConnMuxRelay = GetConnectorMuxesRelays(connId);
        PowerModuleConnections PmConnections;
        PowerModule::ModuleStatus PmStatus = GetDefaultPowerModule(connId, &PmConnections);

        if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::WELD ||
           ConnMuxRelay.SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::WELD ||
           ConnMuxRelay.SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::WELD ||
           ConnMuxRelay.StackMux1NumStatus == GpioModule::GpioStatus::WELD ||
           ConnMuxRelay.StackMux2NumStatus == GpioModule::GpioStatus::WELD)
        {
            ESP_LOGE(TAG, "Connector %hu is in WELD state. Manual Intervention Required.", connId);
            return false;
        }

        while(!IsConnectorIsolated(connId))
        {
            //stand alone : No Muxes. Only 2 connectors
            if(ConnMuxRelay.SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::INVALID ||
            ConnMuxRelay.SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::INVALID ||
            ConnMuxRelay.StackMux1NumStatus == GpioModule::GpioStatus::INVALID ||
            ConnMuxRelay.StackMux2NumStatus == GpioModule::GpioStatus::INVALID)
            {
                if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::ON)
                {
                    if(StopPowerModule(PmConnections.PmId)) // TODO: Add time in stop power module if needed
                        if(RelayOff(ConnMuxRelay.RelayNum))
                            return true;
                }
                else if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::OFF)
                {
                    return true;
                }
            }
        }

        while(!IsConnectorIsolated(connId))
        {
            ESP_LOGI(TAG, "Isolating Connector %hu", connId); 
            uint8_t activeConnections = 0;
            if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::ON)
                activeConnections++;
            if(ConnMuxRelay.StackMux1NumStatus == GpioModule::GpioStatus::ON || 
                ConnMuxRelay.StackMux2NumStatus == GpioModule::GpioStatus::ON)
                activeConnections++;
            if(ConnMuxRelay.SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::ON || 
                ConnMuxRelay.SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::ON)
                activeConnections++;

            if(activeConnections == 1)
            {
                // Last connection to the connector. Just turn off the relay or mux and it will be isolated
                if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::ON)
                {
                    if(StopPowerModule(PmConnections.PmId)) // TODO: Add time in stop power module if needed
                        if(RelayOff(ConnMuxRelay.RelayNum))
                            return true;
                }
                else if(ConnMuxRelay.SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::ON)
                {
                    if(StopPowerModule(PmConnections.PmId))
                        if(RelayOff(ConnMuxRelay.SuperSetInternalMux1Num))
                            return true;
                }
                else if (ConnMuxRelay.SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::ON)
                {
                    if (StopPowerModule(PmConnections.PmId))
                        if (RelayOff(ConnMuxRelay.SuperSetInternalMux2Num))
                            return true;
                }
                else if (ConnMuxRelay.StackMux1NumStatus == GpioModule::GpioStatus::ON)
                {
                    if (StopPowerModule(PmConnections.PmId))
                        if (RelayOff(ConnMuxRelay.StackMux1Num))
                            return true;
                }
                else if (ConnMuxRelay.StackMux2NumStatus == GpioModule::GpioStatus::ON)
                {
                    if (StopPowerModule(PmConnections.PmId))
                        if (RelayOff(ConnMuxRelay.StackMux2Num))
                            return true;
                }
                
            }
            else if (activeConnections == 2)
            {
                //TODO : Later (Not for standlone)
            }
            else if(activeConnections == 3)
            {
                //TODO : Later (Not for standlone)
            }
            else if(activeConnections > 3)
            {
                //TODO : not happening now . remove if not used later
                ESP_LOGE(TAG, "Connector %hu has more than 3 active connections. Manual Intervention Required.", connId);
                return false;
            }
            else
            {
                ESP_LOGI(TAG, "Connector %hu is already isolated.", connId);
                return true;
            }
        }
       
        while (((ConnMuxRelay.RelayNumStatus != GpioModule::GpioStatus::OFF) || (ConnMuxRelay.RelayNumStatus != GpioModule::GpioStatus::INVALID)) &&
               ((ConnMuxRelay.StackMux1NumStatus != GpioModule::GpioStatus::OFF) || (ConnMuxRelay.StackMux1NumStatus != GpioModule::GpioStatus::INVALID)) &&
               ((ConnMuxRelay.StackMux2NumStatus != GpioModule::GpioStatus::OFF) || (ConnMuxRelay.StackMux2NumStatus != GpioModule::GpioStatus::INVALID)) &&
               ((ConnMuxRelay.SuperSetInternalMux1NumStatus != GpioModule::GpioStatus::OFF) || (ConnMuxRelay.SuperSetInternalMux1NumStatus != GpioModule::GpioStatus::INVALID)) &&
               ((ConnMuxRelay.SuperSetInternalMux2NumStatus != GpioModule::GpioStatus::OFF) || (ConnMuxRelay.SuperSetInternalMux2NumStatus != GpioModule::GpioStatus::INVALID)))
        {

            if (ConnMuxRelay.RelayNumStatus != GpioModule::GpioStatus::OFF)
            {
                if (ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::ON)
                {
                    RelayPowerFlowDirection RelayPowerDirection = GetRelayPowerFlowDirection(connId, &ConnMuxRelay, &PmConnections, &PmStatus);
                    if (RelayPowerDirection == RelayPowerFlowDirection::RelayIsNotConnectedToAdjuscentRelay)
                    {
                        uint8_t PmIdToSwitchOff = (OddNumberConnector) ? (PmConnections.PmId + 2) : (PmConnections.PmId - 2);

                        if (StopPowerModule(PmIdToSwitchOff))
                            RelayOff(ConnMuxRelay.RelayNum);
                    }
                    else if (RelayPowerDirection == RelayPowerFlowDirection::SourcingPowerToAdjuscentRelays)
                    {
                        if (StopPowerModule(PmConnections.PmId))
                            RelayOff(ConnMuxRelay.RelayNum);
                    }
                    else if (RelayPowerDirection == RelayPowerFlowDirection::DrawingPowerFromAdjuscent1Relay)
                    {
                        uint8_t PmIdToSwitchOff = (OddNumberConnector) ? (PmConnections.PmId + 4) : (PmConnections.PmId - 4);
                        uint16_t RelayIdToSwitchOff = (OddNumberConnector) ? (ConnMuxRelay.RelayNum + 1) : (ConnMuxRelay.RelayNum - 1);
                        if (StopPowerModule(PmIdToSwitchOff))
                            RelayOff(RelayIdToSwitchOff);
                    }
                    else if (RelayPowerDirection == RelayPowerFlowDirection::DrawingPowerFromAdjuscent2Relays)
                    {
                        uint8_t PmIdToSwitchOff = (OddNumberConnector) ? (PmConnections.PmId + 6) : (PmConnections.PmId - 6);
                        uint16_t RelayIdToSwitchOff = (OddNumberConnector) ? (ConnMuxRelay.RelayNum + 2) : (ConnMuxRelay.RelayNum - 2);
                        if (StopPowerModule(PmIdToSwitchOff))
                            RelayOff(RelayIdToSwitchOff);
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Relay %hu Power Flow Direction Unkown", ConnMuxRelay.RelayNum);
                    }
                }
            }
            else if ((ConnMuxRelay.SuperSetInternalMux1NumStatus != GpioModule::GpioStatus::OFF) && (ConnMuxRelay.SuperSetInternalMux1NumStatus != GpioModule::GpioStatus::INVALID))
            {
                if (SourceConnId != static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT))
                {
                    RelayOff(ConnMuxRelay.SuperSetInternalMux1Num);
                }
                else
                {
                    // isolateConnector(connId, GetDestinationConnectorIdFromSuperSetInternalMuxId(connId, ConnMuxRelay.SuperSetInternalMux1Num));
                }
            }
            else if ((ConnMuxRelay.SuperSetInternalMux2NumStatus != GpioModule::GpioStatus::OFF) && (ConnMuxRelay.SuperSetInternalMux2NumStatus != GpioModule::GpioStatus::INVALID))
            {
                if (SourceConnId != static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT))
                {
                    RelayOff(ConnMuxRelay.SuperSetInternalMux2Num);
                }
                else
                {
                    // isolateConnector(connId, GetDestinationConnectorIdFromSuperSetInternalMuxId(connId, ConnMuxRelay.SuperSetInternalMux2Num));
                }
            }
            else if ((ConnMuxRelay.StackMux1NumStatus != GpioModule::GpioStatus::OFF) && (ConnMuxRelay.StackMux1NumStatus != GpioModule::GpioStatus::INVALID))
            {
                if (SourceConnId != static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT))
                {
                    RelayOff(ConnMuxRelay.StackMux1Num);
                }
                else
                {
                    // isolateConnector(connId, GetDestinationConnectorIdFromStackMuxId(connId, ConnMuxRelay.StackMux1Num));
                }
            }
            else if ((ConnMuxRelay.StackMux2NumStatus != GpioModule::GpioStatus::OFF) && (ConnMuxRelay.StackMux2NumStatus != GpioModule::GpioStatus::INVALID))
            {
                if (SourceConnId != static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT))
                {
                    RelayOff(ConnMuxRelay.StackMux2Num);
                }
                else
                {
                    // isolateConnector(connId, GetDestinationConnectorIdFromStackMuxId(connId, ConnMuxRelay.StackMux2Num));
                }
            }
            if (PmStatus.isActive)
            {
                if (PmStatus.isAlive == false)
                {
                }
            }
        }
        return false;
    }

    void PowerMuxController::AssignPowerModulesAndRelays(void)
    {
        for (uint8_t connId = 1; connId < NUM_OF_CONNECTORS; connId++)
        {
            if ((moduleStatus[connId].stateMachineState >= PLCModule::StateMachineState::Parameter) &&
                ((moduleStatus[connId].stateMachineState <= PLCModule::StateMachineState::Isolation)))
            {
                isolateConnectorAndAssignDefaultPowerModule(connId);
            }
        }
    }

    void PowerMuxController::AssignExtraPowerModules(uint8_t connId)
    {
        //TODO : Implementing only for 2 connectors for now. Add logic for other connectors if needed in future
        bool OddNumberConnector = (connId % 2) ? true : false;
        bool SufficientPower = true;
        if(! SufficientPower)
        {
            ConnectorMuxesRelays ConnMuxRelay = GetConnectorMuxesRelays(connId);

            uint8_t defaultPmRelay = ConnMuxRelay.RelayNum;
            uint8_t nextPmRelay = (OddNumberConnector) ? (defaultPmRelay + 1) : (defaultPmRelay - 1);
            uint8_t nextnextPmrelay = (OddNumberConnector) ? (defaultPmRelay + 2) : (defaultPmRelay - 2);

            uint8_t relays[3] = { defaultPmRelay, nextPmRelay, nextnextPmrelay };

            for(uint8_t i = 0; i < 3; i++)
            {
                if(gpio->GetRelayState(relays[i]) == GpioModule::GpioStatus::OFF)
                {
                    uint8_t pmId[2];
                    if(GetConnectorRelayPMs(connId,relays[i], pmId))
                    {
                        pmc->moduleStatus[pmId[0]].state = PowerModule::ChargingModuleState::NORMAL_OFF;
                        if(IsPowerModuleIsolated(pmId[0]))
                        {
                            gpio->RelayOn(relays[i]);
                            StartPowerModule(pmId[0]);
                            StartPowerModule(pmId[1]);
                        } 
                        else
                        {
                            return;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    continue;
                }
            }
            
        }

    }

    void PowerMuxController::PowerMuxControllerTask(void *pvParameters)
    {
        PowerMuxController *PowerMux = static_cast<PowerMuxController *>(pvParameters);
        while (true)
        {
            for (uint8_t connId = 1; connId <= NUM_OF_CONNECTORS; connId++)
            {
                if (((PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::Charge) ||
                     (PowerMux->moduleStatus[connId].stateMachineState == PLCModule::StateMachineState::PreCharge)) &&
                    (gpio->GetConnectorContactorState(connId) == GpioModule::GpioStatus::ON))
                {
                    uint8_t NumOfPowerModuleAssigned = 0;
                    for (uint16_t pm = 1; pm <= PowerModule::Constants::MAX_MODULES; pm++)
                    {
                        if (pmc->moduleStatus[pm].Connector == static_cast<PowerModule::ConnectorType>(connId))
                        {
                            NumOfPowerModuleAssigned++;
                            connectorPowerModules[connId].PmId[NumOfPowerModuleAssigned - 1] = pm;
                            connectorPowerModules[connId].PmCount = NumOfPowerModuleAssigned;
                        }
                    }
                    for (uint16_t pm = 1; pm <= PowerModule::Constants::MAX_MODULES; pm++)
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
                    
                    connectorPowerModules[connId].EVSEPower = PowerMux->moduleStatus[connId].EVSEPresentCurrent * PowerMux->moduleStatus[connId].EVSEPresentVoltage;
                    connectorPowerModules[connId].EVPower   = PowerMux->moduleStatus[connId].EVTargetCurrent * PowerMux->moduleStatus[connId].EVTargetVoltage;
                }
            }
            pmc->ModuleTimingCommand();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    // Constructor
    PowerMuxController::PowerMuxController(SendDataFunc func)
    {
        memcpy(SubSetRelay, config->SubSetRelay, sizeof(SubSetRelay));
        memcpy(SuperSetMux, config->SuperSetMux, sizeof(SuperSetMux));
        memcpy(StackMux, config->StackMux, sizeof(StackMux));

        memcpy(gpio->SubSetRelay, config->SubSetRelay, sizeof(SubSetRelay));
        memcpy(gpio->SuperSetMux, config->SuperSetMux, sizeof(SuperSetMux));
        memcpy(gpio->StackMux, config->StackMux, sizeof(StackMux));

        for(uint8_t i = 0; i < NUM_OF_POWER_MODULES ; i++)
        {
            uint8_t subsetId = i / 8;
            uint8_t pmPairOffset = ( i % 8 ) / 2;
            uint16_t relays[2] = {0};
            uint16_t relayBase = static_cast<uint16_t>(GpioModule::Contactor::SubSet_RelayBase);
            uint8_t contactor = static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT);

            switch(pmPairOffset)
            {
                case 0:
                    relays[0] = relayBase;
                    relays[1] = relayBase + (subsetId * 3) + 1 ;
                    contactor = (subsetId * 2) + 1 ;
                    break;
                case 1:
                    relays[0] = relayBase + (subsetId * 3) + 1 ;
                    relays[1] = relayBase + (subsetId * 3) + 2 ;
                    break;
                case 2:
                    relays[0] = relayBase + (subsetId * 3) + 2 ;
                    relays[1] = relayBase + (subsetId * 3) + 3 ;
                    break;
                case 3:
                    relays[0] = relayBase + (subsetId * 3) + 3 ;
                    relays[1] = relayBase ;
                    contactor = (subsetId * 2) + 2;
                    break;
            }

            auto &pm = powerModuleConnections[i];

            pm.PmId = i; // TODO : change to pmAddress or pmStruct if needed
            pm.subSetNum = subsetId; 
            pm.leftRelayNum = relays[0];
            pm.rightRelayNum = relays[1];
            pm.subSetAcContactorNum = contactor;
        }

        for (uint8_t i = 0; i < PLCModule::Constants::MAX_MODULES; i++)
        {
            controlPilotState_old[i] = PLCModule::ControlPilotState::SNA;
            stateMachineState_old[i] = PLCModule::StateMachineState::SNA;
            moduleStatus[i].EVSEMaxCurrent = 0;
            moduleStatus[i].EVSEMaxVoltage = 0;
            moduleStatus[i].EVSEMinCurrent = 0;
            moduleStatus[i].EVSEMinVoltage = 0;
            moduleStatus[i].EVSEPresentCurrent = 0;
            moduleStatus[i].EVSEPresentVoltage = 0;
            moduleStatus[i].EVSEMaxPower = 0;
        }
        for (uint8_t i = 1; i < PowerModule::Constants::MAX_MODULES; i++)
        {
            uint8_t SubSet = (i - 1) / 8;
            uint8_t SubSetModuleId = (i - 1) % 8;
            uint8_t SubSetModulePosition = SubSetModuleId / 2;
            uint8_t firstModule = (SubSetModuleId % 2 == 0) ? 0 : 1;
            ConfigModule::PowerModuleConfig *moduleConfig = &config->NumberOfSubSets[SubSet].PowerModule[SubSetModulePosition][firstModule];
            pmc->moduleStatus[i].isActive = moduleConfig->isActive;
            pmc->moduleStatus[i].moduleAddress = moduleConfig->moduleAddress;
            pmc->moduleStatus[i].MaxVoltage = moduleConfig->MaxVoltage;
            pmc->moduleStatus[i].MaxCurrent = moduleConfig->MaxCurrent;
            pmc->moduleStatus[i].MinVoltage = moduleConfig->MinVoltage;
            pmc->moduleStatus[i].MinCurrent = moduleConfig->MinCurrent;
            pmc->moduleStatus[i].MaxPower = moduleConfig->MaxPower;
            pmc->moduleStatus[i].MinPower = moduleConfig->MinPower;
            pmc->moduleStatus[i].MaxTemperature = moduleConfig->MaxTemperature;
            pmc->moduleStatus[i].MinTemperature = moduleConfig->MinTemperature;
        }

        xTaskCreate(&PowerMuxControllerTask, "PowerMuxControllerTask", 4096, this, 2, NULL);
        sendFunc = func;
    }

    // Destructor
    PowerMuxController::~PowerMuxController()
    {
        // Cleanup, if necessary
    }

    void PowerMuxController::SetSendFunction(SendDataFunc func)
    {
        sendFunc = func;
    }

    bool PowerMuxController::SendData(const uint32_t id, const uint64_t data)
    {
        if (sendFunc)
        {
            return sendFunc(id, data);
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

    bool PowerMuxController::IsConnectorIsolated(uint8_t connId)
    {
        ConnectorMuxesRelays ConnMuxRelay = GetConnectorMuxesRelays(connId);

        //TODO : Recheck 
        //Logic 2 : Status != GpioModule::GpioStatus::OFF

        if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::WELD) return false;
        if(ConnMuxRelay.SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::WELD) return false;
        if(ConnMuxRelay.SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::WELD) return false;
        if(ConnMuxRelay.StackMux1NumStatus == GpioModule::GpioStatus::WELD) return false;
        if(ConnMuxRelay.StackMux2NumStatus == GpioModule::GpioStatus::WELD) return false;

        if(ConnMuxRelay.RelayNumStatus == GpioModule::GpioStatus::ON) return false;
        if(ConnMuxRelay.SuperSetInternalMux1NumStatus == GpioModule::GpioStatus::ON) return false;
        if(ConnMuxRelay.SuperSetInternalMux2NumStatus == GpioModule::GpioStatus::ON) return false;
        if(ConnMuxRelay.StackMux1NumStatus == GpioModule::GpioStatus::ON) return false;
        if(ConnMuxRelay.StackMux2NumStatus == GpioModule::GpioStatus::ON) return false;

        return true; // isolated
    }

    bool PowerMuxController::IsPowerModuleIsolated(uint8_t pmId)
    {
        const PowerModuleConnections &pm = powerModuleConnections[pmId];
        
        if(pm.subSetAcContactorNum == static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT))
        {
            // Power Modules with two relays
            bool leftRelayIsoStatus = (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::ON) || (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::WELD);
            bool rightRelayIsoStatus = (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::ON) || (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::WELD);

            if(leftRelayIsoStatus || rightRelayIsoStatus)
                return false;
        }
        else
        {
            // Power Modules directly connected to contactor (only 1 relay)            
            bool contactorIsoStatus = 
                (gpio->GetAcContactorState(pm.subSetAcContactorNum) == GpioModule::GpioStatus::ON) || (gpio->GetAcContactorState(pm.subSetAcContactorNum) == GpioModule::GpioStatus::WELD);  //TODO : use supersetMux and stackMux for not standalones

            bool relayIsoStatus = (pm.leftRelayNum == (int)GpioModule::Contactor::SubSet_RelayBase)  
                            ? (gpio->GetRelayState(pm.rightRelayNum) == GpioModule::GpioStatus::ON ) || (gpio->GetRelayState(pm.rightRelayNum) == GpioModule::GpioStatus::WELD ) 
                            : (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::ON ) || (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::WELD );

            if(contactorIsoStatus || relayIsoStatus)
                return false;

        }

        return true;
    }

    bool PowerMuxController::IsPowerModuleLastConnection(uint8_t pmId)
    {
        const PowerModuleConnections &pm = powerModuleConnections[pmId];
        
        if(pm.subSetAcContactorNum == static_cast<uint8_t>(PowerModule::ConnectorType::DEFAULT))
        {
            // Power Modules with two relays
            bool leftRelayIsoStatus = (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::ON);
            bool rightRelayIsoStatus = (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::ON);

            if(leftRelayIsoStatus ^ rightRelayIsoStatus)
                return true;
        }
        else
        {
            // Power Modules directly connected to contactor (only 1 relay)            
            bool contactorIsoStatus_ = 
                (gpio->GetAcContactorState(pm.subSetAcContactorNum) == GpioModule::GpioStatus::ON) ;  

            bool contactorIsoStatus = 
                (pmm->moduleStatus[pm.subSetAcContactorNum].stateMachineState >= PLCModule::StateMachineState::Isolation &&
                pmm->moduleStatus[pm.subSetAcContactorNum].stateMachineState <= PLCModule::StateMachineState::StopCharge);
            //TODO : Also supersetMux and stackMux for not standalones

            bool relayIsoStatus = (pm.leftRelayNum == (int)GpioModule::Contactor::SubSet_RelayBase)
                            ? (gpio->GetRelayState(pm.rightRelayNum) == GpioModule::GpioStatus::ON ) 
                            : (gpio->GetRelayState(pm.leftRelayNum) == GpioModule::GpioStatus::ON );

            if(contactorIsoStatus ^ relayIsoStatus)
                return true;
        }

        return false;
    }

    bool PowerMuxController::GetConnectorRelayPMs(uint8_t connId, uint16_t relayId, uint8_t pmId[2])
    {
        //Fn that gives PMs that can be connected via given relay to connector
        //Note : Connecter and Relay Should be from same subset

        uint8_t ConnectorSubSet = (connId - 1) / 2;
        uint8_t PmSubset = ((relayId - (int)GpioModule::Contactor::SubSet_RelayBase) - 1 ) / 3;
        if (ConnectorSubSet != PmSubset ) 
            return false;

        bool OddNumberConnector = (connId % 2) ? true : false;
        uint8_t RelayOffset = ((relayId - (int)GpioModule::Contactor::SubSet_RelayBase) % 3) + 1 ;

        if(OddNumberConnector)
        {
            pmId[0] = PmSubset * 8 + RelayOffset * 2 ;
            pmId[1] = pmId[0] + 1 ;
        }
        else
        {
            pmId[0] = PmSubset * 8 + RelayOffset *2 - 2;
            pmId[1] = pmId[0] + 1 ;
        }
        return true;
    }

    void PowerMuxController::UpdateConnectorPowerModules()
    {
        //TODO : pass snapshot of pmc status if needed instead of accessing directly in loop(if better)
        for (uint8_t i = 0; i <= NUM_OF_CONNECTORS; i++) {
            connectorPowerModules[i].PmCount = 0;
        }
        
        for(uint8_t i = 0; i< NUM_OF_POWER_MODULES; i++)
        { 
            const auto& m = pmc->moduleStatus[i];

            if(m.Connector == PowerModule::ConnectorType::DEFAULT)
                continue;
            
            uint8_t connId = static_cast<uint8_t>(m.Connector);
            if(connectorPowerModules[connId].PmCount < MAX_PM_PER_CONNECTOR)
            {
                connectorPowerModules[connId].PmId[connectorPowerModules[connId].PmCount] = i;
                connectorPowerModules[connId].PmCount++;
            }
            else
            {
                //TODO : LOG if needed
            }            
        }
    }

    bool PowerMuxController::sufficientPower(uint8_t connId)
    {
        float currentPower = pmm->moduleStatus[connId].EVSEPresentCurrent * pmm->moduleStatus[connId].EVSEPresentVoltage;
        float targetPower = pmm->moduleStatus[connId].EVTargetCurrent * pmm->moduleStatus[connId].EVTargetVoltage;

        return (currentPower >= targetPower);
    }

    void PowerMuxController::optimizePowerModules()
    {
        //Only relay based optimization
        for(uint8_t pmId = 0; pmId < NUM_OF_POWER_MODULES; pmId = pmId + 2 )
        {
            //Only for middel modules
            if((pmId % 8) / 2 == 0 || (pmId % 8) / 2 == 3)
            {
               if(pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::NORMAL_OFF &&
               pmc->moduleStatus[pmId+1].state == PowerModule::ChargingModuleState::NORMAL_OFF)
               {
                    uint8_t connectorRgt = static_cast<uint8_t>(pmc->moduleStatus[pmId+2].Connector);
               }
            }

            if(pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::NORMAL_OFF &&
               pmc->moduleStatus[pmId+1].state == PowerModule::ChargingModuleState::NORMAL_OFF)
            {
                uint8_t connectorLft = static_cast<uint8_t>(pmc->moduleStatus[pmId-2].Connector);
                uint8_t connectorRgt = static_cast<uint8_t>(pmc->moduleStatus[pmId+2].Connector);
                if(PriorityConnector(connectorLft, connectorRgt) == connectorLft)
                {
                    // make sure relay is on for left connector and off for right connector
                    // if not already in that state
                    ConnectorMuxesRelays ConnMuxRelay = GetConnectorMuxesRelays(connectorLft);
                    if(ConnMuxRelay.RelayNumStatus != GpioModule::GpioStatus::ON)
                    {
                        gpio->RelayOn(ConnMuxRelay.RelayNum);
                        StartPowerModule(pmId);
                        StartPowerModule(pmId+1);
                    }
                }
                else
                {
                    ConnectorMuxesRelays ConnMuxRelay = GetConnectorMuxesRelays(connectorRgt);
                    if(ConnMuxRelay.RelayNumStatus != GpioModule::GpioStatus::ON)
                    {
                        gpio->RelayOn(ConnMuxRelay.RelayNum);
                        StartPowerModule(pmId);
                        StartPowerModule(pmId+1);
                    }
                }
            }
            else
            {
                continue;
            }
        }
    }

    uint8_t PowerMuxController::PriorityConnector(uint8_t ConnId1, uint8_t ConnId2)
    {
        if(connectorPowerModules[ConnId1].StartTime <= connectorPowerModules[ConnId2].StartTime)
            return ConnId1;
        else
            return ConnId2;
    }

    bool PowerMuxController::AddPowerModulesToConnector(uint8_t connId) //TODO: used to add power modules after default addition.
    {
        //TODO : CHECK IF NEEDED AT ALL. OPTIMISE WILL HANDLE THIS ALREADY PERIODICALYY
        //IF NEEDED TO ADD INTENTIONALLY : ADD or INCREASE PREFERENCE

        // if(PowerMuxController::sufficientPower(connId))
        for(int i = 0;i < MAX_NUM_OF_POWER_MODULES; i++)
        {

        }
        return false;
    }

    bool PowerMuxController::RemovePowerModulesFromConnector(uint8_t connId)
    {   
        for(uint8_t i=0; i<connectorPowerModules[connId].PmCount;i++)
        {
            uint8_t pmId = (connectorPowerModules[connId].PmId[i]);
            if(IsPowerModuleLastConnection(pmId))
            {
                //remove  the last connection
                //1. connector, pm same subset - switch off relays (only case in standalone)
                //2. connector, pm same superset, but differnet subsets - swith off superset muxes
                //3. connector, pm diff superset - sswitch off stackmux
                //ONLY ELSE BLOCk changes above cases 
                if(powerModuleConnections[pmId].leftRelayNum != static_cast<uint16_t>(GpioModule::Contactor::SubSet_RelayBase))
                {
                    //switch off powermodule -> if(succeed) switch off relay
                    if(pmc->ModuleStop(pmc->moduleStatus[pmId].moduleAddress, 0.0f, 0.0f))
                        RelayOff(powerModuleConnections[pmId].leftRelayNum);
                }
                else if(powerModuleConnections[pmId].rightRelayNum != static_cast<uint16_t>(GpioModule::Contactor::SubSet_RelayBase))
                {
                    if(pmc->ModuleStop(pmc->moduleStatus[pmId].moduleAddress, 0.0f, 0.0f))
                        RelayOff(powerModuleConnections[pmId].rightRelayNum);
                }
                else
                {
                    // STANDALONE : This means connected to connid - default model - cant remove
                    // OTHERS : Switch off powermodule and switch off muxes
                }                
            }
            else
            {
                continue;
            }
        }
        return true;
    }

    bool PowerMuxController::AssignDefaultPowerModules(uint8_t connId)
    {
        // Requirement : Connector and default power modules are already isolated.
        // 1. switch on connector contactor
        // 2. switch on default power modules

        if(gpio->ConnectorContactorOn(connId))
        {
            uint8_t pm[2] = { (connId == 1) ? 0 : 6, (connId == 1) ? 1 : 7 };
            bool pmStartResult[2] = { StartPowerModule(pm[0]), StartPowerModule(pm[1]) };
            return pmStartResult[0] || pmStartResult[1];
        }
        
    }

    uint8_t PowerMuxController::GetMergerId(uint8_t pmId,uint8_t connId)
    {
        if(connId == 0) return 0;

        switch(pmId % 8)
        {
            case 0:
            case 1:
                return (connId == 1) ? 0 : 1;
            case 2:
            case 3:
                return (connId == 1) ? 1 : 2;
            case 4:
            case 5:
                return (connId == 1) ? 2 : 3;
            case 6:
            case 7:
                return (connId == 1) ? 3 : 0;
            default:
                return 0; // Invalid PM ID and returns invalid Merger ID
        }
    }

    void PowerMuxController::OptimizePowerModulesForStandAlone()
    {
        //TODO : decide periodicity and implement timer or counter based mechanism to call optimizePowerModules function

        if(gpio->GetConnectorContactorState(1) == GpioModule::GpioStatus::OFF && gpio->GetConnectorContactorState(2) == GpioModule::GpioStatus::OFF)
        {
             //both connectors are active. no optimization possible
             return;
        }

        if(gpio->GetConnectorContactorState(1) == GpioModule::GpioStatus::ON && gpio->GetConnectorContactorState(2) == GpioModule::GpioStatus::ON)
        {
            // pm 2,3
            if(pmc->moduleStatus[2].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[3].state == PowerModule::ChargingModuleState::ON)
            {
                //Skip
            }
            else
            {
                if(PriorityConnector(1,2)==1)
                {
                    gpio->RelayOn(1);
                    bool pmStartResult[2] = {StartPowerModule(2),StartPowerModule(3)};                    
                }
                else
                {
                    if(pmc->moduleStatus[4].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[5].state == PowerModule::ChargingModuleState::ON)
                    {
                        gpio->RelayOn(2);
                        bool pmStartResult[2] = {StartPowerModule(2),StartPowerModule(3)};
                    }
                    
                }
            }

            // pm 4,5
            if(pmc->moduleStatus[4].state == PowerModule::ChargingModuleState::NORMAL_OFF && pmc->moduleStatus[5].state == PowerModule::ChargingModuleState::NORMAL_OFF)
            {
                //TODO : verify if one is fault off
                if(PriorityConnector(1,2)==1)
                {
                    if(pmc->moduleStatus[2].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[3].state == PowerModule::ChargingModuleState::ON)
                    {
                        gpio->RelayOn(2);
                        bool pmStartResult[2] = {StartPowerModule(4),StartPowerModule(5)};
                    }                 
                }
                else
                {
                    gpio->RelayOn(1);
                    bool pmStartResult[2] = {StartPowerModule(4),StartPowerModule(5)};                    
                }
            }
        }

        for(uint8_t pmId = 0 ; pmId < 8 ; pmId = pmId + 2 )
        {
            if(pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[pmId + 1].state == PowerModule::ChargingModuleState::ON) 
            {
                continue; // Already working
            }

            if(pmc->moduleStatus[pmId].state == PowerModule::ChargingModuleState::FAULT_OFF && pmc->moduleStatus[pmId + 1].state == PowerModule::ChargingModuleState::FAULT_OFF) 
            {
                continue; // Faulty modules, cant be optimized
            }

            //check if default modules
            if(pmId == 0 || pmId == 1)
            {
                //check if contactor is on
                if(gpio->GetConnectorContactorState(1) == GpioModule::GpioStatus::ON)
                {
                    continue; //TODO : CHECK ONLY : rarely/never comes here. only when contactor on and def powermodules off due to some reason.
                }
                else //TODO : remove this else and make if direct to reduce nesting
                {
                    if(pmc->moduleStatus[2].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[3].state == PowerModule::ChargingModuleState::ON)
                    {
                        gpio->RelayOn(1);
                        bool pmStartResult[2] = {StartPowerModule(0),StartPowerModule(1)};
                    }
                }
            }
            else if(pmId == 6 || pmId == 7)
            {
                if(gpio->GetConnectorContactorState(2) == GpioModule::GpioStatus::ON)
                {
                    continue;
                }
                else
                {
                    if(pmc->moduleStatus[4].state == PowerModule::ChargingModuleState::ON || pmc->moduleStatus[5].state == PowerModule::ChargingModuleState::ON)
                    {
                        gpio->RelayOn(3);
                        bool pmStartResult[2] = {StartPowerModule(6),StartPowerModule(7)};
                    }
                }
            }
            else
            {
                uint8_t connector = PriorityConnector((uint8_t)pmc->moduleStatus[pmId-2].Connector , (uint8_t)pmc->moduleStatus[pmId+2].Connector);
                uint8_t mergerId = GetMergerId(pmId, connector);
                //ON mergerId
                if(gpio->RelayOn(mergerId))
                {
                    bool pmStartResult[2] = {StartPowerModule(pmId),StartPowerModule(pmId+1)};
                }
            }
                


        }
    }

} // namespace PowerMuxModule
