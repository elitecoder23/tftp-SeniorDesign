/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration/ definition of TFTP server entry point.
 **/

#include "TftpServerApplication.hpp"

#include <helper/Logger.hpp>

#include <boost/application.hpp>

/**
 * @brief TFTP server program entry point
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   The arguments
 *
 * @return The success state of this operation.
 **/
int main( int argc, char ** argv);

int main( int argc, char ** argv)
{
  initLogging();

  // The application context
  boost::application::context context;

  // The application
  TftpServerApplication app( context);

  // Arguments
  context.insert < boost::application::args>(
    std::make_shared < boost::application::args > (argc, argv));

  // Termination Handler
  context.insert < boost::application::termination_handler>(
    std::make_shared < boost::application::termination_handler>(
      boost::application::handler< bool>::make_callback(
        app,
        &TftpServerApplication::stop)));

  // start application
  return boost::application::launch < boost::application::common>(
    app,
    context);
}
