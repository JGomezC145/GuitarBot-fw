#include <Arduino.h>
#include <unity.h>
#include "hal/ultrasonics.h"
#include "tasks/us_handle.h"

// Mock Arduino functions for testing
namespace MockArduino {
    static long mockPulseInValue = 0;
    static bool mockPulseInTimeout = false;
}

// Override pulseIn for testing
long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
    if (MockArduino::mockPulseInTimeout) {
        return 0; // Simulate timeout
    }
    return MockArduino::mockPulseInValue;
}

// Test helper to set mock pulse duration
void setMockPulseDuration(long durationMicros) {
    MockArduino::mockPulseInValue = durationMicros;
    MockArduino::mockPulseInTimeout = false;
}

// Test helper to simulate timeout
void setMockTimeout() {
    MockArduino::mockPulseInTimeout = true;
}

// =============================================================================
// Test Case 1: UltrasonicSensor::measureDistanceCm() returns correct distance
// =============================================================================

void test_measureDistanceCm_returns_correct_distance() {
    ultrasonics::UltrasonicSensor sensor(2, 3);
    
    // Test case: 10cm distance
    // Formula: distance = (duration / 2) / 29.1
    // So for 10cm: duration = 10 * 29.1 * 2 = 582 microseconds
    setMockPulseDuration(582);
    float distance = sensor.measureDistanceCm();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 10.0, distance);
    
    // Test case: 20cm distance
    // For 20cm: duration = 20 * 29.1 * 2 = 1164 microseconds
    setMockPulseDuration(1164);
    distance = sensor.measureDistanceCm();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 20.0, distance);
    
    // Test case: 50cm distance
    // For 50cm: duration = 50 * 29.1 * 2 = 2910 microseconds
    setMockPulseDuration(2910);
    distance = sensor.measureDistanceCm();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 50.0, distance);
}

void test_measureDistanceCm_returns_minus_one_on_timeout() {
    ultrasonics::UltrasonicSensor sensor(2, 3);
    
    // Simulate timeout
    setMockTimeout();
    float distance = sensor.measureDistanceCm();
    TEST_ASSERT_EQUAL_FLOAT(-1.0, distance);
}

// =============================================================================
// Test Case 2: UltrasonicSensor::hasObstacle() correctly identifies obstacles
// =============================================================================

void test_hasObstacle_detects_obstacle_within_threshold() {
    ultrasonics::UltrasonicSensor sensor(2, 3);
    
    // Test case: Distance 15cm, threshold 20cm -> should detect obstacle
    setMockPulseDuration(15 * 29.1 * 2);
    bool hasObstacle = sensor.hasObstacle(20.0);
    TEST_ASSERT_TRUE(hasObstacle);
    
    // Test case: Distance exactly at threshold
    setMockPulseDuration(20 * 29.1 * 2);
    hasObstacle = sensor.hasObstacle(20.0);
    TEST_ASSERT_TRUE(hasObstacle);
}

void test_hasObstacle_no_detection_beyond_threshold() {
    ultrasonics::UltrasonicSensor sensor(2, 3);
    
    // Test case: Distance 25cm, threshold 20cm -> should NOT detect obstacle
    setMockPulseDuration(25 * 29.1 * 2);
    bool hasObstacle = sensor.hasObstacle(20.0);
    TEST_ASSERT_FALSE(hasObstacle);
    
    // Test case: Distance 100cm, threshold 20cm -> should NOT detect obstacle
    setMockPulseDuration(100 * 29.1 * 2);
    hasObstacle = sensor.hasObstacle(20.0);
    TEST_ASSERT_FALSE(hasObstacle);
}

void test_hasObstacle_returns_false_on_timeout() {
    ultrasonics::UltrasonicSensor sensor(2, 3);
    
    // Simulate timeout (returns -1)
    setMockTimeout();
    bool hasObstacle = sensor.hasObstacle(20.0);
    TEST_ASSERT_FALSE(hasObstacle);
}

// =============================================================================
// Test Case 3: ultrasonics::hasObstacleAny() returns correct boolean
// =============================================================================

// Note: These tests require the actual sensor array to be initialized
// For proper testing, we would need to create a testable version of the 
// ultrasonics namespace or use dependency injection. Below are conceptual tests.

