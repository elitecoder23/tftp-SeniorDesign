/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of unit tests of class Tftp::Packets::DataPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/DataPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp {
namespace Packets {

BOOST_AUTO_TEST_SUITE( TftpDataPacket)

BOOST_AUTO_TEST_CASE( constructor )
{
  DataPacket dp1;

  BOOST_CHECK( dp1.getPacketType() == PacketType::Data);
  BOOST_CHECK( dp1.getBlockNumber() == BlockNumber());
  BOOST_CHECK( dp1.getDataSize() == 0);
  BOOST_CHECK( dp1.getData().empty());

  DataPacket dp2( BlockNumber(), {'H', 'E', 'L', 'L', 'O' });
  BOOST_CHECK( dp2.getPacketType() == PacketType::Data);
  BOOST_CHECK( dp2.getBlockNumber() == BlockNumber());
  BOOST_CHECK( dp2.getDataSize() == 5);
  BOOST_CHECK( !dp2.getData().empty());

  DataPacket dp3( 55, {'H', 'E', 'L', 'L', 'O' });
  BOOST_CHECK( dp3.getPacketType() == PacketType::Data);
  BOOST_CHECK( dp3.getBlockNumber() == BlockNumber(55));
  BOOST_CHECK( dp3.getDataSize() == 5);
  BOOST_CHECK( !dp3.getData().empty());

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

  RawTftpPacketType raw( data);

  std::cout << Dump( &(*raw.begin()), raw.size());

  DataPacket data2( raw);

  BOOST_CHECK( data.getPacketType()  == data2.getPacketType());
  BOOST_CHECK( data.getBlockNumber() == data2.getBlockNumber());
  BOOST_CHECK( data.getDataSize()    == data2.getDataSize());
  BOOST_CHECK( data.getData()        == data2.getData());
}

BOOST_AUTO_TEST_CASE( setBlockNumber )
{
  DataPacket dp1;
  const DataPacket &dp1Const( dp1);

  BOOST_CHECK( dp1.getBlockNumber() == BlockNumber());
  BOOST_CHECK( dp1Const.getBlockNumber() == BlockNumber());

  dp1.setBlockNumber( 10);
  BOOST_CHECK( dp1.getBlockNumber() == BlockNumber( 10));
  BOOST_CHECK( dp1Const.getBlockNumber() == BlockNumber( 10));

  dp1.getBlockNumber()++;
  BOOST_CHECK( dp1.getBlockNumber() == BlockNumber( 11));
  BOOST_CHECK( dp1Const.getBlockNumber() == BlockNumber( 11));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
