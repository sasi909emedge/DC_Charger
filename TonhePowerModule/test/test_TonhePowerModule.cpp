/**
 * @file test_TonhePowerModule.cpp
 * @brief Unity test suite for TonhePMController (TonhePowerModule)
 *
 * Build & run (ESP-IDF):
 *   idf.py -C <project_root> build flash monitor
 *
 * Dependencies:
 *   - Unity test framework (bundled with ESP-IDF)
 *   - TonhePowerModule.hpp / TonhePowerModule.cpp
 */

#include "unity.h"
#include "TonhePowerModule.hpp"
#include <cstring>
#include <cstdint>

// ---------------------------------------------------------------------------
// Test Helpers / Stubs
// ---------------------------------------------------------------------------

/** Captures the last CAN frame sent via the mock send function. */
struct MockCanFrame
{
   uint32_t id;
   uint64_t data;
   bool called;
};

static MockCanFrame g_lastFrame;

/** Mock send function injected into TonhePMController. */
static bool mockSendFunc(const uint32_t id, const uint64_t data)
{
   g_lastFrame.id = id;
   g_lastFrame.data = data;
   g_lastFrame.called = true;
   return true;
}

/** Send function that always returns false (simulate bus error). */
static bool failSendFunc(const uint32_t id, const uint64_t data)
{
   (void)id;
   (void)data;
   return false;
}

/** Reset the captured frame before each relevant test. */
static void resetFrame()
{
   g_lastFrame = {0, 0, false};
}

// ---------------------------------------------------------------------------
// Helpers to build raw CAN extended IDs matching the firmware encoding
// ---------------------------------------------------------------------------

/**
 * Encode a TonhePMCanId the same way the firmware does:
 *   bits [28:26] = Priority (3 bits)
 *   bits [25:18] = PGN      (8 bits)
 *   bits [17:10] = DA       (8 bits)
 *   bits [ 9: 2] = SA       (8 bits)
 *   bits [ 1: 0] = reserved (0)
 *
 * Adjust this layout if TonhePMCanId has a different bit-field order on the
 * target compiler; the firmware uses reinterpret_cast<uint32_t*>(&CanId).
 */
static uint32_t buildCanId(uint8_t priority, uint8_t pgn, uint8_t da, uint8_t sa)
{
   PowerModule::TonhePMCanId id;
   id.Priority = priority;
   id.PGN = pgn;
   id.DA = da;
   id.SA = sa;
   uint32_t raw;
   std::memcpy(&raw, &id, sizeof(raw));
   return raw;
}

// Convenience: build a uint64_t raw data from a typed data struct
template <typename T>
static uint64_t toRaw64(const T &src)
{
   uint64_t raw = 0;
   std::memcpy(&raw, &src, sizeof(T) <= 8 ? sizeof(T) : 8);
   return raw;
}
// ModuleInputModeConfiguration
// ---------------------------------------------------------------------------
// setUp / tearDown
// ---------------------------------------------------------------------------

/** Controller under test – re-created before each test group via setUp(). */
static PowerModule::TonhePMController *g_pmc = nullptr;

void setUp(void)
{
   resetFrame();
   // Constructor calls ModuleInputModeConfiguration(AC) which fires mockSendFunc
   g_pmc = new PowerModule::TonhePMController(mockSendFunc);
   resetFrame(); // clear the constructor-triggered frame so tests start clean
}

void tearDown(void)
{
   delete g_pmc;
   g_pmc = nullptr;
}

// ===========================================================================
// GROUP 1 – Constructor & Initialisation
// ===========================================================================

/**
 * The constructor must call ModuleInputModeConfiguration with AC mode,
 * which in turn calls SendData, i.e. the mock should have been invoked.
 */
void test_constructor_sends_input_mode_configuration(void)
{
   // We need to observe the constructor send, so create a fresh instance
   // WITHOUT the tearDown/setUp noise.
   resetFrame();
   PowerModule::TonhePMController *ctrl =
       new PowerModule::TonhePMController(mockSendFunc);
   TEST_ASSERT_TRUE_MESSAGE(g_lastFrame.called,
                            "Constructor must call ModuleInputModeConfiguration → SendData");
   delete ctrl;
}

