
// See copyright notice in file Copyright in the root directory of this archive.

#include "PluginManagerTest.h"
#include <utils/PluginManager.h>
#include <utils/StringUtilities.h>
#include <utils/Debug.h>
#include <utils/PluginBase.h>
#include "Greeter.h"

using std::endl;
using std::cout;
using std::cerr;;

PluginManagerTest *
PluginManagerTest::instance(){
  static PluginManagerTest *myPluginManagerTest = new PluginManagerTest();
  
  return myPluginManagerTest;
}

int 
PluginManagerTest::instanceTest() const {
  int retval = 0;

  utils::debug << "Starting instance test - ";

  const PluginManager *fm1 = PluginManager::instance();
  const PluginManager *fm2 = PluginManager::instance();

  if( fm1 == 0 || fm2 == 0 ){
    cerr << "Got a null instance for a PluginManager!" << endl;
    retval = -1;
    goto end;
  }

  if( fm1 != fm2 ){
    cerr << "Got a two separate instances for PluginManagers!" << endl;
    retval = -1;
    goto end;
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }

  return retval;
}

int
PluginManagerTest::pluginListTest( const string &directory ) const {
  int retval = 0;

  utils::debug << "Starting plugin list test - ";

  PluginManager *mm = PluginManager::instance();
  const vector<PluginBase *> *pluginList = mm->getPluginList( directory );
  utils::debug << "Found " << pluginList->size() << " plugins" << endl;
  if( pluginList->size() > 0 ){
    for( vector<PluginBase *>::const_iterator i = pluginList->begin();
	 i != pluginList->end();
	 i++ ){
      PluginBase *currentPlugin = *i;
      utils::debug << "Found " << currentPlugin->getPluginName() << endl;
      Greeter *asGreeter = static_cast<Greeter *>(currentPlugin);
      if( asGreeter == 0 ){
	utils::debug << "Plugin loaded, but cast to Greeter * failed" << endl;
	retval = -1;
	break;
      }
      else{
	const string &greetings = asGreeter->getGreetings();
	utils::debug << "Greetings = " << greetings << endl;
      }
    }
  }
  else{
    cerr << "Failed to find any plugins - something is wrong!" << endl;
    retval = -1;
  }
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }

  return retval;
}

int 
PluginManagerTest::regressionTest(){
  int status = instanceTest();
  const string directory = "plugins";

  if( status != 0 ){
    goto end;
  }

  status = pluginListTest( directory );

 end:
  return status;
}
