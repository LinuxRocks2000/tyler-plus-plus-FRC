#include <frc/RobotBase.h>
#include <atomic>
#include <custom/tyler++better.hpp>


using namespace tpp;

class TPPRobot : public frc::RobotBase {
private:
    std::atomic<bool> m_exit{false};
    unsigned long localTick = 0;
public:
    TPPInterpreter interpreter;
    uint8_t mode = 0; // 0 = disabled, 1 = autonomous, 2 = test, 3 = teleop

    virtual void Init(){

    }

    virtual void BeginDisabled(){

    }

    virtual void DisabledLoop(){

    }

    virtual void CleanUpDisabled(){

    }

    virtual void BeginTeleop(){

    }

    virtual void TeleopLoop(){

    }

    virtual void CleanUpTeleop(){

    }

    virtual void BeginTest(){

    }

    virtual void TestLoop(){

    }

    virtual void CleanUpTest(){

    }

    virtual void BeginAutonomous(){

    }

    virtual void AutonomousLoop(){

    }

    virtual void CleanUpAutonomous(){

    }

    virtual void Loop(){

    } // User mainloop

    virtual void ItsOver(){

    }

    void StartCompetition(){
        Init();
        interpreter.registerFunction(&EXTERN_print, "print");
        interpreter.registerFunction(&EXTERN_printV, "printV");
        interpreter.registerFunction(&EXTERN_input, "input");
        interpreter.registerFunction(&EXTERN_stringMerge, "stringMerge");
        std::ifstream t("robot.tpp");
        std::stringstream buffer;
        buffer << t.rdbuf();
        interpreter.interpret(buffer.str());
        HAL_InitializeDriverStation();
        HAL_ObserveUserProgramStarting();
        while (!m_exit){ // Restructured from the old uglies. This one gives easy-peasy mainlooping without our ugly-mugly turdy-purdy stinky-winky infinite while loop
            Loop(); // General mainloop
            if (IsDisabled()){ // Disabled tasks
                HAL_ObserveUserProgramDisabled();
                if (mode != 0){
                    HAL_SendConsoleLine("Begin Disable mode");
                    localTick = 0; // Reset the local tick counter
                    BeginDisabled();
                }
                if (mode == 1){
                    CleanUpAutonomous();
                }
                else if (mode == 2){
                    CleanUpTest();
                }
                else if (mode == 3){
                    CleanUpTeleop();
                }
                DisabledLoop();
                mode = 0;
            }
            else if (IsAutonomous()){ // Autonomous tasks
                HAL_ObserveUserProgramAutonomous();
                if (mode != 1){
                    HAL_SendConsoleLine("Begin Autonomous mode");
                    localTick = 0; // Reset the local tick counter
                    BeginAutonomous();
                }
                if (mode == 0){
                    CleanUpDisabled();
                }
                else if (mode == 2){
                    CleanUpTest();
                }
                else if (mode == 3){
                    CleanUpTeleop();
                }
                AutonomousLoop();
                mode = 1;
            }
            else if (IsTest()){ // Test tasks
                HAL_ObserveUserProgramTest();
                if (mode != 2){
                    HAL_SendConsoleLine("Begin Test mode");
                    localTick = 0; // Reset the local tick counter
                    BeginTest();
                }
                if (mode == 0){
                    CleanUpDisabled();
                }
                else if (mode == 1){
                    CleanUpAutonomous();
                }
                else if (mode == 3){
                    CleanUpTeleop();
                }
                TestLoop();
                mode = 2;
            }
            else{ // Teleop tasks
                HAL_ObserveUserProgramTeleop();
                if (mode != 3){
                    HAL_SendConsoleLine("Begin Teleop mode");
                    localTick = 0; // Reset the local tick counter
                    BeginTeleop();
                }
                if (mode == 0){
                    CleanUpDisabled();
                }
                else if (mode == 1){
                    CleanUpAutonomous();
                }
                else if (mode == 2){
                    CleanUpTest();
                }
                TeleopLoop();
                mode = 3;
            }
            localTick++;
        }
        ItsOver();
    }
    void EndCompetition() {
        m_exit = true;
    }
};
