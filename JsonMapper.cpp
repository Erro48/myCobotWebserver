#include "JSONMapper.h"

JSONMapper::JSONMapper(IMyCobotController *controller) {
    _mcController = controller;
    _mcController->setup();
    delay(200);
}

const char* JSONMapper::type() {
    return "application/json";
}

/* *******************************************
                    PROPERTIES
   ******************************************* */

StaticJsonDocument<64> JSONMapper::color() {
    StaticJsonDocument<64> json;
    int* color = (int*) malloc(sizeof(int) * 3);
    color = _mcController->color();

    json["r"] = color[0];
    json["g"] = color[1];
    json["b"] = color[2];

    return json;
}

StaticJsonDocument<400> JSONMapper::coords() {
    StaticJsonDocument<400> json;
    Coords coords = _mcController->coords();
    int coordsNumber = _mcController->coordsNumber();

    for (int i = 0; i < coordsNumber; i++) {
        json[_mcController->axisToChar(_mcController->parseAxis(i))] = coords[i];
    }

    return json;
}

StaticJsonDocument<48> JSONMapper::coord(const char* axis) {
    StaticJsonDocument<48> json;
    Coords coords = _mcController->coords();
    int axisId = _mcController->axisToInt(_mcController->parseAxis(axis));

    if (axisId == -1) {
        json["error"] = "Coord axis must be included in [`x`, `y`, `z`, `rx`, `ry`, `rz`]";
        return json;
    }

    json[axis] = coords[axisId];
    return json;
}


StaticJsonDocument<400> JSONMapper::angles() {
    StaticJsonDocument<400> json;
    Angles angles = _mcController->angles();
    int anglesNumber = _mcController->coordsNumber();

    for (int i = 0; i < anglesNumber; i++) {
        char key[2];
        strcpy(key, "j");
        itoa(i+1, &(key[1]), 10);
        json[key] = angles[i];
    }

    return json;
}


StaticJsonDocument<48> JSONMapper::angle(const int joint) {
    StaticJsonDocument<48> json;
    Angles angles = _mcController->angles();
    char key[2];
    strcpy(key, "j");
    itoa(joint, &(key[1]), 10);

    if (joint < _mcController->minAngleId() || joint > _mcController->maxAngleId()) {
        json["error"] = "Angle id must be between 1 and 6";
        return json;
    }
    
    json[key] = angles[joint - 1];

    return json;
}

StaticJsonDocument<16> JSONMapper::speed() {
    StaticJsonDocument<16> json;
    json["speed"] = _mcController->speed();
    return json;
}

StaticJsonDocument<48> JSONMapper::gripper() {
    StaticJsonDocument<48> json;
    json["gripper"] = _mcController->gripper();
    return json;
}

/* *******************************************
                    ACTIONS
   ******************************************* */

bool JSONMapper::setColor(StaticJsonDocument<64> color) {
    return _mcController->setColor(color["color"]["r"], color["color"]["g"], color["color"]["b"]);
}

bool JSONMapper::setCoords(StaticJsonDocument<400> coords) {
    Coords mcCoords;
    for (int i = 0; i < _mcController->coordsNumber(); i++) {
        mcCoords[i] = coords["coords"][i]["value"];
    }
    return _mcController->setCoords(mcCoords);
}

bool JSONMapper::setCoord(StaticJsonDocument<48> coord) {
    Axis axis = _mcController->parseAxis((const char*)coord["axis"]);
    float value = coord["value"];
    return _mcController->setCoord(axis, value);
}

bool JSONMapper::setAngles(StaticJsonDocument<400> angles) {
    Angles mcAngles;
    for (int i = 0; i < _mcController->coordsNumber(); i++) {
        mcAngles[i] = angles["angles"][i]["value"];
    }
    return _mcController->setAngles(mcAngles);
}

bool JSONMapper::setAngle(StaticJsonDocument<48> angle) {
    int angleId = angle["id"];
    float value = angle["value"];
    return _mcController->setAngle(angleId, value);
}