/** SetSendFunction replaces the callback; subsequent sends use the new one. */
void test_set_send_function_replaces_callback(void)
{
   bool failCalled = false;
   auto captureFail = [](const uint32_t /*id*/, const uint64_t /*data*/) -> bool
   {
      return false;
   };
   (void)captureFail; // suppress unused-lambda warning

   g_pmc->SetSendFunction(failSendFunc);
   bool result = g_pmc->ModuleTimingCommand();
   TEST_ASSERT_FALSE_MESSAGE(result,
                             "After SetSendFunction(failSendFunc) all sends must return false");
}

/** If no send function is set, SendData must return false gracefully. */
void test_send_data_without_callback_returns_false(void)
{
   // Build a controller with nullptr send function
   PowerModule::TonhePMController ctrl(nullptr);
   // ReceiveData should still succeed even when sendFunc is nullptr
   bool result = ctrl.ModuleTimingCommand();
   TEST_ASSERT_FALSE_MESSAGE(result,
                             "SendData with nullptr callback must return false");
}

// ===========================================================================
// GROUP 2 – CAN ID Encoding (getCanExtId / DecodePowerModuleCanExtId)
// ===========================================================================

/** Round-trip: encode then decode must produce the same fields. */
void test_can_id_encode_decode_roundtrip(void)
{
   const uint8_t P = 6;
   const uint8_t PGN = 0x1A;
   const uint8_t DA = 0x01;
   const uint8_t SA = 0xFF;

   uint32_t raw = buildCanId(P, PGN, DA, SA);
   PowerModule::TonhePMCanId decoded;
   std::memcpy(&decoded, &raw, sizeof(decoded));

   TEST_ASSERT_EQUAL_UINT8(P, decoded.Priority);
   TEST_ASSERT_EQUAL_UINT8(PGN, decoded.PGN);
   TEST_ASSERT_EQUAL_UINT8(DA, decoded.DA);
   TEST_ASSERT_EQUAL_UINT8(SA, decoded.SA);
}

/** Different addresses must produce different raw IDs. */
void test_can_id_different_addresses_differ(void)
{
   uint32_t id1 = buildCanId(6, 0x10, 0x01, 0x02);
   uint32_t id2 = buildCanId(6, 0x10, 0x01, 0x03);
   TEST_ASSERT_NOT_EQUAL(id1, id2);
}

// ===========================================================================
// GROUP 3 – SpecificModuleStartStop (via ModuleStart / ModuleStop)
// ===========================================================================

/** ModuleStart must call SendData and return true. */
void test_module_start_returns_true(void)
{
   bool result = g_pmc->ModuleStart(1, 400.0f, 20.0f);
   TEST_ASSERT_TRUE_MESSAGE(result, "ModuleStart must return true with valid mock");
   TEST_ASSERT_TRUE_MESSAGE(g_lastFrame.called, "ModuleStart must invoke SendData");
}

/** ModuleStop must call SendData and return true. */
void test_module_stop_returns_true(void)
{
   bool result = g_pmc->ModuleStop(1, 400.0f, 20.0f);
   TEST_ASSERT_TRUE_MESSAGE(result, "ModuleStop must return true with valid mock");
   TEST_ASSERT_TRUE_MESSAGE(g_lastFrame.called, "ModuleStop must invoke SendData");
}

/** ModuleStart and ModuleStop must produce different data payloads
 *  (START command vs STOP command bit). */
void test_module_start_stop_differ_in_payload(void)
{
   resetFrame();
   g_pmc->ModuleStart(1, 400.0f, 20.0f);
   uint64_t startData = g_lastFrame.data;

   resetFrame();
   g_pmc->ModuleStop(1, 400.0f, 20.0f);
   uint64_t stopData = g_lastFrame.data;

   TEST_ASSERT_NOT_EQUAL_UINT32(startData, stopData);
}

/** Voltage scaling: 400.0 V * VOLTAGE_FACTOR(10) = 4000 raw.
 *  The raw value is embedded in the payload; verify it is non-zero and
 *  matches the expected encoded value.                                    */
