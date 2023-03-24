#ifndef __WEBSERVER_ERRORS__
#define __WEBSERVER_ERRORS__

/**
 * Enum with the possible errors given by the web server.
 */
enum class WebServerErrors
{
  /**
   * There are no errors.
   */
  NoErrors,
  
  /**
   * Web server cannot connect to the network.
   */
  WifiConnectionFail
};

#endif