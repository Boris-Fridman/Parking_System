#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>
#include <unistd.h>

#include "CommonData.h"
#include "DataBase.h"

/*======================================================================================================================*/

#define ARG_ERROR_RESULT -1
#define ARG_PRINT_RESULT  0
#define ARG_HELP_RESULT   1
#define ARG_ADD_RESULT    2
#define ARG_REMOVE_RESULT 3
#define ARG_RENAME_RESULT 4


#define ADD_PARAM     "add"
#define REMOVE_PARAM "remove"
#define RENAME_PARAM "rename"
#define HELP_PARAM   "h"

/*======================================================================================================================*/

void PrintHelpMessage(char *ProgName);
void PrintErrorMessage(int argc, char *argv[]);
void PrintCitiesFromDataBase();
int CheckArgs(int argc, char *argv[], char Name[], uint16_t *Price, char NewName[]);

void AddOrUpdateNewCity(char CityName[], int CityPrice);
void RemoveCity(char CityName[]);
void RenameCity(char OldName[], char NewName[]);

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Main Function from which the program starts running. 
 * *************************************************************************************************************
 */
/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */

int main(int argc, char *argv[])
 {
    //ansi clear screen
    printf("\033[2J\033[H");
    char Name[NAME_LEN] = "";
    char NewName[NAME_LEN] = "";
    uint16_t Price;
    int Result;
    
    //code
    Result = CheckArgs(argc, argv, Name, &Price, NewName);
    switch(Result)
     {
      case ARG_ERROR_RESULT:
          PrintErrorMessage(argc, argv);
        break;
      case ARG_PRINT_RESULT:
          PrintCitiesFromDataBase();
        break;
      case ARG_HELP_RESULT:
          PrintHelpMessage(argv[0]);
        break;
      case ARG_ADD_RESULT:
          AddOrUpdateNewCity(Name, Price);
        break;
      case ARG_REMOVE_RESULT:
          RemoveCity(Name);
        break;
      case ARG_RENAME_RESULT:
          RenameCity(Name, NewName);
        break;

      default:
          printf("Isn't implemmented yet.\n\r");
        break;
     }
    return 0;
    fflush(stdout);
    fflush(stderr);
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Additional functions called from main function. 
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Builds city name from the given arguments                                                                           */
void ExtractName(char *Strings[], int NumStrings, char Name[])
 {
  int i;
  bool first = true;
  size_t StringLen, RemLen;
  Name[0] = '\0';
  for(i = 0; i < NumStrings; i++)
   {
    if(strlen(Name) < NAME_LEN - 2)
     {
      if(!first)
       strcat(Name, " ");
      first = false;
      StringLen = strlen(Strings[i]);
      RemLen = NAME_LEN - strlen(Name);
      strncat(Name, Strings[i], MIN(RemLen, StringLen));
     }
    else 
     break;
   
   }
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Function checking arguments given to the program.                                                                   */
int CheckArgs(int argc, char *argv[], char Name[], uint16_t *Price, char NewName[])
 {
  int nintres;
  int Result = ARG_PRINT_RESULT;
  double RealPrice;
  if(argc > 1)
   {
    if((!strcmp(argv[1], ADD_PARAM)) && (argc >= 4))
     {
      ExtractName(&argv[2], argc - 3, Name);
      nintres = sscanf(argv[argc - 1], "%lf", &RealPrice);
      if(nintres != EOF)
       {
        *Price = round(RealPrice * 100);
       }
      else
       {
       }
      
      Result = ARG_ADD_RESULT;
     }
    if((!strcmp(argv[1], REMOVE_PARAM)) && (argc >= 3))
     {
      ExtractName(&argv[2], argc - 2, Name);
      Result = ARG_REMOVE_RESULT;
     }
    if((!strcmp(argv[1], RENAME_PARAM)) && (argc == 4))
     {
      ExtractName(&argv[2], 1, Name);
      ExtractName(&argv[3], 1, NewName);
      Result = ARG_RENAME_RESULT;
     }
    if((!strcmp(argv[1], HELP_PARAM)) && (argc >= 2))
     {
      Result = ARG_HELP_RESULT;
     }
   }
  return Result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Prints on the screen a help message.                                                                                */
void PrintHelpMessage(char *ProgName)
 {
  char *fname = basename(ProgName);
  printf("\n\rUse the program with the next parameters:\n\r");
  printf("\n\r");
  printf("%s [%s <cyty name> <parking price>] or [%s <cyty name>] or [%s \"<old city name>\" \"<new city name>\"]\n\r", fname, ADD_PARAM, REMOVE_PARAM, RENAME_PARAM);
  printf("Where <cyty name> is the name of city and the <parking price> is the price of parking.\n\r" );
  printf("\n\r");
  printf("For example: %s %s Tel Aviv 6.8 - means add Tel Aviv town to the database and set its price 6.80 ₪ / hour\n\r", fname, ADD_PARAM);
  printf("             %s %s Ramat Gan - means remove the Ramat Gan from the database.\n\r", fname, REMOVE_PARAM);
  printf("             %s %s \"Ptah Taua\" \"Petach Tiqua\" - means rename the \"Ptah Taua\" to \"Petach Tiqua\" in the database.\n\r", fname, RENAME_PARAM);
  printf("\n\r");
  printf("Attention !!!\n\r");
  printf("In case of renaming command, the names with spaces must be given in single or double quotes.\n\r");
  printf("Otherwise the program woun't be able to recognize the names correctly.\n\r");
  printf("\n\r");
  printf("If the database file doesn't exist it will be created.\n\r");
  printf("If the town doesn't exist in the database it will be added and if exists it will be only updated.\n\r");
  printf("\n\r");
  printf("By default the program will print the cities existing in the database.\n\r");
  printf("\n\r");
  printf("In any case type %s %s to see this help message.\n\r\n\r", fname, HELP_PARAM);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Prints on the screen an error message.                                                                              */
void PrintErrorMessage(int argc, char *argv[])
 {
  UNUSED(argc);
  fprintf(stderr, "Error in options.\n\r");
  PrintHelpMessage(argv[0]);
 }


/*======================================================================================================================*/

static sqlite3 *conn;

void AddOrUpdateNewCity(char CityName[], int CityPrice)
 {
  int CityID;
  int result = 0;
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  printf("Adding the city: %s and with the price %d.%02d ₪ / hour to the database\n\r", CityName, CityPrice/100, CityPrice%100);
  CreateLoadDatabase(&conn); // Yes, the given pointer to database must be given as pointer to pointer to database because it's address is updated in this function.
  result = UpdateCityPriceInDataBase(&conn, CityName, CityPrice);
  if(result == 0)
   {
    if(StdOutNoPiping)fprintf(stdout, "%s", ResultColors[E_CORRECT]);
    printf("The city \"%s\" was already existing. Was updated only price.\n\r", CityName);
    printf("The price was updated to:%d.%02d ₪ / hour.\n\r", CityPrice / 100, CityPrice % 100);
    if(StdOutNoPiping)fprintf(stdout, "%s", TermColorsReset);    
   }
  else if(result < 0)  // The update wasn't be possible because the city didn't exist
   {
    CityID = GetCityIDNotExistingInDataBase(&conn);
    result = WriteToDataBase(&conn, CityID, CityName, CityPrice);
    if(result == 0)
     {
      if(StdOutNoPiping)fprintf(stdout, "%s", ResultColors[E_CORRECT]);
      printf("The city \"%s\" with the price %d.%02d ₪ / hour was added successrully. \n\r", CityName,  CityPrice / 100, CityPrice % 100);
      if(StdOutNoPiping)fprintf(stdout, "%s", TermColorsReset);    
     }
   }
  if(result == -1)
   {
    if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_FAIL]);
    printf("Error\n\r");
    if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);
   }
 }

void RemoveCity(char CityName[])
 {
  int result;
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  printf("Removing the city: %s from the database.\n\r", CityName);
  result = RemoveCityFromDataBase(&conn, CityName);
  switch (result)
   {
    case 0:
      if(StdOutNoPiping)fprintf(stdout, "%s", ResultColors[E_CORRECT]);
      printf("The city \"%s\" was removed successfully\n\r", CityName);
      if(StdOutNoPiping)fprintf(stdout, "%s", TermColorsReset);
     break;
    case -3: 
      if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_WARNING]);
      printf("The city \"%s\" was not found\n\r", CityName);
      if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);
     break;
    default: 
      if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_FAIL]);
      printf("Error\n\r");
      if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);
     break;
   }
 }

