/**
   @file Debug.h

   Defines the Debug class for setting verbosity level.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_DEBUG_H
#define CROMBIETOOLS_COMMONTOOLS_DEBUG_H

#include <map>
#include <iostream>

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

  /// Sends a message if the verbosity level is appropriate
  template<typename T, typename... V>
    void       Message       ( DebugLevel level, T message, V... more);
  void       Message         ( DebugLevel level );

 protected:
  /// Sends the name of the function during debuggin
  void       DisplayFunc   ( const char* func );

 private:
  /// A map setting the tag to give a given debug level
  std::map<DebugLevel, const char*> mLabels = {
    {eError, "[ERROR]"}, {eInfo, "[INFO]"}, {eDebug, "[DEBUG]"}
  };

  template<typename T>
    void       OneMessage       ( DebugLevel level, T message);

  DebugLevel fDebugLevel = eError;     ///< The verbosity for a class.
  bool _printed = false;               ///< Track if the label has bee printed
};

//--------------------------------------------------------------------
void
Debug::DisplayFunc (const char* func)
{

  Message(eDebug, "-------------------------------------------------------");
  Message(eDebug, "                    ", func);
  Message(eDebug, "-------------------------------------------------------");

}

template<typename T> void Debug::OneMessage (DebugLevel level, T message)
{

  if (fDebugLevel >= level) {
    auto& debug_output = (level > eError) ? std::cout : std::cerr;
    if (not _printed) {
      debug_output << mLabels[level] << " ";
      _printed = true;
    }

    debug_output << message << " ";
  }

}

template<typename T, typename... V> void Debug::Message(DebugLevel level, T message, V... more) {
  OneMessage(level, message);
  Message(level, more...);
}

void Debug::Message(DebugLevel level) {
  _printed = false;
  if (fDebugLevel >= level) {
    auto& debug_output = (level > eError) ? std::cout : std::cerr;
    debug_output << std::endl;
  }
}

#endif
