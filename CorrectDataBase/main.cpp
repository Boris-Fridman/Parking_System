#include <iostream>

#include "CommonData.h"
#include "PriceDataBase.h"

static sqlite3 *conn;
//static sqlite3 *conn_upd;

int OpenDataBase(sqlite3 **conn);
int GetCustomersFileNumRows(sqlite3 **conn);

struct CordsConv_s
 {
  int rid;
  GPS_Cords_s BlogsCords;
  GPS_Cords_s SepCords;
  bool CordsExist;
 };


int main()
 {
  int result = 0;
  //int result1 = 0;
  int list_size;
  int i;
  //GPS_Cords_s Cords;
  GPS_Cords_s *Cords_p;
  //double x, y;
  sqlite3_stmt* stmt = NULL;
  //sqlite3_stmt* stmt1 = NULL;
  SetDBPathName("../" CONF_DIR_NAME "/" DB_FILENAME);

  result = OpenDataBase(&conn);
  if(result == SQLITE_OK)
   {
    list_size = GetCustomersFileNumRows(&conn);
    CordsConv_s *CordsList;
    if(list_size >= 0)
     {
      CordsList = (CordsConv_s *)calloc(list_size, sizeof(CordsConv_s));
      if(CordsList != NULL)
       {
        result = sqlite3_prepare_v2(conn, "SELECT parking_cords, longitude, latitude, rowid FROM CUSTOMERS_PRICE_REPORTS;", -1, &stmt, 0);
        if(result != SQLITE_OK)
         {
          fprintf(stderr, "Cannot prepere the table: %s\n\r", sqlite3_errmsg(conn));
         }
        else
         {
          for(i = 0, result = SQLITE_ROW; (i < list_size)&&(result == SQLITE_ROW); i++)
           {
            result = sqlite3_step(stmt);            
            if(result == SQLITE_ROW)
             {
              Cords_p                         = (GPS_Cords_s*)sqlite3_column_blob  (stmt, 0);
              CordsList[i].SepCords.Longitude =               sqlite3_column_double(stmt, 1);
              CordsList[i].SepCords.Latitude  =               sqlite3_column_double(stmt, 2);
              CordsList[i].rid                =               sqlite3_column_int64 (stmt, 3);
              CordsList[i].CordsExist         = (CordsList[i].SepCords.Longitude != 0)||(CordsList[i].SepCords.Latitude  != 0);
              if(Cords_p != NULL)
               {
                CordsList[i].BlogsCords = *Cords_p;
               }
              else
               {
                CordsList[i].BlogsCords.Longitude = CordsList[i].BlogsCords.Latitude = 0;
               }
              printf("%3d   %d (%03.6f, %03.5f)    (%03.6f, %03.5f)", CordsList[i].rid, CordsList[i].CordsExist, CordsList[i].BlogsCords.Longitude, CordsList[i].BlogsCords.Latitude, CordsList[i].SepCords.Longitude, CordsList[i].SepCords.Latitude);
              printf("\n\r");
             }
           }
          sqlite3_finalize(stmt);
         }


        for(i = 0; i < list_size; i++)
         {
          if(!CordsList[i].CordsExist)
           {
            result = sqlite3_prepare_v2(conn, "UPDATE CUSTOMERS_PRICE_REPORTS SET longitude = ?, latitude = ? WHERE rowid = ?;", -1, &stmt, 0);
            if(result != SQLITE_OK)
             {
              printf("couldn't update\n\r");
             }
            else
             {
              result = sqlite3_bind_double(stmt, 1, CordsList[i].BlogsCords.Longitude);
              result = sqlite3_bind_double(stmt, 2, CordsList[i].BlogsCords.Latitude);
              result = sqlite3_bind_int64 (stmt, 3, CordsList[i].rid);
              result = sqlite3_step(stmt);
              if (result == SQLITE_DONE) 
               {
                printf("%3d  ", CordsList[i].rid);
                printf("Rows actually changed: %d\n", sqlite3_changes(conn));
               }              
              sqlite3_exec(conn, "COMMIT;", NULL, NULL, NULL);
              sqlite3_finalize(stmt);
             }
           }
         }

        free(CordsList);
        CordsList = NULL;
       }
     }

    sqlite3_close(conn);
   }


 }



int GetCustomersFileNumRows(sqlite3 **conn)
 {
  int result, total_rows = -1;
  sqlite3_stmt* stmt;
  result = sqlite3_prepare_v2(*conn, "SELECT COUNT(*) FROM CUSTOMERS_PRICE_REPORTS;", -1, &stmt, 0);
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