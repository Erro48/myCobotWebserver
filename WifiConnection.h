#ifndef __WIFI_CONNECTION__
#define __WIFI_CONNECTION__

#include <Arduino.h>
#include <ArduinoJson.h>
#include "IConnection.h"

/**
 * Class which handles the wifi connection.
 */
class WifiConnection : public IConnection
{
    public:
        WifiConnection();
        char** discover() override;
        bool connect() override;
        IPAddress ip() override;
        char* network() override;

    private:
        char* _ssid;
        char* _password;
        bool _storeData(char* key, char* arg);
        void _saveWifiConfig(char* fileContent);

};

#endif