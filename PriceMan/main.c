#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>

#include "CommonData.h"
#include "DataBase.h"

/*======================================================================================================================*/

#define ARG_ERROR_RESULT -1
#define ARG_PRINT_RESULT  0
#define ARG_HELP_RESULT   1
#define ARG_ADD_RESULT    2
#define ARG_REMOVE_RESULT 3


#define ADD_PARAM     "add"
#define REMOVE_PARAM "remove"
#define HELP_PARAM   "h"

/*======================================================================================================================*/

void PrintHelpMessage(char *ProgName);
void PrintErrorMessage(int argc, char *argv[]);
int CheckArgs(int argc, char *argv[], char Name[], uint16_t *Price);

void WriteUpdateNewCity(char CityName[], int CityPrice);
void RemoveCity(char CityName[]);

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
    uint16_t Price;
    int Result;
    
    //code
    Result = CheckArgs(argc, argv, Name, &Price);
    switch(Result)
     {
      case ARG_ERROR_RESULT:
          PrintErrorMessage(argc, argv);
        break;
      case ARG_PRINT_RESULT:
          printf("The printing database isn't implemmented yet.\n\r");
        break;
      case ARG_HELP_RESULT:
          PrintHelpMessage(argv[0]);
        break;
      case ARG_ADD_RESULT:
          printf("Adding the city: %s and with the price %d.%02d ₪ / hour to the database\n\r", Name, Price/100, Price%100);
          WriteUpdateNewCity(Name, Price);
        break;
      case ARG_REMOVE_RESULT:
          printf("Removing the city: %s from the database.\n\r", Name);
          RemoveCity(Name);
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
int CheckArgs(int argc, char *argv[], char Name[], uint16_t *Price)
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
  printf("\n\rTo use the program it's needed to type the next parameters:\n\r");
  printf("%s [%s <cyty name> <parking price>] [%s <cyty name>] \n\r", fname, ADD_PARAM, REMOVE_PARAM);
  printf("Where <cyty name> is the name of city and the <parking price> is the price of parking.\n\r" );
  printf("For example: %s %s Tel Aviv 6.8 - means add Tel Aviv town to the database and set its price 6.80 ₪ / hour\n\r", fname, ADD_PARAM);
  printf("             %s %s Ramat Gan - means remove the Ramatgan from the database.\n\r", fname, REMOVE_PARAM);
  printf("If the database file doesn't exist it will be created. If the town doesn't exist in the database it will be added and if exists it will be only updated.\n\r");
  printf("In any case to type %s %s to see this help message.\n\r\n\r", fname, HELP_PARAM);
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

void WriteUpdateNewCity(char CityName[], int CityPrice)
 {
  int CityID;
  int UpdateResult = 0;
  CreateLoadDatabase(&conn); // Yes, the given pointer to database must be given as pointer to pointer to database because it's address is updated in this function.
  UpdateResult = UpdateCityInDataBase(&conn, CityName, CityPrice);
  if(UpdateResult < 0)  // The update wasn't be possible because the city didn't exist
   {
    CityID = GetNotExistingInDataBaseCityID(&conn);
    WriteToDataBase(&conn, CityID, CityName, CityPrice);
   }
 }

void RemoveCity(char CityName[])
 {
  RemoveCityFromDataBase(&conn, CityName);
 }

/*======================================================================================================================*/




