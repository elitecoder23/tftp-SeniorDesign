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

#include <tftp/packet/BaseErrorPacket.hpp>
#include <tftp/packet/AcknowledgementPacket.hpp>
#include <tftp/packet/OptionsAcknowledgementPacket.hpp>

#include <helper/Dump.hpp>
#include <helper/Logger.hpp>

#include <iostream>

using namespace Tftp::Packet;
using namespace Tftp;

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
