package com.libtase2.server;

import java.util.Collection;

import com.libtase2.common.EndpointConnection;
import com.libtase2.common.ReportReason;

public interface DSTransferSetReportSentHandler {
    void sentReport(Object parameter, EndpointConnection peer, BilateralTable clientBlt, DSTransferSet ts,
            Collection<SentPointValue> sentData, ReportReason reason);
}
