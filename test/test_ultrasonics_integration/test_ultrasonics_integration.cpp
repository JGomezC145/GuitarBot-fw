#include <Arduino.h>
#include <unity.h>
#include "hal/ultrasonics.h"
#include "tasks/us_handle.h"

// =============================================================================
// Integration Test Setup
// These tests require the actual hardware or proper mocking at the Arduino level
// =============================================================================

// Mock values for pulseIn - need to be set before each test
namespace MockIntegration {
    static long sensorPulseValues[3] = {0, 0, 0};
    static size_t currentSensorIndex = 0;
}

// Override pulseIn to return different values for different sensors
long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
    // Cycle through sensor values based on calls
    long value = MockIntegration::sensorPulseValues[MockIntegration::currentSensorIndex];
    MockIntegration::currentSensorIndex = (MockIntegration::currentSensorIndex + 1) % 3;
    return value;
}

// Helper to set mock distance for each sensor
void setMockDistancesForSensors(float cm0, float cm1, float cm2) {
    // Convert distance to pulse duration
    // Formula: duration = distance * 29.1 * 2
    MockIntegration::sensorPulseValues[0] = (cm0 < 0) ? 0 : (long)(cm0 * 29.1 * 2);
    MockIntegration::sensorPulseValues[1] = (cm1 < 0) ? 0 : (long)(cm1 * 29.1 * 2);
    MockIntegration::sensorPulseValues[2] = (cm2 < 0) ? 0 : (long)(cm2 * 29.1 * 2);
    MockIntegration::currentSensorIndex = 0;
}

// =============================================================================
// Test Case 3: ultrasonics::hasObstacleAny() integration tests
// =============================================================================

void test_hasObstacleAny_returns_true_when_first_sensor_detects() {
    // Setup: First sensor detects (10cm), others don't (50cm, 60cm)
    setMockDistancesForSensors(10.0, 50.0, 60.0);
    
    bool result = ultrasonics::hasObstacleAny(20.0);
    TEST_ASSERT_TRUE_MESSAGE(result, "Should detect obstacle from first sensor");
}

void test_hasObstacleAny_returns_true_when_middle_sensor_detects() {
    // Setup: Middle sensor detects (15cm), others don't (50cm, 60cm)
    setMockDistancesForSensors(50.0, 15.0, 60.0);
    
    bool result = ultrasonics::hasObstacleAny(20.0);
    TEST_ASSERT_TRUE_MESSAGE(result, "Should detect obstacle from middle sensor");
}

void test_hasObstacleAny_returns_true_when_last_sensor_detects() {
    // Setup: Last sensor detects (18cm), others don't (50cm, 60cm)
    setMockDistancesForSensors(50.0, 60.0, 18.0);
    
    bool result = ultrasonics::hasObstacleAny(20.0);
    TEST_ASSERT_TRUE_MESSAGE(result, "Should detect obstacle from last sensor");
}

void test_hasObstacleAny_returns_true_when_all_sensors_detect() {
    // Setup: All sensors detect obstacles
    setMockDistancesForSensors(10.0, 15.0, 18.0);
    
    bool result = ultrasonics::hasObstacleAny(20.0);
    TEST_ASSERT_TRUE_MESSAGE(result, "Should detect obstacle when all sensors detect");
}

void test_hasObstacleAny_returns_false_when_no_sensor_detects() {
    // Setup: No sensor detects obstacles (all distances > threshold)
    setMockDistancesForSensors(50.0, 60.0, 70.0);
    
    bool result = ultrasonics::hasObstacleAny(20.0);
    TEST_ASSERT_FALSE_MESSAGE(result, "Should not detect obstacle when all sensors are clear");
}

void test_hasObstacleAny_returns_false_when_all_timeout() {
    // Setup: All sensors timeout
    setMockDistancesForSensors(-1.0, -1.0, -1.0);
    
    bool result = ultrasonics::hasObstacleAny(20.0);
    TEST_ASSERT_FALSE_MESSAGE(result, "Should not detect obstacle when all sensors timeout");
}

