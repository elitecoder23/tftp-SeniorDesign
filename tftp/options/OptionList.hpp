/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Options::OptionList.
 **/

#ifndef TFTP_OPTIONS_OPTIONLIST_HPP
#define TFTP_OPTIONS_OPTIONLIST_HPP

#include <tftp/options/Options.hpp>
#include <tftp/options/Option.hpp>

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <limits>
#include <memory>
#include <cstdint>

namespace Tftp {
namespace Options {

/**
 * @brief Representation of TFTP option list.
 *
 * A TFTP option is a name - value pair.
 *
 * The option list also contains handler of common TFTP options like:
 * @li blocksize option (RFC 2348)
 * @li timeout option (RFC 2349)
 * @li transfer size option (RFC 2349)
 **/
class OptionList
{
  public:
    //! short form of option map
    using Options = std::map< std::string, OptionPtr>;
    //! short form of raw option
    using RawOptions = std::vector< uint8_t>;

    /**
     * @brief Creates an empty option list
     **/
    OptionList();

    /**
     * @brief Loads the option list from the given raw data.
     *
     * @param[in] begin
     *   Begin of raw option list data
     * @param[in] end
     *   End of raw option list data
     **/
    OptionList(
      RawOptions::const_iterator begin,
      RawOptions::const_iterator end);

    /**
     * @brief Returns, if any option is set.
     *
     * @return is any option is set.
     **/
    bool empty() const;

    /**
     * @brief Returns the option map - non modifiable.
     *
     * @return Returns a constant reference to the options map.
     **/
    const Options& options() const;

    /**
     * @brief Returns the option map.
     *
     * @return Returns a reference to the options map.
     **/
    Options& options();

    /**
     * @brief Replaces the own options by the given one.
     *
     * @param[in] options
     *   The new options.
     **/
    void options( const Options &options);

    /**
     * @brief Returns the option list as raw data
     *
     * The raw option date is used to generate the option data within the
     * TFTP packages.
     *
     * @return The option list as raw data
     **/
    RawOptions rawOptions() const;

    /**
     * @brief Return if the specified option is set within the option list.
     *
     * @param[in] name
     *   The name of the option
     *
     * @return If the option is set.
     * @retval false
     *   The option is not set.
     * @retval true
     *   The option is set.
     **/
    bool has( const std::string &name) const;

    /**
     * @brief Return if the specified option is set within the option list.
     *
     * @param[in] option
     *   The TFTP option
     *
     * @return If the option is set.
     * @retval false
     *   The option is not set.
     * @retval true
     *   The option is set.
     **/
    bool has( KnownOptions option) const;

    /**
     * @brief Obtain for option with the given name
     *
     * @param[in] name
     *   The name of the option
     *
     * @return The value of the option.
     *   If the option is not set, an empty OptionPointer is returned.
     **/
    const OptionPtr get( const std::string &name) const;

    /**
     * @brief Sets the given option to the given value.
     *
     * If the option is set in the current option list, it is removed
     * firstly.
     *
     * @param[in] name
     *   The name of the option.
     * @param[in] value
     *   The option value.
     */
    void set( const std::string &name, const std::string &value);

    /**
     * @brief Set the given option
     *
     * If the option is set in the current option list, it is removed
     * firstly.
     *
     * @param[in] option
     *   The option
     **/
    void set( const OptionPtr option);

    /**
     * @brief Remove the option with the given name from the option list.
     *
     * @param[in] name
     *   The name of the option.
     **/
    void remove( const std::string &name);

    /**
     * @brief Remove the given option from the option list.
     *
     * @param[in] option
     *   The option
     **/
    void remove( KnownOptions option);

    /**
     * @brief Adds the Blocksize option to the option list.
     *
     * This operation is used on client side to request a specific block size.
     * Additionally the client defines the minimal allowed reduced blocksize.
     *
     * @param[in] requestedBlocksize
     *   The requested blocksize option value.
     * @param[in] minBlocksize
     *   The minimal accepted blocksize option value.
     **/
    void addBlocksizeOptionClient(
      uint16_t requestedBlocksize,
      uint16_t minBlocksize = BlocksizeOptionMin);


