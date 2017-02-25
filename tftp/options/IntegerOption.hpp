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
 * @brief Declaration/ definition of template class
 *   Tftp::Options::BaserIntegerOption and Tftp::Options::IntegerOption.
 *
 * Additional named types are generated for the well-known TFTP options.
 **/

#ifndef TFTP_OPTIONS_INTEGEROPTION_HPP
#define TFTP_OPTIONS_INTEGEROPTION_HPP

#include <tftp/options/Option.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/optional.hpp>

#include <stdexcept>

namespace Tftp {
namespace Options {

/**
 * @brief Base Integer Option template.
 *
 * Converts the option string to an integer and delegates the option negotiation
 * to child classes.
 *
 * @tparam IntT
 *   The integer type.
 *
 * @note
 * This template also provides specialisation for uint8_t (unsigned char) and
 * int8_t (char).
 * If not, this would fail because C++ internally handles strings as this type.
 **/
template< typename IntT>
class BaseIntegerOption: public Option
{
  public:
    // static check for integral type
    static_assert( std::is_integral< IntT>::value, "IntT must be integral type");

    //! The used integer type
    using IntegerType = IntT;

    /**
     * Converts value to string and returns the result.
     **/
    virtual operator string() const final;

    /**
     * @brief Returns the option value.
     *
     * @return The option value.
     **/
    operator IntegerType() const;

    /**
     * @brief Sets the value of the option.
     *
     * @param[in] value
     *   Option value
     **/
    BaseIntegerOption& operator=( IntegerType value);

    /**
     * @brief Interprets the string as option value and sets them to it.
     *
     * @param[in] value
     *   Value string
     **/
    BaseIntegerOption& operator=( const string &value);

    /**
     * @copybrief Option::negotiate()
     *
     * Calls #negotiate(IntegerType) const for concrete option
     * negotiation.
     *
     * @param[in] optionValue
     *   The option value received from the client.
     *
     * @return The negotiated Option.
     * @retval OptionPointer()
     *   If option negotiation failed.
     **/
    virtual OptionPtr negotiate( const string &optionValue) const noexcept final;

  protected:
    /**
     * @brief Generates a option with the given parameters.
     *
     * @param[in] name
     *   Option name
     * @param[in] value
     *   Current value.
     *   Should be compliant to option negotiation ranges.
     **/
    BaseIntegerOption( const string &name, IntegerType value);

    /**
     * @copybrief Option::negotiate()
     *
     * @param[in] optionValue
     *   The value to negotiate.
     *
     * @return The negotiated option
     * @retval OptionPtr()
     *   If negotiation failed for this option.
     **/
    virtual OptionPtr negotiate(
      IntegerType optionValue) const noexcept = 0;

  private:
    /**
     * @brief Converts the given value to a string.
     *
     * @param[in] value
     *   The value.
     *
     * @return The value converted to a string.
     **/
    static string toString( IntegerType value);

    /**
     * @brief Converts the given string to an integer.
     *
     * @param[in] value
     *   Value as string
     *
     * @return The converted value
     **/
    static IntegerType toInt( const string &value);

    //! The value
    IntegerType value;
};

template< typename IntT>
BaseIntegerOption< IntT>::operator string() const
{
  return toString( value);
}

template< typename IntT>
BaseIntegerOption< IntT>::operator IntegerType() const
{
  return value;
}

template< typename IntT>
BaseIntegerOption< IntT>& BaseIntegerOption< IntT>::operator=( const IntegerType value)
{
  this->value = value;
  return *this;
}

template< typename IntT>
BaseIntegerOption< IntT>& BaseIntegerOption< IntT>::operator=( const string &value)
{
  setValue( toInt( value));
  return *this;
}

template< typename IntT>
BaseIntegerOption< IntT>::BaseIntegerOption(
  const string &name,
  const IntegerType value):
  Option( name),
  value( value)
{
}

template< typename IntT>
OptionPtr BaseIntegerOption< IntT>::negotiate(
  const string &optionValue) const noexcept
{
  try
  {
    return negotiate( toInt( optionValue));
  }
  catch (...)
  {
    // error during integer conversion -> handle this as negotiation fail.
    return OptionPtr();
  }
}

template< typename IntT>
typename BaseIntegerOption< IntT>::string BaseIntegerOption< IntT>::toString(
  const IntegerType value)
{
  return boost::lexical_cast< string>( value);
}

template<>
inline typename BaseIntegerOption< uint8_t>::string
BaseIntegerOption< uint8_t>::toString(
  const IntegerType value)
{
  return boost::lexical_cast< string>( (uint16_t)value);
}

template< typename IntT>
typename BaseIntegerOption< IntT>::IntegerType BaseIntegerOption< IntT>::toInt(
  const string &value)
{
  return boost::lexical_cast< IntT>( value);
}

template<>
inline typename BaseIntegerOption< uint8_t>::IntegerType
BaseIntegerOption< uint8_t>::toInt( const string &value)
{
  return (uint8_t)boost::lexical_cast< uint16_t>( value);
}


/**
 * @brief TFTP option, which is interpreted as integer.
 **/
template< typename IntT, typename NegotiateT>
class IntegerOption: public BaseIntegerOption< IntT>
{
  public:
    using NegotiateType = NegotiateT;

    using typename BaseIntegerOption< IntT>::IntegerType;
    using typename BaseIntegerOption< IntT>::string;

    //! Optional integer value
    using OptionalIntegerType = boost::optional< IntT>;

