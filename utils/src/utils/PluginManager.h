#ifndef _PLUGIN_MANAGER
#define _PLUGIN_MANAGER

// See copyright notice in file Copyright in the root directory of this archive.

#include <vector>
#include <string>
#include <map>
#include <ltdl.h>

using std::vector;
using std::string;
using std::map;

class PluginBase;

/**
   the purpose of this class is to manage plugin loading.
*/

class PluginManager {

public:
  /**
     Returns the singleton instance of the PluginManager.
   */
  static PluginManager* instance();


  /**
     Determines a list of savant plugins in dir. The file names are added to library_list
     inside of wrapper_container.
     @param dir Directory to look in for plugins.
     @library_list: list where the filenames will be added.

     @return List of available plugins if any are available, 0 otherwise.
   */
  const vector<PluginBase *> *getPluginList( const string &dir );
  
  /**
     Returns the last error seen when attempting to load a plugin.  When a
     call fails, this method should be called to see what happened.
  */
  const string &getLastError() const;

  /**
     Load the named plugin.  Returns NULL if it couldn't be loaded, and
     "getLastError" will contain the reason.
  */
  PluginBase *loadPlugin( const string &fileName );

  /**
       Unload the named plugin.  This allows us to more efficently manage our
     memory by releasing plugins when we're done with them.
  */
  void unloadPlugin( const string &fileName);

  /**
     Add another directory to the library search path.  Returns 0 on success
  */
  int addSearchDir(const string &search_dir );

  /**
     Define the *complete* library search path.  These will be the only
     directories searched when attempting to load a plugin.  Paths should
     be colon ":" separated.  Returns 0 on sucess.
  */
  int setSearchPath(const string &search_path );

  /**
     Gets the current user defined library search path.
  */
  const string getSearchPath();


protected:
  /**
     Destructor
   */
  ~PluginManager();

private:
  /**
     Constructor
   */
  PluginManager();

  /**
     Performs neccessary work to make sure filename is a valid SAVANT archieve.
     @param filename  Filename to check
     returns true if filename is a savant plugin otherwise false.
     
     To determine if filename is a savant archieve, it dlopens the file and calls allocate_plugin.
     It then casts the result to a PluginBase, and calls PluginBase:: plugin_type.  If 
     this matches savant_PluginBase:: savant_plugin_type then filename is condidered to be a 
     savant plugin.
   */
  bool isValidPlugin( const string &fileName );

  /**
     Gets the name of the allocator function from the file name.  It does this
     by stripping off leading directory names, the file suffix, and prepending
     the allocation function prefix.  Returns "" if the plugin name can not
     be figured out.
  */
  const string getAllocatorNameFromFileName( const string &fileName ) const;

  mutable string lastError;

  map<const string, lt_dlhandle>         loadedPlugins;
};


#endif
