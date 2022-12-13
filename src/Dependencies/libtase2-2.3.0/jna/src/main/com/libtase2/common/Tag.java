package com.libtase2.common;

/**
 * Representation of a tag including type, reason code, and information if the
 * device is armed
 */
public class Tag {
    private TagValue value = TagValue.NO_TAG;

    private boolean isArmed = false;
    private String reason = null;

    public Tag(TagValue value, String reason, boolean isArmed) {
        this.value = value;
        this.reason = reason;
        this.isArmed = isArmed;
    }

    public TagValue getValue() {
        return value;
    }

    /**
     * Check if the device is selected (armed)
     * 
     * Value is only valid in case of SBO control type.
     * 
     * @return true when device is selected, false otherwise
     */
    public boolean isArmed() {
        return isArmed;
    }

    public String getReason() {
        return this.reason;
    }
}
