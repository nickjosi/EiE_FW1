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
static u8 UserApp1_PaddlePosition;
static u8 UserApp1_BallLevel;
static u8 UserApp1_BallPosition;
static bool UserApp1_bBallRight;

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
  AllLedsOff();
  LoadMainMenu();
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_MainMenu;
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

/*--------------------------------------------------------------------------------------------------------------------
Function LoadMainMenu()
*/
void LoadMainMenu(void)
{
                             /*0123456789ABCDEF0123*/
  u8 au8UserApp1MainMenu1[] = "------- PONG -------";
  u8 au8UserApp1MainMenu2[] = "1PLYR 2PLYR  X     X";
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
  
  LCDMessage(LINE1_START_ADDR, au8UserApp1MainMenu1);
  LCDMessage(LINE2_START_ADDR, au8UserApp1MainMenu2);
  
} /* end LoadMainMenu() */

/*--------------------------------------------------------------------------------------------------------------------
Function AllLedsOff()
*/
void AllLedsOff(void)
{
  LedPWM(WHITE, LED_PWM_0);
  LedPWM(PURPLE, LED_PWM_0);
  LedPWM(BLUE, LED_PWM_0);
  LedPWM(CYAN, LED_PWM_0);
  LedPWM(GREEN, LED_PWM_0);
  LedPWM(YELLOW, LED_PWM_0);
  LedPWM(ORANGE, LED_PWM_0);
  LedPWM(RED, LED_PWM_0); 
} /* end AllLedsOff() */


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{

} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_MainMenu(void)
{
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    UserApp1_BallLevel = 5;
    UserApp1_BallPosition = 2;
    UserApp1_bBallRight = TRUE;
    
    UserApp1_PaddlePosition = 9;
    LCDCommand(LCD_CLEAR_CMD);
    LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
    LCDMessage(LINE2_START_ADDR + UserApp1_PaddlePosition, "_");
      
    UserApp1_StateMachine = UserApp1SM_1PlyrStart;
  }
} /* end UserApp1SM_MainMenu() */
  

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_1PlyrStart(void)
{
  /* ---------- Paddle Movement ---------- */
  
  /* BUTTON0 moves the paddle left one position */
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);

    /* Handle the case where the paddle is all the way to the left */
    if(UserApp1_PaddlePosition != 0)
    {
      UserApp1_PaddlePosition--;
      
      /* Update display with new paddle position */
      LCDCommand(LCD_CLEAR_CMD);
      LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
      LCDMessage(LINE2_START_ADDR + UserApp1_PaddlePosition, "_");
    }
  } /* end BUTTON0 */
  
  
  /* BUTTON3 moves the cursor right one position */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);

    /* Handle the case where the paddle is all the way to the right */
    if(UserApp1_PaddlePosition != 19)
    {
      UserApp1_PaddlePosition++;

      /* Update display with new paddle position */
      LCDCommand(LCD_CLEAR_CMD);
      LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
      LCDMessage(LINE2_START_ADDR + UserApp1_PaddlePosition, "_");
    }
  } /* end BUTTON3 */
  
  
  /* BUTTON2 exits to the main menu */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);

    LoadMainMenu();  
    
    UserApp1_StateMachine = UserApp1SM_MainMenu;
  } /* end BUTTON 2 */
  
  /* ---------- Paddle Movement End ---------- */
  
  
  
  /* ---------- Ball Movement ----------*/
  
  /* Ball location to be indicated by LEDs */
  if(UserApp1_BallLevel > 1)
  {
    AllLedsOff();
    
    if(UserApp1_BallPosition <= 2)
    {
      LedPWM(WHITE, LED_PWM_100);
    }
    else if(UserApp1_BallPosition >= 3 && UserApp1_BallPosition <= 4)
    {
      LedPWM(PURPLE, LED_PWM_100);
    }
    else if(UserApp1_BallPosition >= 5 && UserApp1_BallPosition <= 6)
    {
      LedPWM(BLUE, LED_PWM_100);
    }
    else if(UserApp1_BallPosition >= 7 && UserApp1_BallPosition <= 9)
    {
      LedPWM(CYAN, LED_PWM_100);
    }
    else if(UserApp1_BallPosition >= 10 && UserApp1_BallPosition <= 12)
    {
      LedPWM(GREEN, LED_PWM_100);
    }
    else if(UserApp1_BallPosition >= 13 && UserApp1_BallPosition <= 14)
    {
      LedPWM(YELLOW, LED_PWM_100);
    }
    else if(UserApp1_BallPosition >= 15 && UserApp1_BallPosition <= 16)
    {
      LedPWM(ORANGE, LED_PWM_100);
    }
    else
    {
      LedPWM(RED, LED_PWM_100);
    }
  } /* end LEDs */
  
  /* Update ball position */
  if(UserApp1_bBallRight)
  {
    if(UserApp1_BallPosition == 19)
    {
      UserApp1_bBallRight = FALSE;
      UserApp1_BallPosition--;
    }
    else
    {
      UserApp1_BallPosition++;
    }
  }
  
  /* ---------- Ball Movement End ---------- */

} /* end UserApp1SM_1PlyrStart() */
    

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
