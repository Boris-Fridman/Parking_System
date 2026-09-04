# Parking System
The final project on course Embedded Linux ( Parking System Project Design Document (V2.0) )

Folders:





    ┏━━━━┳━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  
    ┃    ┃                   ┃                                                                                        ┃  
    ┃ N⍛ ┃ Folder name       ┃ Description                                                                            ┃  
    ┣━━━━╋━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃ 1. ┃🌐 GPSMan          ┃ GPS simulating program generating randomly GPS coorditates.                            ┃  
    ┣━━━━╋━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃ 2. ┃🚗 CarMan          ┃ The car managaer program.                                                              ┃  
    ┃    ┃                   ┃ Is built for the Beagle Bone Green Borard and works as the car sending its place       ┃  
    ┃    ┃                   ┃ of parking to the parking manager program calculating parking price.                   ┃  
    ┣━━━━╋━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃ 3. ┃🅿️ ParkMan         ┃ The parking manager program.                                                           ┃  
    ┃    ┃                   ┃ Contains TCP Server and Database containing information about parkings, prices.        ┃  
    ┃    ┃                   ┃ The Server and DB are communicated via the shared memory.                              ┃  
    ┣━━━━╋━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃ 4. ┃💸 PriceMan        ┃ Manages prices witten in the database. If DB-File doesn't exist it creates it.         ┃  
    ┣━━━━╋━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃ 5. ┃🧩 CommonLibs      ┃ The folder containing libaries common for two or more subprojects.                     ┃  
    ┗━━━━┻━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  



1. GPSMan (GPS Manager)
   The program written in the IDE Cube and must be burent to the STM32F756ZG MCU board. The program has internal table with 
   Points Of Interesses (POIs) and that can belong to any city, village or other living place in the region of Israel. Every 
   second is generated a new random number according to which the program takes a POI from the table and sends its GPS cords
   via the I2C bus while reading it by the CarMan Program.

2. CarMan (Car Manager)
   The program is written on C for Linux Operating System and can be compiled in 3 versions: Regular Computer pgogram version,
   Regular BeagleBone Program version and the BeagleBone Daemon version. According to requirements it must be stored in the
   BeagleBone Green Board and run as a deamon, but for better troubleshooting to the project were added two additional versions.
   The Computer version Generates coordinates randomly from its internal table or any random coordinates referring to the 
   Israel Region according the configuration stored in the .ini file. The Beagle Bone versions in comporison to it read the 
   GPS Coordinates from the I2C bus when the I2C Number and the Slave address are configured in the .ini file. 
   All these 3 version connect to the ParkMan Server by the TCP/IP protocol, send there the randomly generated GPS coordinates,
   Name of the Customer and Vehicle ID number that are configured in the .ini file. The network configuration of the IP address,
   DHCP Name and the port are stored in the .ini file too. In additional in the .ini file is stored the option of usint the
   DHCP Name or the Static IP address. If in the configuration is configured to use the DHCP the program will try to connect
   by the given DHCP name and in case fail it will try to connect by the given static IP address anyway. 
   In addition for emulating the regular parking user the ptoram will stay connected for a randomly generated period of time
   and after the disconnection it will wait for another randonly generated time period before reconnecting back. The randomly
   generated time periods have limits defined in the .ini file for parking time and for pouse between the parkings. In case
   of case of need to park infinitely the parking limit in the .ini file must be set to zero value - "0".
   All the events as connection to a server, disconnection from it, starting/finishing parking... and  errors are stored
   in a .log file.

3. ParkMan (Parking Manager)
   The program simulates the parking server. It is written in C++ and is compiled to a regular computer computer program
   running in an infinit loop. It opens a connection port given in .ini file. For receiving connections from clients it uses 
   ICP/IP protocol. It receives messages from clients where are written GPS coordinates, Customer Name and Vehicle ID, registers
   the client-connection-time and the first received GPS coordinates, detects the region to which the coordinates are referring, 
   finds the hourly pice in the database file and according the connection period and the hourly price it calculates the accumulated
   price that are written to the database file during the connection. The parking time and the final price are calculated according
   the client disconnetion. In addition the parking time, hourly price and the accumulated price are sent to the client as response.
   The map with regions must exist in the shape files: .shp, .shx and .dbf. with the same name. The name the shape files with
   the path to them and the name with path of the database file are written in the .ini file. If the region name doesn't exist
   in the price table or in the shape file the calculated price will be zero and the event in the .log file will be registered 
   as attention. The information about the clints' connection, disconnection detected price, final price ane errors in registered
   in the .log file.
   In the database file are retistered: The Client's name, Vehicle ID, the first-registered-GPS coordinates, Region name and code,
   starting/ending parking time, parking duration, and the hourly price and the accumulated (temporary and than final) prices.
   In addition on startup the program creates the file with its own PID of the main process that is destroyed on exit. The name and
   the path of this file are configured in the .ini file. This file is used for the PriceMan program to send the notifying signals
   about the price update in the price table.
   The parking region is registered from the first message from the client and the parking hourly price is also taken from 
   the databse file from connection only. Any updates in the price table and any new received coordinates will be ignored druing
   the parking session, but will be updted on a reconnection. 
   For example: If the program had received in the first message the Tel-Aviv's GPS coordinates and found the price 5 ₪ per hour
   and then in the next message the program will receive the Ramat-Gan's GPS coordinates or during the parking the price was
   updated to the 6 ₪ per hour the parking will be calculated as Tel-Aviv's parking withe the price 5 ₪ per hour anyway.
   But if the client disconnects from this parking and reconnects to the server again with the Tel-Aviv's coordinates the price
   will be calculated from the 5 ₪ per hour. It was implemented specialy because the GPS coordinate jumping during the parking means
   that the client parked his vehicle and wrode from the parking to other city with its mobile phone. In addition any client wouldn't
   like to park his vehicle with one price and to pay the parking finaly with the updated price.
   In addition the program has its built-in name vocobulary containing the names in regions with thir alternatives. It is used 
   for region name recognition in case the name of region in the shape file and in the parking table is written differently.
   For example: If the the name in the regions' shape file is written as "Petah Tiqua" and in the price list as "Petach-Tikwa" it 
   is will be rechecked in the vocobulary and recognized anyway by ignoring the "-" existance and rechecking the names' versions.

