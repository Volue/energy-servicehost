package com.libtase2.server;

import com.libtase2.common.EndpointConnection;

public interface DSTransferSetUpdateHandler {
    void transferSetUpdated(Object parameter, EndpointConnection peer, BilateralTable clientBlt,
            DSTransferSet transferSet, boolean isEnabled);
}
