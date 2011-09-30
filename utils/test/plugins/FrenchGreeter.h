#ifndef FRENCH_GREETER_H
#define FRENCH_GREETER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "../Greeter.h"

class FrenchGreeter : public Greeter {
public:
  const string getGreetings() const;

  const string getPluginType() const;
  
  const string getPluginName() const;

  
  FrenchGreeter();
  virtual ~FrenchGreeter();

};

#endif
