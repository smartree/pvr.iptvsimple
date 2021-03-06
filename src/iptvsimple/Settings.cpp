/*
 *      Copyright (C) 2005-2019 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1335, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "Settings.h"

#include "../client.h"
#include "utilities/FileUtils.h"

using namespace ADDON;
using namespace iptvsimple;
using namespace iptvsimple::utilities;

/***************************************************************************
 * PVR settings
 **************************************************************************/
void Settings::ReadFromAddon(const std::string& userPath, const std::string clientPath)
{
  m_userPath = userPath;
  m_clientPath = clientPath;

  char buffer[1024];

  // M3U
  if (!XBMC->GetSetting("m3uPathType", &m_m3uPathType))
    m_m3uPathType = PathType::REMOTE_PATH;
  if (XBMC->GetSetting("m3uPath", &buffer))
    m_m3uPath = buffer;
  if (XBMC->GetSetting("m3uUrl", &buffer))
    m_m3uUrl = buffer;
  if (!XBMC->GetSetting("m3uCache", &m_cacheM3U))
      m_cacheM3U = true;
  if (!XBMC->GetSetting("startNum", &m_startChannelNumber))
    m_startChannelNumber = 1;
  if (!XBMC->GetSetting("numberByOrder", &m_numberChannelsByM3uOrderOnly))
    m_numberChannelsByM3uOrderOnly = false;
  if (!XBMC->GetSetting("m3uRefreshMode", &m_m3uRefreshMode))
    m_m3uRefreshMode = RefreshMode::DISABLED;
  if (!XBMC->GetSetting("m3uRefreshIntervalMins", &m_m3uRefreshIntervalMins))
    m_m3uRefreshIntervalMins = 60;
  if (!XBMC->GetSetting("m3uRefreshHour", &m_m3uRefreshHour))
    m_m3uRefreshHour = 4;

  // EPG
  if (!XBMC->GetSetting("epgPathType", &m_epgPathType))
    m_epgPathType = PathType::REMOTE_PATH;
  if (XBMC->GetSetting("epgPath", &buffer))
    m_epgPath = buffer;
  if (XBMC->GetSetting("epgUrl", &buffer))
    m_epgUrl = buffer;
  if (!XBMC->GetSetting("epgCache", &m_cacheEPG))
    m_cacheEPG = true;
  if (!XBMC->GetSetting("epgTimeShift", &m_epgTimeShiftHours))
    m_epgTimeShiftHours = 0.0f;
  if (!XBMC->GetSetting("epgTSOverride", &m_tsOverride))
    m_tsOverride = true;

  //Genres
  if (!XBMC->GetSetting("useEpgGenreText", &m_useEpgGenreTextWhenMapping))
    m_useEpgGenreTextWhenMapping = false;
  if (!XBMC->GetSetting("genresPathType", &m_genresPathType))
    m_genresPathType = PathType::LOCAL_PATH;
  if (XBMC->GetSetting("genresPath", &buffer))
    m_genresPath = buffer;
  if (XBMC->GetSetting("genresUrl", &buffer))
    m_genresUrl = buffer;

  // Channel Logos
  if (!XBMC->GetSetting("logoPathType", &m_logoPathType))
    m_logoPathType = PathType::REMOTE_PATH;
  if (XBMC->GetSetting("logoPath", &buffer))
    m_logoPath = buffer;
  if (XBMC->GetSetting("logoBaseUrl", &buffer))
    m_logoBaseUrl = buffer;
  if (!XBMC->GetSetting("logoFromEpg", &m_epgLogosMode))
    m_epgLogosMode = EpgLogosMode::IGNORE_XMLTV;

  // Catchup
  if (!XBMC->GetSetting("catchupEnabled", &m_catchupEnabled))
    m_catchupEnabled = false;
  if (XBMC->GetSetting("catchupQueryFormat", &buffer))
    m_catchupQueryFormat = buffer;
  if (!XBMC->GetSetting("catchupDays", &m_catchupDays))
    m_catchupDays = 5;
  if (!XBMC->GetSetting("allChannelSupportCatchup", &m_allChannelsSupportCatchup))
    m_allChannelsSupportCatchup = false;
  if (!XBMC->GetSetting("catchupPlayEpgAsLive", &m_catchupPlayEpgAsLive))
    m_catchupPlayEpgAsLive = false;
  if (!XBMC->GetSetting("catchupWatchEpgBeginBufferMins", &m_catchupWatchEpgBeginBufferMins))
    m_catchupWatchEpgBeginBufferMins = 5;
  if (!XBMC->GetSetting("catchupWatchEpgEndBufferMins", &m_catchupWatchEpgEndBufferMins))
    m_catchupWatchEpgEndBufferMins = 15;
  if (!XBMC->GetSetting("catchupOnlyOnFinishedProgrammes", &m_catchupOnlyOnFinishedProgrammes))
    m_catchupOnlyOnFinishedProgrammes = false;

  // Advanced
  if (!XBMC->GetSetting("transformMulticastStreamUrls", &m_transformMulticastStreamUrls))
    m_transformMulticastStreamUrls = false;
  if (XBMC->GetSetting("udpxyHost", &buffer))
    m_udpxyHost = buffer;
  if (!XBMC->GetSetting("udpxyPort", &m_udpxyPort))
    m_udpxyPort = DEFAULT_UDPXY_MULTICAST_RELAY_PORT;
  if (!XBMC->GetSetting("useFFmpegReconnect", &m_useFFmpegReconnect))
    m_useFFmpegReconnect = false;
  if (!XBMC->GetSetting("useInputstreamAdaptiveforHls", &m_useInputstreamAdaptiveforHls))
    m_useInputstreamAdaptiveforHls = false;
}

