#ifndef __IWEB_SERVER__
#define __IWEB_SERVER__

#include "WebServerErrors.h"

/**
 * Web Server interface.
 */
class IWebServer {
  public:
    /**
     * Starts the web server.
     * @returns the status of the connection.
     */
    virtual WebServerErrors start() = 0;
};

#endif