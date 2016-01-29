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
 * @brief Definition of class TftpServerApplication.
 **/

#include "TftpServerApplication.hpp"

#include <tftp/file/StreamFile.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>

#include <helper/Logger.hpp>

#include <boost/exception/all.hpp>

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <memory>

using Tftp::File::StreamFile;
using Tftp::TftpException;
using Tftp::UdpAddressType;
using Tftp::Server::TftpServerOperation;

TftpServerApplication::TftpServerApplication( boost::application::context &context):
	context( context),
	optionsDescription( "TFTP server options")
{
	optionsDescription.add_options()
		(
			"help",
			"print this help screen"
		)

		(
			"server-root",
			boost::program_options::value< boost::filesystem::path>( &baseDir)->default_value( boost::filesystem::current_path()),
			"Directory path, where the server shall have its root"
		)
		(
			"server-port",
			boost::program_options::value< uint16_t>( &port)->default_value(
				Tftp::DEFAULT_TFTP_PORT),
			"UDP port, where the server shall listen"
		)
		(
			"blocksize-option",
			boost::program_options::value< uint16_t>( &configuration.blockSizeOptionValue),
			"blocksize of transfers to use"
		)
		(
			"timeout-option",
			boost::program_options::value< uint16_t>( &configuration.timoutOptionValue),
			"If set handles the timeout option negotiation"
		)
		(
			"handle-transfer-size-option",
			boost::program_options::bool_switch( &configuration.handleTransferSizeOption),
			"If set handles the transfer size option negotiation"
		);
}

TftpServerApplication::~TftpServerApplication( void) noexcept
{
}