bool JSONMapper::setSpeed(StaticJsonDocument<16> speed) {
    return _mcController->setSpeed(speed["speed"]);
}

bool JSONMapper::setGripper(StaticJsonDocument<48> gripper) {
    return _mcController->setGripper(gripper["gripper"]);
}

/* *******************************************
                    UTILITY
   ******************************************* */

DynamicJsonDocument JSONMapper::uriVariables(const char* name, const char* op) {
    DynamicJsonDocument doc(1);

    if (strcmp(name, "coord") == 0) {
        return _coordUriVariables(op);
    }

    if (strcmp(name, "angle") == 0) {
        return _angleUriVariables(op);
    }

    return doc;
}

void JSONMapper::addSchema(const char* name, DynamicJsonDocument *doc, bool inputSchema) {
    
    if (strcmp(name, "color") == 0) {
        _colorSchema(doc, inputSchema);
        return;
    }

    if (strcmp(name, "coord") == 0) {
        _coordSchema(doc, inputSchema);
        return;
    }

    if (strcmp(name, "coords") == 0) {
        _coordsSchema(doc, inputSchema);
        return;
    }

    if (strcmp(name, "angle") == 0) {
        _angleSchema(doc, inputSchema);
        return;
    }

    if (strcmp(name, "angles") == 0) {
        _anglesSchema(doc, inputSchema);
        return;
    }

    if (strcmp(name, "speed") == 0) {
        _speedSchema(doc, inputSchema);
        return;
    }

    if (strcmp(name, "gripper") == 0) {
        _gripperSchema(doc, inputSchema);
        return;
    }
}

char* JSONMapper::parseWebServerName(const char* name) {
    if (strcmp(name, "color") == 0 || strcmp(name, "setColor") == 0)
        return "color";
    if (strcmp(name, "speed") == 0 || strcmp(name, "setSpeed") == 0)
        return "speed";
    if (strcmp(name, "coord") == 0 || strcmp(name, "moveToCoord") == 0)
        return "coord";
    if (strcmp(name, "coords") == 0 || strcmp(name, "moveToCoords") == 0)
        return "coords";
    if (strcmp(name, "angles") == 0 || strcmp(name, "setAngles") == 0)
        return "angles";
    if (strcmp(name, "angle") == 0 || strcmp(name, "setAngle") == 0)
        return "angle";
    if (strcmp(name, "gripper") == 0 || strcmp(name, "openGripper") == 0)
        return "gripper";

    return "";
}

DynamicJsonDocument JSONMapper::createProperty(const char* name, char* path) {
    DynamicJsonDocument property(2000);
    DynamicJsonDocument uriVar(1000);

    uriVar = uriVariables(parseWebServerName(name), "readproperty");

    if (uriVar.size() > 0) {
        property["uriVariables"] = uriVar;
        _updatePathWithQueryString(path, uriVar);
    }
    
    property["observable"] = false;
    property["readOnly"] = true;
    property["writeOnly"] = false;
    property["forms"][0]["href"] = path;
    property["forms"][0]["contentType"] = type();
    property["forms"][0]["op"] = "readproperty";

    addSchema(parseWebServerName(name), &property, true);

    return property;
}

DynamicJsonDocument JSONMapper::createAction(const char* name, char* path) {
    DynamicJsonDocument action(2000);

    action["safe"] = false;
    action["idempotent"] = true;
    action["readOnly"] = false;
    action["writeOnly"] = true;
    action["forms"][0]["href"] = path;
    action["forms"][0]["contentType"] = type();
    action["forms"][0]["op"] = "invokeaction";

    addSchema(parseWebServerName(name), &action, false);

    return action;
}

/* *******************************************
                PRIVATE METHODS
   ******************************************* */