void RenameCity(char OldName[], char NewName[])
 {
  int result;
  int OldCityPlc, NewCityPlc;
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  printf("Renaming city: from %s to %s\n\r", OldName, NewName);
  OldCityPlc = FindCityInDataBase(&conn, OldName);
  NewCityPlc = FindCityInDataBase(&conn, NewName);
  if((OldCityPlc >= 0) && (NewCityPlc >= 0))
   {
    if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_FAIL]);
    fprintf(stderr, "Error: The city with new name \"%s\" already exists. \n\r", NewName);
    fprintf(stderr, "       The renaming is cancelled to prevent the city name duplication.\n\r");
    if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);
   }
  else
   {
    result = RenameCityByName(&conn, OldName, NewName);
    switch (result)
     {
      case 0:
        if(StdOutNoPiping)fprintf(stdout, "%s", ResultColors[E_CORRECT]);
        printf("The city was renamed successfully\n\r");
        if(StdOutNoPiping)fprintf(stdout, "%s", TermColorsReset);
       break;
      case -3: 
        if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_WARNING]);
        printf("The city \"%s\" was not found\n\r", OldName);
        if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);
       break;
      default: 
        if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_FAIL]);
        printf("Error\n\r");
        if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);
       break;
     }
   }
  
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Prints all cities existing in database.                                                                             */
// #define STR(x) #x
// #define XSTR(x) STR(x)

void PrintCitiesFromDataBase()
 {
  int result;
  PriceTab_s *ListOfCities;
  int ListSize, i;
  result = GetCitiesList(&conn, &ListOfCities, &ListSize);

  if(result == 0)
   {
    printf("\n\r");
    printf("%4s       %-*s  %s\n\r","ID", NAME_LEN, "Name","Price");
    for(i = 0; i < ListSize; i++)
     {
      printf("%06d   %-*s     %d.%02d\n\r",ListOfCities[i].City_ID, NAME_LEN, ListOfCities[i].City_Name, ListOfCities[i].Price / 100, ListOfCities[i].Price % 100);
      //printf("%03d   %-" XSTR(NAME_LEN) "s   %d.%02d\n\r",ListOfCities[i].City_ID, ListOfCities[i].City_Name, ListOfCities[i].Price / 100, ListOfCities[i].Price % 100);
     }
    printf("\n\r");
   }
  
  FreeList(&ListOfCities);  /* No need to compare the list to NULL because it is compared in the procedure itself. Even more it should be run anyway without any condition to prevent emergency memory leakage. */
 }


/*======================================================================================================================*/