4. PriceMan (Price Manager)
   The program is written on C language for Linux Operation System.
   The program is used as a CLI program that is run as a command which purpose is to add, remove, rename a region or update the 
   price in it. It is run with parameters add, remove or rename with region name, in case of add with price, in case of rename with
   another name. In case of remove or rename it can run with the city ID name allready existing in the database. The running without
   parameters the program will print all the existing cities in the database with their ID and price. If the command received the 
   parameter "add" with allready existing city it will update only the price in it. If the name of the city is built from two words
   in case of add or remove the program will detect it and add will write the name of the city with all the words by putting the space 
   between them. But in case of using the "rename" command or the number in name with the "add" command or if the name must be 
   splitted with more than one space the name must be written in the quotes because it those cases it is not possible to recognize it
   correctly. For example if was given the commad "priceman add Tel Aviv 6.8" the program will detect the name correctly, but if
   it is needed to rename the name from "Qiriyat Mozkin" to "Kiriat Motskin" the command must be written as 
   "priceman rename "Qiriyat Mozkin" to "Kiriat Motskin"" or "priceman rename 'Qiriyat Mozkin' to 'Kiriat Motskin'".
   The writing "priceman --help" will print the help message.
   In case of pricetable update the program will search for the file with ParkMan's PID and in case of its existance it will send to the
   process with this PID the signal after the list update to notify the ParkMan to reload the regions' prices.


* The .ini and .log files:
  Both files are created by linux program of the project if they don't exist. They have the same name as a program, but with the added 
  extention .ini or .log accordingly. For Example: "priceman.elf" ▬▬▬▶ "priceman.log" or "carman" ▬▬▬▶ "carman.ini".
  The .ini file will be searched or created in the same path as the executable. If in the found .ini file is missing any option the program
  will load only the existing options and rewrite the .ini file when the existed options will be written with previously values and 
  the missing options will get the default values. Each option is built from the key and the value when the key and the value are sepparated
  by the ":" and no spaces are permitted. If to the .ini file are wirtten any options or things not referring to the configuration, they 
  will be ignored during loading, but if the .ini file is overwritten due to missing options all the unnecessary information will be removed.
  The .log file will be searched by the ptogram in the path written in the .ini file. In case of existance the file will not be overwritten,
  but there will be appended new events. Each event message has several log-level-types: "Event", "Message", "Attention", "Warning", "Error"
  and "Fail". Each logging line is built from the next parts: Date, Time, Process ID, Thread ID, Process Name, Log event message level and 
  the message itself. The Log Level Type, Process Name and the Message are created by the user while the other parts are created by 
  the logging command automatically durint the logging.

* Map Files. Shape files. And the GPS recognition:
  The Map files are written in the shape formats and are stored in the .shp, .shx and .dbf files. For working with them to the project was
  implemented the third party open-source GDAL library installed to the operating system separately like the sql library that can be found 
  in the site: https://gdal.org/en/stable/doxygen/index.html
  The shape files were and can be addited by the QGIS program that is downloaded and installed too. It can be found in the site: https://www.qgis.org/
  The original source shape files can be and were downloaded from the two sites: 
  1. https://download.geofabrik.de/asia/israel-and-palestine.html
  2. https://gvulot-shiput-statutory-moinil.opendata.arcgis.com/search?tags=%25D7%2592%25D7%2591%25D7%2595%25D7%259C%25D7%2595%25D7%25AA%2520%25D7%25A9%25D7%2599%25D7%25A4%25D7%2595%25D7%2598
  and than were readited and adjusted to the project.
```
  ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  
  ┃                                           ┃  
  ┃   To install GDAL library:                ┃  
  ┃                                           ┃  
  ┃   sudo apt update                         ┃  
  ┃   sudo apt install gdal-bin libgdal-dev   ┃  
  ┃   (conda install -c conda-forge gdal)     ┃  
  ┃                                           ┃  
  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  
```  
* The I2C connection between the BeagleBone and STMre boards is given in the table below:
```
  ┏━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┓  
  ┃  Beagle Bone I2C_2  ┃ Direction  ┃     STM32 I2C_1     ┃  
  ┃       Master        ┃            ┃       Slave         ┃  
  ┣━━━━━━━━━━┳━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━┳━━━━━━━━━━┫  
  ┃   SCL    ┃  P9-19   ┃    ▬▬▬▶    ┃   PB8    ┃   SCL    ┃  
  ┣━━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━┫  
  ┃   SDA    ┃  P9-20   ┃    ◀▬▬▬    ┃   PB9    ┃   SDA    ┃  
  ┗━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛  
```








```
    ┏━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━┓  
    ┃           ┃                     ┃                      ┃  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┗━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━┛  
```


 







<pre>
    ┏━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━┓  
    ┃           ┃                     ┃                      ┃  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━┫  
    ┃           ┃                     ┃                      ┃  
    ┗━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━┛  
</pre>





