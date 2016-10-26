/**
   @file Debug.h

   Defines the Debug class for setting verbosity level.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_DEBUG_H
#define CROMBIETOOLS_COMMONTOOLS_DEBUG_H

#include <map>
#include <stdio.h>
#include <stdarg.h>

/**
   @ingroup commongroup
   @struct Debug
   Class setting verbosity levels
*/

class Debug
{
 public:
  Debug ()          {}
  virtual ~Debug()  {}

  /// Different possible debug levels
  enum DebugLevel {
    eQuiet = 0,   ///< Only prints out what ROOT insists on
    eError,       ///< Prints when it's obvious something goes wrong
    eInfo,        ///< Prints information that might be interesting
    eDebug,       ///< Prints debugging information
  };

  /// Sets the verbosity for a class
  void       SetDebugLevel ( DebugLevel level )                   { fDebugLevel = level;  }

  /// Gets the verbosity for a class
  DebugLevel GetDebugLevel ()                                     { return fDebugLevel;   }

 protected:
  /// Sends a message if the verbosity level is appropriate
  void       Message       ( DebugLevel level, const char* format, ...);

 private:
  /// A map setting the tag to give a given debug level
  std::map<DebugLevel, const char*> mLabels = {
    {eError, "[ERROR]"}, {eInfo, "[INFO]"}, {eDebug, "[DEBUG]"}
  };

  DebugLevel fDebugLevel = eError;     ///< The verbosity for a class.
};

//--------------------------------------------------------------------
void
Debug::Message (DebugLevel level, const char* format, ...)
{

  if (fDebugLevel >= level) {

    fprintf(stderr, "%s ", mLabels[level]);

    va_list args;
    va_start (args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");

  }

}

#endif
