#ifndef PLUGIN_BASE
#define PLUGIN_BASE

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>
using std::string;

/**
   This is the base class for all modules.  In addition to deriving from
   this class, all Plugin classes must define an extern "C" function void
   *allocate<moduleName> that constructs an appropriate object and returns
   it.  The void * can then be static casted into the correct type.  There
   is also an assumption that the module will be compiled into a file
   called "<moduleName>.la" or "<moduleName>.so".
*/

class PluginBase {
public:
  /** @return a string saying what the module is for.  For example, it
   could return the name of the system for which this plugin was
   constructed. */
  virtual const string getPluginType() const = 0;

  /** @return the name of this particular plugin. */
  virtual const string getPluginName() const = 0;

  virtual ~PluginBase()=0; 

  /**
     @return A statically defined plugin signature.  Used by the plugin
     loader to ensure that when we've loaded a plugin, we really can tell
     that it IS a plugin.
  */
  static const int pluginSignature();

  /**
     @return Same as the static call pluginSignature, but non-static.
     
     @see PluginBase#pluginSignature
  */
  const int getPluginSignature();

protected:
  PluginBase();
};

#endif