int TftpServerApplication::operator()( void)
{
	try
	{
		std::cout << "TFTP server" << std::endl;

		if (!handleCommandLine())
		{
			return EXIT_FAILURE;
		}

		BOOST_LOG_TRIVIAL( info) <<
			"Starting TFTP server in " << baseDir.string() << " on port " << port;

		//! The TFTP server instance
		server = TftpServer::createInstance(
			configuration,
			Tftp::Options::OptionList(),
			UdpAddressType(
				boost::asio::ip::address_v4::any(),
				port));

		server->registerRequestHandler(
			Tftp::TftpRequestType::ReadRequest,
			std::bind(
				&TftpServerApplication::receivedReadRequest,
				this,
				std::placeholders::_2,
				std::placeholders::_3,
				std::placeholders::_4,
				std::placeholders::_5));

		server->registerRequestHandler(
			Tftp::TftpRequestType::WriteRequest,
			std::bind(
				&TftpServerApplication::receivedWriteRequest,
				this,
				std::placeholders::_2,
				std::placeholders::_3,
				std::placeholders::_4,
				std::placeholders::_5));

		server->start();
	}
	catch ( Tftp::TftpException &e)
	{
    std::string const * info = boost::get_error_info< AdditionalInfo>( e);

		std::cerr <<
			"TFTP Server exited with failure: " <<
			((0==info) ? "Unknown" : *info) <<
			std::endl;

		return EXIT_FAILURE;
	}
	catch (boost::exception &e)
	{
		std::cerr << "Error in TFTP server: " << boost::diagnostic_information( e) <<
    	std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Error in TFTP server: UNKNOWN EXCEPTION" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}

bool TftpServerApplication::stop( void)
{
	std::cout << "Termination request" << std::endl;

	server->stop();

	return true;
}

bool TftpServerApplication::handleCommandLine( void)
{
	try
	{
    std::shared_ptr< boost::application::args> args =
    	context.find< boost::application::args>();

		boost::program_options::variables_map options;
		boost::program_options::store(
			boost::program_options::parse_command_line( args->argc(), args->argv(), optionsDescription),
			options);
		boost::program_options::notify( options);

		if (options.count("help")!=0)
		{
			std::cout << optionsDescription << std::endl;
			return false;
		}

		// Activate blocksize option, if parameter is set
		configuration.handleBlockSizeOption = (0!=options.count( "blocksize-option"));
		// Activate timeout option, if parameter is set
		configuration.handleTimeoutOption = (0!=options.count( "timeout-option"));
	}
	catch (boost::program_options::error &e)
	{
		std::cout << e.what() << std::endl <<
			optionsDescription << std::endl;
		return false;
	}

	return true;
}

void TftpServerApplication::checkFilename( const boost::filesystem::path &filename) const
{
	if (filename.is_relative())
	{
		BOOST_THROW_EXCEPTION( TftpException() <<
			AdditionalInfo( "Illegal filename: is relative"));
	}

	if (boost::filesystem::is_directory( filename))
	{
		BOOST_THROW_EXCEPTION( TftpException() <<
			AdditionalInfo( "Illegal filename: is a directory"));
	}

	if (!(filename <= baseDir))
	{
		BOOST_LOG_TRIVIAL( info) << "potentially filename attack" << filename;
	}
}

void TftpServerApplication::receivedReadRequest(
	const UdpAddressType &from,
	const string &filename,
	const TransferMode mode,
	const OptionList &options)
{
	BOOST_LOG_TRIVIAL( info) <<
		"RRQ: " << filename << " from: " << from.address().to_string();

	// Check transfer mode
	if (mode != TransferMode::OCTET)
	{
		BOOST_LOG_TRIVIAL( error) << "Wrong transfer mode";

		TftpServerOperation operation = server->createErrorOperation(
			from,
			Tftp::ErrorCode::ILLEGAL_TFTP_OPERATION,
			"wrong transfer mode");

		operation();

		return;
	}

	try
	{
		checkFilename( baseDir / filename);
	}
	catch (TftpException &e)
	{
		std::string const * info = boost::get_error_info< AdditionalInfo>( e);

		BOOST_LOG_TRIVIAL( error) << "Error filename check: " << ((0==info) ? "Unknown" : *info);

		TftpServerOperation operation = server->createErrorOperation(
			from,
			Tftp::ErrorCode::ACCESS_VIOLATION,
			e.what());

		operation();

		return;
	}

	// open requested file
	std::fstream fileStream( filename.c_str(), std::fstream::in);

	// check that file was opened successfully
	if (!fileStream.good())
	{
		BOOST_LOG_TRIVIAL( error) << "Error opening file";

		TftpServerOperation operation = server->createErrorOperation(
			from,
			Tftp::ErrorCode::FILE_NOT_FOUND,
			"file not found");

		operation();

		return;
	}

	StreamFile file( fileStream, boost::filesystem::file_size( filename));

	// initiate TFTP operation
	TftpServerOperation operation = server->createReadRequestOperation(
		file,
		from,
		options);

	// executes the TFTP operation
	operation();
}

void TftpServerApplication::receivedWriteRequest(
	const UdpAddressType &from,
	const string &filename,
	const TransferMode mode,
	const OptionList &options)
{
	BOOST_LOG_TRIVIAL( info) <<
		"RRQ: " << filename << " from: " << from.address().to_string();

	//! Check transfer mode
	if (mode != TransferMode::OCTET)
	{
		BOOST_LOG_TRIVIAL( error) << "Wrong transfer mode";

		TftpServerOperation operation = server->createErrorOperation(
			from,
			Tftp::ErrorCode::ILLEGAL_TFTP_OPERATION,
			"wrong transfer mode");

		operation();

		return;
	}

	try
	{
		checkFilename( baseDir / filename);
	}
	catch (TftpException &e)
	{
		std::string const * info = boost::get_error_info< AdditionalInfo>( e);

		BOOST_LOG_TRIVIAL( error) << "Error filename check: " << ((0==info) ? "Unknown" : *info);

		TftpServerOperation operation = server->createErrorOperation(
			from,
			Tftp::ErrorCode::ACCESS_VIOLATION,
			e.what());

		operation();

		return;
	}

	// open requested file
	std::fstream fileStream(
		filename.c_str(),
		std::fstream::out | std::fstream::trunc);

	// check that file was opened successfully
	if (!fileStream.good())
	{
		BOOST_LOG_TRIVIAL( error) << "Error opening file";

		TftpServerOperation operation = server->createErrorOperation(
			from,
			Tftp::ErrorCode::ACCESS_VIOLATION);

		operation();

		return;
	}

	StreamFile file( fileStream);

	// initiate TFTP operation
	TftpServerOperation operation = server->createWriteRequestOperation(
		file,
		from,
		options);

	// executes the TFTP operation
	operation();
}

void TftpServerApplication::shutdown( void)
{
	server->stop();
}
