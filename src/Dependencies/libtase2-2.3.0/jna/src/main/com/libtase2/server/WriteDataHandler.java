package com.libtase2.server;

import com.libtase2.common.PointValue;

/**
 * Interface to handle write access to indication points
 */
public interface WriteDataHandler {
    /**
     * Callback handler for write operations to indication points (and protection
     * event data points)
     *
     * @param parameter user provided context parameter
     * @param domain    domain of the data point to be written
     * @param dataPoint data point to be written
     * @param value     the new value of the data point
     *
     * @return HandlerResult.SUCCESS when the new value accepted, or another value
     *         indicating why the new value is not accepted
     */
    HandlerResult writeDataEvent(Object paramter, Domain domain, DataPoint dataPoint, PointValue value);
}
