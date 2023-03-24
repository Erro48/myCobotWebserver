#ifndef __IMYCOBOT_CONTROLLER__
#define __IMYCOBOT_CONTROLLER__

#include "MyCobotBasic.h"
#include "ParameterList.h"

/**
 * Robot controller interface.
 */
class IMyCobotController {
  public:
    /**
     * Setup method.
     */    
    virtual void setup() = 0;

    /**
     * Returns the number of coords of MyCobot.
     * @returns the number of coords.
     */
    virtual unsigned int coordsNumber() = 0;

    /**
     * Returns the minimum accepted coord value.
     * @returns the minimum accepted coord value.
    */
    virtual int minCoordValue() = 0;
    
    /**
     * Returns the maximum accepted coord value.
     * @returns the maximum accepted coord value.
    */
    virtual int maxCoordValue() = 0;

    /**
     * Returns the first angle id.
     * @returns the first angle id.
     */
    virtual int minAngleId() = 0;

    /**
     * Returns the last angle id.
     * @returns the last angle id.
     */    
    virtual int maxAngleId() = 0;

    /**
     * Returns the minimum accepted angle value.
     * @returns the minimum accepted angle value.
    */
    virtual int minAngleValue() = 0;
    
    /**
     * Returns the maximum accepted angle value.
     * @returns the maximum accepted angle value.
    */
    virtual int maxAngleValue() = 0;

    /**
     * Returns the minimum accepted speed value.
     * @returns the minimum accepted speed value.
    */
    virtual int minSpeedValue() = 0;

    /**
     * Returns the maximum accepted speed value.
     * @returns the maximum accepted speed value.
    */
    virtual int maxSpeedValue() = 0;

    /**
     * Returns the minimum accepted gripper value.
     * @returns the minimum accepted gripper value.
    */
    virtual int minGripperValue() = 0;

    /**
     * Returns the maximum accepted gripper value.
     * @returns the maximum accepted gripper value.
    */
    virtual int maxGripperValue() = 0;

    /**
     * Returns last error occured in MyCobot.
     * @returns last error occured.
     */
    virtual char* lastError() = 0;

    /**
     * Check if the passed axis is valid.
     * @param axis the axis to check.
     * @returns `true` if the axis value is included in [`x`, `y`, `z`, `rx`, `ry`, `rz`], `false` otherwise.
     */
    virtual bool verifyAxis(const Axis axis) = 0;

    /**
     * Parse axis given as char* to Axis object.
     * @param axis the char* to parse
     * @returns the corresponding Axis object
     */
    virtual Axis parseAxis(const char* axis) = 0;

    /**
     * Parse axis id to Axis object.
     * @param axis the id of the axis
     * @returns the corresponding Axis object
     */
    virtual Axis parseAxis(const int axis) = 0;

    /**
     * Returns the id of the given axis.
     * @param axis object
     * @returns the id of the axis.
     */
    virtual int axisToInt(const Axis axis) = 0;

    /**
     * Returns the char* representation of the given axis.
     * @param axis object
     * @returns the char* containg the axis name.
     */
    virtual char* axisToChar(const Axis axis) = 0;

    /**
     * Returns the color of the led matrix.
     * @returns the color of the led matrix.
     */
    virtual int* color() = 0;

    /**
     * Returns all the MyCobot's coords.
     * @returns MyCobot's coords values.
     */
    virtual Coords coords() = 0;

    /**
     * Returns one coord value.
     * @param axis the axis of the coord.
     * @returns coord's value.
     */
    virtual float coord(const char* axis) = 0;

    /**
     * Returns all the MyCobot's angles.
     * @returns MyCobot's angles values.
     */
    virtual Angles angles() = 0;

    /**
     * Returns one angle value.
     * @param id the angle id.
     * @returns angle's value.
     */
    virtual float angle(int id) = 0;

    /**
     * Returns the joint running speed.
     * @returns joint running speed.
     */
    virtual int speed() = 0;

    /**
     * Returns the gripper value.
     * @returns gripper value.
    */
    virtual int gripper() = 0;

    /**
     * Sets the color of the led matrix.
     * @param r red component.
     * @param g green component.
     * @param b blue component.
     * @returns `true` if there are no errors, `false` otherwise.
     */
    virtual bool setColor(int r, int g, int b) = 0;

    /**
     * Sets coords of MyCobot.
     * @param coords array with all the coords.
     * @returns `true` if there are no errors, `false` otherwise.
     */
    virtual bool setCoords(Coords coords) = 0;

    /**
     * Sets a single coord.
     * @param axis the coord to set.
     * @param value the value of the coord.
     * @returns `true` if there are no errors, `false` otherwise.
     */
    virtual bool setCoord(Axis axis, float value) = 0;

    /**
     * Sets angles of MyCobot.
     * @param angles array with all the angles.
     * @returns `true` if there are no errors, `false` otherwise.
     */
    virtual bool setAngles(Angles angles) = 0;

    /**
     * Sets a single angle.
     * @param id the id of the angle to set.
     * @param value the value of the angle.
     * @returns `true` if there are no errors, `false` otherwise.
     */
    virtual bool setAngle(int id, float value) = 0;

    /**
     * Sets the running speed of the joints.
     * @param speed the running speed.
     * @returns `true` if there are no errors, `false` otherwise.
     */
    virtual bool setSpeed(int speed) = 0;

    /**
     * Sets the opening gripper value.
     * @param gripper the opening value.
     * @returns `true` if there are no errors, `false` otherwise.
    */
    virtual bool setGripper(int gripper) = 0;

};

#endif