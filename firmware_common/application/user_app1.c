/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief Application to search and be search using two ANT channels.  

It is expected that another device is running this code searching for you!  
The other device should be programmed with the same Device ID.  

The user presses BUTTON0 to open the ANT channels and begin searching.  Both a Master
and Slave channel are opened to ensure that devices will find each other.  During search,
the RSSI level of any received message will be reported on the LCD.  If you get close enough
to the matching systems and the RSSI level is strong enough, the other person's name
will be shown on your LCD.

The board's LEDs are used to indicate the relative signal strength.  RED indicates
that no message are being received.  As signal strength increases, more LEDs will turn on.
The White LED will be lit at maximum RSSI (the same level that must be reached to display
the other person's name).

Reception of a message on the Master's channel will also activate the BLUE LCD backlight
Reception of a message on the Slave's channel will also activate the GREEN LCD backlight


------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!<@brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!<@brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

/* Globals for passing data from the ANT application to the API */
extern u32 G_u32AntApiCurrentMessageTimeStamp;                            // From ant_api.c
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;            // From ant_api.c
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  // From ant_api.c
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                // From ant_api.c


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;                 /*!< @brief The state machine function pointer */
static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */



/* --- Values to be edited ------------------------------------------------- */

/* Enter team number below. 
Choices: TEAM_1, TEAM_2, TEAM_3, TEAM_4 */
static u8 UserApp1_u8TeamNumber = TEAM_1;

/* Enter the 4-board sequence that corresponds to the team above.
Choices: BOARD_1, BOARD_2, ..., BOARD_8 */
static u8 UserApp1_au8BoardSequence[] = {BOARD_1, BOARD_2, BOARD_3, BOARD_4};

/* Enter the 4 clues in order. 
Choices: A colour or row #. */
static u8 UserApp1_au8Clue1[] = "Blue.";
static u8 UserApp1_au8Clue2[] = "Row 5.";
static u8 UserApp1_au8Clue3[] = "Orange.";
static u8 UserApp1_au8Clue4[] = "Yellow.";

/* --- End of values to be edited ------------------------------------------ */



static AntAssignChannelInfoType UserApp1_sSlaveChannel0;
static AntAssignChannelInfoType UserApp1_sSlaveChannel1;
static AntAssignChannelInfoType UserApp1_sSlaveChannel2;
static AntAssignChannelInfoType UserApp1_sSlaveChannel3;
  
static u8 UserApp1_au8LcdStartLine1[] =         "Hi Seeker           ";
static u8 UserApp1_au8LcdStartLine2[] =         "Push B0-B3 to search";
static u8 UserApp1_au8SlaveName[9] =            "Seeker  \0";
static u8 UserApp1_au8LcdInformationMessage[] = "RSSI:-xx dBm        ";


static u8 UserApp1_au8DeviceIdLo[4];
static u8 UserApp1_au8DeviceIdLoLibrary[4][8] = {
  {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67},
  {0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77},
  {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},
  {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97}
};

static u8 UserApp1_u8ClueNum;
static u32 UserApp1_u32SearchingTime;


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  /* Start with all LEDs off except green blacklight */
  LedOff(RED);
  LedOff(ORANGE);
  LedOff(YELLOW);
  LedOff(GREEN);
  LedOff(CYAN);
  LedOff(BLUE);
  LedOff(PURPLE);
  LedOff(WHITE);
  LedOn(LCD_GREEN);
  LedOff(LCD_BLUE);
  LedOff(LCD_RED);
  
  /* Update the name message and UserApp1_au8SlaveName with team number */
  /* (+48 for ASCII conversion, +1 to get correct number) */
  UserApp1_au8LcdStartLine1[10] = UserApp1_u8TeamNumber + 49;
  UserApp1_au8SlaveName[7] = UserApp1_u8TeamNumber + 49;
   
  /* Update LCD to starting screen. */
  LCDCommand(LCD_CLEAR_CMD);
  
  /* This delay is usually required after LCDCommand during INIT */
  for(u32 i = 0; i < 100000; i++);
  
  LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdStartLine1);
  LCDMessage(LINE2_START_ADDR, UserApp1_au8LcdStartLine2);
  
  
  /* Fill UserApp1_au8DeviceIdLo array */
  for(int i = 0; i < 4; i++)
  {
    UserApp1_au8DeviceIdLo[i] = 
      UserApp1_au8DeviceIdLoLibrary[UserApp1_u8TeamNumber][UserApp1_au8BoardSequence[i]];
  }
  
  /* Set up the ANT channels that will be used for the task */
    
  /* Slave 0 (Channel 0) */
  UserApp1_sSlaveChannel0.AntChannel = ANT_CHANNEL_0;
  UserApp1_sSlaveChannel0.AntChannelType = CHANNEL_TYPE_SLAVE;
  UserApp1_sSlaveChannel0.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sSlaveChannel0.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;

  UserApp1_sSlaveChannel0.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sSlaveChannel0.AntDeviceIdLo = UserApp1_au8DeviceIdLo[0];
  UserApp1_sSlaveChannel0.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sSlaveChannel0.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sSlaveChannel0.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sSlaveChannel0.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sSlaveChannel0.AntNetwork = ANT_NETWORK_DEFAULT;
  
  /* Slave 1 (Channel 1) */
  UserApp1_sSlaveChannel1.AntChannel = ANT_CHANNEL_1;
  UserApp1_sSlaveChannel1.AntChannelType = CHANNEL_TYPE_SLAVE;
  UserApp1_sSlaveChannel1.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sSlaveChannel1.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  UserApp1_sSlaveChannel1.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sSlaveChannel1.AntDeviceIdLo = UserApp1_au8DeviceIdLo[1];
  UserApp1_sSlaveChannel1.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sSlaveChannel1.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sSlaveChannel1.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sSlaveChannel1.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sSlaveChannel1.AntNetwork = ANT_NETWORK_DEFAULT;
  
  /* Slave 2 (Channel 2) */
  UserApp1_sSlaveChannel2.AntChannel = ANT_CHANNEL_2;
  UserApp1_sSlaveChannel2.AntChannelType = CHANNEL_TYPE_SLAVE;
  UserApp1_sSlaveChannel2.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sSlaveChannel2.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  UserApp1_sSlaveChannel2.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sSlaveChannel2.AntDeviceIdLo = UserApp1_au8DeviceIdLo[2];
  UserApp1_sSlaveChannel2.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sSlaveChannel2.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sSlaveChannel2.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sSlaveChannel2.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sSlaveChannel2.AntNetwork = ANT_NETWORK_DEFAULT;
  
  /* Slave 3 (Channel 3) */
  UserApp1_sSlaveChannel3.AntChannel = ANT_CHANNEL_3;
  UserApp1_sSlaveChannel3.AntChannelType = CHANNEL_TYPE_SLAVE;
  UserApp1_sSlaveChannel3.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sSlaveChannel3.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  UserApp1_sSlaveChannel3.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sSlaveChannel3.AntDeviceIdLo = UserApp1_au8DeviceIdLo[3];
  UserApp1_sSlaveChannel3.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sSlaveChannel3.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sSlaveChannel3.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sSlaveChannel3.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sSlaveChannel3.AntNetwork = ANT_NETWORK_DEFAULT;
  
  
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    UserApp1_sSlaveChannel0.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sSlaveChannel1.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sSlaveChannel2.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sSlaveChannel3.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
  
  /* Queue configuration of Slave channel 0 */
  AntAssignChannel(&UserApp1_sSlaveChannel0);
  UserApp1_u32Timeout = G_u32SystemTime1ms;
    
  /* If good initialization, set state to AntConifgureSlave0 */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_AntConfigureSlave0;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureSlave0(void)

