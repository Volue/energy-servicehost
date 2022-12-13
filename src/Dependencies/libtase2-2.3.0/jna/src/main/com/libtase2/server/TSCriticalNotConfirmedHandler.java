package com.libtase2.server;

public interface TSCriticalNotConfirmedHandler {
    void criticalReportNotConfirmed(Object parameter, DSTransferSet transferSet, long timeSent, boolean nack);
}