void JSONMapper::_colorSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    if (inputSchema) {
        (*doc)["type"] = "object";

        (*doc)["properties"]["r"]["type"] = "number";
        (*doc)["properties"]["r"]["readOnly"] = true,
        (*doc)["properties"]["r"]["writeOnly"] = false,
        (*doc)["properties"]["r"]["minimum"] = 0;
        (*doc)["properties"]["r"]["maximum"] = 255;

        (*doc)["properties"]["g"]["type"] = "number";
        (*doc)["properties"]["g"]["readOnly"] = true,
        (*doc)["properties"]["g"]["writeOnly"] = false,
        (*doc)["properties"]["g"]["minimum"] = 0;
        (*doc)["properties"]["g"]["maximum"] = 255;

        (*doc)["properties"]["b"]["type"] = "number";
        (*doc)["properties"]["b"]["readOnly"] = true,
        (*doc)["properties"]["b"]["writeOnly"] = false,
        (*doc)["properties"]["b"]["minimum"] = 0;
        (*doc)["properties"]["b"]["maximum"] = 255;

    } else {
        DynamicJsonDocument input(512);
        _colorSchema(&input, true);

        (*doc)["input"] = input;
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;
        (*doc)["input"]["required"][0] = "r";
        (*doc)["input"]["required"][1] = "g";
        (*doc)["input"]["required"][2] = "b";

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}

void JSONMapper::_coordSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    if (inputSchema) {
        (*doc)["type"] = "number";
        (*doc)["minimum"] = _mcController->minCoordValue();
        (*doc)["maximum"] = _mcController->maxCoordValue();

    } else {
        DynamicJsonDocument input(512);
        _coordSchema(&input, true);
        (*doc)["input"] = input;
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;
        (*doc)["input"]["required"][0] = "axis";
        (*doc)["input"]["required"][1] = "value";

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}

void JSONMapper::_coordsSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    int paramsLen = 6;
    char* params[paramsLen] = {"x", "y", "z", "rx", "ry", "rz"};

    if (inputSchema) {
        (*doc)["type"] = "object";

        for (int i = 0; i < paramsLen; i++) {
            (*doc)["properties"][params[i]]["type"] = "number";
            (*doc)["properties"][params[i]]["readOnly"] = true;
            (*doc)["properties"][params[i]]["writeOnly"] = false;
            (*doc)["properties"][params[i]]["minimum"] = _mcController->minCoordValue();
            (*doc)["properties"][params[i]]["maximum"] = _mcController->maxCoordValue();
        }
    } else {

        (*doc)["input"]["type"] = "object";
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;

        for (int i = 0; i < paramsLen; i++) {
            (*doc)["input"]["properties"][params[i]]["type"] = "number";
            (*doc)["input"]["properties"][params[i]]["readOnly"] = true;
            (*doc)["input"]["properties"][params[i]]["writeOnly"] = false;
            (*doc)["input"]["properties"][params[i]]["minimum"] = _mcController->minCoordValue();
            (*doc)["input"]["properties"][params[i]]["maximum"] = _mcController->maxCoordValue();
            
            (*doc)["input"]["required"][i] = params[i];
        }

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}

void JSONMapper::_angleSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    if (inputSchema) {
        (*doc)["type"] = "number";
        (*doc)["minimum"] = _mcController->minAngleId();
        (*doc)["maximum"] = _mcController->maxAngleId();
    } else {
        DynamicJsonDocument input(512);
        _angleSchema(&input, true);
        (*doc)["input"] = input;
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;
        (*doc)["input"]["required"][0] = "joint";
        (*doc)["input"]["required"][1] = "value";

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}

void JSONMapper::_anglesSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    int paramsLen = 6;
    char* params[paramsLen] = {"j1", "j2", "j3", "j4", "j5", "j6"};

    if (inputSchema) {
        (*doc)["type"] = "object";

        for (int i = 0; i < paramsLen; i++) {
            (*doc)["properties"][params[i]]["type"] = "number";
            (*doc)["properties"][params[i]]["readOnly"] = true;
            (*doc)["properties"][params[i]]["writeOnly"] = false;
            (*doc)["properties"][params[i]]["minimum"] = _mcController->minAngleValue();
            (*doc)["properties"][params[i]]["maximum"] = _mcController->maxAngleValue();
        }
    } else {

        (*doc)["input"]["type"] = "object";
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;

        for (int i = 0; i < paramsLen; i++) {
            (*doc)["input"]["properties"][params[i]]["type"] = "number";
            (*doc)["input"]["properties"][params[i]]["readOnly"] = true;
            (*doc)["input"]["properties"][params[i]]["writeOnly"] = false;
            (*doc)["input"]["properties"][params[i]]["minimum"] = _mcController->minAngleId();
            (*doc)["input"]["properties"][params[i]]["maximum"] = _mcController->maxAngleId();
            
            (*doc)["input"]["required"][i] = params[i];
        }

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}

