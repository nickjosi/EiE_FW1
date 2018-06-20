/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_AntConfigureSlave0(void);
static void UserApp1SM_AntConfigureSlave1(void);
static void UserApp1SM_AntConfigureSlave2(void);
static void UserApp1SM_AntConfigureSlave3(void);
static void UserApp1SM_AntConfigureSlave4(void);
static void UserApp1SM_AntConfigureSlave5(void);
static void UserApp1SM_AntConfigureSlave6(void);
static void UserApp1SM_AntConfigureSlave7(void);
static void UserApp1SM_OpeningChannels(void);
static void UserApp1SM_ClosingChannels(void);
static void UserApp1SM_RadioActive(void);

static void UserApp1SM_Idle(void);    
static void UserApp1SM_Error(void);         


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
                         
#define TEAM_1          (u8)0
#define TEAM_2          (u8)1
#define TEAM_3          (u8)2
#define TEAM_4          (u8)3
                          
#define BOARD_1         (u8)0
#define BOARD_2         (u8)1
#define BOARD_3         (u8)2
#define BOARD_4         (u8)3
#define BOARD_5         (u8)4
#define BOARD_6         (u8)5
#define BOARD_7         (u8)6
#define BOARD_8         (u8)7

#define CLUE_1          (u8)0
#define CLUE_2          (u8)1
#define CLUE_3          (u8)2
#define CLUE_4          (u8)3


#define INDEX_RSSI_DBM             (u8)5                      /*!< @brief Position in au8LcdInformationMessage to write the RSSI level */
#define ADDRESS_LCD_CLUE           (u8)(LINE2_START_ADDR + 8) /*!< @brief LCD address where to write Slave's name */
#define INDEX_CHANNEL_NUM          (u8)10

#define ALLOWED_MISSED_MESSAGES    (u32)8                     /*!< @brief Number of message cycles allowed before the Master decides no Slave is listening */

#define DBM_LEVEL1          (s8)-99
#define DBM_LEVEL2          (s8)-84
#define DBM_LEVEL3          (s8)-76
#define DBM_LEVEL4          (s8)-69
#define DBM_LEVEL5          (s8)-63
#define DBM_LEVEL6          (s8)-58
#define DBM_LEVEL7          (s8)-54
#define DBM_LEVEL8          (s8)-51
#define DBM_MAX_LEVEL       DBM_LEVEL8
#define NUM_DBM_LEVELS      (u8)8

#define EIE_DEVICE_TYPE     (u8)50
#define EIE_TRANS_TYPE      (u8)50
#define DEVICE_ID_HI        (u8)0x11



#endif /* __USER_APP1_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
