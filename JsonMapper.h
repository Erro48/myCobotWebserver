#ifndef __JSON_MAPPER__
#define __JSON_MAPPER__

// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include "MyCobotController.h"

class JSONMapper {
public:
    JSONMapper(IMyCobotController *controller);
    /**
     * Return the mapper type.
     * @returns mapper type.
    */
    const char* type();

    /**
     * Returns a JSON object containg the color informations.
     * @returns JSON with color informations.
    */
    StaticJsonDocument<64> color();

    /**
     * Returns a JSON object containg the coords informations.
     * @returns JSON with coords informations.
    */
    StaticJsonDocument<400> coords();

    /**
     * Returns a JSON object containg the coord informations.
     * @returns JSON with coord informations.
    */
    StaticJsonDocument<48> coord(const char* axis);

    /**
     * Returns a JSON object containg the angles informations.
     * @returns JSON with angles informations.
    */
    StaticJsonDocument<400> angles();

    /**
     * Returns a JSON object containg the angle informations.
     * @returns JSON with angle informations.
    */
    StaticJsonDocument<48> angle(const int joint);

    /**
     * Returns a JSON object containg the speed informations.
     * @returns JSON with speed informations.
    */
    StaticJsonDocument<16> speed();

    /**
     * Returns a JSON object containg the gripper informations.
     * @returns JSON with gripper informations.
    */
    StaticJsonDocument<48> gripper();


    /**
     * Sets the color based on the input JSON.
     * @param color JSON with informations of color.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setColor(StaticJsonDocument<64> color);
    
    /**
     * Sets the coords based on the input JSON.
     * @param coords JSON with informations of coords.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setCoords(StaticJsonDocument<400> coords);
    
    /**
     * Sets the coord based on the input JSON.
     * @param coord JSON with informations of coord.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setCoord(StaticJsonDocument<48> coord);
    
    /**
     * Sets the angles based on the input JSON.
     * @param angles JSON with informations of angles.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setAngles(StaticJsonDocument<400> angles);
    
    /**
     * Sets the angle based on the input JSON.
     * @param angle JSON with informations of angle.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setAngle(StaticJsonDocument<48> angle);
    
    /**
     * Sets the speed based on the input JSON.
     * @param speed JSON with informations of speed.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setSpeed(StaticJsonDocument<16> speed);
    
    /**
     * Sets the gripper based on the input JSON.
     * @param gripper JSON with informations of gripper.
     * @returns `true` if the assignment succed, `false` otherwise.
    */
    bool setGripper(StaticJsonDocument<48> gripper);


    /**
     * Returns a JSON object with the URI variables of the web server route.
     * @param name of the web server route.
     * @param op type of operation.
     * @returns a JSON with the URI variables if present, an empty JSON otherwise.
    */
    DynamicJsonDocument uriVariables(const char* name, const char* op);

    /**
     * Add a schema, based on the name and if it is of input or output.
     * @param name of the web server route.
     * @param doc JSON document where to add the schema.
     * @param inputSchema flag indicating if it is an input or an output schema.
    */
    void addSchema(const char* name, DynamicJsonDocument *doc, bool inputSchema);

    /**
     * Parses the given name in a valid web server name.
     * @param name of the web server route.
     * @returns a simpler name.
    */
    char* parseWebServerName(const char* name);


    /**
     * Returns JSON object representing the properties schema.
     * @param name of the web server route.
     * @param path link of the route.
     * @returns the JSON object of the properties schema.
    */
    DynamicJsonDocument createProperty(const char* name, char* path);
    
    /**
     * Returns JSON object representing the actions schema.
     * @param name of the web server route.
     * @param path link of the route.
     * @returns the JSON object of the actions schema.
    */
    DynamicJsonDocument createAction(const char* name, char* path);

private:
    IMyCobotController *_mcController;

    void _colorSchema(DynamicJsonDocument *doc, const bool inputSchema);
    void _coordSchema(DynamicJsonDocument *doc, const bool inputSchema);
    void _coordsSchema(DynamicJsonDocument *doc, const bool inputSchema);
    void _angleSchema(DynamicJsonDocument *doc, const bool inputSchema);
    void _anglesSchema(DynamicJsonDocument *doc, const bool inputSchema);
    void _speedSchema(DynamicJsonDocument *doc, const bool inputSchema);
    void _gripperSchema(DynamicJsonDocument *doc, const bool inputSchema);

    void _updatePathWithQueryString(char* path, DynamicJsonDocument doc);
    DynamicJsonDocument _coordUriVariables(const char* op);
    DynamicJsonDocument _angleUriVariables(const char* op);
};

#endif