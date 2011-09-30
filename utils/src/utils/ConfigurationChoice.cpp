
// See copyright notice in file Copyright in the root directory of this archive.

#include <assert.h>
#include <string.h>
#include "ConfigurationValue.h"
#include "ConfigurationChoice.h"
#include "StringUtilities.h"

ConfigurationChoice::ConfigurationChoice( const string propertyName ): 
  myPropertyName( propertyName ), myConfigurationValue( 0 ){}

ConfigurationChoice::ConfigurationChoice( const char *propertyName ): 
  myPropertyName( *new string( strdup(propertyName) ) ), myConfigurationValue( 0 ){}

const string &
ConfigurationChoice::getStringValue() const {
  assert( myConfigurationValue != 0 );
  return myConfigurationValue->getStringValue();
}

int 
ConfigurationChoice::getIntValue() const {
  assert( myConfigurationValue != 0 );
  return myConfigurationValue->getIntValue();
}
