package com.libtase2.common;

public enum Validity {
    VALID(0), HELD(1), ON(2), INVALID(3);

    private Validity(int value) {
        this.value = value;
    }

    private int value;

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static Validity fromValue(int val) {
        Validity[] errors = Validity.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return Validity.INVALID;
    }
}
