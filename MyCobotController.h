#ifndef __MYCOBOT_CONTROLLER__
#define __MYCOBOT_CONTROLLER__

#include <MyCobotBasic.h>
#include <ParameterList.h>
#include <ArduinoJson.h>
#include "IMyCobotController.h"

/**
 * Controller which controls MyCobot
 */
class MyCobotController : public IMyCobotController {
  public:
    MyCobotController();
    void setup() override;
    unsigned int coordsNumber() override;
    int minCoordValue() override;
    int maxCoordValue() override;
    int minAngleId() override;
    int maxAngleId() override;
    int minAngleValue() override;
    int maxAngleValue() override;
    int minSpeedValue() override;
    int maxSpeedValue() override;
    int minGripperValue() override;
    int maxGripperValue() override;
    char* lastError() override;
    bool verifyAxis(const Axis axis) override;

    Axis parseAxis(const char* axis) override;
    Axis parseAxis(const int axis) override;
    int axisToInt(const Axis axis) override;
    char* axisToChar(const Axis axis) override;

    int* color() override;
    Coords coords() override;
    float coord(const char* axis) override;
    Angles angles() override;
    float angle(int id) override;
    int speed() override;
    int gripper() override;

    bool setColor(int r, int g, int b) override;
    bool setCoords(Coords coords) override;
    bool setCoord(Axis axis, float value) override;
    bool setAngles(Angles angles) override;
    bool setAngle(int id, float value) override;
    bool setSpeed(int speed) override;
    bool setGripper(int gripper) override;

  private:
    MyCobotBasic _myCobot;
    char* _error;
    int _speed;
    unsigned int _color[3];
    unsigned int _gripper;
    
    const unsigned int _DELAY = 200;
    const unsigned int _COORDSNUMBER = 6;
    const int _DEFAULT_SPEED = 30;
    const int _DEFAULT_GRIPPER = 0;
    const int _REPETITION_COUNTER = 5;
    const float _COORDS_ERROR_VALUE = -1000.0;
    const int _GENERIC_ERROR_VALUE = -1;
    const char* _SETTINGS_FILE = "/myCobot.json";

    const int _MIN_COLOR_VALUE = 0;
    const int _MAX_COLOR_VALUE = 255;
    const int _MIN_COORD = -500;
    const int _MAX_COORD = 500;
    const int _MIN_HEAD_ANGLE = -327.68;
    const int _MAX_HEAD_ANGLE = 327.68;
    const int _MIN_ANGLE_ID = 1;
    const int _MAX_ANGLE_ID = 6;
    const int _MIN_ANGLE = -180;
    const int _MAX_ANGLE = 180;
    const int _MIN_SPEED = 0;
    const int _MAX_SPEED = 100;
    const int _MIN_GRIPPER = 0;
    const int _MAX_GRIPPER = 100;

    bool _colorInRange(int color);
    void _uploadSettings();
    void _saveSettings();

};

#endif