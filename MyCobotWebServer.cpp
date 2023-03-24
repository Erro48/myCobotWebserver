#include "ESPAsyncWebServer.h"
#include "MyCobotWebServer.h"
#include "WifiConnection.h"
#include "WebServerErrors.h"
#include "MyCobotController.h"
#include "M5Controller.h"
#include "IGuiController.h"
#include "Utils.h"
#include <ArduinoJson.h>

MyCobotWebServer::MyCobotWebServer(unsigned int port) {
  _td = new DynamicJsonDocument(9000);
  _mcController = new MyCobotController();
  _mapper = new JSONMapper(_mcController);

  _port = port;
  _server = new AsyncWebServer(port);
  _wifiConnection = new WifiConnection();

  _guiController = new M5Controller();
  _guiController->setup();
}

WebServerErrors MyCobotWebServer::start() {
  if (!_wifiConnection->connect()) {
    _guiController->print("WiFi Connect Failed!");
    return WebServerErrors::WifiConnectionFail;
  }

  int portLen = floor(log10(_port)) + 2;
  int ipLen = strlen(_wifiConnection->ip().toString().c_str());
  char* c_port = (char*) malloc(sizeof(char) * portLen);
  itoa(_port, c_port, 10);

  _webServerAddress = (char*) malloc(sizeof(char) * (10 + ipLen + portLen));
  strcpy(_webServerAddress, "http://");
  strcat(_webServerAddress, _wifiConnection->ip().toString().c_str());
  strcat(_webServerAddress, ":");
  strcat(_webServerAddress, c_port);

  _thingDescriptionInit();

  _guiController->print("Network:\n");
  _guiController->print(_wifiConnection->network());
  _guiController->print("\n\n");
  _guiController->print("Address:\n");
  _guiController->print(_webServerAddress);

  _server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) { _rootHandler(request); });

  _addRoute("color", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getColor(request); });
  _addRoute("coords", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getCoords(request); });
  _addRoute("coord", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getCoord(request); });
  _addRoute("angles", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getAngles(request); });
  _addRoute("angle", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getAngle(request); });
  _addRoute("speed", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getSpeed(request); });
  _addRoute("gripper", "/properties", "readproperty", [this](AsyncWebServerRequest *request) { _getGripper(request); });

  _addRoute("setColor", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setColor(request); });
  _addRoute("moveToCoords", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setCoords(request); });
  _addRoute("moveToCoord", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setCoord(request); });
  _addRoute("setAngles", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setAngles(request); });
  _addRoute("setAngle", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setAngle(request); });
  _addRoute("setSpeed", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setSpeed(request); });
  _addRoute("openGripper", "/actions", "writeproperty", [this](AsyncWebServerRequest *request) { _setGripper(request); });
  
  _server->onNotFound([this](AsyncWebServerRequest *request) { _notFoundHandler(request); });
  _begin();

  return WebServerErrors::NoErrors;
}

void MyCobotWebServer::_addRoute(char* name, char* URL, char* op, ArRequestHandlerFunction callback) {
  char* relPath;
  char* pathCopy;
  WebRequestMethodComposite method = _getHttpMethod(op);

  relPath = (char*) malloc(sizeof(char) * (strlen(name) + strlen(URL) + 2));
  pathCopy = (char*) malloc(sizeof(char) * (strlen(name) + strlen(URL) + 2));

  strcpy(relPath, URL);
  strcat(relPath, "/");
  strcat(relPath, name);

  strcpy(pathCopy, relPath);

  if (method == HTTP_GET) {
    (*_td)["properties"][name] = _mapper->createProperty(name, relPath);

  } else if (method == HTTP_POST) {
    (*_td)["actions"][name] = _mapper->createAction(name, relPath);
  }

  _server->on(pathCopy, method, callback);
}

void MyCobotWebServer::_begin() {
  char* content;
  int jsonSize = measureJson(*_td);

  content = (char*) malloc(sizeof(char) * jsonSize);
  serializeJson(*_td, content, jsonSize);

  content[jsonSize] = '\0';
  bool status = Utils::writeFile("/td.json", content);

  if (!status) {
    Serial.println("errore nella scrittura del file");
    return;
  }

  delete _td;
  _server->begin();
}

char* MyCobotWebServer::_getParameter(char* name, AsyncWebServerRequest *request) {
  for (int i = 0; i < request->params(); i++) {
    AsyncWebParameter* param = request->getParam(i);

    if (strcmp(param->name().c_str(), name) == 0) {
      int paramLen = strlen(param->value().c_str()) + 1;
      char* result = new char[paramLen];
      strcpy(result, param->value().c_str());
      return result;
    }
  }
  return NULL;
}

