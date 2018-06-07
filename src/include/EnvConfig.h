#ifndef CROMBIE_ENVCONFIG_H
#define CROMBIE_ENVCONFIG_H

/**
   @file EnvConfig.h

   Gathers a number of variables from the environment
*/

#include <cstdlib>
#include <string>

namespace Crombie {

  namespace EnvConfig {

    static unsigned ncores = getenv("ncores") ? std::stoi(getenv("ncores")) : 1;

  }

}

#endif
