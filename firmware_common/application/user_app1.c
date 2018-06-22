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

static u8 UserApp1_au8LcdStartLine1[] = "RGB MIXER      Clear";
static u8 UserApp1_au8LcdStartLine2[] = "R:00  G:00  B:00   v";
static u8 UserApp1_au8PwmMessages[11][4] = {
  {'0', '0', ' ', '\0'},
  {'1', '0', ' ', '\0'},
  {'2', '0', ' ', '\0'},
  {'3', '0', ' ', '\0'},
  {'4', '0', ' ', '\0'},
  {'5', '0', ' ', '\0'},
  {'6', '0', ' ', '\0'},
  {'7', '0', ' ', '\0'},
  {'8', '0', ' ', '\0'},
  {'9', '0', ' ', '\0'},
  {'1', '0', '0', '\0'}
};

static LedRateType UserApp1_aeLedPwmLevels[] = {LED_PWM_0, LED_PWM_10, LED_PWM_20,
                                                LED_PWM_30, LED_PWM_40, LED_PWM_50,
                                                LED_PWM_60, LED_PWM_70, LED_PWM_80,
                                                LED_PWM_90, LED_PWM_100};
static u8 UserApp1_u8RedPwmIndex;
static u8 UserApp1_u8GreenPwmIndex;
static u8 UserApp1_u8BluePwmIndex;

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
  /* Start with all LEDs and LCD backlights off */
  LedOff(RED);
  LedOff(ORANGE);
  LedOff(YELLOW);
  LedOff(GREEN);
  LedOff(CYAN);
  LedOff(BLUE);
  LedOff(PURPLE);
  LedOff(WHITE);
  LedOff(LCD_GREEN);
  LedOff(LCD_BLUE);
  LedOff(LCD_RED);
  
  /* Initialize PWM rate indexes to 0 */
  UserApp1_u8RedPwmIndex = 0;
  UserApp1_u8GreenPwmIndex = 0;
  UserApp1_u8BluePwmIndex = 0;
  
  
  /* Update LCD. */
  LCDCommand(LCD_CLEAR_CMD);
  
  /* This delay is usually required after LCDCommand during INIT */
  for(u32 i = 0; i < 100000; i++);

  LCDMessage(LINE1_START_ADDR, UserApp1_au8LcdStartLine1);  
  LCDMessage(LINE2_START_ADDR, UserApp1_au8LcdStartLine2);
 
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
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


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  /* Button 0: Red backlight */
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    /* If at max PWM rate, do nothing */
    if(UserApp1_u8RedPwmIndex < 10)
    {
      /* Increase red backlight */
      UserApp1_u8RedPwmIndex++;
      LedPWM(LCD_RED, UserApp1_aeLedPwmLevels[UserApp1_u8RedPwmIndex]);
      
      /* Update LCD message */
      LCDMessage(RED_PWM_ADDR, UserApp1_au8PwmMessages[UserApp1_u8RedPwmIndex]);
    }
  } /* end if(WasButtonPressed(BUTTON0)) */
  
  /* Button 1: Green backlight */
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    /* If at max PWM rate, do nothing */
    if(UserApp1_u8GreenPwmIndex < 10)
    {
      /* Increase green backlight */
      UserApp1_u8GreenPwmIndex++;
      LedPWM(LCD_GREEN, UserApp1_aeLedPwmLevels[UserApp1_u8GreenPwmIndex]);
      
      /* Update LCD message */
      LCDMessage(GREEN_PWM_ADDR, UserApp1_au8PwmMessages[UserApp1_u8GreenPwmIndex]);
    }
  } /* end if(WasButtonPressed(BUTTON1)) */
  
  /* Button 2: Blue backlight */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    
    /* If at max PWM rate, do nothing */
    if(UserApp1_u8BluePwmIndex < 10)
    {
      /* Increase blue backlight */
      UserApp1_u8BluePwmIndex++;  
      LedPWM(LCD_BLUE, UserApp1_aeLedPwmLevels[UserApp1_u8BluePwmIndex]);
      
      /* Update LCD message */
      LCDMessage(BLUE_PWM_ADDR, UserApp1_au8PwmMessages[UserApp1_u8BluePwmIndex]);
    }
  } /* end if(WasButtonPressed(BUTTON2)) */
  
  /* Button 3: Set all PWM rates to zero */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    
    /* Turn off all LCD backlights */
    UserApp1_u8RedPwmIndex = 0;
    UserApp1_u8GreenPwmIndex = 0;
    UserApp1_u8BluePwmIndex = 0;
    LedPWM(LCD_RED, UserApp1_aeLedPwmLevels[UserApp1_u8RedPwmIndex]);
    LedPWM(LCD_GREEN, UserApp1_aeLedPwmLevels[UserApp1_u8GreenPwmIndex]);
    LedPWM(LCD_BLUE, UserApp1_aeLedPwmLevels[UserApp1_u8BluePwmIndex]);
    
    /* Reset LCD messages to all zeros */
    LCDMessage(LINE2_START_ADDR, UserApp1_au8LcdStartLine2);
  } /* end if(WasButtonPressed(BUTTON3)) */

} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