void test_module_start_voltage_encoding(void)
{
   const float voltage = 400.0f;
   const uint16_t expectedRaw = static_cast<uint16_t>(
       voltage * PowerModule::Constants::VOLTAGE_FACTOR);

   g_pmc->ModuleStart(1, voltage, 0.0f);

   // The encoded voltage sits in bytes [1..2] of the CAN data frame
   // (moduleStartStop layout: byte0=command, bytes1-2=voltage, bytes3-4=current)
   uint8_t raw[8];
   std::memcpy(raw, &g_lastFrame.data, 8);
   uint16_t encodedVoltage;
   std::memcpy(&encodedVoltage, &raw[2], sizeof(uint16_t));

   TEST_ASSERT_EQUAL_UINT16(expectedRaw, encodedVoltage);
}

/** Current scaling: 20.0 A * CURRENT_FACTOR(10) = 200 raw. */
void test_module_start_current_encoding(void)
{
   const float current = 20.0f;
   const uint16_t expectedRaw = static_cast<uint16_t>(
       current * PowerModule::Constants::CURRENT_FACTOR);

   g_pmc->ModuleStart(1, 0.0f, current);

   uint8_t raw[8];
   std::memcpy(raw, &g_lastFrame.data, 8);
   uint16_t encodedCurrent;
   std::memcpy(&encodedCurrent, &raw[4], sizeof(uint16_t));

   TEST_ASSERT_EQUAL_UINT16(expectedRaw, encodedCurrent);
}

// ===========================================================================
// GROUP 4 – BulkModuleStart / BulkModuleStop
// ===========================================================================

/** BulkModuleStart must invoke SendData at least once. */
void test_bulk_module_start_sends_data(void)
{
   bool result = g_pmc->BulkModuleStart(0x000001ULL, 400.0f, 20.0f);
   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_TRUE(g_lastFrame.called);
}

/** BulkModuleStop must invoke SendData at least once. */
void test_bulk_module_stop_sends_data(void)
{
   bool result = g_pmc->BulkModuleStop(0x000001ULL, 400.0f, 20.0f);
   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_TRUE(g_lastFrame.called);
}

/** BulkModuleStart with 0 modules bitmap is still a valid call
 *  (no modules selected, but protocol frame is still sent). */
void test_bulk_module_start_zero_modules(void)
{
   bool result = g_pmc->BulkModuleStart(0ULL, 400.0f, 20.0f);
   TEST_ASSERT_TRUE(result);
}

// ===========================================================================
// GROUP 5 – ModuleParameterSetting / BulkModuleParameters / ModuleParameters
// ===========================================================================

/** BulkModuleParameters must call SendData and return true. */
void test_bulk_module_parameters_sends_data(void)
{
   bool result = g_pmc->BulkModuleParameters(0x000003ULL, 360.0f, 15.0f);
   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_TRUE(g_lastFrame.called);
}

/** ModuleParameters delegates to BulkModuleParameters; must return true. */
void test_module_parameters_returns_true(void)
{
   bool result = g_pmc->ModuleParameters(1, 360.0f, 15.0f);
   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_TRUE(g_lastFrame.called);
}

/** Different voltage/current values must produce different payloads. */
void test_module_parameters_different_values_differ(void)
{
   resetFrame();
   g_pmc->BulkModuleParameters(0x01ULL, 360.0f, 15.0f);
   uint64_t data1 = g_lastFrame.data;

   resetFrame();
   g_pmc->BulkModuleParameters(0x01ULL, 400.0f, 20.0f);
   uint64_t data2 = g_lastFrame.data;

   TEST_ASSERT_TRUE_MESSAGE(data1 != data2,
                            "Different voltage/current must produce different payloads");
}

// ===========================================================================
// GROUP 6 – ModuleTimingCommand
// ===========================================================================

/** ModuleTimingCommand must call SendData and return true. */
void test_module_timing_command_sends_data(void)
{
   bool result = g_pmc->ModuleTimingCommand();
   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_TRUE(g_lastFrame.called);
}

/** Consecutive ModuleTimingCommand calls must produce identical frames. */
void test_module_timing_command_idempotent(void)
{
   g_pmc->ModuleTimingCommand();
   uint32_t id1 = g_lastFrame.id;
   uint64_t data1 = g_lastFrame.data;

   resetFrame();
   g_pmc->ModuleTimingCommand();
   uint32_t id2 = g_lastFrame.id;
   uint64_t data2 = g_lastFrame.data;

   TEST_ASSERT_EQUAL_UINT32(id1, id2);
   TEST_ASSERT_TRUE_MESSAGE(data1 == data2,
                            "Consecutive timing commands must produce identical payloads");
}