ADDON_STATUS Settings::SetValue(const std::string& settingName, const void* settingValue)
{
  // reset cache and restart addon

  std::string strFile = FileUtils::GetUserDataAddonFilePath(M3U_CACHE_FILENAME);
  if (FileUtils::FileExists(strFile.c_str()))
    FileUtils::DeleteFile(strFile);

  strFile = FileUtils::GetUserDataAddonFilePath(XMLTV_CACHE_FILENAME);
  if (FileUtils::FileExists(strFile.c_str()))
    FileUtils::DeleteFile(strFile);

  // M3U
  if (settingName == "m3uPathType")
    return SetSetting<PathType, ADDON_STATUS>(settingName, settingValue, m_m3uPathType, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "m3uPath")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_m3uPath, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "m3uUrl")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_m3uUrl, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "m3uCache")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_cacheM3U, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "startNum")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_startChannelNumber, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "numberByOrder")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_numberChannelsByM3uOrderOnly, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "m3uRefreshMode")
    return SetSetting<RefreshMode, ADDON_STATUS>(settingName, settingValue, m_m3uRefreshMode, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "m3uRefreshIntervalMins")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_m3uRefreshIntervalMins, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "m3uRefreshHour")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_m3uRefreshHour, ADDON_STATUS_OK, ADDON_STATUS_OK);

  // EPG
  if (settingName == "epgPathType")
    return SetSetting<PathType, ADDON_STATUS>(settingName, settingValue, m_epgPathType, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "epgPath")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_epgPath, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "epgUrl")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_epgUrl, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "epgCache")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_cacheEPG, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "epgTimeShift")
    return SetSetting<float, ADDON_STATUS>(settingName, settingValue, m_epgTimeShiftHours, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "epgTSOverride")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_tsOverride, ADDON_STATUS_OK, ADDON_STATUS_OK);

  // Genres
  if (settingName == "useEpgGenreText")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_useEpgGenreTextWhenMapping, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "genresPathType")
    return SetSetting<PathType, ADDON_STATUS>(settingName, settingValue, m_genresPathType, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "genresPath")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_genresPath, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "genresUrl")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_genresUrl, ADDON_STATUS_OK, ADDON_STATUS_OK);

  // Channel Logos
  if (settingName == "logoPathType")
    return SetSetting<PathType, ADDON_STATUS>(settingName, settingValue, m_logoPathType, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "logoPath")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_logoPath, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "logoBaseUrl")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_logoBaseUrl, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "logoFromEpg")
    return SetSetting<EpgLogosMode, ADDON_STATUS>(settingName, settingValue, m_epgLogosMode, ADDON_STATUS_OK, ADDON_STATUS_OK);

  // Catchup
  if (settingName == "catchupEnabled")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_catchupEnabled, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "catchupQueryFormat")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_catchupQueryFormat, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "catchupDays")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_catchupDays, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "allChannelSupportCatchup")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_allChannelsSupportCatchup, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "catchupPlayEpgAsLive")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_catchupPlayEpgAsLive, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "catchupWatchEpgBeginBufferMins")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_catchupWatchEpgBeginBufferMins, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "catchupWatchEpgEndBufferMins")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_catchupWatchEpgEndBufferMins, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "catchupOnlyOnFinishedProgrammes")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_catchupOnlyOnFinishedProgrammes, ADDON_STATUS_OK, ADDON_STATUS_OK);

  // Advanced
  if (settingName == "transformMulticastStreamUrls")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_transformMulticastStreamUrls, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "udpxyHost")
    return SetStringSetting<ADDON_STATUS>(settingName, settingValue, m_udpxyHost, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "udpxyPort")
    return SetSetting<int, ADDON_STATUS>(settingName, settingValue, m_udpxyPort, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "useFFmpegReconnect")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_useFFmpegReconnect, ADDON_STATUS_OK, ADDON_STATUS_OK);
  if (settingName == "useInputstreamAdaptiveforHls")
    return SetSetting<bool, ADDON_STATUS>(settingName, settingValue, m_useInputstreamAdaptiveforHls, ADDON_STATUS_OK, ADDON_STATUS_OK);

  return ADDON_STATUS_OK;
}
