package com.libtase2.common;

public enum TLSVersion {
    TLS_VERSION_NOT_SELECTED(0), TLS_VERSION_SSL_3_0(3), TLS_VERSION_TLS_1_0(4), TLS_VERSION_TLS_1_1(5),
    TLS_VERSION_TLS_1_2(6), TLS_VERSION_TLS_1_3(7);

    private int value;

    private TLSVersion(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static TLSVersion fromValue(int val) {
        TLSVersion[] errors = TLSVersion.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return TLSVersion.TLS_VERSION_NOT_SELECTED;
    }
}
