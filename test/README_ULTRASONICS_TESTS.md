# Ultrasonic Sensor Unit Tests

This directory contains comprehensive unit tests for the ultrasonic sensor functionality in the GuitarBot firmware.

## Test Files

### 1. `test_ultrasonics/test_ultrasonics.cpp`
**Unit tests for individual sensor methods** - Tests the core functionality of the `UltrasonicSensor` class without requiring the full sensor array initialization.

#### Test Cases Covered:
- ✅ **Test Case 1**: `UltrasonicSensor::measureDistanceCm()` 
  - Returns correct distance in cm for various distances (10cm, 20cm, 50cm)
  - Returns -1 on timeout
  
- ✅ **Test Case 2**: `UltrasonicSensor::hasObstacle()`
  - Correctly detects obstacles within threshold
  - Does not detect obstacles beyond threshold
  - Returns false on timeout

### 2. `test_ultrasonics_integration/test_ultrasonics_integration.cpp`
**Integration tests for the full sensor system** - Tests the behavior of the sensor array and task functions.

#### Test Cases Covered:
- ✅ **Test Case 3**: `ultrasonics::hasObstacleAny()`
  - Returns true when any sensor (first, middle, or last) detects an obstacle
  - Returns true when all sensors detect obstacles
  - Returns false when no sensors detect obstacles
  - Returns false when all sensors timeout

- ✅ **Test Case 4**: `task_ultrasonics::checkObstacleAndExecute()`
  - Executes callback only when an obstacle is detected
  - Does not execute callback when no obstacle is detected
  - Works correctly with different threshold values

- ✅ **Test Case 5**: `task_ultrasonics::getEachSensorObstacleStatus()`
  - Returns accurate boolean array for mixed sensor states
  - Returns all false when no obstacles detected
  - Returns all true when all sensors detect obstacles
  - Handles threshold boundary conditions correctly
  - Handles sensor timeouts correctly

## Running the Tests

### Using PlatformIO CLI

Run a specific test suite:
```bash
# Run unit tests
pio test -f test_ultrasonics

# Run integration tests
pio test -f test_ultrasonics_integration

# Run all tests
pio test
```

### Using PlatformIO IDE (VSCode)
1. Open the PlatformIO extension
2. Go to the "Testing" section
3. Select the test you want to run
4. Click the play button

### Test Output
The tests use the Unity testing framework. Output will show:
- Number of tests run
- Number of tests passed/failed
- Details of any failures

Example output:
```
test/test_ultrasonics/test_ultrasonics.cpp:35:test_measureDistanceCm_returns_correct_distance	[PASSED]
test/test_ultrasonics/test_ultrasonics.cpp:58:test_measureDistanceCm_returns_minus_one_on_timeout	[PASSED]
...
-----------------------
5 Tests 0 Failures 0 Ignored
OK
```

## Test Architecture

### Mocking Strategy
Both test files override the `pulseIn()` Arduino function to simulate sensor readings without requiring actual hardware:

- **Unit tests**: Mock returns a single configurable value for testing individual sensor instances
- **Integration tests**: Mock cycles through an array of values to simulate multiple sensors

### Test Helpers

#### `setMockPulseDuration(long durationMicros)`
Sets the pulse duration that the mock will return (for unit tests).

#### `setMockTimeout()`
Simulates a sensor timeout condition (for unit tests).

#### `setMockDistancesForSensors(float cm0, float cm1, float cm2)`
Configures all three sensors with specific distance values (for integration tests).

## Distance Calculation
The ultrasonic sensor uses the following formula:
```
distance (cm) = (pulse_duration_microseconds / 2) / 29.1
```

Therefore, to set a mock distance:
```
pulse_duration = distance * 29.1 * 2
```

## Adding New Tests

To add a new test:

1. Create a test function:
```cpp
void test_my_new_test() {
    // Setup
    setMockPulseDuration(582); // 10cm
    
    // Execute
    float distance = sensor.measureDistanceCm();
    
    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.1, 10.0, distance);
}
```

2. Register it in `setup()`:
```cpp
void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_my_new_test);
    UNITY_END();
}
```

## Notes and Limitations

### Current Limitations
- Tests marked with `TEST_IGNORE_MESSAGE` in the unit test file indicate areas where the current architecture makes testing difficult without dependency injection
- The integration tests assume the sensor array in `ultrasonics.cpp` is properly initialized

### Future Improvements
Consider refactoring the production code to:
- Allow dependency injection of the sensor array
- Provide a testable interface for the ultrasonics module
- Make it easier to test without hardware mocking

## Dependencies
- **Unity**: PlatformIO's built-in unit testing framework
- **Arduino Framework**: For Arduino types and basic functions

## References
- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity)
- [HC-SR04 Ultrasonic Sensor Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf)