// =============================================================================
// Test Case 4: task_ultrasonics::checkObstacleAndExecute() integration tests
// =============================================================================

static int callbackExecutionCount = 0;

void testCallback() {
    callbackExecutionCount++;
}

void test_checkObstacleAndExecute_executes_callback_when_obstacle_detected() {
    callbackExecutionCount = 0;
    
    // Setup: One sensor detects obstacle
    setMockDistancesForSensors(15.0, 50.0, 60.0);
    
    task_ultrasonics::checkObstacleAndExecute(20.0, testCallback);
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, callbackExecutionCount, 
        "Callback should be executed exactly once when obstacle is detected");
}

void test_checkObstacleAndExecute_does_not_execute_callback_when_no_obstacle() {
    callbackExecutionCount = 0;
    
    // Setup: No sensor detects obstacle
    setMockDistancesForSensors(50.0, 60.0, 70.0);
    
    task_ultrasonics::checkObstacleAndExecute(20.0, testCallback);
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, callbackExecutionCount, 
        "Callback should not be executed when no obstacle is detected");
}

void test_checkObstacleAndExecute_with_different_thresholds() {
    callbackExecutionCount = 0;
    
    // Setup: Distance is 25cm
    setMockDistancesForSensors(25.0, 50.0, 60.0);
    
    // Test with threshold 20cm - should not execute
    task_ultrasonics::checkObstacleAndExecute(20.0, testCallback);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, callbackExecutionCount, 
        "Should not execute with threshold 20cm");
    
    // Test with threshold 30cm - should execute
    setMockDistancesForSensors(25.0, 50.0, 60.0);
    task_ultrasonics::checkObstacleAndExecute(30.0, testCallback);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, callbackExecutionCount, 
        "Should execute with threshold 30cm");
}

// =============================================================================
// Test Case 5: task_ultrasonics::getEachSensorObstacleStatus() integration tests
// =============================================================================

void test_getEachSensorObstacleStatus_returns_accurate_mixed_array() {
    // Setup: Sensor 0 detects (10cm), Sensor 1 doesn't (50cm), Sensor 2 detects (15cm)
    setMockDistancesForSensors(10.0, 50.0, 15.0);
    
    auto status = task_ultrasonics::getEachSensorObstacleStatus(20.0);
    
    TEST_ASSERT_EQUAL_size_t_MESSAGE(3, status.size(), "Status array should have 3 elements");
    TEST_ASSERT_TRUE_MESSAGE(status[0], "Sensor 0 should detect obstacle (10cm < 20cm)");
    TEST_ASSERT_FALSE_MESSAGE(status[1], "Sensor 1 should not detect obstacle (50cm > 20cm)");
    TEST_ASSERT_TRUE_MESSAGE(status[2], "Sensor 2 should detect obstacle (15cm < 20cm)");
}

void test_getEachSensorObstacleStatus_all_false_when_no_obstacles() {
    // Setup: All sensors don't detect obstacles
    setMockDistancesForSensors(50.0, 60.0, 70.0);
    
    auto status = task_ultrasonics::getEachSensorObstacleStatus(20.0);
    
    TEST_ASSERT_EQUAL_size_t_MESSAGE(3, status.size(), "Status array should have 3 elements");
    TEST_ASSERT_FALSE_MESSAGE(status[0], "Sensor 0 should not detect obstacle");
    TEST_ASSERT_FALSE_MESSAGE(status[1], "Sensor 1 should not detect obstacle");
    TEST_ASSERT_FALSE_MESSAGE(status[2], "Sensor 2 should not detect obstacle");
}

