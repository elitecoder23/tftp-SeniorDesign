/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::BlockNumber.
 **/

#ifndef TFTP_PACKETS_BLOCKNUMBER_HPP
#define TFTP_PACKETS_BLOCKNUMBER_HPP

#include <tftp/packets/Packets.hpp>
#include <cstdint>

namespace Tftp::Packets {

/**
 * @brief Block Number in TFTP Data and Acknowledgement %Packet.
 *
 * Block numbers are used within the TFTP Data and TFTP Acknowledgement
 * packet.
 * A block number is an 16 Bit integer, which has a special meaning of the
 * 0-value.
 * This 0-value handling is implemented within this class.
 *
 * @sa DataPacket
 * @sa AcknowledgementPacket
 **/
class BlockNumber
{
  public:
    //! Initialises a block number with value 0.
    BlockNumber() noexcept;

    /**
     * @brief Generates a new Block Number.
     *
     * The block number is initialised to the given value.
     *
     * @param[in] blockNumber
     *   Base block number.
     **/
    explicit BlockNumber( uint16_t blockNumber ) noexcept;

    //! Destructor
    ~BlockNumber() noexcept = default;

    /**
     * @brief Returns the next block number.
     *
     * @return Next block number.
     **/
    [[nodiscard]] uint16_t next() const;

    /**
     * @brief Cast operator of the block number class to an uint16_t.
     *
     * @return The block number as uint16_t.
     **/
    explicit operator uint16_t() const;

    /**
     * @brief Cast operator of the block number class to an uint16_t&.
     *
     * This operator allows direct manipulation of the underlying base type.
     *
     * @return The block number as uint16_t&.
     **/
    explicit operator uint16_t&();

    /**
     * @brief Assignment Operator to the Block Number.
     *
     * @param[in] blockNumber
     *   New block number value.
     *
     * @return The class itself.
     **/
    BlockNumber& operator=( uint16_t blockNumber );

    /**
     * @brief Pre-Increment of the block number.
     *
     * @return The class itself
     **/
    BlockNumber& operator++();

    /**
     * @brief Post-Increment of the block number.
     *
     * @return The class itself
     **/

    const BlockNumber operator++( int );

    /**
     * @brief Un-equality Comparison to Block Number.
     *
     * @param[in] rhs
     *   The block number to compare to.
     *
     * @return Equality of the block numbers.
     **/
    bool operator==( const BlockNumber &rhs ) const;

    /**
     * @brief Equality Comparison to uint16_t Block Number.
     *
     * @param[in] rhs
     *   The block number to compare to.
     *
     * @return Equality of the block numbers.
     **/
    bool operator==( uint16_t rhs ) const;

    /**
     * @brief Un-equality Comparison to Block Number.
     *
     * @param[in] rhs
     *   The block number to compare to.
     *
     * @return In-Equality of the block numbers.
     **/
    bool operator!=( const BlockNumber &rhs ) const;

    /**
     * @brief Un-equality Comparison to uint16_t Block Number.
     *
     * @param[in] rhs
     *   The block number to compare to.
     *
     * @return In-Equality of the block numbers.
     **/
    bool operator!=( uint16_t rhs ) const;

  private:
    //! Block Number Value
    uint16_t blockNumberV;
};

/**
 * @brief Equality Comparison of uint16_t Block Number and Block Number Class.
 *
 * @param[in] lhs
 *   Block Number (uint16_t representation).
 * @param[in] rhs
 *   Block Number.
 *
 * @return Equality of the block numbers.
 *
 * @return
 */
bool operator==( uint16_t lhs, const BlockNumber &rhs );

/**
 * @brief Un-equality Comparison of uint16_t Block Number and Block Number
 *   Class.
 *
 * @param[in] lhs
 *   Block Number (uint16_t representation).
 * @param[in] rhs
 *   Block Number.
 *
 * @return Un-Equality of the block numbers.
 **/
bool operator!=( uint16_t lhs, const BlockNumber &rhs );

}

#endif
