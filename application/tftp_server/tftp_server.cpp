/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of TFTP Server Entry Point.
 **/

/**
 * @dir
 * @brief TFTP Server CLI Application
 **/

#include "TftpServerApplication.hpp"

#include <helper/Logger.hpp>

/**
 * @brief TFTP Server Program Entry Point
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   The arguments
 *
 * @return The success state of this operation.
 **/
int main( int argc, char * argv[]);

int main( int argc, char * argv[])
{
  initLogging();

  // The application
  TftpServerApplication app;

  // start application
  return app( argc, argv);
}
