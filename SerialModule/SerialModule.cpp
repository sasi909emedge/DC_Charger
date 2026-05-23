#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include "SerialModule.hpp"
#include "ConfigModule.hpp"
#include <driver/uart.h>
#include <cstring>

#define TAG "SerialModule"

const uart_port_t SErial_UART = UART_NUM_2;

#define SLAVE_BUFF_SIZE 1024

SerialModule::SerialController *serial;

namespace SerialModule
{
    struct UART_Data
    {
        bool messageStart;
        bool errorStatus;
        bool writeStatus;
        bool errorMessage;
        bool messageCompletedStatus;
        bool messageLengthFirstByteStatus;
        bool messageLengthSecondByteStatus;

        uint8_t CommunicationFlag;
        uint16_t messageLengthFirstByte;
        uint16_t messageLengthSecondByte;
        uint16_t messageLength;
        uint16_t messageReceiveCounter;
        uint16_t messagechecksumValue;
        uint16_t checksumValue;
        uint16_t messageTimeOutCount;

        UART_Data()
            : messageStart(false),
              errorStatus(false),
              writeStatus(false),
              errorMessage(false),
              messageCompletedStatus(false),
              messageLengthFirstByteStatus(false),
              messageLengthSecondByteStatus(false),
              CommunicationFlag(0),
              messageLengthFirstByte(0),
              messageLengthSecondByte(0),
              messageLength(0),
              messageReceiveCounter(0),
              messagechecksumValue(0),
              checksumValue(0),
              messageTimeOutCount(0)
        {
        }
    };

    UART_Data serial_uart;

    void SerialController::serialTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to ensure other modules are initialized
        uint8_t slave_data[SLAVE_BUFF_SIZE];
        uint8_t receivedByte = 0;
        memset(&serial_uart, 0, sizeof(serial_uart));