void JSONMapper::_speedSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    if (inputSchema) {
        (*doc)["type"] = "number";
        (*doc)["readOnly"] = true;
        (*doc)["writeOnly"] = false;
        (*doc)["minimum"] = _mcController->minSpeedValue();
        (*doc)["maximum"] = _mcController->maxSpeedValue();
    } else {
        DynamicJsonDocument input(512);
        _speedSchema(&input, true);
        (*doc)["input"] = input;
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}

void JSONMapper::_gripperSchema(DynamicJsonDocument *doc, const bool inputSchema) {
    if (inputSchema) {
        (*doc)["type"] = "number";
        (*doc)["readOnly"] = true;
        (*doc)["writeOnly"] = false;
        (*doc)["minimum"] = _mcController->minGripperValue();
        (*doc)["maximum"] = _mcController->maxGripperValue();
    } else {
        DynamicJsonDocument input(512);
        _gripperSchema(&input, true);
        (*doc)["input"] = input;
        (*doc)["input"]["readOnly"] = false;
        (*doc)["input"]["writeOnly"] = true;

        (*doc)["output"]["type"] = "object";
        (*doc)["output"]["properties"]["status"]["type"] = "string";
        (*doc)["output"]["properties"]["status"]["readOnly"] = true;
        (*doc)["output"]["properties"]["status"]["writeOnly"] = false;
        (*doc)["output"]["readOnly"] = true;
        (*doc)["output"]["writeOnly"] = false;
    }
}


void JSONMapper::_updatePathWithQueryString(char* path, DynamicJsonDocument doc) {
    int keyLenSum = 0;
    int i = 0;
    char** keys = (char**) malloc(sizeof(char*) * doc.size());

    for (JsonPair entry : doc.as<JsonObject>()) {
        int keyLen = strlen(entry.key().c_str());
        
        keys[i] = (char*) malloc(sizeof(char) * keyLen);
        strcpy(keys[i], entry.key().c_str());

        keyLenSum += keyLen * 2 + 3;
        i++;

    }

    path = (char*) realloc(path, sizeof(char) * (strlen(path) + 1 + keyLenSum + (i-1)));
    strcat(path, "?");


    for (i = i - 1; i >= 0; i--) {
        strcat(path, keys[i]);
        strcat(path, "={");
        strcat(path, keys[i]);
        strcat(path, "}");

        if (i > 0) {
            strcat(path, "&");
        }
    }

}

DynamicJsonDocument JSONMapper::_coordUriVariables(const char* op) {
    DynamicJsonDocument coord(192);

    if (strcmp(op, "readproperty") == 0) {
        int coordsNumber = _mcController->coordsNumber();

        coord["axis"]["type"] = "string";
        coord["axis"]["readOnly"] = true;
        coord["axis"]["writeOnly"] = false;
        for (int i = 0; i < coordsNumber; i++) {
            coord["axis"]["enum"][i] = _mcController->axisToChar(_mcController->parseAxis(i));
        }
    }

    return coord;
}

DynamicJsonDocument JSONMapper::_angleUriVariables(const char* op) {
    DynamicJsonDocument angle(192);

    if (strcmp(op, "readproperty") == 0) {
        int anglesNumber = _mcController->coordsNumber();

        angle["joint"]["type"] = "number";
        angle["joint"]["readOnly"] = true;
        angle["joint"]["writeOnly"] = false;
        for (int i = 0; i < anglesNumber; i++) {
            angle["joint"]["enum"][i] = i + 1;
        }
    }

    return angle;
}