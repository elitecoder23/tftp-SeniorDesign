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
 * @brief Declaration/ definition of template class Tftp::Options::IntegerOption.
 **/

#ifndef TFTP_OPTIONS_INTEGEROPTION_HPP
#define TFTP_OPTIONS_INTEGEROPTION_HPP

#include <tftp/options/Option.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/optional.hpp>

#include <stdexcept>

namespace Tftp {
namespace Options {

template< typename IntT>
class BaseIntegerOption: public Option
{
  public:
    static_assert( std::is_integral< IntT>::value, "IntT must be integral type");

    //! The used integer type
    using IntegerType = IntT;

    /**
     * @brief returns the current value.
     *
     * @return The current option value
     **/
    IntegerType getValue() const;

    /**
     * @brief Returns the option value as string.
     *
     * @return The option value as string.
     **/
    virtual string getValueString() const override;

    /**
     * @brief Sets the value of the option.
     *
     * @param[in] value
     *   Option value
     **/
    void setValue( IntegerType value);

    /**
     * @brief Interprets the string as option value and sets them to it.
     *
     * @param[in] value
     *   Value string
     **/
    void setValue( const string &value);

    /**
     * @copybrief Option::negotiateServer()
     *
     * @param[in] optionValue
     *   The option value received from the client.
     *
     * @return The negotiated Option.
     * @retval OptionPointer()
     *   If option negotiation failed.
     **/
    virtual OptionPtr negotiateServer(
      const string &optionValue) const override final;

    /**
     * @copybrief Option::negotiateClient()
     *
     * @param[in] optionValue
     *   The option value received from the server.
     *
     * @return The negotiated Option.
     * @retval OptionPointer()
     *   if option negotiation failed.
     **/
    virtual OptionPtr negotiateClient(
      const string &optionValue) const override final;

  protected:
    /**
     * @brief Generates a option with the given parameters.
     *
     * @param[in] name
     *   Option name
     * @param[in] minValue
     *   minimum allowed value
     * @param[in] maxValue
     *   maximum allowed value
     * @param[in] value
     *   Current value
     **/
    BaseIntegerOption(
      const string &name,
      IntegerType value);

    virtual OptionPtr negotiateServer(
      IntegerType optionValue) const = 0;

