/**
   @file Debug.h

   Defines the Debug class for setting verbosity level.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_DEBUG_H
#define CROMBIETOOLS_COMMONTOOLS_DEBUG_H

#include <map>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

FILE* debug_output;

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

  /// Sends the name of the function during debuggin
  void       DisplayFunc   ( const char* func );

 private:
  /// A map setting the tag to give a given debug level
  std::map<DebugLevel, const char*> mLabels = {
    {eError, "[ERROR]"}, {eInfo, "[INFO]"}, {eDebug, "[DEBUG]"}
  };

  DebugLevel fDebugLevel = eError;     ///< The verbosity for a class.
};

//--------------------------------------------------------------------
void
Debug::DisplayFunc (const char* func)
{

  Message(eDebug, "-------------------------------------------------------");
  Message(eDebug, "                    %s", func);
  Message(eDebug, "-------------------------------------------------------");

}

//--------------------------------------------------------------------
void
Debug::Message (DebugLevel level, const char* format, ...)
{

  if (fDebugLevel >= level) {

    debug_output = (level > eError) ? stdout : stderr;

    va_list args;
    va_start (args, format);

    fprintf(debug_output, "%s ", mLabels[level]);
    vfprintf(debug_output, format, args);
    fprintf(debug_output, "\n");

    va_end(args);

  }

}

#endif
