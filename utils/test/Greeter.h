#ifndef GREETER_H
#define GREETER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>
#include <utils/PluginBase.h>

using std::string;

class Greeter : public PluginBase {
public:
  virtual const string getGreetings() const = 0;

protected:
  inline Greeter(){}
  inline virtual ~Greeter(){}
};

#endif
