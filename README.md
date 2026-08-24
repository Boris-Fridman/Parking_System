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









    ┏━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┓
    ┃  Beagle Bone I2C_2  ┃ Direction  ┃     STM32 I2C_1     ┃
    ┃       Master        ┃            ┃       Slave         ┃
    ┣━━━━━━━━━━┳━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━┳━━━━━━━━━━┫
    ┃   SCL    ┃  P9-19   ┃    ▬▬▬▶    ┃   PB8    ┃   SCL    ┃
    ┣━━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━┫
    ┃   SDA    ┃  P9-20   ┃    ◀▬▬▬    ┃   PB9    ┃   SDA    ┃
    ┗━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛





    ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  
    ┃                                           ┃  
    ┃   To install GDAL library:                ┃  
    ┃                                           ┃  
    ┃   sudo apt update                         ┃  
    ┃   sudo apt install gdal-bin libgdal-dev   ┃  
    ┃   (conda install -c conda-forge gdal)     ┃  
    ┃                                           ┃  
    ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  





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



 












