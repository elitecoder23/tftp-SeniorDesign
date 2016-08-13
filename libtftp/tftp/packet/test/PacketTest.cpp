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
 * @brief Definition of uint tests of TFTP packet classes
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packet/ReadRequestPacket.hpp>
#include <tftp/packet/WriteRequestPacket.hpp>
#include <tftp/packet/DataPacket.hpp>
#include <tftp/packet/ErrorPacket.hpp>
#include <tftp/packet/AcknowledgementPacket.hpp>
#include <tftp/packet/OptionsAcknowledgementPacket.hpp>

#include <helper/Dump.hpp>
#include <helper/Logger.hpp>

#include <iostream>

using namespace Tftp::Packet;
using namespace Tftp;

static void tftp_packet_rrq( void)
{
  Tftp::Options::OptionList options;

  options.setOption( "blocksize", "4096");

  ReadRequestPacket rrq( "testfile.bin", TransferMode::OCTET, options);

  RawTftpPacketType raw = rrq.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  ReadRequestPacket rrq2( raw);

  BOOST_CHECK( rrq.getPacketType() == rrq2.getPacketType());
  BOOST_CHECK( rrq.getFilename() == rrq2.getFilename());
  BOOST_CHECK( rrq.getMode() == rrq2.getMode());
  BOOST_CHECK( rrq.getOption( "blocksize") == rrq2.getOption( "blocksize"));
  BOOST_CHECK( rrq.getOption( "XXX") == "");
}

static void tftp_packet_wrq( void)
{
  Tftp::Options::OptionList options;

  options.setOption( "blocksize", "4096");

  WriteRequestPacket wrq( "testfile.bin", TransferMode::OCTET, options);

  RawTftpPacketType raw = wrq.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  WriteRequestPacket wrq2( raw);

  BOOST_CHECK( wrq.getPacketType() == wrq2.getPacketType());
  BOOST_CHECK( wrq.getFilename() == wrq2.getFilename());
  BOOST_CHECK( wrq.getMode() == wrq2.getMode());
  BOOST_CHECK( wrq.getOption( "blocksize") == wrq2.getOption( "blocksize"));
  BOOST_CHECK( wrq.getOption( "XXX") == "");
}

static void tftp_packet_data( void)
{
  std::vector< uint8_t> bindata;

  bindata.push_back( 'H');
  bindata.push_back( 'E');
  bindata.push_back( 'L');
  bindata.push_back( 'L');
  bindata.push_back( 'O');
  bindata.push_back( ' ');
  bindata.push_back( 'W');
  bindata.push_back( 'O');
  bindata.push_back( 'R');
  bindata.push_back( 'L');
  bindata.push_back( 'D');
  bindata.push_back( '!');

  DataPacket data( 10, bindata);

  RawTftpPacketType raw = data.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  DataPacket data2( raw);

  BOOST_CHECK( data.getPacketType()  == data2.getPacketType());
  BOOST_CHECK( data.getBlockNumber() == data2.getBlockNumber());
  BOOST_CHECK( data.getDataSize()    == data2.getDataSize());
  BOOST_CHECK( data.getData()        == data2.getData());
}

static void tftp_packet_error( void)
{
  ErrorPacket error( ErrorCode::NOT_DEFINED, "ERROR MESSAGE");

  RawTftpPacketType raw = error.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  ErrorPacket error2( raw);

  BOOST_CHECK( error.getPacketType() == error2.getPacketType());
  BOOST_CHECK( error.getErrorCode() == error2.getErrorCode());
  BOOST_CHECK( error.getErrorMessage() == error2.getErrorMessage());
}

static void tftp_packet_error_str( void)
{
  std::cout <<
    static_cast< int>( ErrorCode::NOT_DEFINED) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::NOT_DEFINED) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::FILE_NOT_FOUND) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::FILE_NOT_FOUND) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::ACCESS_VIOLATION) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::ACCESS_VIOLATION) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::DISK_FULL_OR_ALLOCATION_EXCEEDS) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::DISK_FULL_OR_ALLOCATION_EXCEEDS) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::ILLEGAL_TFTP_OPERATION) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::ILLEGAL_TFTP_OPERATION) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::UNKNOWN_TRANSFER_ID) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::UNKNOWN_TRANSFER_ID) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::FILE_ALLREADY_EXISTS) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::FILE_ALLREADY_EXISTS) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::NO_SUCH_USER) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::NO_SUCH_USER) <<
    std::endl;
  std::cout <<
    static_cast< int>( ErrorCode::TFTP_OPTION_REFUSED) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode::TFTP_OPTION_REFUSED) <<
    std::endl;
  std::cout <<
    static_cast< int>( 99) << ": " <<
    BaseErrorPacket::getErrorCodeString( ErrorCode(99)) <<
    std::endl;
}

static void tftp_packet_ack( void)
{
  AcknowledgementPacket ack( 10);

  RawTftpPacketType raw = ack.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  AcknowledgementPacket ack2( raw);

  BOOST_CHECK( ack.getPacketType()  == ack2.getPacketType());
  BOOST_CHECK( ack.getBlockNumber() == ack2.getBlockNumber());
}

static void tftp_packet_oack( void)
{
  Tftp::Options::OptionList options;

  options.setOption( "blocksize", "4096");

  OptionsAcknowledgementPacket oack( options);

  RawTftpPacketType raw = oack.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  OptionsAcknowledgementPacket oack2( raw);

  BOOST_CHECK( oack.getPacketType()         == oack2.getPacketType());
  BOOST_CHECK( oack.getOption( "blocksize") == oack2.getOption( "blocksize"));
  BOOST_CHECK( oack.getOption( "XXX")       == "");
}

void initTftpPacketTest( void)
{
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_rrq));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_wrq));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_data));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_error));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_error_str));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_ack));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &tftp_packet_oack));
}
