/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Packets::ReadWriteRequestPacket.
 **/

#ifndef TFTP_PACKETS_READWRITEREQUESTPACKET_HPP
#define TFTP_PACKETS_READWRITEREQUESTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>
#include <tftp/options/OptionList.hpp>

#include <map>
#include <string>

namespace Tftp {
namespace Packets {

/**
 * @brief Base class for TFTP Read-/ Write- Request packet.
 *
 * | RRQ/WRQ | FName |  0  | Mode |  0  | Opt 1 |  0  | OptV 1 |  0  | ... | Opt N |  0  | OptV N |  0  |
 * |:-------:|:-----:|:---:|:----:|:---:|:-----:|:---:|:------:|:---:|:---:|:-----:|:---:|:------:|:---:|
 * |   2 B   |  str  | 1 B | str  | 1 B |  str  | 1 B |   str  | 1 B |     |  str  | 1 B |  str   | 1 B |
 **/
class ReadWriteRequestPacket: public Packet
{
  public:
    /**
     * @brief Converts the mode enumeration to the corresponding packet
     *   string.
     *
     * @param[in] mode
     *   The transfer mode.
     *
     * @return The corresponding string representation.
     *
     * @throw InvalidPacketException
     *   When mode is not a valid transfer mode.
     **/
    static string getMode( TransferMode mode);

    /**
     * @brief Converts the mode string to the corresponding enumeration.
     *
     * @param[in] mode
     *   The transfer mode.
     *
     * @return The corresponding mode.
     *
     * @throw InvalidPacketException
     *   When mode is not a valid transfer mode.
     **/
    static TransferMode getMode( const string &mode);

    //! @copydoc Packet::operator=(const RawTftpPacket&)
    ReadWriteRequestPacket& operator=( const RawTftpPacket &rawPacket) final;

    /**
     * @brief Returns the request filename.
     *
     * @return The filename
     **/
    const string& getFilename() const;

    /**
     * @brief Sets the filename
     *
     * @param[in] filename
     *   The new filename.
     **/
    void setFilename( const string &filename);

    /**
     * @brief Returns the transfer mode.
     *
     * @return The transfer mode.
     **/
    TransferMode getMode() const;

    /**
     * @brief Sets the transfer mode.
     *
     * @param[in] mode
     *   The new transfer mode.
     *
     * @throw TftpPacketException
     *   When mode is not a valid transfer mode.
     **/
    void setMode( TransferMode mode);

    /**
     * @brief Sets the transfer mode.
     *
     * @param[in] mode
     *   The new transfer mode.
     **/
    void setMode( const string &mode);

    /**
     * @brief Returns the set TFTP options.
     *
     * @return The TFTP options.
     **/
    const Options::OptionList& getOptions() const;

    /**
     * @brief Returns the set TFTP options.
     *
     * @return The TFTP options.
     **/
    Options::OptionList& getOptions();

    /**
     * @brief Sets the TFTP options.
     *
     * @param[in] options
     *   The TFTP options.
     **/
    void setOptions( const Options::OptionList &options);

    /**
     * @brief Returns the option value with the given name.
     *
     * @param[in] name
     *   The option name.
     *
     * @return The option value.
     * @retval std::string()
     *   When the given option is not set.
     **/
    const string getOption( const string &name) const;

    /**
     * @brief set a option
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
    operator string() const final;

  protected:
    /**
     * @brief Creates a read/ write request packet with the given data.
     *
     * @param[in] packetType
     *   The packet type of the packet. Valid are RRQ and WRQ
     * @param[in] filename
     *   The filename, which will be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] options
     *   The options, which are set
     *
     * @throw InvalidPacketException
     *   When packetType is not valid.
     **/
    ReadWriteRequestPacket(
      PacketType packetType,
      const string &filename,
      TransferMode mode,
      const Options::OptionList &options);

    /**
     * @brief Generates a TFTP Read/ Write Request packet from a data buffer
     *
     * @param[in] packetType
     *   The type of the packet. Only READ_REQUEST or
     *   WRITE_REQUEST is allowed.
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    ReadWriteRequestPacket(
      PacketType packetType,
      const RawTftpPacket &rawPacket);

  private:
    /**
     * @copydoc Packet::encode()
     **/
    RawTftpPacket encode() const final;

    /**
     * @brief Decodes the TFTP body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     **/
    void decodeBody( const RawTftpPacket &rawPacket);

    //! stored request filename
    string filename;
    //! stored transfer mode
    string mode;
    //! stored options
    Options::OptionList options;
};

}
}

#endif
