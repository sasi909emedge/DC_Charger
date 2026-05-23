/* test_mean.c: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "unity.h"
#include "PLCModule.hpp"

static PLCModule::PLCController *g_plc = nullptr;

bool SendData(const uint32_t id, const uint64_t data)
{
   return true; // Mock implementation, always return true
}

void setUp(void)
{
   g_plc = new PLCModule::PLCController(SendData);
}

void tearDown(void)
{
   delete g_plc;
   g_plc = nullptr;
}

void test_canReceiveErrorCodes(void)
{
   uint32_t canid = static_cast<uint32_t>(PLCModule::PLC_ID::ErrorCodes);
   uint64_t data = 0;
   PLCModule::ErrorCodes ErrorCode = {};
   ErrorCode.interface0 = static_cast<uint64_t>(PLCModule::Interface::Customer);
   ErrorCode.severity0 = static_cast<uint64_t>(PLCModule::Severity::Critical);
   ErrorCode.category0 = static_cast<uint64_t>(PLCModule::Category::CAN);
   ErrorCode.errorCode0 = static_cast<uint64_t>(PLCModule::EC_CAN::STATE_MACHINE_STATE_RANGE_OVERFLOW);
   memcpy(&data, &ErrorCode, sizeof(ErrorCode));

   bool result = g_plc->ReceiveData(canid, data);
   TEST_ASSERT_TRUE_MESSAGE(result, "ReceiveCanFrame must return true with valid mock");
}

// ===========================================================================
// Test App Entry Point
// ===========================================================================

extern "C" void app_main(void)
{
   UNITY_BEGIN();

   // Group 1 – Constructor & Init
   RUN_TEST(test_canReceiveErrorCodes);

   UNITY_END();
}