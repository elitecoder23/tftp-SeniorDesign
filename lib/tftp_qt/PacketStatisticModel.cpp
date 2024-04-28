// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class TftpQt::PacketStatisticModel.
 **/

#include "PacketStatisticModel.hpp"

#include "tftp/packets/PacketTypeDescription.hpp"

#include "helper_qt/String.hpp"

#include <utility>

namespace TftpQt {

PacketStatisticModel::PacketStatisticModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

int PacketStatisticModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( statisticV.size() );
}

int PacketStatisticModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant
PacketStatisticModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), statisticV.size() ) )
  {
    return {};
  }

  const auto packet{ std::next( statisticV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns >( index.column() ) )
      {
        case Columns::PacketType:
          return HelperQt::toQString(
            Tftp::Packets::PacketTypeDescription::instance().name(
              packet->first ) );

        case Columns::PacketCount:
          return QString::number( std::get< 0 >( packet->second ) );

        case Columns::PacketSize:
          return QString::number( std::get< 1 >( packet->second ) );

        default:
          return {};
      }

    default:
      return {};
  }
}
QVariant PacketStatisticModel::headerData(
  int const section,
  Qt::Orientation const orientation,
  int const role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  switch ( static_cast< Columns >( section ) )
  {
    case Columns::PacketType:
      return tr( "Packet Type" );

    case Columns::PacketCount:
      return tr( "Packet Count" );

    case Columns::PacketSize:
      return tr( "Packet Size" );

    default:
      return {};
  }
}

void PacketStatisticModel::statistic(
  Tftp::Packets::PacketStatistic::Statistic statistic )
{
  beginResetModel();
  statisticV = std::move( statistic );
  endResetModel();
}

}
