/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Class Tftp::Options::OptionNegotiation.
 *
 * Additional named types are generated for the well-known TFTP options.
 **/

#ifndef TFTP_OPTIONS_OPTIONNEGOTIATION_HPP
#define TFTP_OPTIONS_OPTIONNEGOTIATION_HPP

#include <tftp/options/Options.hpp>

#include <helper/SafeCast.hpp>

#include <boost/lexical_cast.hpp>

#include <optional>
#include <string_view>
#include <stdexcept>

namespace Tftp::Options {

//! Base Interface for Option Negotiation based on uint64_t values.
class OptionNegotiation
{
  public:
    /**
      * @brief Converts the given value to a string.
      *
      * @param[in] value
      *   The value.
      *
      * @return The value converted to a string.
      **/
    static std::string toString( uint64_t value);

    /**
     * @brief Converts the given string to an integer.
     *
     * @param[in] value
     *   Value as string
     *
     * @return The converted value
     **/
    static uint64_t toInt( std::string_view value);

    /**
     * @brief Negotiates the given option value.
     *
     * Converts @p optionValue to uint64_t and calls negotiateInt().
     *
     * @param[in] optionValue
     *   Option Value to negotiate.
     *
     * @return The negotiated value.
     * @retval std::string{}
     *   If Negotiation fails.
     **/
    [[nodiscard]] std::string negotiate( std::string_view optionValue) const;

    /**
     * @brief Negotiates the given option value.
     *
    * @param[in] optionValue
     *   Option Value to negotiate.
     *
     * @return The negotiated value.
     * @retval std::string{}
     *   If Negotiation fails.
     **/
    [[nodiscard]] virtual std::string negotiateInt( uint64_t optionValue) const = 0;
};


inline std::string OptionNegotiation::toString( const uint64_t value)
{
  return std::to_string( value);
}

inline uint64_t OptionNegotiation::toInt( std::string_view value)
{
  return boost::lexical_cast< uint64_t>( value);
}

inline std::string OptionNegotiation::negotiate( std::string_view optionValue) const
{
  return negotiateInt( toInt( optionValue));
}

/**
 * @brief Negotiation Handler which checks the value against a range and
 *   performs upper-cut down.
 *
 * If the negotiation value is bigger then max, max is returned.
 * If the negotiation value is in range the value itself is returned.
 * Otherwise the negotiation fails.
 **/
class NegotiateMinMaxSmaller : public OptionNegotiation
{
  public:
    /**
     * @brief Initialises the negotiation instance.
     *
     * @param[in] minValue
     *   The minimal expected value
     * @param[in] maxValue
     *   The maximal expected value
     **/
    NegotiateMinMaxSmaller( const uint64_t minValue, const uint64_t maxValue):
      minValue{ minValue},
      maxValue{ maxValue}
    {
    }

    /**
     * @brief Negotiates the passed in value.
     *
     * @param[in] value
     *   The value to negotiate.
     *
     * @return the negotiated value.
     * @retval std::optional< IntT>()
     *   if [value] < [minValue]
     * @retval [value]
     *   if [minValue] <= [value] <= [maxValue]
     * @retval [maxValue]
     *   otherwise.
     **/
    [[nodiscard]] std::string negotiateInt( const uint64_t value) const final
    {
      // If value is smaller then min -> option negotiation fails
      if (value < minValue)
      {
        return {};
      }

      // if value is bigger than maximum-> cut down to max.
      if (value > maxValue)
      {
        return toString( maxValue);
      }

      return toString( value);
    }

  private:
    //! Allowed minimum value
    const uint64_t minValue;
    //! Allowed maximum value
    const uint64_t maxValue;
};

/**
 * @brief Negotiation Handler which checks the value against a range.
 *
 * If the negotiation value is in range the value itself is returned.
 * Otherwise the negotiation fails.
 **/
class NegotiateMinMaxRange : public OptionNegotiation
{
  public:
    /**
     * @brief Initialises the negotiation instance.
     *
     * @param[in] minValue
     *   The minimal expected value
     * @param[in] maxValue
     *   The maximal expected value
     **/
    NegotiateMinMaxRange( const uint64_t minValue, const uint64_t maxValue):
      minValue{ minValue},
      maxValue{ maxValue}
    {
    }

    /**
     * @brief Negotiates the passed in value.
     *
     * @param[in] value
     *   The value to negotiate.
     *
     * @return [value] if [minValue] >= [value] <= [maxValue], otherwise fail
     *   negotiation.
     **/
    [[nodiscard]] std::string negotiateInt( const uint64_t value) const final
    {
      // If value is out of range -> option negotiation fails
      if ((value < minValue) || (value > maxValue))
      {
        return {};
      }

      return toString( value);
    }

  private:
    //! Allowed minimum value
    const uint64_t minValue;
    //! Allowed maximum value
    const uint64_t maxValue;
};

/**
 * @brief Negotiation Handler which checks the value against a expected value.
 *
 * If the negotiation value is not the expected value the negotiation fails.
 **/
class NegotiateExactValue : public OptionNegotiation
{
  public:
    /**
     * @brief Initialises the negotiation instance.
     *
     * @param[in] expectedValue
     *   Expected value
     **/
    explicit NegotiateExactValue( const uint64_t expectedValue):
      expectedValue{ expectedValue}
    {
    }

    /**
     * @brief Negotiates the passed in value.
     *
     * @param[in] value
     *   The value to negotiate.
     *
     * @return [value] if [value] == [expectedValue] otherwise fail negotiation.
     **/
    [[nodiscard]] std::string negotiateInt( const uint64_t value) const final
    {
      if (expectedValue != value)
      {
        return {};
      }

      return toString( value);
    }

  private:
    //! Expected Value
    const uint64_t expectedValue;
};

/**
 * @brief Negotiation Handler which does nothing.
 *
 * The value itself is returned always.
 **/
class NegotiateAlwaysPass : public OptionNegotiation
{
  public:
    //! Constructor
    NegotiateAlwaysPass() = default;

    /**
     * @brief Negotiates the passed in value.
     *
     * @param[in] value
     *   The value to negotiate.
     *
     * @return @p value
     **/
    [[nodiscard]] std::string negotiateInt( const uint64_t value) const final
    {
      return toString( value);
    }
};

#if 0

//! Blocksize Option base class
using BlockSizeOptionBase = BaseIntegerOption< uint16_t>;

//! Blocksize option on server side - Negotiates in range with cut down to max
using BlockSizeOptionServer =
  IntegerOption< uint16_t, NegotiateMinMaxSmaller< uint16_t>>;

//! Blocksize option on client side - Negotiates in range.
using BlockSizeOptionClient =
  IntegerOption< uint16_t, NegotiateMinMaxRange< uint16_t>>;

//! Timeout Option base class
using TimeoutOptionBase = BaseIntegerOption< uint8_t>;

//! Timeout option on server side - Negotiates in range.
using TimeoutOptionServer =
  IntegerOption< uint8_t, NegotiateMinMaxRange< uint8_t>>;

//! Timeout option on client side - Negotiates exact value.
using TimeoutOptionClient =
  IntegerOption< uint8_t, NegotiateExactValue< uint8_t>>;

//! Transfer size option base class
using TransferSizeOptionBase = BaseIntegerOption< uint64_t>;

//! Transfer size option on client and server side - accepts every value.
using TransferSizeOptionServerClient =
  IntegerOption< uint64_t, NegotiateAlwaysPass< uint64_t>>;
#endif

}

#endif
