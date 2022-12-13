package com.libtase2.server;

public interface ClientConnectionHandler {
    void connectionEvent(Object parameter, String peerAddress, BilateralTable blt, boolean connect);
}