void test_hasObstacleAny_returns_true_when_any_sensor_detects() {
    // This test would require proper setup of the sensors array
    // In a real implementation, you might want to refactor the code to allow
    // dependency injection for testing purposes
    
    // Conceptual test:
    // - Set sensor 0 to detect nothing (distance > threshold)
    // - Set sensor 1 to detect obstacle (distance < threshold)
    // - Set sensor 2 to detect nothing
    // - Expected: hasObstacleAny() returns true
    
    // For now, we'll document this as a manual/integration test
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

void test_hasObstacleAny_returns_false_when_no_sensor_detects() {
    // This test would require proper setup of the sensors array
    
    // Conceptual test:
    // - Set all sensors to detect nothing (distance > threshold)
    // - Expected: hasObstacleAny() returns false
    
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

// =============================================================================
// Test Case 4: task_ultrasonics::checkObstacleAndExecute() executes callback
// =============================================================================

static bool callbackWasExecuted = false;

void testCallback() {
    callbackWasExecuted = true;
}

void test_checkObstacleAndExecute_executes_callback_when_obstacle_detected() {
    // Reset state
    callbackWasExecuted = false;
    
    // Note: This test requires the ultrasonics module to be properly initialized
    // and mocked. In a real scenario, you'd need dependency injection.
    
    // Conceptual test:
    // - Configure at least one sensor to detect an obstacle
    // - Call checkObstacleAndExecute with a callback
    // - Verify callback was executed
    
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

void test_checkObstacleAndExecute_does_not_execute_callback_when_no_obstacle() {
    // Reset state
    callbackWasExecuted = false;
    
    // Conceptual test:
    // - Configure all sensors to NOT detect obstacles
    // - Call checkObstacleAndExecute with a callback
    // - Verify callback was NOT executed
    
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

// =============================================================================
// Test Case 5: task_ultrasonics::getEachSensorObstacleStatus() returns accurate array
// =============================================================================

void test_getEachSensorObstacleStatus_returns_accurate_array() {
    // Conceptual test:
    // - Configure sensor 0 to detect obstacle (true)
    // - Configure sensor 1 to NOT detect obstacle (false)
    // - Configure sensor 2 to detect obstacle (true)
    // - Call getEachSensorObstacleStatus()
    // - Verify returned array is [true, false, true]
    
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

void test_getEachSensorObstacleStatus_all_false_when_no_obstacles() {
    // Conceptual test:
    // - Configure all sensors to NOT detect obstacles
    // - Call getEachSensorObstacleStatus()
    // - Verify all elements in array are false
    
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

void test_getEachSensorObstacleStatus_all_true_when_all_detect() {
    // Conceptual test:
    // - Configure all sensors to detect obstacles
    // - Call getEachSensorObstacleStatus()
    // - Verify all elements in array are true
    
    TEST_IGNORE_MESSAGE("Requires sensor array initialization - integration test needed");
}

// =============================================================================
// Test Runner Setup
// =============================================================================

void setUp(void) {
    // Set up code here (runs before each test)
    MockArduino::mockPulseInValue = 0;
    MockArduino::mockPulseInTimeout = false;
    callbackWasExecuted = false;
}

void tearDown(void) {
    // Clean up code here (runs after each test)
}

void setup() {
    delay(2000); // Wait for serial
    
    UNITY_BEGIN();
    
    // Test Case 1: measureDistanceCm
    RUN_TEST(test_measureDistanceCm_returns_correct_distance);
    RUN_TEST(test_measureDistanceCm_returns_minus_one_on_timeout);
    
    // Test Case 2: hasObstacle
    RUN_TEST(test_hasObstacle_detects_obstacle_within_threshold);
    RUN_TEST(test_hasObstacle_no_detection_beyond_threshold);
    RUN_TEST(test_hasObstacle_returns_false_on_timeout);
    
    // Test Case 3: hasObstacleAny (integration tests)
    RUN_TEST(test_hasObstacleAny_returns_true_when_any_sensor_detects);
    RUN_TEST(test_hasObstacleAny_returns_false_when_no_sensor_detects);
    
    // Test Case 4: checkObstacleAndExecute (integration tests)
    RUN_TEST(test_checkObstacleAndExecute_executes_callback_when_obstacle_detected);
    RUN_TEST(test_checkObstacleAndExecute_does_not_execute_callback_when_no_obstacle);
    
    // Test Case 5: getEachSensorObstacleStatus (integration tests)
    RUN_TEST(test_getEachSensorObstacleStatus_returns_accurate_array);
    RUN_TEST(test_getEachSensorObstacleStatus_all_false_when_no_obstacles);
    RUN_TEST(test_getEachSensorObstacleStatus_all_true_when_all_detect);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
