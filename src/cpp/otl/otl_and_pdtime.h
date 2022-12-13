#pragma once
/*
 * OTL and PDTime headers, with conversion methods for converting
 * between otl_datetime and PDTime/pdtime.
 *
 * Includes otl.h and PDTime.h.
 * NB: Linkage requires LibUtil due to PDTime reference, this must be done
 * from the project including the current file!
 *
 * Purpose is to avoid include of PDTime.h and therefore also linkage of LibUtil
 * into the OTL base, which would require including the entire library LibUtil
 * library for every project using OTL.
 */
#include "otl.h"
#include <PDTime.h>
#include <genlib.h>

inline pdtime aspdtime(const otl_value<otl_datetime>& Tid,const PDZone& z) {
	return Tid.is_null() ? NOPDTIME : 
		z.aspdtime(Tid.v.year,Tid.v.month,Tid.v.day,Tid.v.hour,Tid.v.minute,Tid.v.second);
}

inline pdtime aspdtime(const otl_datetime& Tid,const PDZone& z) {
	return z.aspdtime(Tid.year,Tid.month,Tid.day,Tid.hour,Tid.minute,Tid.second);
}

inline otl_datetime asOtlDatetime(pdtime Tid,const PDZone& Z) {
	otl_datetime t;
	int wd;
	Z.asYMDhms_wd(Tid,t.year,t.month,t.day,t.hour,t.minute,t.second,wd);
#ifdef OTL_ORA_TIMESTAMP
	t.tz_hour = (short)Z.diffGMT(Tid)/3600;	// Betydningen av fortegnet varierer litt. Usikker på dette.
	t.tz_minute = (short)Z.diffGMT(Tid) % 3600;
#endif
	return t;
}

inline void addPdtimeToOtlStream(otl_stream& s,pdtime t,const PDZone& z) {
	// Since otl_datetime does not have null flag
	if (t == NOPDTIME)
		s << otl_null();
	else
		s << asOtlDatetime(t,z);
}

inline void addDoubleToOtlStream(otl_stream& s,double d) {
	// Set null flag if double value is NaN
	otl_value<double> ov(d);
	if (!icc_isdouble(ov.v))
		ov.set_null();
	s << ov;
}
