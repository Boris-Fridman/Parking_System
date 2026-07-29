#pragma once

#include "main.hpp"

/**
 * Attention !!!
 * This prucudure must be run from the main file before creating any external process to ensure that the copied data to a new-created-process is allready up-to-date.
 * 
*/

void InitConf(char const *OwnProgName);

char const *GetDataBaseFilePathName();

char const *GetGeoLocShapeFilePathName();  // GeoLocation Shape Path & Name of the shape file.

char const *GetProgInfoPIDFilePathName();  // Path & Name of the file with the program PID.

uint16_t GetDestinPort();

char const *GetColumnNameWithGeolocationName();