@brief Wait for Slave 0 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureSlave0(void)
{
  /* Wait for the ANT Slave channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CONFIGURED)
  {
    DebugPrintf("Slave 0 channel configured\n\n\r");
    
    /* Queue configuration of Slave 1 channel */
    AntAssignChannel(&UserApp1_sSlaveChannel1);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureSlave1;
    
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Slave0 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureSlave0() */
    

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureSlave1(void)

@brief Wait for Slave 1 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureSlave1(void)
{
  /* Wait for the ANT Slave channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CONFIGURED)
  {
    DebugPrintf("Slave 1 channel configured\n\n\r");
    
    /* Queue configuration of Slave 2 channel */
    AntAssignChannel(&UserApp1_sSlaveChannel2);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureSlave2;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Slave1 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureSlave1() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureSlave2(void)

@brief Wait for Slave 2 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureSlave2(void)
{
  /* Wait for the ANT Slave channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_CONFIGURED)
  {
    DebugPrintf("Slave 2 channel configured\n\n\r");
    
    /* Queue configuration of Slave 3 channel */
    AntAssignChannel(&UserApp1_sSlaveChannel3);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureSlave3;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Slave2 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureSlave2() */
    

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureSlave3(void)

@brief Wait for Slave 3 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureSlave3(void)
{
  /* Wait for the ANT Slave channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_3) == ANT_CONFIGURED)
  {
    DebugPrintf("Slave 3 channel configured\n\n\r");
    
    /* Update the broadcast message data to send the user's name the go to Idle */
    AntQueueBroadcastMessage(ANT_CHANNEL_0, UserApp1_au8SlaveName);
    AntQueueBroadcastMessage(ANT_CHANNEL_1, UserApp1_au8SlaveName);
    AntQueueBroadcastMessage(ANT_CHANNEL_2, UserApp1_au8SlaveName);
    AntQueueBroadcastMessage(ANT_CHANNEL_3, UserApp1_au8SlaveName);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Slave3 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureSlave3() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_Idle(void)

@brief Wait for the user to press button press to start search mode
*/
static void UserApp1SM_Idle(void)
{
  
  /* BUTTON0 opens channel 0 for Clue 1 */ 
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    UserApp1_u8ClueNum = CLUE_1;
    
    /* Queue the Channel Open messages and then go to wait state */
    AntOpenChannelNumber(ANT_CHANNEL_0);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_OpeningChannels;    
  }
  
  /* BUTTON1 opens channel 1 for Clue 2 */ 
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    UserApp1_u8ClueNum = CLUE_2;
    
    /* Queue the Channel Open messages and then go to wait state */
    AntOpenChannelNumber(ANT_CHANNEL_1);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_OpeningChannels;    
  }
  
  /* BUTTON2 opens channel 2 for Clue 3 */ 
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    UserApp1_u8ClueNum = CLUE_3;
    
    /* Queue the Channel Open messages and then go to wait state */
    AntOpenChannelNumber(ANT_CHANNEL_2);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_OpeningChannels;    
  }
  
  /* BUTTON3 opens channel 3 for Clue 4 */ 
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    UserApp1_u8ClueNum = CLUE_4;
    
    /* Queue the Channel Open messages and then go to wait state */
    AntOpenChannelNumber(ANT_CHANNEL_3);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_OpeningChannels;    
  }

} /* end UserApp1SM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_OpeningChannels(void)

@brief Wait for ANT channels to open
*/
static void UserApp1SM_OpeningChannels(void)
{
  /* Ensure that the correct channel has opened */
  if( 
     ((UserApp1_u8ClueNum == CLUE_1) &&
      (AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_OPEN))
      ||
     ((UserApp1_u8ClueNum == CLUE_2) &&
      (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_OPEN))
      ||
     ((UserApp1_u8ClueNum == CLUE_3) &&
      (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_OPEN))
      ||
     ((UserApp1_u8ClueNum == CLUE_4) &&
      (AntRadioStatusChannel(ANT_CHANNEL_3) == ANT_OPEN)) )
  {    
    /* Update LCD and go to main Radio monitoring state */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "RSSI:-xx dBm");
    if(UserApp1_u8ClueNum == CLUE_1)
      LCDMessage(LINE2_START_ADDR, "Clue 1:");
    else if(UserApp1_u8ClueNum == CLUE_2)
      LCDMessage(LINE2_START_ADDR, "Clue 2:");
    else if(UserApp1_u8ClueNum == CLUE_3)
      LCDMessage(LINE2_START_ADDR, "Clue 3:");
    else if(UserApp1_u8ClueNum == CLUE_4)
      LCDMessage(LINE2_START_ADDR, "Clue 4:");
        
    UserApp1_u32SearchingTime = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_RadioActive;    
  }

  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Channel open failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_OpeningChannels() */
    

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_RadioActive(void)