    /**
     * @brief Adds the Blocksize option to the option list.
     *
     * This version allows to modify the min and max values, which are used
     * to perform the option negotiation.
     *
     * The current value (used by the client as offer to the server) is set
     * to maxBlocksize.
     *
     * @param[in] minBlocksize
     *   The minimum blocksize option value.
     * @param[in] maxBlocksize
     *    The minimum blocksize option value.
     **/
    void addBlocksizeOptionServer(
      uint16_t minBlocksize = BlocksizeOptionMin,
      uint16_t maxBlocksize = BlocksizeOptionMax);

    /**
     * @brief Returns the set blocksize option value.
     *
     * @return The set blocksize option value.
     * @retval 0
     *   If blocksize option has not been added to this option list.
     **/
    uint16_t getBlocksizeOption() const;

    /**
     * @brief Adds the timeout option to the option list.
     *
     * This operation is used to set the intended timeout value on client side.
     *
     * @param[in] timeout
     *   The requested timeout.
     **/
    void addTimeoutOptionClient( uint8_t timeout);

    /**
     * @brief Adds the timeout option to the option list.
     *
     * The specified timeout value must match the value specified by the
     * client.
     *
     * This operation is used on server side to set the acceptable range
     * of the TFTP server.
     *
     * @param[in] minTimeout
     *   The minimum acceptable timeout.
     * @param[in] maxTimeout
     *   The maximum acceptable timeout
     **/
    void addTimeoutOptionServer(
      uint8_t minTimeout = TimeoutOptionMin,
      uint8_t maxTimeout = TimeoutOptionMax);

    /**
     * @brief Returns the value of the timeout option.
     *
     * @return The timeout option value.
     * @retval 0
     *   If timeout option has not been added to this option list.
     **/
    uint8_t getTimeoutOption() const;

    /**
     * @brief Add the transfer size option with the given transfer size.
     *
     * In Read Request packets, a size of "0" is specified in the request
     * and the size of the file, in octets, is returned in the OACK.
     * If the file is too large for the client to handle, it may abort the
     * transfer with an Error packet (error code 3).
     * In Write Request packets, the size of the file, in octets, is specified
     * in the request and echoed back in the OACK.
     * If the file is too large for the server to handle, it may abort the
     * transfer with an Error packet (error code 3).
     *
     * @param[in] transferSize
     *   The transfer size option value.
     **/
    void addTransferSizeOption( uint64_t transferSize = 0ULL);

    /**
     * @brief Removes the transfer size option.
     **/
    void removeTransferSizeOption();

    /**
     * @brief Checks, if the transfer size option has been set.
     *
     * @return If the transfer size option has been set.
     **/
    bool hasTransferSizeOption() const;

    /**
     * @brief Returns the value of the transfer size option.
     *
     * When the transfer size option is not set, 0 is returned.
     * Due to the fact, that 0 is a valid transfer size option value, this value
     * does not mean that the option is set either.
     *
     * To check that the option is set, call hasTransferSizeOptionOption().
     *
     * @return The transfer size option value.
     **/
    uint64_t getTransferSizeOption() const;

    /**
     * @brief Performs the option negotiation on server side.
     *
     * If an option is not acceptable or is unknown, this option is removed
     * from the resulting option list.
     *
     * @param[in] clientOptions
     *   The received options from client side
     *
     * @return The negotiated options, which shall be send back to the
     *   client.
     * @retval OptionList()
     *   When no option can be fulfilled, an empty option list is returned.
     *   The server should not send an OACK with an empty option list
     **/
    OptionList negotiateServer( const OptionList &clientOptions) const;

    /**
     * @brief Performs the option negotiation on client side.
     *
     * If the returned option list is empty, the option negotiation has
     * failed and the transfer should be aborted.
     *
     * The option list, received from the server, must not be empty to make
     * this operation work correctly.
     *
     * @param[in] serverOptions
     *   The received options from server side.
     *
     * @return The negotiated options.
     * @retval OptionList()
     *   If an unexpected option has been received or the returned option
     *   is unacceptable.
     **/
    OptionList negotiateClient( const OptionList &serverOptions) const;

    /**
     * @brief Returns a string, which describes the option list.
     *
     * This operation is used for debugging and information purposes.
     *
     * @return Option list description.
     **/
    std::string toString() const;

  private:
    //! the options.
    Options optionsValue;
};

}
}

#endif