    /**
     * @brief Generates a option with the given parameters.
     *
     * @param[in] name
     *   Option name
     * @param[in] value
     *   Current value
     * @param[in] negotiateOperation
     *   The negotiation operation to use.
     **/
    IntegerOption(
      const typename BaseIntegerOption< IntT>::string &name,
      typename BaseIntegerOption< IntT>::IntegerType value,
      NegotiateType negotiateOperation = NegotiateType());

    //! Default destructor
    virtual ~IntegerOption() noexcept = default;

    /**
     * @brief Sets the value of the option.
     *
     * @param[in] value
     *   Option value
     **/
    IntegerOption& operator=( IntegerType value);

    /**
     * @brief Interprets the string as option value and sets them to it.
     *
     * @param[in] value
     *   Value string
     **/
    IntegerOption& operator=( const string &value);

    using BaseIntegerOption< IntT>::negotiate;

    /**
     * @copybrief Option::negotiate()
     *
     * @param[in] optionValue
     *   The option value received from the client.
     *
     * @return The negotiated Option.
     * @retval OptionPointer()
     *   If option negotiation failed.
     **/
    virtual OptionPtr negotiate( IntegerType optionValue) const noexcept final;

  private:
    NegotiateType negotiateOperation;
};

template< typename IntT, typename NegotiateT>
IntegerOption< IntT, NegotiateT>::IntegerOption(
  const typename BaseIntegerOption< IntT>::string &name,
  const typename BaseIntegerOption< IntT>::IntegerType value,
  NegotiateType negotiateOperation):
  BaseIntegerOption< IntT>( name, value),
  negotiateOperation( negotiateOperation)
{
}

template< typename IntT, typename NegotiateT>
IntegerOption< IntT, NegotiateT>& IntegerOption< IntT, NegotiateT>::operator=( IntegerType value)
{
  BaseIntegerOption< IntT>::operator=( value);
  return *this;
}

template< typename IntT, typename NegotiateT>
IntegerOption< IntT, NegotiateT>& IntegerOption< IntT, NegotiateT>::operator=(
  const string &value)
{
  BaseIntegerOption< IntT>::operator=( value);
  return *this;
}

template< typename IntT, typename NegotiateT>
OptionPtr IntegerOption< IntT, NegotiateT>::negotiate(
  const IntegerType optionValue) const noexcept
{
  // negotiate the value
  const OptionalIntegerType negotiateValue(
    negotiateOperation( optionValue));

  // If value is not present -> option negotiation fails
  if (!negotiateValue)
  {
    return OptionPtr();
  }

  // return the negotiated option
  return std::make_shared< IntegerOption< IntT, NegotiateT>>(
    BaseIntegerOption< IntT>::getName(),
    *negotiateValue,
    negotiateOperation);
}

/**
 * @brief Negotiation Handler which checks the value against a range and performs
 *   upper-cut down.
 *
 * If the negotiation value is bigger then max, max is returned.
 * If the negotiation value is in range the value itself is returned.
 * Otherwise the negotiation fails.
 **/
template< typename IntT>
class NegotiateMinMaxSmaller
{
  public:
    NegotiateMinMaxSmaller( const IntT minValue, const IntT maxValue):
      minValue( minValue),
      maxValue( maxValue)
    {
    }

    boost::optional< IntT> operator()( const IntT value) const
    {
      // If value is smaller then min -> option negotiation fails
      if (value < minValue)
      {
        return boost::optional< IntT>();
      }

      // if value is bigger than maximum-> cut down to max.
      if (value > maxValue)
      {
        return maxValue;
      }

      return value;
    }

  private:
    const IntT minValue;
    const IntT maxValue;
};

/**
 * @brief Negotiation Handler which checks the value against a range.
 *
 * If the negotiation value is in range the value itself is returned.
 * Otherwise the negotiation fails.
 **/
template< typename IntT>
class NegotiateMinMaxRange
{
  public:
    NegotiateMinMaxRange( const IntT minValue, const IntT maxValue):
      minValue( minValue),
      maxValue( maxValue)
    {
    }

    boost::optional< IntT> operator()( const IntT value) const
    {
      // If value is out of range -> option negotiation fails
      if ((value < minValue) || (value > maxValue))
      {
        return boost::optional< IntT>();
      }

      return value;
    }

  private:
    const IntT minValue;
    const IntT maxValue;
};

/**
 * @brief Negotiation Handler which checks the value against a expected value.
 *
 * If the negotiation value is not the expected value the negotiation fails.
 **/
template< typename IntT>
class NegotiateExactValue
{
  public:
    NegotiateExactValue( const IntT expectedValue):
      expectedValue( expectedValue)
    {
    }

    boost::optional< IntT> operator()( const IntT value) const
    {
      if (expectedValue != value)
      {
        return boost::optional< IntT>();
      }

      return value;
    }

  private:
    //! the expected value
    const IntT expectedValue;
};

/**
 * @brief Negotiation Handler which does nothing.
 *
 * The value itself is returned always.
 **/
template< typename IntT>
class NegotiateAlwaysPass
{
  public:
    NegotiateAlwaysPass() = default;

    boost::optional< IntT> operator()( const IntT value) const
    {
      return value;
    }
};


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

//! Timeout option on server side - Negotiates exact value.
using TimeoutOptionClient =
  IntegerOption< uint8_t, NegotiateExactValue< uint8_t>>;

//! Transfer size option base class
using TransferSizeOptionBase = BaseIntegerOption< uint64_t>;

//! Timeout option on client and server side - accepts every value.
using TransferSizeOptionServerClient =
  IntegerOption< uint64_t, NegotiateAlwaysPass< uint64_t>>;

}
}


#endif
