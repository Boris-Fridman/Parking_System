#ifndef ____ConfMan_h__
#define ____ConfMan_h__

#include "CommonData.h"

void InitConf(char ConfData[][2][PATH_LEN], ssize_t NumConfIt, char const *OwnProgName);

char const *GetDataByName(char ConfData[][2][PATH_LEN], ssize_t NumConfIt, char const DataName[]);


#endif  //  ____ConfMan_h__


