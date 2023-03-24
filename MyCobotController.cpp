#include <MyCobotBasic.h>
#include <ParameterList.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "MyCobotController.h"
#include "Utils.h"

MyCobotController::MyCobotController() {}

void MyCobotController::setup() {
  int tmpSpeed;
  _myCobot.setup();
  delay(_DELAY);
  _myCobot.powerOn();
  delay(_DELAY);

  _myCobot.setGripperIni();
  delay(_DELAY);

  _uploadSettings();

  setColor(_color[0], _color[1], _color[2]);
  _myCobot.setSpeed(_speed);
  _myCobot.setGripperValue(_gripper, _speed);
}

unsigned int MyCobotController::coordsNumber() {
  return _COORDSNUMBER;
}

int MyCobotController::minCoordValue() {
  return _MIN_COORD;
}

int MyCobotController::maxCoordValue() {
  return _MAX_COORD;
}


int MyCobotController::minAngleId() {
  return _MIN_ANGLE_ID;
}

int MyCobotController::maxAngleId() {
  return _MAX_ANGLE_ID;
}

int MyCobotController::minAngleValue() {
  return _MIN_ANGLE;
}

int MyCobotController::maxAngleValue() {
  return _MAX_ANGLE;
}

int MyCobotController::minSpeedValue() {
  return _MIN_SPEED;
}

int MyCobotController::maxSpeedValue() {
  return _MAX_SPEED;
}

int MyCobotController::minGripperValue() {
  return _MIN_GRIPPER;
}

int MyCobotController::maxGripperValue() {
  return _MAX_GRIPPER;
}

char* MyCobotController::lastError() {
  return _error;  
}

bool MyCobotController::verifyAxis(const Axis axis) {
  return axisToInt(axis) >= 0 && axisToInt(axis) <= 5;
}

Axis MyCobotController::parseAxis(const char* axis) {
    if (strcmp(axis, "x") == 0) return Axis::X;
    if (strcmp(axis, "y") == 0) return Axis::Y;
    if (strcmp(axis, "z") == 0) return Axis::Z;
    if (strcmp(axis, "rx") == 0) return Axis::RX;
    if (strcmp(axis, "ry") == 0) return Axis::RY;
    if (strcmp(axis, "rz") == 0) return Axis::RZ;
    return (Axis)(-1);
}

Axis MyCobotController::parseAxis(const int axis) {
    return (Axis)axis;
}

int MyCobotController::axisToInt(const Axis axis) {
    return (int)axis;
}

char* MyCobotController::axisToChar(const Axis axis) {
    if (axis == Axis::X) return "x";
    if (axis == Axis::Y) return "y";
    if (axis == Axis::Z) return "z";
    if (axis == Axis::RX) return "rx";
    if (axis == Axis::RY) return "ry";
    if (axis == Axis::RZ) return "rz";
    return "";
}



int* MyCobotController::color() {
  int* tempColor = (int*) malloc(sizeof(int) * 3);

  tempColor[0] = _color[0];
  tempColor[1] = _color[1];
  tempColor[2] = _color[2];

  return tempColor;
}

Coords MyCobotController::coords() {
  Coords coords;
  int counter = 0;

  /*
   * The loop is necessary beacause sometimes _myCobot.getCoords() function returns an array of -1000.0
   * but if the functions is executed again may return the correct values.
   */
  do {
    coords = _myCobot.getCoords();
    delay(_DELAY);

    counter++;
  } while(coords[0] == _COORDS_ERROR_VALUE && counter < _REPETITION_COUNTER);

  return coords;
}

float MyCobotController::coord(const char* axis) {
  Coords tempCoords = coords();
  float coord = tempCoords[axisToInt(parseAxis(axis))];

  return coord;  
}

Angles MyCobotController::angles() {
  Angles angles;
  int counter = 0;
  
  /*
   * The loop is necessary beacause sometimes _myCobot.getCoords() function returns an array of -1000.0
   * but if the functions is executed again may return the correct values.
   */
  do {
    angles = _myCobot.getAngles();
    delay(_DELAY);

    counter++;
  } while(angles[0] == _COORDS_ERROR_VALUE && counter < _REPETITION_COUNTER);

  return angles;  
}

float MyCobotController::angle(int id) {
  Angles tempAngles = angles();
  float angle = tempAngles[id - 1];

  return angle;
}

int MyCobotController::speed() {
  return _speed;  
}