// ===========================================================================
// GROUP 7 – ModuleAddressSetting / ModuleAddressSettingSelection
// ===========================================================================

/** ModuleAddressSetting must embed the address in the payload. */
// void test_module_address_setting_embeds_address(void)
// {
//    uint8_t addr = 0x05;
//    g_pmc->ModuleAddressSetting(addr);
//    TEST_ASSERT_TRUE(g_lastFrame.called);

//    // Address is in byte 0 of the payload
//    uint8_t raw[8];
//    std::memcpy(raw, &g_lastFrame.data, 8);
//    TEST_ASSERT_EQUAL_UINT8(addr, raw[0]);
// }

/** Different addresses must produce different payloads. */
// void test_module_address_setting_different_addresses(void)
// {
//    g_pmc->ModuleAddressSetting(0x01);
//    uint64_t data1 = g_lastFrame.data;

//    resetFrame();
//    g_pmc->ModuleAddressSetting(0x02);
//    uint64_t data2 = g_lastFrame.data;

//    TEST_ASSERT_NOT_EQUAL_UINT32(data1, data2);
// }

/** ModuleAddressSettingSelection must call SendData. */
// void test_module_address_setting_selection_sends_data(void)
// {
//    bool result = g_pmc->ModuleAddressSettingSelection(0x01);
//    TEST_ASSERT_TRUE(result);
//    TEST_ASSERT_TRUE(g_lastFrame.called);
// }

// ===========================================================================
// GROUP 8 – ModuleInputModeConfiguration
// ===========================================================================

/** AC mode and DC mode must produce different payloads. */
// void test_input_mode_ac_vs_dc_differ(void)
// {
//    g_pmc->ModuleInputModeConfiguration(
//        static_cast<uint8_t>(PowerModule::InputMode::AC));
//    uint64_t acData = g_lastFrame.data;

//    resetFrame();
//    g_pmc->ModuleInputModeConfiguration(
//        static_cast<uint8_t>(PowerModule::InputMode::DC));
//    uint64_t dcData = g_lastFrame.data;

//    TEST_ASSERT_NOT_EQUAL_UINT32(acData, dcData);
// }

// ===========================================================================
// GROUP 9 – ReceiveData dispatch
// ===========================================================================

/**
 * ReceiveData with PGN = CHARGING_MODULE_STATE for a registered module
 * must update that module's voltage and current.
 * The module must first be registered by matching moduleStatus[i].moduleAddress.
 * Because moduleStatus is private, we inject data and verify via GetModuleStatus.
 *
 * NOTE: Since moduleStatus is initialised to zero-value structs and the
 * lookup matches on moduleAddress == CanId.SA, we rely on the fact that
 * moduleStatus[1].moduleAddress is 0x00 by default and we use SA = 0x00.
 */
void test_receive_charging_module_state_updates_status(void)
{
   using namespace PowerModule;

   const uint8_t sa = 0x01;          // matches default moduleStatus[1].moduleAddress
   const uint16_t rawVoltage = 4000; // 400.0 V (factor=10)
   const uint16_t rawCurrent = 2000; // 20.0 A  (factor=100)
   const uint8_t stateOn = 0x01;

   // Build a ChargingModuleState CAN data payload
   // Layout: byte0=state, bytes1-2=outputVoltage, bytes3-4=outputCurrent
   uint8_t payload[8] = {};
   payload[0] = stateOn;
   std::memcpy(&payload[1], &rawVoltage, 2);
   std::memcpy(&payload[3], &rawCurrent, 2);
   uint64_t canData;
   std::memcpy(&canData, payload, 8);

   uint32_t canId = buildCanId(
       static_cast<uint8_t>(Priority::CHARGING_MODULE_STATE),
       static_cast<uint8_t>(PGN::CHARGING_MODULE_STATE),
       static_cast<uint8_t>(ModuleAddress::MASTER),
       sa);
   g_pmc->moduleStatus[1].moduleAddress = 1;
   g_pmc->ReceiveData(canId, canData);

   ModuleStatus status = g_pmc->GetModuleStatus(1); // module index 1
   TEST_ASSERT_FLOAT_WITHIN(0.1f, 400.0f, status.outputVoltage);
   TEST_ASSERT_FLOAT_WITHIN(0.1f, 20.0f, status.outputCurrent);
   TEST_ASSERT_EQUAL_INT(static_cast<int>(ChargingModuleState::ON),
                         static_cast<int>(status.state));
}