bool MyCobotWebServer::_checkParams(char** params, int nParams, AsyncWebServerRequest *request) {
  if (request->params() != nParams) return false;
  for (int i = 0; i < nParams; i++) {
    if (_getParameter(params[i], request) == NULL) {
      return false;
    }
  }
  return true;
}

WebRequestMethodComposite MyCobotWebServer::_getHttpMethod(char* op) {
  if (strcmp(op, "writeproperty") == 0) {
    return HTTP_POST;
  }
  return HTTP_GET;
}

void MyCobotWebServer::_thingDescriptionInit() {
  (*_td)["@context"] = "https://www.w3.org/2022/wot/td/v1.1";
  (*_td)["id"] = _webServerAddress;
  (*_td)["base"] = _webServerAddress;
  (*_td)["title"] = "MyCobot";
  (*_td)["securityDefinitions"]["basic_sc"]["scheme"] = "basic";
  (*_td)["securityDefinitions"]["basic_sc"]["in"] = "header";
  (*_td)["security"][0] = "basic_sc";
}

/* Route Handlers */
void MyCobotWebServer::_rootHandler(AsyncWebServerRequest *request) {
  request->send(201, "application/json", Utils::readFile("/td.json"));
}

void MyCobotWebServer::_notFoundHandler(AsyncWebServerRequest *request) {
  request->send(404, "application/json", "{\"error\": \"Resource not found\"}");
}

/* Route Handler - GET */
void MyCobotWebServer::_getCoord(AsyncWebServerRequest *request) {
  char* axis;
  StaticJsonDocument<48> coord;
  char* response;

  if (request->params() != 1 || _getParameter("axis", request) == NULL) {
    request->send(400, "application/json", "{\"error\": \"You must pass one argument\", \"arguments\": { \"axis\": \"axis of the coord\" }}");
    return;
  }
  
  axis = _getParameter("axis", request);  

  coord = _mapper->coord(axis);
  response = (char*) malloc(sizeof(char) * 48);

  if (coord["error"] != NULL) {
    response = (char*) realloc(response, sizeof(char) * 100);
    serializeJson(coord, response, 100);
    request->send(400, "application/json", response);
    return;
  }

  serializeJson(coord, response, 48);
  request->send(201, "application/json", response);
}

void MyCobotWebServer::_getCoords(AsyncWebServerRequest *request) {
  StaticJsonDocument<400> coords = _mapper->coords();
  char* response = (char*) malloc(sizeof(char) * 400);

  serializeJson(coords, response, 400);
  request->send(201, "application/json", response);
}

void MyCobotWebServer::_getAngle(AsyncWebServerRequest *request) {
  int joint;
  StaticJsonDocument<48> angle;
  char* response;

  if (request->params() != 1 || _getParameter("joint", request) == NULL) {
    request->send(400, "application/json", "{\"error\": \"You must pass one argument\", \"arguments\": { \"joint\": \"id of the angle\" }}");
    return;
  }
  
  joint = atoi(_getParameter("joint", request));

  angle = _mapper->angle(joint);
  response = (char*) malloc(sizeof(char) * 48);

  if (angle["error"] != NULL) {
    response = (char*) realloc(response, sizeof(char) * 100);
    serializeJson(angle, response, 100);
    request->send(400, "application/json", response);
    return;
  }

  serializeJson(angle, response, 48);
  request->send(201, "application/json", response);
}

void MyCobotWebServer::_getAngles(AsyncWebServerRequest *request) {
  StaticJsonDocument<400> angles = _mapper->angles();
  char* response = (char*) malloc(sizeof(char) * 400);

  serializeJson(angles, response, 400);
  request->send(201, "application/json", response);
}

void MyCobotWebServer::_getSpeed(AsyncWebServerRequest *request) {
  StaticJsonDocument<16> speed = _mapper->speed();
  char* response = (char*) malloc(sizeof(char) * 16);

  serializeJson(speed, response, 16);
  request->send(201, "application/json", response);
}

void MyCobotWebServer::_getColor(AsyncWebServerRequest *request) {
  StaticJsonDocument<64> color = _mapper->color();
  char* response = (char*) malloc(sizeof(char) * 64);

  serializeJson(color, response, 64);
  request->send(201, "application/json", response);
}

void MyCobotWebServer::_getGripper(AsyncWebServerRequest *request) {
  StaticJsonDocument<48> gripper = _mapper->gripper();
  char* response = (char*) malloc(sizeof(char) * 48);

  serializeJson(gripper, response, 48);
  request->send(201, "application/json", response);
}

