#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>
#include <string.h>

using std::string;

/** Uppercase the passed-in input string */
const string upperCase( const string &inputString );

/** Cut out all white spaces in passed-in input string */
const string trimWhiteSpaces( const string &inputString );

/**
   Return the integer passed in as a string.
*/
const string intToString( int input );

/**
   Return the double passed in as a string.
*/
const string doubleToString( double input );

/**
   Take the string that represents an integer, and return an int.  The bool
   passed in will be set to true if the conversion is successful, else it
   will be false.
*/
long stringToLong( const string &input, int &endPos );

/**
   Take the string containing a floating point value, and return a float.
   The bool passed in will be set to true if the conversion is successful,
   else it will be false.
*/
double stringToDouble( const string &input, int &endPos );

/**
   Compares if two strings are equal if one ignores case differences.
*/
bool stringCaseCompare( const string &s1, const string &s2 );

#endif
