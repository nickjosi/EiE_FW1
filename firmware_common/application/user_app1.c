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
Choices: HILL_1, HILL_2, ..., HILL_8 */
static u8 UserApp1_u8HillNumber = HILL_1;



/* --- End of values to be edited ------------------------------------------ */



static AntAssignChannelInfoType UserApp1_sMasterChannel0;
static AntAssignChannelInfoType UserApp1_sMasterChannel1;
static AntAssignChannelInfoType UserApp1_sMasterChannel2;
static AntAssignChannelInfoType UserApp1_sMasterChannel3;
  
static u8 UserApp1_au8LcdStartLine1[] =         "ANThill             ";
static u8 UserApp1_au8LcdStartLine2[] =         "Push B0 to open     ";
static u8 UserApp1_au8MasterName[10] =          "ANThill  \0";
static u8 UserApp1_au8LcdInformationMessage[] = "-xx  -xx   -xx  -xx ";


static u8 UserApp1_au8DeviceIdLo[4];
static u8 UserApp1_au8DeviceIdLoLibrary[4][8] = {
  {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67},
  {0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77},
  {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},
  {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97}
};

static u32 UserApp1_u32LCDTimer;


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
  LedOn(LCD_BLUE);
  LedOff(LCD_RED);
  
  /* Update the name message and UserApp1_au8MasterName with team number */
  /* (+48 for ASCII conversion, +1 to get correct number) */
  UserApp1_au8LcdStartLine1[8] = UserApp1_u8HillNumber + 49;
  UserApp1_au8MasterName[8] = UserApp1_u8HillNumber + 49;
   
  /* Update LCD to starting screen. */
  LCDCommand(LCD_CLEAR_CMD);
  
  /* This delay is usually required after LCDCommand during INIT */
  for(u32 i = 0; i < 100000; i++);
  
  LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdStartLine1);
  LCDMessage(LINE2_START_ADDR, UserApp1_au8LcdStartLine2);
  
  
  /* Fill UserApp1_au8DeviceIdLo array */
  for(int i = 0; i < 4; i++)
  {
    UserApp1_au8DeviceIdLo[i] = UserApp1_au8DeviceIdLoLibrary[i][UserApp1_u8HillNumber];
  }
  
  /* Set up the ANT channels that will be used for the task */
    
  /* Master 0 (Channel 0) */
  UserApp1_sMasterChannel0.AntChannel = ANT_CHANNEL_0;
  UserApp1_sMasterChannel0.AntChannelType = CHANNEL_TYPE_MASTER;
  UserApp1_sMasterChannel0.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sMasterChannel0.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;

  UserApp1_sMasterChannel0.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sMasterChannel0.AntDeviceIdLo = UserApp1_au8DeviceIdLo[0];
  UserApp1_sMasterChannel0.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sMasterChannel0.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sMasterChannel0.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sMasterChannel0.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sMasterChannel0.AntNetwork = ANT_NETWORK_DEFAULT;
  
  /* Master 1 (Channel 1) */
  UserApp1_sMasterChannel1.AntChannel = ANT_CHANNEL_1;
  UserApp1_sMasterChannel1.AntChannelType = CHANNEL_TYPE_MASTER;
  UserApp1_sMasterChannel1.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sMasterChannel1.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  UserApp1_sMasterChannel1.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sMasterChannel1.AntDeviceIdLo = UserApp1_au8DeviceIdLo[1];
  UserApp1_sMasterChannel1.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sMasterChannel1.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sMasterChannel1.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sMasterChannel1.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sMasterChannel1.AntNetwork = ANT_NETWORK_DEFAULT;
  
  /* Master 2 (Channel 2) */
  UserApp1_sMasterChannel2.AntChannel = ANT_CHANNEL_2;
  UserApp1_sMasterChannel2.AntChannelType = CHANNEL_TYPE_MASTER;
  UserApp1_sMasterChannel2.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sMasterChannel2.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  UserApp1_sMasterChannel2.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sMasterChannel2.AntDeviceIdLo = UserApp1_au8DeviceIdLo[2];
  UserApp1_sMasterChannel2.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sMasterChannel2.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sMasterChannel2.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sMasterChannel2.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sMasterChannel2.AntNetwork = ANT_NETWORK_DEFAULT;
  
  /* Master 3 (Channel 3) */
  UserApp1_sMasterChannel3.AntChannel = ANT_CHANNEL_3;
  UserApp1_sMasterChannel3.AntChannelType = CHANNEL_TYPE_MASTER;
  UserApp1_sMasterChannel3.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_DEFAULT;
  UserApp1_sMasterChannel3.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_DEFAULT;
  
  UserApp1_sMasterChannel3.AntDeviceIdHi = DEVICE_ID_HI;
  UserApp1_sMasterChannel3.AntDeviceIdLo = UserApp1_au8DeviceIdLo[3];
  UserApp1_sMasterChannel3.AntDeviceType = EIE_DEVICE_TYPE;
  UserApp1_sMasterChannel3.AntTransmissionType = EIE_TRANS_TYPE;
  
  UserApp1_sMasterChannel3.AntFrequency = ANT_FREQUENCY_DEFAULT;
  UserApp1_sMasterChannel3.AntTxPower = ANT_TX_POWER_DEFAULT;
  UserApp1_sMasterChannel3.AntNetwork = ANT_NETWORK_DEFAULT;
  
  
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    UserApp1_sMasterChannel0.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sMasterChannel1.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sMasterChannel2.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    UserApp1_sMasterChannel3.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
  
  /* Queue configuration of Master channel 0 */
  AntAssignChannel(&UserApp1_sMasterChannel0);
  UserApp1_u32Timeout = G_u32SystemTime1ms;
    
  /* If good initialization, set state to AntConifgureMaster0 */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_AntConfigureMaster0;
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
@fn static void UserApp1SM_AntConfigureMaster0(void)

