package com.libtase2.server;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

public enum QualityClass {
    NO_QUALITY(0), QUALITY(1);

    private int value;

    private QualityClass(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static QualityClass fromValue(int val) {
        QualityClass[] errors = QualityClass.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return QualityClass.NO_QUALITY;
    }
}
