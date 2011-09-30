#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <string>

using std::string;

class PluginManagerTest {
public:
  /**
     This is a method that runs a self test to see if the PluginManager class
     is working as expected.

     @return 0 if passed, non zero if failed.
  */
  int regressionTest();
  
  /**
     Return the singleton test object.
  */
  static PluginManagerTest *instance();

protected:
  /** Destructor */
  ~PluginManagerTest(){}

private:
  /** Checks to see if we can get an instance of the plugin manager. */
  int instanceTest() const;

  int pluginListTest( const string &directory ) const;

  /** Constructor */
  PluginManagerTest(){}
};

#endif
