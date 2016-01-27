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
 * @brief Declaration of class Tftp::Packet::BlockNumber.
 **/

#ifndef TFTP_PACKET_BLOCKNUMBER_HPP
#define TFTP_PACKET_BLOCKNUMBER_HPP

#include <tftp/packet/Packet.hpp>
#include <cstdint>

namespace Tftp {
namespace Packet {

/**
 * @brief Represents a block number in TFTP Data and Acknowledgement packets.
 *
 * Block numbers are used within the TFTP Data and TFTP Acknowledgement
 * packet. A block number is an 16 Bit integer, which has a special meaning
 * of the 0-value. This 0-value handling is implemented within this class.
 *
 * @sa DataPacket
 * @sa AcknowledgementPacket
 **/
class BlockNumber
{
	public:
		/**
		 * @brief Generates a new block number.
		 *
		 * The block number is initialised to the given value.
		 *
		 * @param[in] blockNumber
		 *   Base block number.
		 **/
		BlockNumber( const uint16_t blockNumber = 0) noexcept;

		/**
		 * @brief Returns the next block number.
		 *
		 * @return The next block number.
		 **/
		uint16_t next( void) const;

		/**
		 * @brief Returns the previous block number.
		 *
		 * @return The next block number.
		 **/
		uint16_t previous( void) const;

		/**
		 * @brief Cast operator of the block number class to an uint16_t.
		 *
		 * @return The block number as uint16_t.
		 **/
		operator uint16_t( void) const;

		/**
		 * @brief Cast operator of the block number class to an uint16_t&.
		 *
		 * @return The block number as uint16_t&.
		 **/
		operator uint16_t&( void);

		/**
		 * @brief Assignment operator to the block number.
		 *
		 * @param[in] blockNumber
		 *   New block number value.
		 *
		 * @return The class itself.
		 **/
		BlockNumber& operator=( const uint16_t blockNumber);

		/**
		 * @brief Increment of the block number.
		 *
		 * @return The class itself
		 **/
		BlockNumber& operator++( void);

		/**
		 * @brief Increment of the block number.
		 *
		 * @return The class itself
		 **/

		BlockNumber operator++( int);

		/**
		 * @brief Decrement of the block number.
		 *
		 * @return The class itself
		 **/
		BlockNumber& operator--( void);

		/**
		 * @brief Decrement of the block number.
		 *
		 * @return The class itself.
		 **/
		BlockNumber operator--( int);

		/**
		 * @brief Comparison operator of block number class.
		 *
		 * @param[in] rhs
		 *   The block number to compare to.
		 *
		 * @return Equality of the block numbers.
		 **/
		bool operator ==( const BlockNumber &rhs) const;

		/**
		 * @brief Comparison operator of block number class.
		 *
		 * @param[in] rhs
		 *   The block number to compare to.
		 *
		 * @return Equality of the block numbers.
		 **/
		bool operator ==( const uint16_t &rhs) const;

		/**
		 * @brief Comparison operator of block number class.
		 *
		 * @param[in] rhs
		 *   The block number to compare to.
		 *
		 * @return In-Equality of the block numbers.
		 **/
		bool operator !=( const BlockNumber &rhs) const;

		/**
		 * @brief Comparison operator of block number class.
		 *
		 * @param[in] rhs
		 *   The block number to compare to.
		 *
		 * @return In-Equality of the block numbers.
		 **/
		bool operator !=( const uint16_t &rhs) const;

	private:
		//! The block number value
		uint16_t blockNumber;
};

}
}

#endif
