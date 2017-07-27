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
 * @brief Declaration of class Tftp::Packets::OptionsAcknowledgementPacket.
 **/

#ifndef TFTP_PACKETS_OPTIONSACKNOWLEDGEMENTPACKET_HPP
#define TFTP_PACKETS_OPTIONSACKNOWLEDGEMENTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>
#include <tftp/options/OptionList.hpp>

#include <map>

namespace Tftp {
namespace Packets {


/**
 * @brief TFTP Option Acknowledgement packet (ACK).
 *
 * This package is only sent from the TFTP server in response to an RRQ or
 * WRQ packet from the client, a TFTP option is transmitted within the request
 * packet and the server accepts at least one option.
 *
 * | OACK | Opt 1 |  0  | OptV 1 |  0  | ... | Opt N |  0  | OptV N |  0  |
 * |:----:|:-----:|:---:|:------:|:---:|:---:|:-----:|:---:|:------:|:---:|
 * | 2 B  |  str  | 1 B |  str   | 1 B |     |  str  | 1 B |  str   | 1 B |
 *
 **/
class OptionsAcknowledgementPacket: public Packet
{
  public:
    /**
     * @brief Initialises TFTP Options Acknowledgememnt packet with the given
     *   options list.
     *
     * @param[in] options
     *   TFTP Options list.
     **/
    OptionsAcknowledgementPacket( const Options::OptionList &options) noexcept;

    /**
     * @brief Generates a TFTP Options Acknowledgement packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    OptionsAcknowledgementPacket( const RawTftpPacketType &rawPacket);

    //! @copydoc Packet::operator=(const RawTftpPacketType&)
    virtual OptionsAcknowledgementPacket& operator=(
      const RawTftpPacketType &rawPacket) override;

    /**
     * @brief Returns the options within the packet (const reference)
     *
     * @return Option list (as const reference)
     **/
    const Options::OptionList& getOptions() const;

    /**
     * @brief Returns the options within the packet (as reference)
     *
     * @return Option list (as reference)
     **/
    Options::OptionList& getOptions();

    /**
     * @brief Overwrite the options of this packet with the new one.
     *
     * @param[in] options
     *   New option list, which shall be used.
     **/
    void setOptions( const Options::OptionList &options);

    /**
     * @brief Get a option value for a given name.
     *
     * If the option is not set, return an empty string.
     *
     * @param[in] name
     *   The option name.
     *
     * @return The value for the option identified by name.
     **/
    const string getOption( const string &name) const;

    /**
     * @brief Sets the option with the given name and value.
     *
     * @param[in] name
     *   The option name.
     * @param[in] value
     *   The option value.
     **/
    void setOption( const string &name, const string &value);

    /**
     * @copydoc Packet::operator string() const
     **/
    virtual operator string() const override;

  private:
    /**
     * @copydoc Packet::encode()
     **/
    virtual RawTftpPacketType encode() const override;

    void decodeBody( const RawTftpPacketType &rawPacket);

    //! The stored options.
    Options::OptionList options;
};

}
}

#endif
