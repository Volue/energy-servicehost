package com.libtase2.server;

public enum Edition {
    EDITION_1996_08(1), EDITION_2000_08(2);

    private int value;

    private Edition(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public boolean compare(int i) {
        return (value == i);
    }

    public static Edition fromValue(int val) {
        Edition[] editionValues = Edition.values();

        for (int i = 0; i < editionValues.length; i++) {
            if (editionValues[i].compare(val))
                return editionValues[i];
        }

        return Edition.EDITION_2000_08;
    }
}