int MyCobotController::gripper() {
  return _gripper;
}

bool MyCobotController::setColor(int r, int g, int b) {
  if (!_colorInRange(r) || !_colorInRange(g) || !_colorInRange(b)) {
    _error = "{\"error\": \"Color values must be between 0 and 255\"}";
    return false;
  }
  
  _myCobot.setLEDRGB(r, g, b);
  _color[0] = r;
  _color[1] = g;
  _color[2] = b;
  _saveSettings();
  return true;
}

bool MyCobotController::setCoords(Coords coords) {
  for (int i = 0; i < coordsNumber(); i++) {
    if (i < 3 && (coords[i] < _MIN_COORD || coords[i] > _MAX_COORD)) {
      _error = "{\"error\": \"Coords values out of range\"}";
      return false;
      
    } else if (i >= 3 && (coords[i] < _MIN_HEAD_ANGLE || coords[i] > _MAX_HEAD_ANGLE)) {
      _error = "{\"error\": \"Head angles values must be between -327.68 and 327.68\"}";
      return false;
    }
  }
  _myCobot.writeCoords(coords, _speed);
  return true;
}

bool MyCobotController::setCoord(Axis axis, float value) {
  int axisId = (int)(axis);
  Coords coordsArray;

  if (!verifyAxis(axis)) {
    _error = "{\"error\": \"Axis must be included in [`x`, `y`, `z`, `rx`, `ry`, `rz`]\"}";
    return false;
  }

  coordsArray = coords();
  coordsArray[axisId] = value;

  return setCoords(coordsArray);
}

bool MyCobotController::setAngles(Angles angles) {
  for (int i = 0; i < coordsNumber(); i++) {
    if (angles[i] < _MIN_ANGLE || angles[i] > _MAX_ANGLE) {
      _error = "{\"error\": \"Angle value must be between -180 and 180\"}";
      return false;
    }
  }

  _myCobot.writeAngles(angles, _speed);
  return true;
}

bool MyCobotController::setAngle(int id, float value) {
  if (id < _MIN_ANGLE_ID || id > _MAX_ANGLE_ID) {
    _error = "{\"error\": \"Id value must be between 1 and 6\"}";
    return false;
  }

  if (value < _MIN_ANGLE || value > _MAX_ANGLE) {
    _error = "{\"error\": \"Angle value must be between -180 and 180\"}";
    return false;
  }
  
  _myCobot.writeAngle(id, value, _speed);
  return true;
}

bool MyCobotController::setSpeed(int speed) {
  if (speed < _MIN_SPEED || speed > _MAX_SPEED) {
    _error = "{\"error\": \"Speed value must be between 0 and 100\"}";
    return false;
  }

  _speed = speed;
  _myCobot.setSpeed(speed);
  _saveSettings();
  return true;
}

bool MyCobotController::setGripper(int gripper) {
  if (gripper < _MIN_GRIPPER || gripper > _MAX_GRIPPER) {
    _error = "{\"error\": \"Gripper value must be between 0 and 100\"}";
    return false;
  }

  _gripper = gripper;
  _myCobot.setGripperValue(gripper, _speed);
  delay(_DELAY);

  _saveSettings();
  return true;
}

bool MyCobotController::_colorInRange(int color) {
  return color >= _MIN_COLOR_VALUE && color <= _MAX_COLOR_VALUE;
}

void MyCobotController::_uploadSettings() {
  char* content;
  StaticJsonDocument<192> settings;
  content = Utils::readFile(_SETTINGS_FILE);

  deserializeJson(settings, content);

  _speed = settings["speed"] == _GENERIC_ERROR_VALUE ? _DEFAULT_SPEED : settings["speed"];
  _gripper = settings["gripper"] == _GENERIC_ERROR_VALUE ? _DEFAULT_GRIPPER : settings["gripper"];
  _color[0] = settings["color"]["r"];
  _color[1] = settings["color"]["g"];
  _color[2] = settings["color"]["b"];
}

void MyCobotController::_saveSettings() {
  char* content;
  StaticJsonDocument<192> settings;
  settings["speed"] = _speed;
  settings["gripper"] = _gripper;
  settings["color"]["r"] = _color[0];
  settings["color"]["g"] = _color[1];
  settings["color"]["b"] = _color[2];

  content = (char*) malloc(sizeof(char) * 192);
  serializeJson(settings, content, 192);

  Utils::writeFile(_SETTINGS_FILE, content);
}
