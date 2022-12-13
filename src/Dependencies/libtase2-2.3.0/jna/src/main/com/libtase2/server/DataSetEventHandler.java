package com.libtase2.server;

/*
 *  Copyright 2021 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.util.Collection;
import com.libtase2.common.EndpointConnection;

public interface DataSetEventHandler {
    void dataSetEvent(Object parameter, boolean create, EndpointConnection peer, BilateralTable clientBlt,
            Domain domain, String dataSetName, Collection<DataPoint> dataPoints);
}
