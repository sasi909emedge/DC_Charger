#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#define MAX_NUM_OF_SUBSETS 2
#define MAX_NUM_OF_SUPERSETS 1
#define MAX_NUM_OF_CONNECTORS 4
#define MAX_NUM_OF_POWER_MODULES 16

#define NUM_OF_SUBSETS 1
#define NUM_OF_SUPERSETS 1
#define NUM_OF_CONNECTORS 2
#define NUM_OF_POWER_MODULES 8

// STM32 GPIO Pin definitions
#define GPIO_PIN_0 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10 ((uint16_t)0x0400) /* Pin 10 selected   */
#define GPIO_PIN_11 ((uint16_t)0x0800) /* Pin 11 selected   */
#define GPIO_PIN_12 ((uint16_t)0x1000) /* Pin 12 selected   */
#define GPIO_PIN_13 ((uint16_t)0x2000) /* Pin 13 selected   */
#define GPIO_PIN_14 ((uint16_t)0x4000) /* Pin 14 selected   */
#define GPIO_PIN_15 ((uint16_t)0x8000) /* Pin 15 selected   */

// STM32 GPIO Port definitions
#define NOPORT 0
#define GPIOA 1
#define GPIOB 2
#define GPIOC 3
#define GPIOD 4
#define GPIOE 5
#define GPIOF 6
#define GPIOG 7
#define GPIOH 8
#define GPIOI 9

#define StmReset_Pin 18
#define ModemReset_Pin 21
#define EmergencyBtn_Pin 34
#define EarthFault_Pin 35
#define SpdFault_Pin 5
#define SmokeFault_Pin 4

#define AcPm2Fb_Pin GPIO_PIN_1
#define AcPm2Fb_GPIO_Port GPIOF
#define AcPm3Fb_Pin GPIO_PIN_2
#define AcPm3Fb_GPIO_Port GPIOF
#define AcPm4Fb_Pin GPIO_PIN_4
#define AcPm4Fb_GPIO_Port GPIOF
#define DcGun2NegFb_Pin GPIO_PIN_5
#define DcGun2NegFb_GPIO_Port GPIOF
#define DcGun2PosFb_Pin GPIO_PIN_6
#define DcGun2PosFb_GPIO_Port GPIOF
#define DcGun1PosFb_Pin GPIO_PIN_7
#define DcGun1PosFb_GPIO_Port GPIOF
#define DcGun1IsoFb_Pin GPIO_PIN_8
#define DcGun1IsoFb_GPIO_Port GPIOF
#define DcGun1NegFb_Pin GPIO_PIN_9
#define DcGun1NegFb_GPIO_Port GPIOF
#define Door_Pin GPIO_PIN_10
#define Door_GPIO_Port GPIOF
#define DcGun1FuseFb_Pin GPIO_PIN_4
#define DcGun1FuseFb_GPIO_Port GPIOA
#define DcGun2FuseFb_Pin GPIO_PIN_5
#define DcGun2FuseFb_GPIO_Port GPIOA
#define DcMerger3NegFb_Pin GPIO_PIN_6
#define DcMerger3NegFb_GPIO_Port GPIOA
#define DcMerger3PosFb_Pin GPIO_PIN_7
#define DcMerger3PosFb_GPIO_Port GPIOA
#define DcMerger2NegFb_Pin GPIO_PIN_4
#define DcMerger2NegFb_GPIO_Port GPIOC
#define DcMerger2PosFb_Pin GPIO_PIN_5
#define DcMerger2PosFb_GPIO_Port GPIOC
#define DcMerger1NegFb_Pin GPIO_PIN_2
#define DcMerger1NegFb_GPIO_Port GPIOB
#define DcMerger1PosFb_Pin GPIO_PIN_11
#define DcMerger1PosFb_GPIO_Port GPIOF
#define DcGun2IsoFb_Pin GPIO_PIN_12
#define DcGun2IsoFb_GPIO_Port GPIOF
#define AcPm1Fb_Pin GPIO_PIN_13
#define AcPm1Fb_GPIO_Port GPIOF
#define AcPm2_Pin GPIO_PIN_3
#define AcPm2_GPIO_Port GPIOG
#define AcPm3_Pin GPIO_PIN_4
#define AcPm3_GPIO_Port GPIOG
#define V12Control_Pin GPIO_PIN_6
#define V12Control_GPIO_Port GPIOG
#define DcMerger3_Pin GPIO_PIN_7
#define DcMerger3_GPIO_Port GPIOG
#define DcMerger2_Pin GPIO_PIN_8
#define DcMerger2_GPIO_Port GPIOG
#define DcMerger1_Pin GPIO_PIN_9
#define DcMerger1_GPIO_Port GPIOC
#define DcGun2_Pin GPIO_PIN_8
#define DcGun2_GPIO_Port GPIOA
#define DcGun1_Pin GPIO_PIN_9
#define DcGun1_GPIO_Port GPIOA
#define BatteryControl_Pin GPIO_PIN_10
#define BatteryControl_GPIO_Port GPIOA
#define AcPm4_Pin GPIO_PIN_12
#define AcPm4_GPIO_Port GPIOC
#define VentilationFan2_Pin GPIO_PIN_0
#define VentilationFan2_GPIO_Port GPIOD
#define VentilationFan1_Pin GPIO_PIN_1
#define VentilationFan1_GPIO_Port GPIOD
#define AcPm1_Pin GPIO_PIN_2
#define AcPm1_GPIO_Port GPIOD

#if CONFIG_IDF_TARGET_ESP32
#define ESP_TX_PIN 25
#define ESP_RX_PIN 33
#define ESP_RTS_PIN 6
#define ESP_CTS_PIN 7
#else
#define ESP_TX_PIN 1
#define ESP_RX_PIN 2
#define ESP_RTS_PIN 6
#define ESP_CTS_PIN 7
#endif

#define MODEM_RTS_PIN 15
#define MODEM_CTS_PIN 16
#define MODEM_TX_PIN 17
#define MODEM_RX_PIN 18

#define ETH_CS_PIN 10
#define ETH_MOSI_PIN 11
#define ETH_MISO_PIN 13
#define ETH_SCLK_PIN 12
#define ETH_SPI_HOST SPI2_HOST

#endif