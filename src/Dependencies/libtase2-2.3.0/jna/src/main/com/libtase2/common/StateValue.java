package com.libtase2.common;

/**
 * Value of an indication point of type STATE
 */
public enum StateValue {
    BETWEEN(0), OFF(1), ON(2), INVALID(3);

    private StateValue(int value) {
        this.value = value;
    }

    private int value;

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static StateValue fromValue(int val) {
        StateValue[] errors = StateValue.values();

        for (int i = 0; i < errors.length; i++) {
            if (errors[i].compare(val))
                return errors[i];
        }

        return StateValue.INVALID;
    }
}
