/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::DataPacket.
 **/

#ifndef TFTP_PACKETS_DATAPACKET_HPP
#define TFTP_PACKETS_DATAPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>
#include <tftp/packets/BlockNumber.hpp>

#include <vector>
#include <cstdint>

namespace Tftp::Packets {

/**
 * @brief TFTP Data %Packet (DATA).
 *
 * | DATA | Block # | Data 1 |
 * |:----:|:-------:|:------:|
 * |  2 B |   2 B   |  n B   |
 **/
class DataPacket: public Packet
{
  public:
    //! Minimum Header Size (Opcode + Block Number)
    static constexpr size_t MinPacketSize{ HeaderSize + 2U };

    //! Data Type
    using Data = std::vector< uint8_t >;

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
      BlockNumber blockNumber,
      const Data &data ) noexcept;

    //! @copydoc DataPacket(BlockNumber,const Data&)
    explicit DataPacket(
      BlockNumber blockNumber = {},
      Data &&data = {} ) noexcept;

    /**
     * @brief Generates a TFTP Data packet from a data buffer.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    explicit DataPacket( ConstRawTftpPacketSpan rawPacket );

    /**
     * @brief Assigns a Raw Packet to this Packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    DataPacket& operator=( ConstRawTftpPacketSpan rawPacket );

    /**
     * @name Block Number
     * @{
     **/

    /**
     * @brief Returns the block number.
     *
     * @return Current block number.
     **/
    [[nodiscard]] BlockNumber blockNumber() const;

    /**
     * @brief Returns the block number.
     *
     * @return Current block number (modifiable).
     **/
    [[nodiscard]] BlockNumber& blockNumber();

    /**
     * @brief Sets the block number of the packet.
     *
     * @param[in] blockNumber
     *   Block number of packet.
     **/
    void blockNumber( BlockNumber blockNumber );

    //! @}

    /**
     * @name Data
     * @{
     **/

    /**
     * @brief Returns the data as const reference.
     *
     * It is not possible to modify the data.
     *
     * @return The data as const reference.
     **/
    [[nodiscard]] const Data& data() const;

    /**
     * @brief Returns the data as reference.
     *
     * It is possible to modify the data.
     *
     * @return The data as reference.
     **/
    [[nodiscard]] Data& data();

    /**
     * @brief Sets the data of the packet.
     *
     * The given data is copied to the local data.
     *
     * @param[in] data
     *   The data to set.
     **/
    void data( const Data &data );

    /**
     * @brief Sets the data of the packet by moving the content of [data].
     *
     * The given data is moved to the local data.
     *
     * @param[in] data
     *   The data to set.
     **/
    void data( Data &&data );

    /**
     * @brief Returns the data size.
     *
     * @return The data size in bytes.
     **/
    [[nodiscard]] size_t dataSize() const;

    //! @}

    //! @copydoc Packet::operator std::string() const
    explicit operator std::string() const final;

  private:
    //! @copydoc Packet::encode()
    [[nodiscard]] RawTftpPacket encode() const final;

    /**
     * @brief Decodes the TFTP body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     **/
    void decodeBody( ConstRawTftpPacketSpan rawPacket );

    //! Block Number of Packet.
    BlockNumber blockNumberV;
    //! Data of Packet.
    Data dataV;
};

}

#endif
