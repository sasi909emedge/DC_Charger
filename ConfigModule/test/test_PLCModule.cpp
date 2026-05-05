/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <limits.h>
#include "unity.h"
#include "PLCModule.hpp"

bool SendPLCModuleDataToSerial(const uint32_t id, const uint64_t data);

TEST_CASE("ReceiveData Test", "[PLC_ReceiveData]")
{
   uint32_t id = 768;
   uint64_t data = 16891075845162460;
   PLCModule::PLCController PLC(SendPLCModuleDataToSerial);
   PLC.ReceiveData(id, data);
   PLCModule::PLCData plcData;
   plcData.EVDCMaxLimits.EVMaxCurrent = 150.0f;
   plcData.EVDCMaxLimits.EVMaxVoltage = 400.0f;
   plcData.EVDCMaxLimits.EVMaxPower = 60000.0f;

   float voltage = PLC.Get_EVSEMaxVoltage(0);
   TEST_ASSERT_EQUAL(400.0f, voltage);
   float current = PLC.Get_EVSEMaxCurrent(0);
   TEST_ASSERT_EQUAL(150.0f, current);
   float power = PLC.Get_EVSEMaxPower(0);
   TEST_ASSERT_EQUAL(60000.0f, power);
   float energy = PLC.Get_EVSEEnergyToBeDelivered(0);
   TEST_ASSERT_EQUAL(6000.0f, energy);
}