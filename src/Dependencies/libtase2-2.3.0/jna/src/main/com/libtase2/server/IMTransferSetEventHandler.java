package com.libtase2.server;

import com.libtase2.common.EndpointConnection;

public interface IMTransferSetEventHandler {
    void event(Object parameter, EndpointConnection peer, BilateralTable clientBlt, boolean enabled);
}
