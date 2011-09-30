#ifndef CONFIGURABLE_H
#define CONFIGURABLE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <vector>
using std::vector;

class Configurer;
class SimulationConfiguration;

class Configurable {
public:
  /** Get this Configurable's configurable subitems. */
  //  virtual vector<Configurer *> &getConfigurers() const = 0;

  virtual void configure( SimulationConfiguration &configuration ) = 0;

  /**
     This is a default implementation that returns an empty vector of
     configurers.
  */
  static vector<Configurer *> &getConfigurersEmpty();

protected:
  Configurable(){}
  virtual ~Configurable(){}
};

#endif

