/**
   @file Debug.h

   Defines the Debug class for setting verbosity level.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_DEBUG_H
#define CROMBIETOOLS_COMMONTOOLS_DEBUG_H

#include "string.h"

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
    kQuiet = 0,   /// Only prints out what ROOT insists on
    kErrors,      /// Prints when it's obvious something goes wrong
    kInfo         /// Prints debugging information
  }

  /// Sets the verbosity for a class
  void SetDebugLevel ( DebugLevel level )           { fDebugLevel = level; }

 protected:
  /// Sends a message if the verbosity level is appropriate
  void Message       ( DebugLevel level, char* message );

 private:
  DebugLevel fDebugLevel = kErrors;    /// The verbosity for a class.
};

//--------------------------------------------------------------------
void
Debug::Message (DebugLevel level, char* message)
{

  if (fDebugLevel >= level)
    printf("%s\n", message);

}

#endif
