#include "DataBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


/*======================================================================================================================*/

char PathFileName[300];

/*======================================================================================================================*/

int OpenDataBase(sqlite3 **conn);
int GetNumRows(sqlite3 **conn);
void PrintDBError(int ErrorCode);

/*======================================================================================================================*/

/* 
 * *************************************************************************************************************
 **          Database accessing Functions / Procedures
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Loads existing database or creates a new one if any database doesn't exist.                                         */
int CreateLoadDatabase(sqlite3 **conn)
 {
  int result;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  if(conn == NULL)
   return -2;  /* The pointer to the database wasn't given. */
  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    char *err_msg;
    result = sqlite3_exec(*conn, "CREATE TABLE IF NOT EXISTS CITIES_PRICES(city_id INT, city_name TEXT, price_per_hour_in_ag INT);", 0, 0, &err_msg);
    if(result != SQLITE_OK)
     {
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot prepare the table: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
     }
    sqlite3_close(*conn);
    return 0; /* The database was loaded successfully. */
   }
  else
   {
    return -1;
   }
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/*  Additional Auxilar function that is used for sorting in the "GetCityIDNotExistingInDataBase()" function.            */
int compare(const void *d1, const void *d2)
 {
  return *(int*)d1 > *(int*)d2;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Checks all the cities' ids in the database and returns the not esisting one. (Tries to find a smallest not existing.) */
int GetCityIDNotExistingInDataBase(sqlite3 **conn)
 {
  int result, valtoret = 0, v;
  int total_rows;
  int *CityIDs = NULL;
  int i;
  sqlite3_stmt* stmt;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    total_rows = GetNumRows(conn);
    if(total_rows >= 0)
     {
      result = sqlite3_prepare_v2(*conn, "SELECT city_id FROM CITIES_PRICES;", -1, &stmt, 0);
      if(result != SQLITE_OK)
       {
        if(NoPiping)fprintf(stderr, TermRed);
        fprintf(stderr, "Cannot prepere the table: %s\n\r", sqlite3_errmsg(*conn));
        if(NoPiping)fprintf(stderr, TermColorsReset);
       }
      else
       {
        CityIDs = calloc(total_rows, sizeof(int));
        if(CityIDs != NULL)
         {
          for(i = 0, result = SQLITE_ROW; (i < total_rows)&&(result == SQLITE_ROW); i++)
           {
            result = sqlite3_step(stmt);
            if(result == SQLITE_ROW)
             {
              v = sqlite3_column_int(stmt,0);
              CityIDs[i] = v;
             }
           }
          // sorting.
          qsort(CityIDs, total_rows, sizeof(CityIDs[0]), compare);
          for(i = 0; i < total_rows - 1; i++)
           {
            if((CityIDs[i+1] - CityIDs[i]) > 1)
             break;
           }
          valtoret = CityIDs[i] + 1;
          free(CityIDs);
         }
        else
         {
          do
           {
            result = sqlite3_step(stmt);
            if(result == SQLITE_ROW)
             {
              v = sqlite3_column_int(stmt,0);
              valtoret = MAX(valtoret, v);
             }
           }
          while(result == SQLITE_ROW);
          ++valtoret;
         }
       }
     }
   }
  // if(CityIDs != NULL)
  //  free(CityIDs);
  sqlite3_finalize(stmt);
  sqlite3_close(*conn);
  return valtoret;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Finds city by name in the database.                                                                                 */
int FindCityInDataBase(sqlite3 **conn, const char city_name[])
 {
  int result, valtoret = 0;
  //int id, price;
  const unsigned char *foundname;
  sqlite3_stmt* stmt;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    result = sqlite3_prepare_v2(*conn, "SELECT city_id, city_name, price_per_hour_in_ag FROM CITIES_PRICES;", -1, &stmt, 0);
    if(result != SQLITE_OK)
     {
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot prepere the table: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
      valtoret = -1;
     }
    else
     {
      do
       {
        result = sqlite3_step(stmt);
        if(result == SQLITE_ROW)
         {
          foundname = sqlite3_column_text(stmt,1);
          if(!strcmp((const char*)foundname, city_name))
           {
            // id = sqlite3_column_int(stmt, 0);
            // price = sqlite3_column_int(stmt, 2);
            break;
            //valtoret = sqlite3_column_int(stmt,0);
           }
          valtoret++;
         }
       }
      while(result == SQLITE_ROW);
      if(result != SQLITE_ROW)  // The name wasn't found.
       valtoret = -3;
     }
   }
  else
   valtoret = -1;

  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(*conn);
  return valtoret;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Updates existing city in the database.                                                                              */
int UpdateCityPriceInDataBase(sqlite3 **conn, char city_name[], int city_price)
 {
  int result, valtoret = SQLITE_OK;
  int affected_rows;
  sqlite3_stmt* stmt = NULL;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    result = sqlite3_prepare_v2(*conn, "UPDATE CITIES_PRICES SET price_per_hour_in_ag = ? WHERE city_name = ?;", -1, &stmt, 0);
    if(result != SQLITE_OK)
     {
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot prepair for updating data: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
      valtoret = -1;
     }

    result = sqlite3_bind_int(stmt, 1, city_price);
    if(result != SQLITE_OK){PrintDBError(result);}
    result = sqlite3_bind_text(stmt, 2, city_name, -1, SQLITE_STATIC);
    if(result != SQLITE_OK){PrintDBError(result);}

    result = sqlite3_step(stmt);
    if((result != SQLITE_OK) && (result != SQLITE_DONE))
     {
      valtoret = -1;
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot update data: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
     }
    affected_rows = sqlite3_changes(*conn);
    if(affected_rows == 0) // No value was found.
     {
      valtoret = -3; 
     }
   }
  else
   valtoret = -1;
  sqlite3_finalize(stmt);
  sqlite3_close(*conn);
  return valtoret;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Writes new city to the database.                                                                                    */
int WriteToDataBase(sqlite3 **conn, int city_id, char city_name[], int city_price)
 {
  int valtoret = SQLITE_OK;
  sqlite3_stmt* stmt = NULL;
  valtoret = OpenDataBase(conn);
  if(valtoret == SQLITE_OK)
   {
    do
     {
       valtoret = sqlite3_prepare_v2(*conn, "INSERT INTO CITIES_PRICES (city_id, city_name, price_per_hour_in_ag) VALUES (?, ?, ?);", -1, &stmt, 0);
       if(valtoret != SQLITE_OK){PrintDBError(valtoret);break;}     
       valtoret = sqlite3_bind_int (stmt, 1, city_id                     );
       if(valtoret != SQLITE_OK){PrintDBError(valtoret);break;}
       valtoret = sqlite3_bind_text(stmt, 2, city_name, -1, SQLITE_STATIC);
       if(valtoret != SQLITE_OK){PrintDBError(valtoret);break;}
       valtoret = sqlite3_bind_int (stmt, 3, city_price                  );
       if(valtoret != SQLITE_OK){PrintDBError(valtoret);break;}
      sqlite3_step(stmt);
      valtoret = sqlite3_finalize(stmt);
      if(valtoret != SQLITE_OK){PrintDBError(valtoret);break;}
     } 
    while (0);
    //sqlite3_finalize(stmt);
    sqlite3_close(*conn);
   }
  return valtoret;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Removes the given city from the database.                                                                           */
 int RemoveCityFromDataBase(sqlite3 **conn, char city_name[])
  {
  int result, valtoret = SQLITE_OK;
  int affected_rows;
  sqlite3_stmt* stmt = NULL;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    result = sqlite3_prepare_v2(*conn, "DELETE FROM CITIES_PRICES WHERE city_name = ?;", -1, &stmt, 0);
    if(result != SQLITE_OK)
     {
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot prepair for deleteing data: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
     }

    result = sqlite3_bind_text(stmt, 1, city_name, -1, SQLITE_STATIC);
    if(result != SQLITE_OK){PrintDBError(result);}

    result = sqlite3_step(stmt);
    if((result != SQLITE_OK) && (result != SQLITE_DONE))
     {
      valtoret = -1;
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot delete data: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
     }
    affected_rows = sqlite3_changes(*conn);
    if(affected_rows == 0) // No value was found.
     {
      valtoret = -3; 
     }
   }
  else
   valtoret = -1;
  sqlite3_finalize(stmt);
  sqlite3_close(*conn);
  return valtoret;

  }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Renames city according old and new names.                                                                           */
int RenameCityByName(sqlite3 **conn, char old_name[], char new_name[])
 {
  int result, valtoret = SQLITE_OK;
  int affected_rows;
  sqlite3_stmt* stmt = NULL;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    result = sqlite3_prepare_v2(*conn, "UPDATE CITIES_PRICES SET city_name = ? WHERE city_name = ?;", -1, &stmt, 0);
    if(result != SQLITE_OK)
     {
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot prepair for updating data: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
     }

    result = sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
    if(result != SQLITE_OK){PrintDBError(result);}
    result = sqlite3_bind_text(stmt, 2, old_name, -1, SQLITE_STATIC);
    if(result != SQLITE_OK){PrintDBError(result);}

    result = sqlite3_step(stmt);
    if((result != SQLITE_OK) && (result != SQLITE_DONE))
     {
      valtoret = -1;
      if(NoPiping)fprintf(stderr, TermRed);
      fprintf(stderr, "Cannot update data: %s\n\r", sqlite3_errmsg(*conn));
      if(NoPiping)fprintf(stderr, TermColorsReset);
     }
    affected_rows = sqlite3_changes(*conn);
    if(affected_rows == 0) // No value was found.
     {
      valtoret = -3; 
     }
   }
  else
   valtoret = -1;
  sqlite3_finalize(stmt);
  sqlite3_close(*conn);
  return valtoret;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Gives a list of the all existing cities in the database.                                                            */
int GetCitiesList(sqlite3 **conn, PriceTab_s **list, int *list_size)
 {
  int result = 0, valtoret = 0;
  int i;
  sqlite3_stmt* stmt = NULL;
  const unsigned char *name;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  
  *list = NULL;  // To ensure that the pointer is NULL even the allocating memory function didn't run.
  result = OpenDataBase(conn);
  if(result == SQLITE_OK)
   {
    *list_size = GetNumRows(conn);
    if(*list_size >= 0)
     {
      *list = calloc(*list_size, sizeof(PriceTab_s));
      if(*list == NULL)  /*  Memory couldn't be allocated. */
       {
        valtoret = -2;
        *list_size = 0;
       }
      else /*  Memory was allocated successfully. */
       {
        result = sqlite3_prepare_v2(*conn, "SELECT city_id, city_name, price_per_hour_in_ag FROM CITIES_PRICES;", -1, &stmt, 0);
        if(result != SQLITE_OK)
         {
          if(NoPiping)fprintf(stderr, TermRed);
          fprintf(stderr, "Cannot prepere the table: %s\n\r", sqlite3_errmsg(*conn));
          if(NoPiping)fprintf(stderr, TermColorsReset);
          FreeList(list);
          *list_size = 0;
          valtoret = -3;
         }
        else
         {
          for(i = 0, result = SQLITE_ROW; (i < *list_size) && (result == SQLITE_ROW); i++)
           {
            result = sqlite3_step(stmt);
            if(result == SQLITE_ROW)
             {
              name = sqlite3_column_text(stmt,1);
              strcpy((*list)[i].City_Name, (char*)name);
              (*list)[i].City_ID = sqlite3_column_int(stmt, 0);
              (*list)[i].Price = sqlite3_column_int(stmt, 2);
             }
           }
          valtoret = 0;
         }
       }
     }
   }
  else
   valtoret = -1;
  sqlite3_finalize(stmt);
  sqlite3_close(*conn);
  return valtoret;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Frees list after usage reserved by the GetCitiesList() function.                                                    */
void FreeList(PriceTab_s **list_to_free)
 {
  if(*list_to_free != NULL)
   {
    free(*list_to_free);
    *list_to_free = NULL;
   }
 }

/*======================================================================================================================*/

/* 
 * *************************************************************************************************************
 **          Additional auxilar Functions / Procedures
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Applys the path to the database file.                                                                               */
void ApplyDBPath(int argc, char *argv[])
 {
  GetDataBaseFile(argc, argv, PathFileName);
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/*  Opens database. Prints error message if error.                                                                      */
int OpenDataBase(sqlite3 **conn)
 {
  int result;
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  result = sqlite3_open(PathFileName, conn);  //result = sqlite3_open(DB_FILENAME, conn);
  if(result != SQLITE_OK)
   {
    if(NoPiping)fprintf(stderr, TermRed);
    fprintf(stderr, "Cannot open the file with table: %s\n\r", sqlite3_errmsg(*conn));
    if(NoPiping)fprintf(stderr, TermColorsReset);
   }
  return result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Returns Number of columns in database or "-1" if error.                                                             */
int GetNumRows(sqlite3 **conn)
 {
  int result, total_rows = -1;
  sqlite3_stmt* stmt;
  result = sqlite3_prepare_v2(*conn, "SELECT COUNT(*) FROM CITIES_PRICES;", -1, &stmt, 0);
  if(result == SQLITE_OK)
   {
    result = sqlite3_step(stmt);
    if(result == SQLITE_ROW)
     {
      total_rows = sqlite3_column_int(stmt, 0);
     }
   }
  sqlite3_finalize(stmt);
  return total_rows;
 }


/*======================================================================================================================*/

/* 
 * *************************************************************************************************************
 **          Error pringing procedure
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Prints error message according to the error number.                                                                 */
void PrintDBError(int ErrorCode)
 {
  bool NoPiping;
  NoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  if(NoPiping) 
   {
    Error_Results_e ErrRes;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic" // Disables the non-standard warning for not standard "case low ... high:" cpmdotopm.
    switch(ErrorCode)
     {
      case SQLITE_OK:
        ErrRes = E_CORRECT;
       break;
      case SQLITE_ROW ... SQLITE_DONE:
        ErrRes = E_SUCCESS;
       break;
      case SQLITE_WARNING:
        ErrRes = E_WARNING;
       break;
      default:
        ErrRes = E_FAIL;
       break;
     }
#pragma GCC diagnostic pop // Restores your original warning settings     
    //fprintf(stderr, "%s", ResultColors[!ErrorCode]);
    fprintf(stderr, "%s", ResultColors[ErrRes]);
   }
  switch(ErrorCode)
   {
    case SQLITE_OK         : fprintf(stderr, "Successful result\n\r"); break;
    case SQLITE_ERROR      : fprintf(stderr, "Generic error\n\r"); break;
    case SQLITE_INTERNAL   : fprintf(stderr, "Internal logic error in SQLite\n\r"); break;
    case SQLITE_PERM       : fprintf(stderr, "Access permission denied\n\r"); break;
    case SQLITE_ABORT      : fprintf(stderr, "Callback routine requested an abort\n\r"); break;
    case SQLITE_BUSY       : fprintf(stderr, "The database file is locked\n\r"); break;
    case SQLITE_LOCKED     : fprintf(stderr, "A table in the database is locked\n\r"); break;
    case SQLITE_NOMEM      : fprintf(stderr, "A malloc() failed\n\r"); break;
    case SQLITE_READONLY   : fprintf(stderr, "Attempt to write a readonly database\n\r"); break;
    case SQLITE_INTERRUPT  : fprintf(stderr, "Operation terminated by sqlite3_interrupt(\n\r"); break;
    case SQLITE_IOERR      : fprintf(stderr, "Some kind of disk I/O error occurred\n\r"); break;
    case SQLITE_CORRUPT    : fprintf(stderr, "The database disk image is malformed\n\r"); break;
    case SQLITE_NOTFOUND   : fprintf(stderr, "Unknown opcode in sqlite3_file_control()\n\r"); break;
    case SQLITE_FULL       : fprintf(stderr, "Insertion failed because database is full\n\r"); break;
    case SQLITE_CANTOPEN   : fprintf(stderr, "Unable to open the database file\n\r"); break;
    case SQLITE_PROTOCOL   : fprintf(stderr, "Database lock protocol error\n\r"); break;
    case SQLITE_EMPTY      : fprintf(stderr, "Internal use only\n\r"); break;
    case SQLITE_SCHEMA     : fprintf(stderr, "The database schema changed\n\r"); break;
    case SQLITE_TOOBIG     : fprintf(stderr, "String or BLOB exceeds size limit\n\r"); break;
    case SQLITE_CONSTRAINT : fprintf(stderr, "Abort due to constraint violation\n\r"); break;
    case SQLITE_MISMATCH   : fprintf(stderr, "Data type mismatch\n\r"); break;
    case SQLITE_MISUSE     : fprintf(stderr, "Library used incorrectly\n\r"); break;
    case SQLITE_NOLFS      : fprintf(stderr, "Uses OS features not supported on host\n\r"); break;
    case SQLITE_AUTH       : fprintf(stderr, "Authorization denied\n\r"); break;
    case SQLITE_FORMAT     : fprintf(stderr, "Not used\n\r"); break;
    case SQLITE_RANGE      : fprintf(stderr, "2nd parameter to sqlite3_bind out of range\n\r"); break;
    case SQLITE_NOTADB     : fprintf(stderr, "File opened that is not a database file\n\r"); break;
    case SQLITE_NOTICE     : fprintf(stderr, "Notifications from sqlite3_log()\n\r"); break;
    case SQLITE_WARNING    : fprintf(stderr, "Warnings from sqlite3_log()\n\r"); break;
    case SQLITE_ROW        : fprintf(stderr, "sqlite3_step() has another row ready\n\r"); break;
    case SQLITE_DONE       : fprintf(stderr, "sqlite3_step() has finished executing\n\r"); break;
   }
  if(NoPiping) fprintf(stderr, "%s", TermColorsReset);
 }
