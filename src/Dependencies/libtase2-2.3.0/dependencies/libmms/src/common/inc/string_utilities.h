/*
 *  string_utilities.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef STRING_UTILITIES_H_
#define STRING_UTILITIES_H_

#include "libmms_platform_includes.h"
#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

char*
StringUtils_copyString(const char* string);

char*
StringUtils_copyStringToBuffer(const char* string, char* buffer);

char*
StringUtils_copySubString(char* startPos, char* endPos);

/**
 * \brief Concatenate strings. count indicates the number of strings
 * to concatenate.
 */
char*
StringUtils_createString(int count, ...);

/**
 * \brief Concatenate strings in user provided buffer. count indicates the number of strings
 * to concatenate.
 */
char*
StringUtils_createStringInBuffer(char* buffer, int count, ...);

char*
StringUtils_createStringFromBuffer(const uint8_t* buf, int size);

char*
StringUtils_createStringFromBufferInBuffer(char* newString, const uint8_t* buf, int size);

void
StringUtils_replace(char* string, char oldChar, char newChar);

bool
StringUtils_isDigit(char character);

int
StringUtils_digitToInt(char digit);

int
StringUtils_digitsToInt(const char* digits, int count);

int
StringUtils_createBufferFromHexString(char* hexString, uint8_t* buffer);

/**
 * \brief test if string starts with prefix
 */
bool
StringUtils_startsWith(const char* string, const char* prefix);

bool
StringUtils_endsWith(const char* str, const char* suffix);

/**
 * \brief Compare to characters using the collation order as defined in ISO 9506-2 7.5.2
 *
 * \param a the first string
 * \param b the second string
 *
 * \returns 0 if a equals b; a positive number if b > a; a negative number if b < a
 */
int
StringUtils_compareChars(char a, char b);

/**
 * \brief Compare to strings using the collation order as defined in ISO 9506-2 7.5.2
 *
 * \param a the first string
 * \param b the second string
 *
 * \returns 0 if a equals b; a positive number if b > a; a negative number if b < a
 */
int
StringUtils_compareStrings(const char* a, const char* b);

/**
 * \brief sort a list of strings alphabetically (according to the MMS identifier collation order)
 *
 * \param list a list that contains string elements
 */
void
StringUtils_sortList(LinkedList list);

#ifdef __cplusplus
}
#endif


#endif /* STRING_UTILITIES_H_ */