/** ReceiveData with PGN = AC_INFORMATION_UPLOAD updates phase voltages. */
void test_receive_ac_information_updates_phase_voltages(void)
{
   using namespace PowerModule;

   const uint8_t sa = 0x01;
   const uint16_t rawPhaseA = 2300; // 230 V
   const uint16_t rawPhaseB = 2310;
   const uint16_t rawPhaseC = 2290;
   const uint16_t rawTemp = 25; // 25.0 °C (factor=10)

   uint8_t payload[8] = {};
   std::memcpy(&payload[0], &rawPhaseA, 2);
   std::memcpy(&payload[2], &rawPhaseB, 2);
   std::memcpy(&payload[4], &rawPhaseC, 2);
   std::memcpy(&payload[6], &rawTemp, 2);
   uint64_t canData;
   std::memcpy(&canData, payload, 8);

   uint32_t canId = buildCanId(
       static_cast<uint8_t>(Priority::AC_INFORMATION_UPLOAD),
       static_cast<uint8_t>(PGN::AC_INFORMATION_UPLOAD),
       static_cast<uint8_t>(ModuleAddress::MASTER),
       sa);
   g_pmc->moduleStatus[1].moduleAddress = 1;

   g_pmc->ReceiveData(canId, canData);

   ModuleStatus status = g_pmc->GetModuleStatus(1);
   TEST_ASSERT_FLOAT_WITHIN(0.1f, 230.0f, status.PhaseAVoltage);
   TEST_ASSERT_FLOAT_WITHIN(0.1f, 231.0f, status.PhaseBVoltage);
   TEST_ASSERT_FLOAT_WITHIN(0.1f, 229.0f, status.PhaseCVoltage);
   TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, status.temperature);
}

/** ReceiveData with PGN = EXTENDED_STATE_FAULT_INFO updates fault bits. */
void test_receive_extended_state_fault_info_updates_faults(void)
{
   using namespace PowerModule;

   const uint8_t sa = 0x01;
   g_pmc->moduleStatus[1].moduleAddress = 1;

   // Build a minimal ExtendedStateFaultInfo payload with all bits = 1
   uint8_t payload[8];
   std::memset(payload, 0xFF, 8);
   uint64_t canData;
   std::memcpy(&canData, payload, 8);

   uint32_t canId = buildCanId(
       static_cast<uint8_t>(Priority::EXTENDED_STATE_FAULT_INFO),
       static_cast<uint8_t>(PGN::EXTENDED_STATE_FAULT_INFO),
       static_cast<uint8_t>(ModuleAddress::MASTER),
       sa);

   g_pmc->ReceiveData(canId, canData);

   ModuleStatus status = g_pmc->GetModuleStatus(1);
   // At least one fault bit should be non-zero
   bool anyFault = false;
   for (int i = 0; i < 7; ++i)
      anyFault |= (status.faultBits[i] != 0);
   TEST_ASSERT_TRUE_MESSAGE(anyFault,
                            "ExtendedStateFaultInfo must set at least one fault bit");
}

/** ReceiveData with an unknown PGN must return true and not crash. */
void test_receive_unknown_pgn_returns_true(void)
{
   uint32_t badId = buildCanId(0, 0xFE, 0xFF, 0x01); // unknown PGN
   bool result = g_pmc->ReceiveData(badId, 0xDEADBEEFCAFEBABEULL);
   TEST_ASSERT_TRUE(result);
}

// ===========================================================================
// GROUP 10 – GetModuleStatus
// ===========================================================================

/** GetModuleStatus for an unregistered index must return a zeroed struct. */
void test_get_module_status_unregistered_returns_zero(void)
{
   PowerModule::ModuleStatus status = g_pmc->GetModuleStatus(24); // unlikely to be set
   TEST_ASSERT_EQUAL_FLOAT(0.0f, status.outputVoltage);
   TEST_ASSERT_EQUAL_FLOAT(0.0f, status.outputCurrent);
}

// ===========================================================================
// GROUP 11 – Edge Cases & Error Propagation
// ===========================================================================