/* Route Handler - POST */
void MyCobotWebServer::_setColor(AsyncWebServerRequest *request) {
  int r, g, b;
  int nParams = 3;
  char *params[nParams] = {"r", "g", "b"};
  StaticJsonDocument<64> color;

  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass three arguments\", \"arguments\": { \"r\": \"red value\", \"g\": \"green value\", \"b\": \"blue value\" }}");
    return;
  }

  color["color"]["r"] = atoi(_getParameter("r", request));
  color["color"]["g"] = atoi(_getParameter("g", request));
  color["color"]["b"] = atoi(_getParameter("b", request));

  if (!_mapper->setColor(color)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}

void MyCobotWebServer::_setCoords(AsyncWebServerRequest *request) {
  StaticJsonDocument<400> coords;
  int nParams = 6;
  char* params[nParams] = { "x", "y", "z", "rx", "ry", "rz"};

  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass six arguments\", \"arguments\": { \"x\": \"x coord\", \"y\": \"y coord\", \"z\": \"z coord\", \"rx\": \"rx angle\", \"ry\": \"ry angle\", \"rz\": \"rz angle\" }}");
    return;
  }

  coords["coords"][0]["value"] = atof(_getParameter("x", request));
  coords["coords"][1]["value"] = atof(_getParameter("y", request));
  coords["coords"][2]["value"] = atof(_getParameter("z", request));
  coords["coords"][3]["value"] = atof(_getParameter("rx", request));
  coords["coords"][4]["value"] = atof(_getParameter("ry", request));
  coords["coords"][5]["value"] = atof(_getParameter("rz", request));

  if (!_mapper->setCoords(coords)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}

void MyCobotWebServer::_setCoord(AsyncWebServerRequest *request) {
  StaticJsonDocument<48> coord;
  int nParams = 2;
  char *params[nParams] = {"axis", "value"};

  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass two arguments\", \"arguments\": { \"axis\": \"axis\", \"value\": \"value\" }}");
    return;
  }

  coord["axis"] = _getParameter("axis", request);
  coord["value"] = atof(_getParameter("value", request));

  if (!_mapper->setCoord(coord)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}

void MyCobotWebServer::_setAngles(AsyncWebServerRequest *request) {
  StaticJsonDocument<400> angles;
  int nParams = 6;
  char *params[nParams] = {"j1", "j2", "j3", "j4", "j5", "j6"};
  
  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass six arguments\", \"arguments\": { \"j1\": \"first joint angle\", \"j2\": \"second joint angle\", \"j3\": \"third joint angle\", \"j4\": \"fourth joint angle\", \"j5\": \"fifth joint angle\", \"j6\": \"sixth joint angle\" }}");
    return;
  }

  angles["angles"][0]["value"] = atof(_getParameter("j1", request));
  angles["angles"][1]["value"] = atof(_getParameter("j2", request));
  angles["angles"][2]["value"] = atof(_getParameter("j3", request));
  angles["angles"][3]["value"] = atof(_getParameter("j4", request));
  angles["angles"][4]["value"] = atof(_getParameter("j5", request));
  angles["angles"][5]["value"] = atof(_getParameter("j6", request));

  if (!_mapper->setAngles(angles)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}

void MyCobotWebServer::_setAngle(AsyncWebServerRequest *request) {
  StaticJsonDocument<48> angle;
  int nParams = 2;
  char *params[nParams] = {"id", "value"};

  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass two arguments\", \"arguments\": { \"id\": \"id of the joint to move\", \"value\": \"value\"}}");
    return;
  }

  angle["id"] = atoi(_getParameter("id", request));
  angle["value"] = atof(_getParameter("value", request));

  if (!_mapper->setAngle(angle)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}

void MyCobotWebServer::_setSpeed(AsyncWebServerRequest *request) {
  StaticJsonDocument<16> speed;
  int nParams = 1;
  char *params[nParams] = {"speed"};

  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass one argument\", \"arguments\": { \"speed\": \"running speed\"}}");
    return;
  }

  speed["speed"] = atoi(_getParameter("speed", request));

  if (!_mapper->setSpeed(speed)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}

void MyCobotWebServer::_setGripper(AsyncWebServerRequest *request) {
  StaticJsonDocument<48> gripper;
  int nParams = 1;
  char *params[nParams] = {"gripper"};

  if (!_checkParams(params, nParams, request)) {
    request->send(400, "application/json", "{\"error\": \"You must pass one argument\", \"arguments\": { \"gripper\": \"opening gripper value\"}}");
    return;
  }

  gripper["gripper"] = atoi(_getParameter("gripper", request));

  if (!_mapper->setGripper(gripper)) {
    request->send(400, "application/json", "Errore");
    return;
  }
  request->send(201, "application/json", "{\"status\": \"ok\"}");
}
