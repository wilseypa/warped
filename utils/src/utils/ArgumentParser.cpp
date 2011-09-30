
// See copyright notice in file Copyright in the root directory of this archive.

#include "StringUtilities.h"
#include "ArgumentParser.h"

using std::endl;

vector<ArgumentParser::ArgRecord>
ArgumentParser::getArgArray( ArgRecord ptr[] ) {
  vector<ArgRecord> retval;
  
  int i = 0;
  while( ptr[i].argText != "" ) {
    if (i > 100) {
      // If you got into this conditional, you probably forgot
      // to put "ArgumentParser::last_arg" in your array.  If not,
      // increase the number above and recompile
      std::cerr << "More than 100 arguments?\n";
      exit(-1);
    }
    retval.push_back( ptr[i] );
    i++;
  }
  return retval;
}

vector<string> 
ArgumentParser::vectorifyArguments( int argc, char **argv, bool skipFirst ){
  vector<string> retval;
  int i;
  if( skipFirst ){
    i = 1;
  }
  else{
    i = 0;
  }
  for( ; i < argc; i++ ){
    retval.push_back( string( argv[i] ) );
  }
  return retval;
}


void 
ArgumentParser::checkArgs( vector<string> &args, bool caxoe ){
  // complain_and_exit_on_error defaults to true...
  // This loop cycles through the arguments.
  for( vector<string>::iterator currentArgument = args.begin();
       currentArgument < args.end(); 
       currentArgument++ ){
    // This loop compares the arguments passed in with those we're
    // checking them against
    bool matchedOne = false;
    for( vector<ArgRecord>::iterator currentRecord = argRecordArray.begin();
	 currentRecord < argRecordArray.end() && currentArgument < args.end(); ){
      // the first check is necessary because args.size() can change during
      // execution...
      if( *currentArgument == (*currentRecord).argText ){
	matchedOne = true;
	switch( (*currentRecord).type) {
	case BOOLEAN:{
	  // They matched - let's read in the data
	  *(reinterpret_cast<bool *>((*currentRecord).data)) = true;
	  args.erase( currentArgument );
	  break;
	};
	case INTEGER:{
	  // Step to the next argument for the value
	  int endPos;
	  currentArgument++;
	  *(reinterpret_cast<int *>((*currentRecord).data)) = stringToLong( *currentArgument,
									    endPos );
	  // Step back to the flag
	  currentArgument--;
	  // Erase it
	  args.erase( currentArgument );
	  // Will step up to the value
	  // Erase it...
	  args.erase( currentArgument );
	  break;
	}
	case STRING:{
	  // Advance to input string
	  currentArgument++;
	  *(reinterpret_cast<string *>((*currentRecord).data)) = (*currentArgument);
	  // Walk back to arg flag
	  currentArgument--;
	  // Erase it
	  args.erase( currentArgument );
	  // Erase the string.
	  args.erase( currentArgument );
	  break;
	}
	default:{
	  // Do nothing...
	  break;
	}
	}
      }
      if( matchedOne == true ){
	// Reset the args pointer and record pointer.
	currentArgument = args.begin();
	currentRecord = argRecordArray.begin();
	matchedOne = false;
      }
      else{
	currentRecord++;
      }
    } // currentRecord
  } // currentArg
  
  checkRemaining( args, caxoe );
}

void 
ArgumentParser::checkRemaining( vector<string> &args,
			        bool complainAndExitOnError ) const {
  for( vector<string>::iterator currentArg = args.begin();
       currentArg != args.end();
       currentArg++ ){
    if( *currentArg == "-help" || *currentArg == "--help" ){
      std::cout << "Valid arguments are:\n";
      std::cout << *this << std::endl;
      exit( 0 );
    }
    
    if( (*currentArg)[0] == '-' ){
      // Then someone passed in an illegal argument!
      if(  complainAndExitOnError == true) {
	std::cerr << "Invalid argument \"" << (*currentArg) << "\"\n";
	std::cerr << "Valid arguments: \n";
	std::cerr << *this << std::endl;
	exit( -1 );
      }
    }
  }
}

void
ArgumentParser::printUsage( const string &binaryName, ostream &stream ) const {
  stream << "Usage: " << binaryName << " <options>" << endl;
  stream << *this;
}


ostream &
operator<<( ostream &os, const ArgumentParser &ap ){
   const unsigned int numSpaces = 3;
   const unsigned int indentation = 2;
   
   // calculate the length of the longest argument
   unsigned int maxlen = 0;
   for( vector<ArgumentParser::ArgRecord>::const_iterator currentRecord =
	  ap.argRecordArray.begin();
	currentRecord <  ap.argRecordArray.end();
	currentRecord++ ){
     if( (*currentRecord).argText.length() > maxlen ){
       maxlen = (*currentRecord).argText.length();
     }
   }
   
   // print the argument array
   for( vector<ArgumentParser::ArgRecord>::const_iterator currentRecord =
	  ap.argRecordArray.begin();
	currentRecord <  ap.argRecordArray.end();
	currentRecord++ ){
     // indent the proper amount
     for( unsigned int j = 0; j < indentation; j++ ){
       os << " ";
     }
     
      // a note if this argument is mandatory...
     if ( (*currentRecord).mandatoryFlag == true) {
       os << "* ";
     }
     else {
       os << "  ";
     }
     
     // here is the argument
     os << (*currentRecord).argText;
    
     // print out the padding - leave num_spaces spaces between args and 
     // help text...
     for( unsigned int j = 0;
	  j < (maxlen - (*currentRecord).argText.length()) + numSpaces;
	  j++ ){
       os << " ";
     }
      
     // here is the help string.
     os << (*currentRecord).argHelp << std::endl;
   }
   
   for( unsigned int j = 0; j < indentation; j++ ){
      os << " ";
   }
   
   os << "  -help";
   for( unsigned int j = 0; j < maxlen - strlen("-help")  + numSpaces; j++ ){
      os << " ";
   }
   os << "print this message" << std::endl;
   
   return os;
}