        while (1)
        {
            int len = uart_read_bytes(SErial_UART, &receivedByte, 1, portMAX_DELAY);
            if (len > 0)
            {
                ESP_LOGD(TAG, "Byte : %hhu", receivedByte);
                if (serial_uart.messageStart == false)
                {
                    if (receivedByte == 0x55)
                        slave_data[0] = 0x55;
                    else if (receivedByte == 0xAA && slave_data[0] == 0x55)
                    {
                        serial_uart.messageStart = true;
                        slave_data[1] = 0xAA;
                        serial_uart.messagechecksumValue = 255; /*Integer value of 0xAA55 is 43605*/
                    }
                    else
                    {
                        slave_data[0] = 0;
                        serial_uart.messagechecksumValue = 0;
                    }
                }
                else if (serial_uart.messageLengthFirstByteStatus == false)
                {
                    slave_data[2] = receivedByte;
                    serial_uart.messageLengthFirstByte = receivedByte;
                    serial_uart.messageLengthFirstByteStatus = true;
                    serial_uart.messagechecksumValue += receivedByte;
                }
                else if (serial_uart.messageLengthSecondByteStatus == false)
                {
                    slave_data[3] = receivedByte;
                    serial_uart.messageLengthSecondByte = receivedByte;
                    serial_uart.messageLength = (serial_uart.messageLengthSecondByte * 256 + serial_uart.messageLengthFirstByte);
                    serial_uart.messageReceiveCounter = 0;
                    serial_uart.messageLengthSecondByteStatus = true;
                    serial_uart.messagechecksumValue += receivedByte;
                }
                else
                {
                    if (serial_uart.messageReceiveCounter < (serial_uart.messageLength))
                    {
                        slave_data[4 + serial_uart.messageReceiveCounter] = receivedByte;
                        if (serial_uart.messageReceiveCounter < (serial_uart.messageLength - 2))
                        {
                            serial_uart.messagechecksumValue += receivedByte;
                        }
                        else
                        {
                            if (serial_uart.messageReceiveCounter < (serial_uart.messageLength - 1))
                                serial_uart.checksumValue = receivedByte;
                            else
                            {
                                serial_uart.checksumValue += 256 * (receivedByte);
                                serial_uart.messageStart = false;
                                serial_uart.messageLengthFirstByteStatus = false;
                                serial_uart.messageLengthSecondByteStatus = false;
                                if (serial_uart.messagechecksumValue == serial_uart.checksumValue)
                                {
                                    serial_uart.CommunicationFlag = 0;
                                    serial_uart.messageTimeOutCount = 0;
                                    serial_uart.messageCompletedStatus = true;
                                    serial_uart.messagechecksumValue = 0;
                                    serial_uart.checksumValue = 0;
                                    serial_uart.errorMessage = false;
                                    if ((slave_data[4] + (slave_data[5] << 8)) == static_cast<uint16_t>(PacketId::CAN1))
                                    {
                                        serial->ReceiveData(PacketId::CAN1, &slave_data[6], serial_uart.messageLength - 4);
                                    }
                                    else if ((slave_data[4] + (slave_data[5] << 8)) == static_cast<uint16_t>(PacketId::CAN2))
                                    {
                                        serial->ReceiveData(PacketId::CAN2, &slave_data[6], serial_uart.messageLength - 4);
                                    }
                                    else if ((slave_data[4] + (slave_data[5] << 8)) == static_cast<uint16_t>(PacketId::ENERGY))
                                    {
                                        serial->ReceiveData(PacketId::ENERGY, &slave_data[6], serial_uart.messageLength - 4);
                                    }
                                    else if ((slave_data[4] + (slave_data[5] << 8)) == static_cast<uint16_t>(PacketId::HeartBeat))
                                    {
                                        serial->isActive = true;
                                        serial->HeartBeatCount = 0;
                                    }
                                    else
                                    {
                                        serial_uart.errorMessage = true;
                                        ESP_LOGE(TAG, "Error Message Id %d", (uint16_t)(slave_data[4] + (slave_data[5] << 8)));
                                    }
                                }
                                else
                                {
                                    serial_uart.errorMessage = true;
                                    serial_uart.messagechecksumValue = 0;
                                    serial_uart.checksumValue = 0;
                                    ESP_LOGE(TAG, "Error Message Checksum");
                                }
                            }
                        }
                    }
                    serial_uart.messageReceiveCounter++;
                }
            }
        }
    }
    // Constructor
    SerialController::SerialController(ReceiveDataFunc func)
    {
        uart_config_t uart_config = {};

        uart_config.baud_rate = 115200;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        uart_config.rx_flow_ctrl_thresh = 0;
        uart_config.source_clk = UART_SCLK_DEFAULT;

        uart_param_config(SErial_UART, &uart_config);
        uart_set_pin(SErial_UART, ESP_TX_PIN, ESP_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_driver_install(SErial_UART, 2048, 2048, 0, NULL, 0);

        xTaskCreate(&serialTask, "serialTask", 4096, this, 2, NULL);

        this->receiveFunc = func;
    }

    // Destructor
    SerialController::~SerialController()
    {
        // Cleanup, if necessary
    }

    void SerialController::SetReceiveFunction(ReceiveDataFunc func)
    {
        this->receiveFunc = func;
    }

    bool SerialController::SendData(const PacketId id, uint8_t *data, uint16_t length)
    {
        uint8_t dataBuffer[length + 8];
        uint16_t start = 0xAA55;
        uint16_t messageLength = length + 4; // 4 bytes for header
        std::memcpy(dataBuffer, &start, sizeof(start));
        std::memcpy(&dataBuffer[2], &messageLength, sizeof(messageLength));
        std::memcpy(&dataBuffer[4], &id, sizeof(id));
        std::memcpy(&dataBuffer[6], data, length);
        uint16_t checksum = 0;
        for (uint16_t i = 0; i < length + 6; i++)
        {
            checksum += dataBuffer[i];
        }
        std::memcpy(&dataBuffer[length + 6], &checksum, sizeof(checksum));
        uart_write_bytes(SErial_UART, (const uint8_t *)dataBuffer, sizeof(dataBuffer));
        uart_wait_tx_done(SErial_UART, 100 / portTICK_PERIOD_MS);
        return true; // or handle error
    }

    bool SerialController::ReceiveData(const PacketId id, uint8_t *data, uint16_t length)
    {
        if (receiveFunc)
        {
            return receiveFunc(id, data, length);
        }
        return false; // or handle error
    }

    bool SerialController::SendHeartBeat(void)
    {
        HeartBeatCount++;
        if (HeartBeatCount > 10)
        {
            isActive = false;
            ESP_LOGE(TAG, "Serial Communication Failure");
        }
        uint8_t dataBuffer[8];
        uint64_t currentTime = SystemTime->Get_NumofSecondsFromCurrentTime();
        std::memcpy(dataBuffer, &currentTime, sizeof(currentTime));
        return SendData(PacketId::HeartBeat, dataBuffer, sizeof(dataBuffer));
    }
} // namespace SerialModule
