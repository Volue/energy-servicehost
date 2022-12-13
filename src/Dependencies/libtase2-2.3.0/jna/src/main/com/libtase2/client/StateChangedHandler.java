package com.libtase2.client;

/**
 * Callback handler for {@link com.libtase2.client.Client Client} state changes
 */
public interface StateChangedHandler {
    /**
     * Callback method that is called whenever the client state changes
     * 
     * @param parameter a user provided parameter
     * @param client    the related client instance
     * @param newState  the new state
     */
    void stateChanged(Object parameter, Client client, ClientState newState);
}
