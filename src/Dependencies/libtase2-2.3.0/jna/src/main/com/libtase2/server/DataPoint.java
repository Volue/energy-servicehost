package com.libtase2.server;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.sun.jna.Pointer;

public class DataPoint {
    protected Pointer self;
    private String name;
    private Domain domain;

    protected DataPoint(Pointer ptr, String name, Domain domain) {
        self = ptr;
        this.name = name;
        this.domain = domain;
    }

    public String getName() {
        return name;
    }

    public Domain getDomain() {
        return domain;
    }

}
