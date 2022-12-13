package com.libtase2.server;

public class OperateValue {
    private short shortValue;
    private float realValue;

    public OperateValue(short shortValue, float realValue) {
        this.shortValue = shortValue;
        this.realValue = realValue;
    }

    public short getCommand() {
        return shortValue;
    }

    public short getDiscreteSetPoint() {
        return shortValue;
    }

    public float getRealSetPoint() {
        return realValue;
    }
}