    virtual OptionPtr negotiateClient(
      IntegerType optionValue) const = 0;

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
IntT BaseIntegerOption< IntT>::getValue() const
{
  return value;
}

template< typename IntT>
typename BaseIntegerOption< IntT>::string
BaseIntegerOption< IntT>::getValueString() const
{
  return toString( value);
}

template< typename IntT>
void BaseIntegerOption< IntT>::setValue( const IntegerType value)
{
  this->value = value;
}

template< typename IntT>
void BaseIntegerOption< IntT>::setValue( const string &value)
{
  setValue( toInt( value));
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
OptionPtr BaseIntegerOption< IntT>::negotiateServer(
  const string &optionValue) const
{
  IntegerType value = toInt( optionValue);

  return negotiateServer( value);
}

template< typename IntT>
OptionPtr BaseIntegerOption< IntT>::negotiateClient(
  const string &optionValue) const
{
  IntegerType value = toInt( optionValue);

  return negotiateClient( value);
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
 *
 * @note
 * Do not specialise this class to uint8_t (unsigned char) or int8_t (char)!
 * This would fail because C++ internally handles strings as this type.
 **/
template< typename IntT, typename NegotiateServerT, typename NegotiateClientT>
class IntegerOption: public BaseIntegerOption< IntT>
{
  public:
    using NegotiateServerType = NegotiateServerT;
    using NegotiateClientType = NegotiateClientT;

    /**
     * @brief Generates a option with the given parameters.
     *
     * @param[in] name
     *   Option name
     * @param[in] minValue
     *   minimum allowed value
     * @param[in] maxValue
     *   maximum allowed value
     * @param[in] value
     *   Current value
     **/
    IntegerOption(
      const typename BaseIntegerOption< IntT>::string &name,
      typename BaseIntegerOption< IntT>::IntegerType value,
      NegotiateServerType negotiateServer = NegotiateServerType(),
      NegotiateClientType negotiateClient = NegotiateClientType());

    using BaseIntegerOption< IntT>::negotiateServer;
    using BaseIntegerOption< IntT>::negotiateClient;

    /**
     * @copybrief Option::negotiateServer()
     *
     * @param[in] optionValue
     *   The option value received from the client.
     *
     * @return The negotiated Option.
     * @retval OptionPointer()
     *   If option negotiation failed.
     **/
    virtual OptionPtr negotiateServer(
      typename BaseIntegerOption< IntT>::IntegerType optionValue) const override final;

    /**
     * @copybrief Option::negotiateClient()
     *
     * @param[in] optionValue
     *   The option value received from the server.
     *
     * @return The negotiated Option.
     * @retval OptionPointer()
     *   if option negotiation failed.
     **/
    virtual OptionPtr negotiateClient(
      typename BaseIntegerOption< IntT>::IntegerType optionValue) const override final;

  private:
    NegotiateServerType negServer;
    NegotiateClientType negClient;
};

template< typename IntT, typename NegotiateServerT, typename NegotiateClientT>
IntegerOption< IntT, NegotiateServerT, NegotiateClientT>::IntegerOption(
  const typename BaseIntegerOption< IntT>::string &name,
  const typename BaseIntegerOption< IntT>::IntegerType value,
  NegotiateServerType negotiateServer,
  NegotiateClientType negotiateClient):
  BaseIntegerOption< IntT>( name, value),
  negServer( negotiateServer),
  negClient( negotiateClient)
{
}

template< typename IntT, typename NegotiateServerT, typename NegotiateClientT>
OptionPtr IntegerOption< IntT, NegotiateServerT, NegotiateClientT>::negotiateServer(
  typename BaseIntegerOption< IntT>::IntegerType optionValue) const
{
  const boost::optional< IntT> negotiateValue( negServer( optionValue));

  // If value is smaller then min -> option negotiation fails
  if (!negotiateValue)
  {
    return OptionPtr();
  }

  return std::make_shared< IntegerOption< IntT, NegotiateServerT, NegotiateClientT> >(
    BaseIntegerOption< IntT>::getName(),
    *negotiateValue,
    negServer,
    negClient);
}

template< typename IntT, typename NegotiateServerT, typename NegotiateClientT>
OptionPtr IntegerOption< IntT, NegotiateServerT, NegotiateClientT>::negotiateClient(
  typename BaseIntegerOption< IntT>::IntegerType optionValue) const
{
  const boost::optional< IntT> negotiateValue( negClient( optionValue));

  // If value is smaller then min -> option negotiation fails
  if (!negotiateValue)
  {
    return OptionPtr();
  }

  return std::make_shared< IntegerOption< IntT, NegotiateServerT, NegotiateClientT> >(
    BaseIntegerOption< IntT>::getName(),
    *negotiateValue,
    negServer,
    negClient);
}

template< typename IntT>
class NegotiateMinMaxSmaller
{
  public:
    NegotiateMinMaxSmaller( IntT min, IntT max):
      min( min),
      max( max)
    {
    }

    boost::optional< IntT> operator()( IntT value) const
    {
      // If value is smaller then min -> option negotiation fails
      if (value < min)
      {
        return boost::optional< IntT>();
      }

      if (value > max)
      {
        return max;
      }

      return value;
    }

  private:
    const IntT min;
    const IntT max;
};

template< typename IntT>
class NegotiateMinMaxRange
{
  public:
    NegotiateMinMaxRange( IntT min, IntT max):
      min( min),
      max( max)
    {
    }

    boost::optional< IntT> operator()( IntT value) const
    {
      // If value is out of range -> option negotiation fails
      if ((value < min) || (value > max))
      {
        return boost::optional< IntT>();
      }

      return value;
    }

  private:
    const IntT min;
    const IntT max;
};

template< typename IntT>
class NegotiateAlwaysPass
{
  public:
    NegotiateAlwaysPass() = default;

    boost::optional< IntT> operator()( IntT value) const
    {
      return value;
    }
};

template< typename IntT>
class NegotiateExactValue
{
  public:
    NegotiateExactValue( IntT value):
      value( value)
    {
    }

    boost::optional< IntT> operator()( IntT value) const
    {
      if (this->value != value)
      {
        boost::optional< IntT>();
      }

      return value;
    }

  private:
    const IntT value;
};

using BlockSizeOption =
  IntegerOption<
    uint16_t,
    NegotiateMinMaxSmaller< uint16_t>,
    NegotiateMinMaxRange< uint16_t>>;

using TimeoutOption =
  IntegerOption<
    uint8_t,
    NegotiateMinMaxRange< uint8_t>,
    NegotiateExactValue< uint8_t>>;

using TransferSizeOption =
  IntegerOption< uint64_t, NegotiateAlwaysPass< uint64_t>, NegotiateAlwaysPass< uint64_t>>;

}
}


#endif
