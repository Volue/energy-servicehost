package com.libtase2.server;

/*
 *  Copyright 2021 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.libtase2.common.PointValue;

public class SentPointValue {
    private DataPoint dataPoint;
    private PointValue pointValue = null;

    SentPointValue(DataPoint dataPoint, PointValue pointValue) {
        this.dataPoint = dataPoint;
        this.pointValue = pointValue;
    }

    public DataPoint getDataPoint() {
        return this.dataPoint;
    }

    public PointValue getPointValue() {
        return this.pointValue;
    }
}
