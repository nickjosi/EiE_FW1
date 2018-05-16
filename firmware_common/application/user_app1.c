/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

static bool UserApp1_bInstrTBE;
static bool UserApp1_bAB;
static bool UserApp1_bCD;
static bool UserApp1_bEF;

static u8 UserApp1_au8Sequence[6];


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{ 
  UserApp1_bInstrTBE = FALSE;
  UserApp1_bAB = FALSE;
  UserApp1_bCD = FALSE;
  UserApp1_bEF = FALSE;
  for(int i = 0; i < 6; i++)
  {
    UserApp1_au8Sequence[i] = 'x';
  }
  
  AllLedsOff();
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
  UpdateLCD();
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Config;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
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


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/* --- Function AllLedsOff() --- */
void AllLedsOff(void)
{
  LedPWM(WHITE, LED_PWM_0);
  LedPWM(PURPLE, LED_PWM_0);
  LedPWM(BLUE, LED_PWM_0);
  LedPWM(CYAN, LED_PWM_0);
  LedPWM(GREEN, LED_PWM_0);
  LedPWM(YELLOW, LED_PWM_0);
  LedPWM(ORANGE,LED_PWM_0);
  LedPWM(RED, LED_PWM_0);
}
/* --- end AllLedsOff() --- */

/* ---Function UpdateLCD() --- */
void UpdateLCD(void)
{
  u8 au8MainMenu1[] = "Unscramble          ";
  u8 au8MainMenu2[] = "A,B   C,D   E,F    0";
  u8 au8AB[] =        "A     B             ";
  
  LCDMessage(LINE1_START_ADDR, au8MainMenu1);
  LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8Sequence);
  
  if(!UserApp1_bInstrTBE)
  {
    LCDMessage(LINE2_START_ADDR, au8MainMenu2);
  }
  else
  {
    if(UserApp1_bAB)
    {
      LCDMessage(LINE2_START_ADDR, au8AB);
    }
  }
}
/* --- end UpdateLCD() --- */

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Config(void)
{
  UserApp1_StateMachine = UserApp1SM_Unactivated;
} /* end UserApp1SM_Config() */
 


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Unactivated(void)
{
  UserApp1_StateMachine = UserApp1SM_Activated;
} /* end UserApp1SM_Unactivated() */
  


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Activated(void)
{
  /* Button 0 */
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    if(!UserApp1_bInstrTBE)
    {
      UserApp1_bInstrTBE = TRUE;
      UserApp1_bAB = TRUE;
      
      UpdateLCD();
    }
    
    else
    {
      if(UserApp1_bAB)
      {
        UserApp1_bAB = FALSE;
      }
      if(UserApp1_bCD)
      {
        UserApp1_bCD = FALSE;
      }
      if(UserApp1_bEF)
      {
        UserApp1_bEF = FALSE;
      }
      
      UserApp1_bInstrTBE = FALSE;
      UpdateLCD();
    }
  } /* end Button 0 */
  
  /* Button 1 */
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    if(!UserApp1_bCD)
    {
      UserApp1_bCD = TRUE;
      LedOn(WHITE);
    }
    else
    {
      UserApp1_bCD = FALSE;
      LedOff(WHITE);
    }
  } /* end Button 1 */
  
  /* Button 2 */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    
    if(!UserApp1_bEF)
    {
      UserApp1_bEF = TRUE;
      LedOn(PURPLE);
    }
    else
    {
      UserApp1_bEF = FALSE;
      LedOff(PURPLE);
    }
  } /* end Button 2 */
  
  /* Button 3 */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    
    LedOn(BLUE);
  } /* end Button 3 */
  
} /* end UserApp1SM_Activated() */
  


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  LedOn(GREEN);
  LedOn(YELLOW);
  LedOn(RED);
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