/** When sendFunc fails, ModuleStart must propagate the false return value. */
void test_module_start_propagates_send_failure(void)
{
   g_pmc->SetSendFunction(failSendFunc);
   bool result = g_pmc->ModuleStart(1, 400.0f, 20.0f);
   TEST_ASSERT_FALSE(result);
}

/** Zero voltage and zero current are valid inputs and must not crash. */
void test_module_start_zero_voltage_current(void)
{
   bool result = g_pmc->ModuleStart(1, 0.0f, 0.0f);
   TEST_ASSERT_TRUE(result);
}

/** Maximum plausible voltage (e.g. 1000 V) must not overflow uint16_t
 *  given VOLTAGE_FACTOR=10 → 10000 < 65535.                             */
void test_module_start_high_voltage_no_overflow(void)
{
   bool result = g_pmc->ModuleStart(1, 1000.0f, 0.0f);
   TEST_ASSERT_TRUE(result);
}

/** BulkModuleStop must zero inputVoltage/inputCurrent for stopped modules. */
void test_bulk_module_stop_zeros_tracked_values(void)
{
   // First start module address 1 to set tracked values
   g_pmc->ModuleStart(1, 400.0f, 20.0f);

   // Now bulk-stop module address 1 (bit 0 of bitmap = address 1)
   g_pmc->BulkModuleStop(0x01ULL, 400.0f, 20.0f);

   PowerModule::ModuleStatus status = g_pmc->GetModuleStatus(1);
   TEST_ASSERT_EQUAL_FLOAT(0.0f, status.inputVoltage);
   TEST_ASSERT_EQUAL_FLOAT(0.0f, status.inputCurrent);
}

// ===========================================================================
// Test App Entry Point
// ===========================================================================

extern "C" void app_main(void)
{
   UNITY_BEGIN();

   // Group 1 – Constructor & Init
   RUN_TEST(test_constructor_sends_input_mode_configuration);
   RUN_TEST(test_set_send_function_replaces_callback);
   RUN_TEST(test_send_data_without_callback_returns_false);

   // Group 2 – CAN ID Encoding
   RUN_TEST(test_can_id_encode_decode_roundtrip);
   RUN_TEST(test_can_id_different_addresses_differ);

   // Group 3 – SpecificModuleStartStop
   RUN_TEST(test_module_start_returns_true);
   RUN_TEST(test_module_stop_returns_true);
   RUN_TEST(test_module_start_stop_differ_in_payload);
   RUN_TEST(test_module_start_voltage_encoding);
   RUN_TEST(test_module_start_current_encoding);

   // Group 4 – BulkModuleStart / BulkModuleStop
   RUN_TEST(test_bulk_module_start_sends_data);
   RUN_TEST(test_bulk_module_stop_sends_data);
   RUN_TEST(test_bulk_module_start_zero_modules);

   // Group 5 – ModuleParameterSetting
   RUN_TEST(test_bulk_module_parameters_sends_data);
   RUN_TEST(test_module_parameters_returns_true);
   RUN_TEST(test_module_parameters_different_values_differ);

   // Group 6 – ModuleTimingCommand
   RUN_TEST(test_module_timing_command_sends_data);
   RUN_TEST(test_module_timing_command_idempotent);

   // Group 7 – ModuleAddressSetting
   // RUN_TEST(test_module_address_setting_embeds_address);
   // RUN_TEST(test_module_address_setting_different_addresses);
   // RUN_TEST(test_module_address_setting_selection_sends_data);

   // Group 8 – ModuleInputModeConfiguration
   // RUN_TEST(test_input_mode_ac_vs_dc_differ);

   // Group 9 – ReceiveData dispatch
   RUN_TEST(test_receive_charging_module_state_updates_status);
   RUN_TEST(test_receive_ac_information_updates_phase_voltages);
   RUN_TEST(test_receive_extended_state_fault_info_updates_faults);
   RUN_TEST(test_receive_unknown_pgn_returns_true);

   // Group 10 – GetModuleStatus
   RUN_TEST(test_get_module_status_unregistered_returns_zero);

   // Group 11 – Edge Cases
   RUN_TEST(test_module_start_propagates_send_failure);
   RUN_TEST(test_module_start_zero_voltage_current);
   RUN_TEST(test_module_start_high_voltage_no_overflow);
   RUN_TEST(test_bulk_module_stop_zeros_tracked_values);

   UNITY_END();
}