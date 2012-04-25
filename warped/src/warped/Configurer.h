#ifndef CONFIGURER_H
#define CONFIGURER_H

// See copyright notice in file Copyright in the root directory of this archive.
#include "warped.h"
#include <string>
using std::string;
#ifdef HAVE_TR1_UNORDERED_MAP
#include<tr1/unordered_map>
using std::tr1::unordered_map;
#else
#include <unordered_map>
using std::unordered_map;
#endif


class SimulationConfiguration;
class SimulationManager;
class Configurable;
class Application;

/**
   This class implements an interface that allows confiuration of a series
   of choices.  In general, factory classes will implement this class.
*/

class Configurer {
public:
  class Choice;

  class ChoiceHashMap : public unordered_map<string, const Choice *> {
  public:
    ChoiceHashMap(){}
    ~ChoiceHashMap(){}
  };

  class Choice {
  public:
    Choice( string optionName,
	    string optionDescription,
	    string value,
	    ChoiceHashMap &additionalArguments ) :
      myOptionName( optionName ),
      myValue( value ),
      myOptionDescription( optionDescription ),
      myAdditionalArguments( additionalArguments ){}

    const string getOptionName() const {
      return myOptionName;
    }

    const string getOptionDescription() const {
      return myOptionDescription;
    }

    const string getValue() const {
      return myValue;
    }

    ChoiceHashMap &getAdditionalArguments() const {
      return myAdditionalArguments;
    }

  private:
    const string myOptionName;
    const string myValue;
    const string myOptionDescription;
    ChoiceHashMap &myAdditionalArguments;
  };

  /** Allocate one of my objects based on the configuration.  This method
      is overloaded by all ancestors.  @param configurationMap Set of
      configuration options.

      @param myConfiguration The configuration that will determine what
      type of child to instantiate.

      @param parent The parent that is allocating this child.  Can be NULL.

  */
  virtual Configurable *allocate( SimulationConfiguration &myConfiguration,
				  Configurable *parent ) const = 0;


protected:

//   /**
//      This is a factory method.  That is, we pass in a choice, and we get
//      back an object.
//   */
//   virtual Configurable *allocate( const Choice * ) = 0;

  Configurer(){}
  virtual ~Configurer(){}
};

#endif
