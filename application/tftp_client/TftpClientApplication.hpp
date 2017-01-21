/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class TftpClientApplication.
 **/

#ifndef TFTPCLIENTAPPLICATION_HPP
#define TFTPCLIENTAPPLICATION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/TftpConfiguration.hpp>

#include <boost/program_options.hpp>
#include <boost/application.hpp>
#include <boost/asio.hpp>

#include <string>

class TftpClientApplication
{
  public:
    /**
     * @brief Constructs the TFTP client application.
     *
     * @param[in] context
     *   The application context.
     **/
    TftpClientApplication( boost::application::context &context);

    /**
     * @brief Executes the TFTP client.
     *
     * @return The return code, which should be returned to the system.
     * @retval EXIT_SUCCESS
     *   If operation has been performed successfully.
     * @retval EXIT_FAILURE
     *   If an error has occurred.
     **/
    int operator()();

  private:
    /**
     * @brief Parsed the command line and assigns parameter.
     *
     * @return If parsing was successful
     **/
    bool handleCommandLine();

    //! The application context
    boost::application::context& context;
    //! Program Options description
    boost::program_options::options_description optionsDescription;

    std::string operationStr;
    Tftp::RequestType requestType;
    std::string localFile;
    std::string remoteFile;
    boost::asio::ip::address address;
    Tftp::TftpConfiguration configuration;
};

#endif
