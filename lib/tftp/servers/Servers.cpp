// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Tftp::Servers.
 **/

#include "Servers.hpp"

#include <spdlog/spdlog.h>

namespace Tftp::Servers {

std::optional< std::filesystem::path > checkFilename(
  const std::filesystem::path &baseDir,
  const std::filesystem::path &filename,
  bool mustExist )
{
  std::error_code errorCode;

  // make base path canonical and check existence (implicit by canonical)
  auto canonicalBaseDir{ std::filesystem::canonical( baseDir, errorCode ) };
  if ( errorCode )
  {
    spdlog::error( "Could not make base directory canonical or does not exist." );

    return std::nullopt;
  }

  // generate file path
  auto filePath{ std::filesystem::weakly_canonical( canonicalBaseDir / filename, errorCode ) };
  if ( errorCode )
  {
    spdlog::error( "Could not make file path canonical." );
    return std::nullopt;
  }

  // Using "equal" we can check if "requested_file_path" starts
  // with base_resolved_path.
  // Because we previously canonicalized both paths they can't contain any ".."
  // segments, so this check is sufficient.
  if ( !std::equal( canonicalBaseDir.begin(), canonicalBaseDir.end(), filePath.begin() ) )
  {
    spdlog::error( "File path not within base directory." );

    return std::nullopt;
  }

  if ( mustExist && !std::filesystem::is_regular_file( filePath, errorCode ) && errorCode )
  {
    spdlog::error( "File does not exist." );

    return std::nullopt;
  }

  return filePath;
}

}
