
#                           BMS Firmware Development Tracker
			
##  Current Version
    1.0.5
			
##  About this file
    	
  This file is meant to track the bug fixes and enhancements done in the BMS code. Each time a pull request is issued, 
  the firmware version is updated. 
  
  The BMS code repository has the following branches:
    1. Master:-Major Code Release which has been thoroughly tested both on bench and in flight.
    2. Develop:-This branch contains the sub-releases. All the code committed to this branch is first tested on the bench in a 
	   simulation environment, and then tested in the flight. 
    3. Test_develop:- This branch contains the beta releases. Code committed to this branch have only been tested with minimal
       system (Basic Testing).
  
  The firmware version has the following format:
  <Major Code Release>.<Current Stable Code Release>.<Beta Code Release>
  
  Major Code Release: This number is updated every time the code from the develop is Merged to the Master branch.
  Current Stable Code Release: This number is updated every time the develop branch is merged with the test_develop branch.
  Beta Code Release: This number is update whenever the code in the test_develop branch is update.
  
##  How to maintain this file
    
  This file is needs to be updated whenever any of the branch of the BMS code is updated. 
  
  If a developer is issuing a pull request against test_develop, the <Beta Code Release> parameter must be updated by the developer 
  in the code as well as at the top of this file.
  
  If a developer is merging the test_develop code with the develop, the <Current Stable Code Release> parameter will be updated in 
  the code as well as at the top of the file. This must be done in the merge pull request itself.
  
  If a developer is merging the develop branch code with the Master branch, The <Major Code Release> parameter will be updated in the 
  code as well as at the top of the file. This must be done in the merge pull request itself.
  
  Format of updating the file:
    *** BMS_CODE  1.0.0 ***   (This line should be sub-heading using the ### in the beginning of the file)
	
    Files modified:
    Files Added:
    Files deleted:
  
    Issues Resolved:
	**********************

### ****BMS_CODE  1.0.5  ****
	Pull request number: 8
			
Features developed in BMS code for this release:

	Code changes and implementation at requirement level:

    1. BMS logs the battery parameters read in the header of the log file. #Requirement1
	   BMS reads the configuration file stored on the SD card. Generalized logic is used to read the parameters with carriage return and line feed character
	   as termination characters. In current release of the code following are the parameters read from configuration file.
       Battery ID/Serial Number,Battery Chemistry,Battery Number of Cells,Battery Capacity(mAH),Battery Cell Max(V),Battery Cell Min(V),Battery Pack Cycles	        
	2. BMS logs the data at 20Hz. Following are the parameters that are getting logged on SD card. #Requirement5
	   a. RTC_Time,MCU_Time,C1_Volt,C2_Volt,C3_Volt,C4_Volt,C5_Volt,C6_Volt,Pack_Voltage,Accumulated_Pack_Voltage,Pack_Current,
	   	  Pack_Current_Adjusted,Total_Capacity,Capacity_Remaining,Capacity_Used,Pack_Charge_Cycles,Current_Gain,Battery_C_D_Rate,mAH_IN_OUT,
	   	  C_D_Indicator,Pack_Temperature,Health_Error_Status,I2C_Error_Status,Loop_Rate,ASIC_Restart_Count,Watchdog_Flag,AP_Status,MCU_Reset_Source,
		  Sleep_Mode_Status
	3. BMS sends the following information to the AP as and when requested. Requirement6
	   a. All cells voltages (address ranges from 0x01 to 0x06)
	   b. Pack voltage in volts (address 0x07)
	   c. Pack Current in mA (address 0x08)
	   d. All data consisting of pack voltage,pack current and all cell voltages at once (address 0x0C)
	4. BMS receives the following information from the AP asynchronously. #Requirement6
	   a. GPS packet consisting of local time and date. (BMS address 0x0A)
	   b. AP flight status flags such as ARMed,DISARMed,Air/Ground etc (flags are yet to be decided. BMS address is 0x0B) 
	5. BMS shows the SOC on LEDs as per the following conditions. To show the SOC on LEDs pushbutton has to be kept pressed for more than 500ms. #Requirement7
	   LED 1: solid if SoC >= 15%, blinking if SoC < 15%
   	   LED 2: solid if SoC >= 35%, blinking if 25% <= SoC < 35%, off if SoC < 25%
	   LED 3: solid if SoC >= 55%, blinking if 45% <= SoC < 55%, off if SoC < 45%
	   LED 4: solid if SoC >= 75%, blinking if 65% <= SoC < 75%, off if SoC < 65%
	   LED 5: solid if SoC >= 95%, blinking if 85% <= SoC < 95%, off if SoC < 85%      
	   LEDs stay ON for 3 seconds, and blinking happens at 2 Hz
	6. BMS shows the SOH on LEDs as per the following conditions. To show the SOH on LEDs pushbutton has to be kept pressed two times within 1 second. #Requirement8
	   LED 1: solid if CC < 180, 180 <= blinking < 200, blinking fast if CC >= 200
	   LED 2: solid if CC < 140, blinking if 140 <= CC< 160, off if CC >= 160
	   LED 3: solid if CC < 100, blinking if 100 <= CC< 120, off if CC >= 120
	   LED 4: solid if CC < 60, blinking if 60 <= CC < 80, off if CC >=  80
	   LED 5: solid if charge cycle (CC) < 20, blinking if 20 <= CC < 40, off if CC >= 40 
	   LEDs stay ON for 3 seconds, and blinking happens at 2 Hz and fast blinking happens at 5 Hz   	
    7. BMS restores the factory default settings if pushbutton is kept pressed for more than or equal to 8 seconds. #requirement13 
    8. BMS enters the debug mode if the pushbutton is kept pressed for more than 5 seconds and less than 8 seconds.
	
	Modifications and Code changes at low level:
	
	9. Made separate functions for status/error LEDs and SOC/SOH LEDs. If pushbutton is kept pressed for more than 5 seconds and less than 8 seconds, 
	   USART debug and SOC/SOH status will toggle their functionalities. This implementation is because USART and LEDs are multi functioned on same port pins.
   10. Bug fix for logging the reset source for MCU in SD card. Initially Get_Reset_Source function was being called in the log.c where at very start 
       variable was being cleared before logging. Now variable value is stored in the log array buffer directly. 
   11. Changed the function for RTC_Showtime. Now one more parameter is added which will decide whether to fill DATE,TIME or DATE_TIME_COMBINED in the 
       buffer passed to the function. It was necessary to make it separate as the same buffer is used to log the RTC information on the SD card.
   12. Bug fix for SD_CARD_DETECT pin. Earlier, different pin was used for sd card detection (while developing the code on development board).    
   13. Optimized the configuration settings functions in BMS_ASIC.c and the same is validated. Also, added some more configuration parameters which are 
       listed below.
       a. OV threshold voltage
	   b. OV recovery threshold 
	   c. UV threshold voltage
	   d. UV recovery threshold 
	   f. OV lockout threshold 
	   g. UV lockout threshold 
	   h. End of charge (EOC) threshold
	   i. Internal over temperature threshold
	   j. Disable cell balancing by external MCU
	   k. Internal over temperature recovery threshold
	   l. Internal current gain 
	   m. OV delay timeout 
	   n. UV delay timeout
	   o. Open wiring delay timeout  
       p. Number of cells configuration
   14. If ISL resets to factory default settings in between the operation, configuration parameters are again set in the ISL and it is done with the help of 
       read number of cells from ISL at 20Hz. When ISL resets to factory default settings, number of cells are configured to 3.
   15. All the configuration parameters are defined as the macros in terms of floats. Generalized logic is written for converting the float voltage values 
       into the hex value to set them in the ISL registers.
   16. Bug fix for Last_Charge_Discharge_Status. It was defined as the bool variable in the code and value of 2 was getting assigned initially at the start 
       of the code.
   17. Bug fixes for I2C status for all the functions which are being called at 20Hz. Initially logging was happening only for writing/reading status which 
       was not giving clear idea whether problem was there in writing or reading. Now logic is changed, if written value is not same as that of read value 
	   from the respective register then set the corresponding flag defined in I2C_Status structure.    
   18. Bug fix for calculating the charge and discharge rate. Now code is logging both rates (for charging and for discharging) separately.
   19. If any of the I2C error occurs while querying the data form ISL, then code will restart the I2C for proper functioning. 
   20. Bug fix for incrementing the power up number of BMS (logged in Log summary file). Initially, after calling BMS_Log_Init, logic was incrementing the file 
	   number along with power up number.
   21. Added the logic for updating the RTC date and time sent by AP and tested the same with AP. Now BMS will send acknowledge to AP only when date and time is 
       set in the RTC registers. Bug fix for BMS_Pack data structure member's memory location after connecting the AP.   
   22. Added the debug functionality for the testing purpose which is described as follows.
		1. Character 'A' for RTC DATE AND TIME
		2. Character 'B' for MCU ON TIME
		3. Character 'C' for pack data (all cell voltages,pack voltage and pack current)
		4. Character 'D' for Pack current adjusted and C_D_Accumulated for charging or discharging cycle
		5. Character 'E' for Total capacity, capacity remaining, capacity used
		6. Character 'F' for Charge/discharge cycles and total pack cycles
		7. Character 'G' for Health and I2C error status
		8. Character 'H' for Pack temperature
		9. Character 'I' for Watchdog reset test
		10. Character 'J' for Software reset 
		11. Character 'N' for Power up number and total number of files on SD card
		12. Character '?' to enter the low power mode
        13. Character '$' to enter the normal mode 
	23. Whenever SD card is removed from the slot, it will throw SD_WRITE_ERROR string on debug port (USART). As soon as SD card is inserted in the slot,
	    code will re-init the SD card and start logging from the location in the file at which SD card was removed.
	24. If any error occurs while logging the data on SD card, code will try to write the data for 5 times (250ms) after that it will re-init the log and 
	    will try to start the logging from the location from where it had stopped.    
	25. Bug fix for charge discharge cycles calculation.  	 
	26. Bug fix for calculations which were dependent on the pack current. Initially, calculations were done using raw pack current. Now everything is 
	    calculated from the actual pack current (after applying LPF)
	27. Low power mode is implemented where clock to for all the peripherals is disabled. It is tested by sending the character over USART to enter in to 
	    the low power mode. Another character is used to exit the low power mode.   
			
		       							
### ****BMS_CODE  1.0.1  ****
	Pull request number: 6
			
	Features developed in BMS code for this release:
	1. BMS MCU queries all pack data from ISL at 25Hz.
	2. BMS MCU reads the external switch, if it is short press (<500ms) then MCU will show SOC status on LEDs; If it is long press (>2 seconds and 
	   < 5seconds) then MCU will show SOH status on LEDs. SOC logic is implemented based on capacity remaining calculations taken from AP code. SOC 
	   and SOH will be displayed only upon releasing the pressed switch. One LED for (0-40%),two LEDs(41-80%), three LEDs(81-95%) and all four LEDs (>96%)  
	3. Real time clock(RTC) is implemented and tested. MCU can set and get the date/time from RTC module.
	4. BMS MCU reads the following pack data from ISL
		a. Pack voltage 
		b. Pack Current
		c. All cell voltages 
		d. Pack temperature
		e. All status flags (error and status)
	5. BMS MCU sets the following parameters in ISL EEPROM. The same parameters are read from EEPROM to confirm the configuration settings are OK. 	   
	    a. OV threshold voltage
	    b. OV recovery threshold 
	    c. UV threshold voltage
	    d. UV recovery threshold 
	    f. OV lockout threshold 
	    g. UV lockout threshold 
	    h. End of charge (EOC) threshold
	    i. Internal over temperature threshold
		j. Disable cell balancing by external MCU
		k. Internal over temperature recovery threshold
		l. Internal current gain 
	6. BMS logs all the status and error flags related to ISL on SD card.
	7. BMS MCU estimates the pack capacity remaining based on pack voltage (voltage based capacity section taken from AP code)
	8. BMS MCU resets the code if it stuck somewhere for more than 2 seconds (Watchdog timer functionality). Refreshing the watchdog timer happens at 25Hz.
	9. BMS creates one summary file consisting of power up number and total number of files. It creates new log file after every power up and if the
	   size of the log file exceeds 200MB size then MCU creates the new log file by incrementing the total number of files count in log summary file.
   10. BMS MCU logs all the I2C related error flags on SD card.
   11. If there is any problem in the logging then MCU will re-initialize the SD card and start logging by creating the new file.   
   12. If there is any problem in querying data from ISL then re-initialize the communication bus.
   13. BMS MCU communicates with AP over SMBUS to query all pack data, set the GPS date and time for RTC in BMS and reads AP flight status and log
   	   it on SD card. The packet protocol is mentioned in the BMS_V1_0_2.docx document.
   14. Sleep mode functionality is implemented and tested for following criteria without connecting the cells to the BMS cell port.
   		a. If BMS current consumption is less than 200mA and it remains same for more than 60 seconds then BMS MCU puts the ISL into sleep mode. Once 
		   ISL goes into the sleep mode, MCU also goes into sleep mode after 5 seconds.
		b. As of now, the only mechanism to wake up the MCU is external switch press. To wakeup BMS, BMS_WAKEUP switch must be pressed.
		c. Once MCU is awaken and if current consumption is still less than 200mA for continuous 10 seconds then MCU puts BMS in sleep mode again.
		   and make himself to the sleep mode after 5 seconds
   15. BMS MCU maintains the charge and discharge cycles and logs the same on the SD card. During discharge if current consumption is more than 1A 
       for continuous 5 minutes and if previous cycle of the pack was discharge cycle, then increment the discharge count and log the same on SD card.
	   During charging, if current is more than 1A for continuous 5 minutes and if previous cycle of the pack was discharging, then increment 
	   charge count and log the same on SD card. Pack cycles used is calculated and logged on SD card based on maximum of the charge and 
	   discharge cycles. 
   16. BMS logs the following battery information which is hard coded in the code.
   	 	a. Battery ID 
		b. Battery type
		c. Number of cells 
		d. Battery capacity
		e. Max cell voltage 
		f. Min cell voltage	 	   	       	   
		g. Battery pack cycles 
    17. BMS goes to the debug mode if switch is pressed for more than 5 seconds. To come out of debug mode, switch should be pressed again for more than
    	5 seconds 
					   	   
### ****BMS_CODE  1.0.0  ****
	Pull request number: 4
	
	In this pull request version number is not added. Only added comment in Main.c  
	
    Major Development on API side:
        1. Generalized APIs for SPI,I2C,GPIO,RTC,FLASH,SD_SPI,USART,TIMERS are developed.
		2. All APIs contains the functionality for specific micro controller i.e. STM32L432KC.
		   Developer can use the same APIs for other controllers by adding respective controller's low-level drivers 
        3. All APIs are tested for their basic functionality
		4. This project contains one hardware configuration (Hardware_Config.h) file in which all peripherals 
		   with their remap functionality is included
		5. Timers (TIM2,TIM6 and TIM6) - tested for interrupt as well as polling method
		   PWM channels (TIM1- 4 channels, TIM2- 4 channels, TIM15- 2 channels and TIM16- 1 channel) - tested for various PWM outputs
		   I2C buses (I2C1 and I2C3) - tested along with BMS ISL94203 chip for EEPROM write and read functionality
		   SD_SPI functionality - tested for various file operations like create, delete, open existing and write, seek, create directories etc
		   USART (USART1 and USART2) both are tested for read and write functionality
		   FLASH - Micro controllers internal flash tested for storing and retrieving user data (4Kb sectors for stm32l432kc)
		   RTC - RTC functionality is tested for almost 12Hrs and found it correct without any lag/lead in real time.
		   GPIO - tested for different GPIOs required in application like SD_CARD_SELECT, SD_CHIP_SELECT, BOARD_LED etc
	
	Major Development on application side:
		1. Created the functions for reading all cell voltages, pack voltage, pack temperature, pack current flowing into and out of the pack
		   and tested the same
		2. Added sleep and wakeup functionality for BMS IC and STM MCU and tested the same
		3. Added logging structure to log the BMS data for 1Hz
		4. External switch is tested for wakeup and to show the LED patterns for SOC and SOH(SOC and SOH logic is yet to be implemented)
		5. The logic is implemented to increase the write/erase cycles of the STM's internal flash and tested the same  
		

	Folder Structure: 
		Asteria_BMS:
			1. Binaries:
				This folder contains .elf file used for debugging the code
			2. Includes:
				Path of all libraries (Include folder locations for all libraries) required to build the project
			3. API_Includes:
				Contains all the general API header files which can be used by various micro controllers for application development.
				Following APIs are developed for BMS application
				FLASH_API.h - Contains prototypes for flash driver functions and controller specific flash addresses
				GPIO_API.h  - Contains prototypes for gpio driver functions and various enumerations for gpios
				I2C_API.h   - Contains prototypes for i2c driver functions and enums for I2C numbers for specific controller
				RTC_API.h	- Contains prototypes for rtc driver functions and enumerations for weekdays, months
				SPI_API.h   - Contains prototypes for spi driver functions and number of spi buses enums for specific controller
				TIMER_API.h - Contains prototypes for timer driver functions and enums for TIMER numbers along with PWM channels
							  for specific controller
				USART_API.h - Contains prototypes for usart driver functions and enums for USART numbers for specific controller
			4. API_Source:
				Contains all the general API source files developed for various micro controllers
				Each header file mentioned above has source file associated with it and source files contains the functions definitions
				for respective module
			5. Include: 
				Contains the header files for application development using the API library files
			6. Middlewares :
				Contains higher level fatfs library files 
			7. Source:
				Contains the Source files for application development using API library files
			8. startup,CMSIS:
				These files are controller specific and developer must add these files for respective controllers
			9. Debug:
				Contains objects files generated for all source files after building the project 
			10. Project configuration:
				This folder contains the preference file (keyboard shortcuts and other c/c++ preferences), settings file
				(c/c++ settings, include paths for project files)	
			11. Enter_Character.txt:
				This is the blank file containing only new line and carriage return characters to exit from CLI after downloading the program
	
		Github Issues Resolved:
			1. RTC Module #2
			2. ISL94203 Module #1 	 	 
			   		    			
						

