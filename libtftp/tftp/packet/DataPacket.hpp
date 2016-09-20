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
 * @brief Declaration of class Tftp::Packet::DataPacket.
 **/

#ifndef TFTP_PACKET_DATAPACKET_HPP
#define TFTP_PACKET_DATAPACKET_HPP

#include <tftp/packet/Packet.hpp>
#include <tftp/packet/TftpPacket.hpp>
#include <tftp/packet/BlockNumber.hpp>

#include <vector>
#include <cstdint>

namespace Tftp {
namespace Packet {

/**
 * @brief TFTP Data packet (DATA).
 *
 * | DATA | Block # | Data 1 |
 * |:----:|:-------:|:------:|
 * |  2 B |   2 B   |  n B   |
 **/
class DataPacket: public TftpPacket
{
  public:
    //! Data type
    using DataType = std::vector< uint8_t>;

    /**
     * @brief Generates a TFTP Data Packet with the given block number and data.
     *
     * If the data parameter is suppressed, no data is used.
     *
     * If the blockNumber parameter is suppressed, a default block number is
     * chosen.
     *
     * @param[in] blockNumber
     *   Block number of the packet
     * @param[in] data
     *   The data within the packet.
     **/
    DataPacket(
      const BlockNumber blockNumber = BlockNumber(),
      const DataType &data = DataType()) noexcept;

    /**
     * @brief Generates a TFTP Data packet from a data buffer.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    DataPacket( const RawTftpPacketType &rawPacket);

    /**
     * @brief Returns the set block number.
     *
     * @return The set block number
     **/
    BlockNumber getBlockNumber( ) const;

    /**
     * @brief Sets the block number of the packet.
     *
     * @param[in] blockBumber
     *   Block number of packet.
     **/
    void setBlockNumber( const BlockNumber blockBumber);

    /**
     * @brief Returns the data as const reference.
     *
     * It is not possible to modify the data.
     *
     * @return The data as const reference.
     **/
    const DataType& getData( ) const;

    /**
     * @brief Returns the data as reference.
     *
     * It is possible to modify the data.
     *
     * @return The data as reference.
     **/
    DataType& getData( );

    /**
     * @brief Sets the data of the packet.
     *
     * The given data is copied to the local data.
     *
     * @param[in] data
     *   The data to set.
     **/
    void setData( const DataType &data);

    /**
     * @brief Sets the data of the packet by moving the content of [data].
     *
     * The given data is moved to the local data.
     *
     * @param[in] data
     *   The data to set.
     **/
    void setData( DataType &&data);

    /**
     * @brief Returns the data size.
     *
     * @return The data size in bytes.
     **/
    unsigned int getDataSize( ) const;

    //! @copydoc TftpPacket::encode()
    virtual RawTftpPacketType encode( ) const override;

    //! @copydoc TftpPacket::toString()
    virtual string toString( ) const override;

  private:
    //! Block number of the packet.
    BlockNumber blockNumber;
    //! The data of the packet.
    DataType data;
};

}
}

#endif
