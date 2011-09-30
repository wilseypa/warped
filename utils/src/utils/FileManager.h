#ifndef FILE_MANAGER_HH
#define FILE_MANAGER_HH

// See copyright notice in file Copyright in the root directory of this archive.

#include <sys/types.h>
#include <vector>
#include <string>

using std::string;
using std::vector;

/**
   This class defines an interface for generic file access.  The intent
   is that child classes will provide architecture-specific support for
   this interface.
*/

class FileManager {
public:
  /**
   * enumerates results of the "check_file_status" method.
   */
  enum FileStatus { OK,        /**< the file exists and is of the correct mode. */
		    NOT_FOUND, /**< the file doesn't exist. */
		    NOT_TYPE,  /**< the file exists, but isn't the right type of file. */
		    ERROR      /**< something bad happened. */
  };

  /**
     enumerates the types of files that we might care about.
     REGULAR_FILE - specifies a regular file, as opposed to a directory.
     DIRECTORY - specifies a directory.
     DONT_CARE - specifies that in this instance, simply use the name and don't
       worry about what type of file it is.
   */
  enum FileType { 
    REGULAR_FILE,  /**< specifies a regular file, as opposed to a directory. */
    DIRECTORY,     /**< specifies a directory. */
    DONTCARE       /**< specifies that in this instance, simply use the name and don't
		      worry about what type of file it is. */
  };

  /**
   * Get the status of a file.
   
   This method checks the status of a file.  Returns a file_status.
   @param filename an absolute or relative path to a file.
   @param MODE a file_type that specifies what type of file we're looking for. 
   @return the status of the file that was queried
   @see file_status
   @see file_type
   */
  virtual FileStatus checkFileStatus( const string &filename, 
				      FileType MODE = DONTCARE ) const = 0;

  /**
     Make the directory named.
     @param directoryName The name of the new directory.
     @return The status of the operation.
  */
  virtual FileStatus makeDirectory( const string &directoryName, 
				    mode_t mode = 0755 ) const = 0;
  
  /**
     Remove the named directory.
     @param directoryName The name of the directory to remove.
     @return The status of the operation.
  */
  virtual FileStatus removeDirectory( const string &directoryName ) const = 0;

  virtual FileStatus changeDirectory( const string &to_directory ) const = 0;
  
  /**
     Remove the named file.
  */
  virtual FileStatus unlink(const string &fileName ) const = 0;

  /**
     Renames old_name to new_name.
  */
  virtual FileStatus rename(const string &old_name, const string &new_name ) const = 0;

  /**
     Finds the newest file that matches the regular expression reg_exp.  If there are two file
     and both match reg_exp and they have the same date then file_name will contain the name of
     one of the files, but which file is random.

     @param reg_exp is the regular expression to match against.

     @param directroy is the directory to search

     @return A string with the file name if file is found, an empty string
     if it's not.
  */
  virtual const string findNewestFile( const string &reg_exp, 
				       const string &directory ) const = 0;
  /**
     Finds all files that match the pattern reg_ex in directory dir.  Places any
     mathcing files in filelist using a wrapper_container.

     @param dir: is the directory to look in.
     @param reg_ex: is the regular expression to match against.
     @param filelist: is the list where the results are put.

     Returns OK if nothing is wrong.  Will return ERROR if it could not open dir.
  */
  virtual const vector<string> *getAllFiles( const string &regEx, 
					     const string &dir ) const = 0;

  /**
     Compares files to see if they contain identical bytes.  
   */
  virtual int fileCompare(const string &, const string &) const = 0;
  
  /** For unix this returns "/".  For DOS, it would return "\". */
  virtual const string getDirectorySeparator() const = 0;
  
  /* This returns a NULL terminated array of strings containing standard
     library directories on this platform. */
  virtual const vector<string> &getLibraryDirectories() const = 0;

  /**
     This method returns a full path to the file passed in.  It's
     canoncalized.  If there is an error - for instance, if the file
     doesn't exist, the return value is null.  In the case of a POSIX
     implementation, errno will be set...

     The string returned is allocated in a static buffer and should be
     copied before the next call to this method, and cannot be freed.
  */
  virtual const string getRealPath( const string &file_name ) const = 0;

  /**
     Takes a path and returns just the filename part of it.
  */
  virtual const string baseName( const string &pathToFile ) const = 0;

  /**
     Returns a message for the last error that occurred.  The idea is
     to act very much "perror" does in a posix system.
  */
  virtual const string getLastError() const = 0;

  /**
     This method should be called to get a singleton instance of this
     class.  Only one implementation should ever be compiled into the
     system, and it should supply this method.  Therefore, the source to
     this method will not be in this class's .cc file.
  */
  static const FileManager *instance();

  virtual ~FileManager(){}

protected:
  /** This class wasn't intended to be directly instantiated, but we'll let
      an ancestor instantiate it. */
  FileManager();
};

#endif
