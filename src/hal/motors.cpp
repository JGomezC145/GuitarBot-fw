#include "motors.h"
#include <AccelStepper.h>
#include "config.h"

namespace motors {

    static AccelStepper motor1(AccelStepper::DRIVER, M1_PUL_PLUS, M1_DIR_PLUS);
    static AccelStepper motor2(AccelStepper::DRIVER, M2_PUL_PLUS, M2_DIR_PLUS);

    void init() {
        motor1.setMaxSpeed(1000);
        motor1.setAcceleration(500);
        motor2.setMaxSpeed(1000);
        motor2.setAcceleration(500);
    }

    void setMotor1Speed(float speed) {
        motor1.setSpeed(speed);
    }

    void setMotor2Speed(float speed) {
        motor2.setSpeed(speed);
    }

    void moveMotor1Infinitely(Direction direction) {
        if (direction == Direction::Forward) {
            motor1.moveTo(1000000); // Mover a una posición muy lejana
        } else if (direction == Direction::Backward) {
            motor1.moveTo(-1000000); // Mover a una posición muy lejana en sentido contrario
        } else if (direction == Direction::Stop) {
            motor1.stop(); // Detener motor
        }
    }

    void moveMotor2Infinitely(Direction direction) {
        if (direction == Direction::Forward) {
            motor2.moveTo(1000000); // Mover a una posición muy lejana
        } else if (direction == Direction::Backward) {
            motor2.moveTo(-1000000); // Mover a una posición muy lejana en sentido contrario
        } else if (direction == Direction::Stop) {
            motor2.stop(); // Detener motor
        }
    }

    void moveMotor1To(long position) {
        motor1.moveTo(position);
    }

    void moveMotor2To(long position) {
        motor2.moveTo(position);
    }

    void run() {
        motor1.run();
        motor2.run();
    }

    void stopAll() {
        motor1.stop();
        motor2.stop();
    }

    void emergencyStop() {
        motor1.setSpeed(0);
        motor1.moveTo(motor1.currentPosition());  // fuerza fin del movimiento
        motor2.setSpeed(0);
        motor2.moveTo(motor2.currentPosition());  // fuerza fin del movimiento
    }


} // namespace motors