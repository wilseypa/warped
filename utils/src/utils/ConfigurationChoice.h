#ifndef CONFIG_CHOICE_H
#define CONFIG_CHOICE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <stdlib.h>

class ConfigurationValue;

class ConfigurationChoice {
public:
  /**
     Constructor
   */
  ConfigurationChoice( const string propertyName );
  
  /**
     Constructor
   */
  ConfigurationChoice( const char *propertyName );

  ~ConfigurationChoice() {
	  delete myConfigurationValue;
  }
  /**
     Returns the property name.
   */
  const string &getPropertyName() const {
    return myPropertyName;
  }

  /** 
      If this is a string valued choice, the string value is returned.
      Else a runtime error is produced.
   */
  const string &getStringValue() const;

  int getIntValue() const;

  const ConfigurationValue *getConfigurationValue() const {
    return myConfigurationValue;
  }

  void setConfigurationValue( const ConfigurationValue *newValue ){
    myConfigurationValue = newValue;
  }

private:
  ConfigurationChoice( const ConfigurationChoice & ){ abort(); }
  ConfigurationChoice operator=( const ConfigurationChoice & ){ abort(); }


  const string myPropertyName;
  const ConfigurationValue *myConfigurationValue;
};

#endif