void test_getEachSensorObstacleStatus_all_true_when_all_detect() {
    // Setup: All sensors detect obstacles
    setMockDistancesForSensors(10.0, 15.0, 18.0);
    
    auto status = task_ultrasonics::getEachSensorObstacleStatus(20.0);
    
    TEST_ASSERT_EQUAL_size_t_MESSAGE(3, status.size(), "Status array should have 3 elements");
    TEST_ASSERT_TRUE_MESSAGE(status[0], "Sensor 0 should detect obstacle");
    TEST_ASSERT_TRUE_MESSAGE(status[1], "Sensor 1 should detect obstacle");
    TEST_ASSERT_TRUE_MESSAGE(status[2], "Sensor 2 should detect obstacle");
}

void test_getEachSensorObstacleStatus_at_threshold_boundary() {
    // Setup: Sensors at exact threshold (20cm), just below (19.5cm), just above (20.5cm)
    setMockDistancesForSensors(20.0, 19.5, 20.5);
    
    auto status = task_ultrasonics::getEachSensorObstacleStatus(20.0);
    
    TEST_ASSERT_EQUAL_size_t_MESSAGE(3, status.size(), "Status array should have 3 elements");
    TEST_ASSERT_TRUE_MESSAGE(status[0], "Sensor 0 should detect obstacle (exactly at threshold)");
    TEST_ASSERT_TRUE_MESSAGE(status[1], "Sensor 1 should detect obstacle (19.5cm < 20cm)");
    TEST_ASSERT_FALSE_MESSAGE(status[2], "Sensor 2 should not detect obstacle (20.5cm > 20cm)");
}

void test_getEachSensorObstacleStatus_with_timeout() {
    // Setup: One sensor times out, others have valid readings
    setMockDistancesForSensors(10.0, -1.0, 15.0);
    
    auto status = task_ultrasonics::getEachSensorObstacleStatus(20.0);
    
    TEST_ASSERT_EQUAL_size_t_MESSAGE(3, status.size(), "Status array should have 3 elements");
    TEST_ASSERT_TRUE_MESSAGE(status[0], "Sensor 0 should detect obstacle");
    TEST_ASSERT_FALSE_MESSAGE(status[1], "Sensor 1 should not detect obstacle (timeout)");
    TEST_ASSERT_TRUE_MESSAGE(status[2], "Sensor 2 should detect obstacle");
}

// =============================================================================
// Test Runner Setup
// =============================================================================

void setUp(void) {
    // Reset mock state before each test
    MockIntegration::currentSensorIndex = 0;
    callbackExecutionCount = 0;
}

void tearDown(void) {
    // Clean up after each test
}

void setup() {
    delay(2000); // Wait for serial monitor
    
    UNITY_BEGIN();
    
    // Test Case 3: hasObstacleAny
    RUN_TEST(test_hasObstacleAny_returns_true_when_first_sensor_detects);
    RUN_TEST(test_hasObstacleAny_returns_true_when_middle_sensor_detects);
    RUN_TEST(test_hasObstacleAny_returns_true_when_last_sensor_detects);
    RUN_TEST(test_hasObstacleAny_returns_true_when_all_sensors_detect);
    RUN_TEST(test_hasObstacleAny_returns_false_when_no_sensor_detects);
    RUN_TEST(test_hasObstacleAny_returns_false_when_all_timeout);
    
    // Test Case 4: checkObstacleAndExecute
    RUN_TEST(test_checkObstacleAndExecute_executes_callback_when_obstacle_detected);
    RUN_TEST(test_checkObstacleAndExecute_does_not_execute_callback_when_no_obstacle);
    RUN_TEST(test_checkObstacleAndExecute_with_different_thresholds);
    
    // Test Case 5: getEachSensorObstacleStatus
    RUN_TEST(test_getEachSensorObstacleStatus_returns_accurate_mixed_array);
    RUN_TEST(test_getEachSensorObstacleStatus_all_false_when_no_obstacles);
    RUN_TEST(test_getEachSensorObstacleStatus_all_true_when_all_detect);
    RUN_TEST(test_getEachSensorObstacleStatus_at_threshold_boundary);
    RUN_TEST(test_getEachSensorObstacleStatus_with_timeout);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
