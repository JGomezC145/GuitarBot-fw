#include "us_handle.h"
namespace task_ultrasonics {

    bool hasObstacleAny(float thresholdCm) {
        return ultrasonics::hasObstacleAny(thresholdCm);
    }

    void checkObstacleAndExecute(float thresholdCm, const std::function<void()> &callback) {
        if (ultrasonics::hasObstacleAny(thresholdCm)) {
            callback();
        }
    }

    void checkEachSensorObstacle(float thresholdCm, const std::function<void(const std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)>&)>& callback) {
        std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)> status = getEachSensorObstacleStatus(thresholdCm);
        callback(status);
    }

    std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)> getEachSensorObstacleStatus(float thresholdCm) {
        std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)> status{};
        for (size_t i = 0; i < static_cast<size_t>(ultrasonics::Sensor::Count); ++i) {
            status[i] = ultrasonics::hasObstacle(static_cast<ultrasonics::Sensor>(i), thresholdCm);
        }
        return status;
    }

} // namespace task_ultrasonics