@brief Monitor incoming messages.  Watch signal strength of messages and update LCD and LEDs.
*/
static void UserApp1SM_RadioActive(void)
{
  LedNumberType aeLedDisplayLevels[] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE};
  s8 as8dBmLevels[] = {DBM_LEVEL1, DBM_LEVEL2, DBM_LEVEL3, DBM_LEVEL4, 
                       DBM_LEVEL5, DBM_LEVEL6, DBM_LEVEL7, DBM_LEVEL8};
  u8 u8EventCode;
    
  static s8 s8Rssi = -99;
  static s8 s8StrongestRssi = -99;
  
  /* Monitor ANT messages: looking for any incoming messages
  that indicates a matching device has been located. */
  if( AntReadAppMessageBuffer() )
  {
    UserApp1_u32SearchingTime = G_u32SystemTime1ms;
    
    /* Check the message class to determine how to process the message */
    if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      /* Get the EVENT code from the ANT_TICK message */ 
      u8EventCode = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
      
      /* Check the Event code and respond */
        switch (u8EventCode)
        {
          case EVENT_RX_FAIL_GO_TO_SEARCH:
          {
            s8Rssi = DBM_LEVEL1;
            LedOff(LCD_RED);
            UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM + 1] = 'x';
            UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM + 2] = 'x';
            break;
          }
          
          default:
          {
            DebugPrintf("Slave unhandled event\n\n\r");
            break;
          }
        } /* end switch u8EventCode */      
    } /* end if(G_eAntApiCurrentMessageClass == ANT_TICK) */

    
    /* Check for DATA messages */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* When the slave receives a message, queue a response message */
        //AntQueueBroadcastMessage(ANT_CHANNEL_1, UserApp1_au8MasterName);

        /* Channel 1 is Blue (but don't touch red or green) */
        LedOn(LCD_RED);

        /* Record RSSI level and update LCD message */
        s8Rssi = G_sAntApiCurrentMessageExtData.s8RSSI;
        AntGetdBmAscii(s8Rssi, &UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM]);
        
      
      
      /* Read and display user name if level is high enough */
      if(s8StrongestRssi > DBM_MAX_LEVEL)
      {
        if(UserApp1_u8ClueNum == CLUE_1)        
          LCDMessage(ADDRESS_LCD_CLUE, UserApp1_au8Clue1);    
        else if(UserApp1_u8ClueNum == CLUE_2)
          LCDMessage(ADDRESS_LCD_CLUE, UserApp1_au8Clue2);    
        else if(UserApp1_u8ClueNum == CLUE_3)
          LCDMessage(ADDRESS_LCD_CLUE, UserApp1_au8Clue3);    
        else if(UserApp1_u8ClueNum == CLUE_4)
          LCDMessage(ADDRESS_LCD_CLUE, UserApp1_au8Clue4);    
      }
      else
      {
        /* Otherwise clear the name area */
        LCDClearChars(ADDRESS_LCD_CLUE, 8);
      }
      
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */

    /* Make sure LCD has the current message - this should happen infrequently
    enough to no cause problems, but if that's untrue this needs to be throttled back */
    LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdInformationMessage);

    
    /* Update the strongest signal being received */
    s8StrongestRssi = s8Rssi;

    /* Loop through all of the levels to check which LEDs to turn on */
    for(u8 i = 0; i < NUM_DBM_LEVELS; i++)
    {
      if(s8StrongestRssi > as8dBmLevels[i])
      {
        LedOn(aeLedDisplayLevels[i]);
      }
      else
      {
        LedOff(aeLedDisplayLevels[i]);
      }
    }
    
  } /* end if( AntReadAppMessageBuffer() )*/

  if(G_u32SystemTime1ms - UserApp1_u32SearchingTime > 500)
  {
    s8Rssi = DBM_LEVEL1;
    s8StrongestRssi = DBM_LEVEL1;
    UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM + 1] = 'x';
    UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM + 2] = 'x';
    
    for(u8 i = 0; i < 8; i++)
    {
      LedOff(aeLedDisplayLevels[i]);
    }
    LedOff(LCD_RED);
    LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdInformationMessage);
    
    UserApp1_u32SearchingTime = G_u32SystemTime1ms;
  }
  
  /* Watch for button press to turn off radio */
  if(WasButtonPressed(BUTTON3))
  {
    /* Ack the button and turn off LCD backlight */
    ButtonAcknowledge(BUTTON3);
    LedOff(LCD_RED);
    LedOff(LCD_BLUE);
    
    s8Rssi = DBM_LEVEL1;
    s8StrongestRssi = DBM_LEVEL1;
    UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM + 1] = 'x';
    UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM + 2] = 'x';
    
    /* Make sure all LEDs are off */
    for(u8 i = 0; i < 8; i++)
    {
      LedOff(aeLedDisplayLevels[i]);
    }
    
    /* Update LCD back to the starting screen */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdStartLine1);
    LCDMessage(LINE2_START_ADDR, UserApp1_au8LcdStartLine2);
    
      
    /* Queue request to close the channel */
    if(UserApp1_u8ClueNum == CLUE_1)
      AntCloseChannelNumber(ANT_CHANNEL_0);
    if(UserApp1_u8ClueNum == CLUE_2)
      AntCloseChannelNumber(ANT_CHANNEL_1);
    if(UserApp1_u8ClueNum == CLUE_3)
      AntCloseChannelNumber(ANT_CHANNEL_2);
    if(UserApp1_u8ClueNum == CLUE_4)
      AntCloseChannelNumber(ANT_CHANNEL_3);

    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_ClosingChannels;    
  }

} /* end UserApp1SM_RadioActive() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_ClosingChannels(void)

@brief Wait for ANT channels to close.
*/
static void UserApp1SM_ClosingChannels(void)
{
  /* Ensure that the channel has closed */
  if( 
     ((UserApp1_u8ClueNum == CLUE_1) &&
      (AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CLOSED))
      ||
     ((UserApp1_u8ClueNum == CLUE_2) &&
      (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CLOSED))
      ||
     ((UserApp1_u8ClueNum == CLUE_3) &&
      (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_CLOSED))
      ||
     ((UserApp1_u8ClueNum == CLUE_4) &&
      (AntRadioStatusChannel(ANT_CHANNEL_3) == ANT_CLOSED)) )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;    
  }

  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Channel close failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_ClosingChannels() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