@brief Wait for Master 0 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureMaster0(void)
{
  /* Wait for the ANT Master channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CONFIGURED)
  {
    DebugPrintf("Master 0 channel configured\n\n\r");
    
    /* Queue configuration of Master 1 channel */
    AntAssignChannel(&UserApp1_sMasterChannel1);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureMaster1;
    
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Mast0 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureMaster0() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureMaster1(void)

@brief Wait for Master 1 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureMaster1(void)
{
  /* Wait for the ANT Master channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CONFIGURED)
  {
    DebugPrintf("Master 1 channel configured\n\n\r");
    
    /* Queue configuration of Master 2 channel */
    AntAssignChannel(&UserApp1_sMasterChannel2);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureMaster2;
    
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Mast1 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureMaster1() */
    

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureMaster2(void)

@brief Wait for Master 2 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureMaster2(void)
{
  /* Wait for the ANT Master channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_CONFIGURED)
  {
    DebugPrintf("Master 2 channel configured\n\n\r");
    
    /* Queue configuration of Master 3 channel */
    AntAssignChannel(&UserApp1_sMasterChannel3);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureMaster3;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Mast2 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureMaster2() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_AntConfigureMaster3(void)

@brief Wait for Master 3 channel configuration to be completed.
*/
static void UserApp1SM_AntConfigureMaster3(void)
{
  /* Wait for the ANT Master channel to be configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_3) == ANT_CONFIGURED)
  {
    DebugPrintf("Master 3 channel configured\n\n\r");
    
    /* Update the broadcast message data to send the user's name the go to Idle */
    AntQueueBroadcastMessage(ANT_CHANNEL_0, UserApp1_au8MasterName);
    AntQueueBroadcastMessage(ANT_CHANNEL_1, UserApp1_au8MasterName);
    AntQueueBroadcastMessage(ANT_CHANNEL_2, UserApp1_au8MasterName);
    AntQueueBroadcastMessage(ANT_CHANNEL_3, UserApp1_au8MasterName);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Mast3 config failed");
    UserApp1_StateMachine = UserApp1SM_Error;    
  }

} /* end UserApp1SM_AntConfigureMaster3() */
    

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_Idle(void)

@brief Wait for the user to press button press to start search mode
*/
static void UserApp1SM_Idle(void)
{
  /* BUTTON0 opens the channels */ 
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    /* Queue the Channel Open messages and then go to wait state */
    AntOpenChannelNumber(ANT_CHANNEL_0);
    AntOpenChannelNumber(ANT_CHANNEL_1);
    AntOpenChannelNumber(ANT_CHANNEL_2);
    AntOpenChannelNumber(ANT_CHANNEL_3);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_OpeningChannels;    
  }
  
  /* Check for other button presses to avoid any unintended events */
  if(WasButtonPressed(BUTTON1))
    ButtonAcknowledge(BUTTON1);
  if(WasButtonPressed(BUTTON2))
    ButtonAcknowledge(BUTTON2);
  if(WasButtonPressed(BUTTON3))
    ButtonAcknowledge(BUTTON3);
  
} /* end UserApp1SM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_OpeningChannels(void)

@brief Wait for ANT channels to open
*/
static void UserApp1SM_OpeningChannels(void)
{
  /* Ensure that the channels have opened */
  if( (AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_OPEN) &&
      (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_OPEN) &&
      (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_OPEN) &&
      (AntRadioStatusChannel(ANT_CHANNEL_3) == ANT_OPEN) )
  {
    /* Update LCD and go to main Radio monitoring state */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdInformationMessage);
    LCDMessage(LINE2_START_ADDR, UserApp1_au8MasterName);
    
    UserApp1_u32LCDTimer = G_u32SystemTime1ms;
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
  u8 u8EventCode;
  
  static u8 u32MessageCounter0 = 0;
  static u8 u32MessageCounter1 = 0;
  static u8 u32MessageCounter2 = 0;
  static u8 u32MessageCounter3 = 0;
  
  static s8 s8RssiChannel0 = -99;
  static s8 s8RssiChannel1 = -99;
  static s8 s8RssiChannel2 = -99;
  static s8 s8RssiChannel3 = -99;

  
  /* Monitor ANT messages: looking for any incoming messages
  that indicates a matching device has been located. */
  if( AntReadAppMessageBuffer() )
  {
    /* Check the message class to determine how to process the message */
    
    /* --- TICK MESSAGE ------------------------------------------------------*/
    if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      /* Get the EVENT code from the ANT_TICK message */ 
      u8EventCode = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
      
      /* Channel 0 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 0)
      {
        switch (u8EventCode)
        {
          case EVENT_TX:
          {
            /* Keep track of message and update LCD if too many messages have been sent
            without any being received.  The counter is cleared whenever the Master channel
            receives a message from the Slave it is trying to talk to. */
            u32MessageCounter0++;
            if(u32MessageCounter0 >= ALLOWED_MISSED_MESSAGES)
            {
              s8RssiChannel0 = DBM_LEVEL1;
              LedOff(PURPLE);
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM0 + 1] = 'x';
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM0 + 2] = 'x';
            }
            break;
          }
          default:
          {
            DebugPrintf("Master0 unhandled event\n\n\r");
            break;
          }
        } /* end switch u8EventCode */
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 0) */
      
      /* Channel 1 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 1)
      {
        switch (u8EventCode)
        {
          case EVENT_TX:
          {
            /* Keep track of message and update LCD if too many messages have been sent
            without any being received.  The counter is cleared whenever the Master channel
            receives a message from the Slave it is trying to talk to. */
            u32MessageCounter1++;
            if(u32MessageCounter1 >= ALLOWED_MISSED_MESSAGES)
            {
              s8RssiChannel1 = DBM_LEVEL1;
              LedOff(CYAN);
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM1 + 1] = 'x';
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM1 + 2] = 'x';
            }
            break;
          }
          default:
          {
            DebugPrintf("Master1 unhandled event\n\n\r");
            break;
          }
        } /* end switch u8EventCode */
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 1) */
      
      /* Channel 2 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 2)
      {
        switch (u8EventCode)
        {
          case EVENT_TX:
          {
            /* Keep track of message and update LCD if too many messages have been sent
            without any being received.  The counter is cleared whenever the Master channel
            receives a message from the Slave it is trying to talk to. */
            u32MessageCounter2++;
            if(u32MessageCounter2 >= ALLOWED_MISSED_MESSAGES)
            {
              s8RssiChannel2 = DBM_LEVEL1;
              LedOff(YELLOW);
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM2 + 1] = 'x';
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM2 + 2] = 'x';
            }
            break;
          }
          default:
          {
            DebugPrintf("Master2 unhandled event\n\n\r");
            break;
          }
        } /* end switch u8EventCode */
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 2) */
      
      /* Channel 3 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 3)
      {
        switch (u8EventCode)
        {
          case EVENT_TX:
          {
            /* Keep track of message and update LCD if too many messages have been sent
            without any being received.  The counter is cleared whenever the Master channel
            receives a message from the Slave it is trying to talk to. */
            u32MessageCounter3++;
            if(u32MessageCounter3 >= ALLOWED_MISSED_MESSAGES)
            {
              s8RssiChannel3 = DBM_LEVEL1;
              LedOff(RED);
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM3 + 1] = 'x';
              UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM3 + 2] = 'x';
            }
            break;
          }
          default:
          {
            DebugPrintf("Master3 unhandled event\n\n\r");
            break;
          }
        } /* end switch u8EventCode */
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 3) */
    } /* end if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    
    /* --- end TICK MESSAGE --------------------------------------------------*/

    
    /* --- DATA MESSAGE ----------------------------------------------------- */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* Channel 0 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 0)
      {
        /* Reset the message counter */
        u32MessageCounter0 = 0;
        
        /* Channel 0 is red (but don't touch blue or green) */
        LedOn(PURPLE);
        
        /* Record RSSI level and update LCD message */
        s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;
        AntGetdBmAscii(s8RssiChannel0, &UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM0]);
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 0) */
      
      /* Channel 1 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 1)
      {
        /* Reset the message counter */
        u32MessageCounter1 = 0;
        
        /* Channel 0 is red (but don't touch blue or green) */
        LedOn(CYAN);
        
        /* Record RSSI level and update LCD message */
        s8RssiChannel1 = G_sAntApiCurrentMessageExtData.s8RSSI;
        AntGetdBmAscii(s8RssiChannel1, &UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM1]);
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 1) */
      
      /* Channel 2 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 2)
      {
        /* Reset the message counter */
        u32MessageCounter2 = 0;
        
        /* Channel 0 is red (but don't touch blue or green) */
        LedOn(YELLOW);
        
        /* Record RSSI level and update LCD message */
        s8RssiChannel2 = G_sAntApiCurrentMessageExtData.s8RSSI;
        AntGetdBmAscii(s8RssiChannel2, &UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM2]);
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 2) */
      
      /* Channel 3 */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 3)
      {
        /* Reset the message counter */
        u32MessageCounter3 = 0;
        
        /* Channel 0 is red (but don't touch blue or green) */
        LedOn(RED);
        
        /* Record RSSI level and update LCD message */
        s8RssiChannel3 = G_sAntApiCurrentMessageExtData.s8RSSI;
        AntGetdBmAscii(s8RssiChannel3, &UserApp1_au8LcdInformationMessage[INDEX_RSSI_DBM3]);
      } /* end if(G_sAntApiCurrentMessageExtData.u8Channel == 3) */
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    
    /* --- end DATA MESSAGE --------------------------------------------------*/
    
    
    /* Make sure LCD has the correct message every time >500ms have passed */
    if((G_u32SystemTime1ms - UserApp1_u32LCDTimer) > 500)
    {
      LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdInformationMessage);
      UserApp1_u32LCDTimer = G_u32SystemTime1ms;
    }
    
  } /* end if( AntReadAppMessageBuffer() )*/
  
    
  /* Watch for button press to turn off radio */
  if(WasButtonPressed(BUTTON3))
  {
    /* Ack the button and turn off LCD backlight */
    ButtonAcknowledge(BUTTON3);
    
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
    AntCloseChannelNumber(ANT_CHANNEL_0);
    AntCloseChannelNumber(ANT_CHANNEL_1);
    AntCloseChannelNumber(ANT_CHANNEL_2);
    AntCloseChannelNumber(ANT_CHANNEL_3);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_ClosingChannels;    
  }
  
  /* Check for other button presses to avoid any unintended events */
  if(WasButtonPressed(BUTTON0))
    ButtonAcknowledge(BUTTON0);
  if(WasButtonPressed(BUTTON1))
    ButtonAcknowledge(BUTTON1);
  if(WasButtonPressed(BUTTON2))
    ButtonAcknowledge(BUTTON2);
  

} /* end UserApp1SM_RadioActive() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_ClosingChannels(void)

@brief Wait for ANT channels to close.
*/
static void UserApp1SM_ClosingChannels(void)
{
  /* Ensure that the channel has closed */
  if((AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CLOSED) &&
     (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CLOSED) &&
     (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_CLOSED) &&
     (AntRadioStatusChannel(ANT_CHANNEL_3) == ANT_CLOSED))
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
