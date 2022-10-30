#include <TPPRobot.hpp>
#include <Motor.hpp>
#include <rev/CANSparkMax.h>


struct Drivetrain{
    Motor* frontLeft = Motor::CreateMotor(new rev::CANSparkMax{MOTOR_FRONT_LEFT, rev::CANSparkMax::MotorType::kBrushless});
    Motor* backLeft = Motor::CreateMotor(new rev::CANSparkMax{MOTOR_BACK_LEFT, rev::CANSparkMax::MotorType::kBrushless});
    double _left;
    Motor* frontRight = Motor::CreateMotor(new rev::CANSparkMax{MOTOR_FRONT_RIGHT, rev::CANSparkMax::MotorType::kBrushless});
    Motor* backRight = Motor::CreateMotor(new rev::CANSparkMax{MOTOR_BACK_RIGHT, rev::CANSparkMax::MotorType::kBrushless});
    double _right;

    void left(double amount){
        _left += amount;
    }

    void right(double amount){
        _right += amount;
    }

    void forward(double amount){
        left(amount);
        right(amount);
    }

    void turn(double amount){
        left(amount);
        right(-amount);
    }

    void apply(){
        frontLeft -> Set(_left);
        backLeft -> Set(_left);
        frontRight -> Set(_right);
        backRight -> Set(_right);
        _left = 0;
        _right = 0;
    }
};


Drivetrain drive; // Global so the externs can use it


void EXTERN_driveForward(TPP_FUNCTION_ARGS){
    drive.forward(*(interpreter -> popStack() -> number));
}

void EXTERN_driveApply(TPP_FUNCTION_ARGS){
    drive.apply();
}


class Robot : public TPPRobot {
public:
    void Init(){
        interpreter.registerFunction(EXTERN_driveForward, "robot_driveForward");
        interpreter.registerFunction(EXTERN_driveApply, "robot_driveApply");
    }

    void TeleopLoop(){
        interpreter.call("teleopLoop");
        TPP_GARBAGECOLLECT();
    }
};


#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
