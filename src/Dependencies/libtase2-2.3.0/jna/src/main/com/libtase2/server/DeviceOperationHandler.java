package com.libtase2.server;

import com.libtase2.common.Tag;

/**
 * Callback handlers for device operations (select, operate, set tag)
 */
public interface DeviceOperationHandler {
    /**
     * Is called when a set tag operation for a device is received from the client.
     * 
     * @param parameter    user provided parameter
     * @param controlPoint the ControlPoint instance that represents the device
     * @param tagValue
     * @return HandlerResult.SUCCESS when the operation is accepted, error code
     *         otherwise
     */
    HandlerResult setTag(Object parameter, ControlPoint controlPoint, Tag tagValue);

    /**
     * Is called when an operate operation for a device is received (command or set
     * point received)
     * 
     * @param parameter    user provided parameter
     * @param controlPoint the ControlPoint instance that represents the device
     * @param value        command or set point value
     * @return HandlerResult.SUCCESS when the operation is accepted, error code
     *         otherwise
     */
    HandlerResult operate(Object parameter, ControlPoint controlPoint, OperateValue value);

    /**
     * Is called when a select operation for a device is received.
     * 
     * @param parameter    user provided parameter
     * @param controlPoint the ControlPoint instance that represents the device
     * @return HandlerResult.SUCCESS when the operation is accepted, error code
     *         otherwise
     */
    HandlerResult select(Object parameter, ControlPoint controlPoint);
}
