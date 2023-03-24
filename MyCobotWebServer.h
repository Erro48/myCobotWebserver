#ifndef __MYCOBOT_WEBSERVER__
#define __MYCOBOT_WEBSERVER__

#include <ArduinoJson.h>
#include "ESPAsyncWebServer.h"
#include "IMyCobotController.h"
#include "IConnection.h"
#include "WebServerErrors.h"
#include "IGuiController.h"
#include "JSONMapper.h"
#include "IWebServer.h"

class MyCobotWebServer : public IWebServer {
public:
  MyCobotWebServer(unsigned int port);
  WebServerErrors start() override;

private:
  AsyncWebServer *_server;
  IConnection *_wifiConnection;
  JSONMapper *_mapper;
  IMyCobotController *_mcController;
  IGuiController *_guiController;
  unsigned int _port;
  DynamicJsonDocument *_td;
  char* _webServerAddress;

  void _addRoute(char* name, char* URL, char* op, ArRequestHandlerFunction callback);
  void _begin();
  char* _getParameter(char *name, AsyncWebServerRequest *request);
  bool _checkParams(char **params, int nParams, AsyncWebServerRequest *request);
  WebRequestMethodComposite _getHttpMethod(char* op);
  void _thingDescriptionInit();

  void _rootHandler(AsyncWebServerRequest *request);
  void _notFoundHandler(AsyncWebServerRequest *request);

  void _getCoord(AsyncWebServerRequest *request);
  void _getCoords(AsyncWebServerRequest *request);
  void _getAngle(AsyncWebServerRequest *request);
  void _getAngles(AsyncWebServerRequest *request);
  void _getSpeed(AsyncWebServerRequest *request);
  void _getColor(AsyncWebServerRequest *request);
  void _getGripper(AsyncWebServerRequest *request);

  void _setColor(AsyncWebServerRequest *request);
  void _setCoords(AsyncWebServerRequest *request);
  void _setCoord(AsyncWebServerRequest *request);
  void _setAngles(AsyncWebServerRequest *request);
  void _setAngle(AsyncWebServerRequest *request);
  void _setSpeed(AsyncWebServerRequest *request);
  void _setGripper(AsyncWebServerRequest *request);
};

#endif