#ifndef __ICONNECTION__
#define __ICONNECTION__

/**
 * Internet connection interface.
 */
class IConnection
{
  public:
    /**
     * Discovers all the available WiFi networks.
     * @returns an array with all the available networks.
     */
    virtual char** discover() = 0;

    /**
     * Connect to the network specified in the config.json file.
     * @returns `true` if the connection is successful, `false` otherwise.
     */
    virtual bool connect() = 0;

    /**
     * Returns the current IP address.
     * @returns the IP address.
     */
    virtual IPAddress ip() = 0;

    /**
     * Returns the network to which it is connected.
     * @returns the network to which it is connected.
     */
    virtual char* network() = 0;
};

#endif