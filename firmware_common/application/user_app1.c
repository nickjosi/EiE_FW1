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
static bool UserApp1_bBallApproach;

static u32 UserApp1_Time;

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
  //u8 au80123[] = "0123456789ABCDEF0123";
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
  
  //LCDMessage(LINE1_START_ADDR, au80123);
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

/*--------------------------------------------------------------------------------------------------------------------
Function UpdateGameScreen()
*/
void UpdateGameScreen(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
  LCDMessage(LINE1_START_ADDR, "|                  |");
  LCDMessage(LINE2_START_ADDR, "|                  |");
  LCDMessage(LINE2_START_ADDR + UserApp1_PaddlePosition, "_");
  
  /* Ball location on LCD */
  if(UserApp1_BallLevel == 1)
  {
    LCDMessage(LINE1_START_ADDR + UserApp1_BallPosition, "o");
  }
  else if(UserApp1_BallLevel == 0)
  {
    if(UserApp1_PaddlePosition == UserApp1_BallPosition)
    {
      LCDMessage(LINE2_START_ADDR + UserApp1_BallPosition, "#");
    }
    else
    {
      LCDMessage(LINE2_START_ADDR + UserApp1_BallPosition, "o");
    }
  } /* end Ball on LCD */
} /* end UpdateGameScreen() */


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
    UserApp1_bBallApproach = TRUE;
    
    UserApp1_PaddlePosition = 9;
    UpdateGameScreen();
      
    UserApp1_Time = G_u32SystemTime1s;
    
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
    if(UserApp1_PaddlePosition != 1)
    {
      UserApp1_PaddlePosition--;
      
      /* Update display with new paddle position */
      UpdateGameScreen();
    }
  } /* end BUTTON0 */
  
  
  /* BUTTON3 moves the cursor right one position */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);

    /* Handle the case where the paddle is all the way to the right */
    if(UserApp1_PaddlePosition != 18)
    {
      UserApp1_PaddlePosition++;

      /* Update display with new paddle position */
      UpdateGameScreen();
    }
  } /* end BUTTON3 */
  
  
  /* BUTTON2 exits to the main menu */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);

    AllLedsOff();
    LoadMainMenu();  
    
    UserApp1_StateMachine = UserApp1SM_MainMenu;
  } /* end BUTTON 2 */
  
  /* ---------- Paddle Movement End ---------- */
  
  
  
  /* ---------- Ball Movement ----------*/
  
  if(G_u32SystemTime1s == UserApp1_Time + 1)
  {
    UserApp1_Time = G_u32SystemTime1s;
    
    /* Check for missed ball */
    if(UserApp1_BallLevel == 1 && !UserApp1_bBallApproach)
    {
      if((UserApp1_bBallRight && UserApp1_BallPosition != UserApp1_PaddlePosition + 1)
         || (!UserApp1_bBallRight && UserApp1_BallPosition != UserApp1_PaddlePosition - 1))
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
        LCDMessage(LINE1_START_ADDR, "======= GAME =======");
        LCDMessage(LINE2_START_ADDR, "======= OVER =======");
        
        UserApp1_StateMachine = UserApp1SM_GameOver;
      }
    }
    
    /* Ball location to be indicated by LEDs */
    if(UserApp1_BallLevel > 1)
    {
      AllLedsOff();
      UpdateGameScreen();
      
      if(UserApp1_BallPosition == 1)
      {
        LedPWM(WHITE, LED_PWM_50);
      }
      else if(UserApp1_BallPosition == 2)
      {
        LedPWM(WHITE, LED_PWM_50);
        LedPWM(PURPLE, LED_PWM_50);
      }
      else if(UserApp1_BallPosition == 3)
      {
        LedPWM(PURPLE, LED_PWM_50);
      }
      else if(UserApp1_BallPosition == 4 || UserApp1_BallPosition == 5)
      {
        LedPWM(PURPLE, LED_PWM_50);
        LedPWM(BLUE, LED_PWM_75);
      }
      else if(UserApp1_BallPosition == 6)
      {
        LedPWM(BLUE, LED_PWM_75);
      }
      else if(UserApp1_BallPosition == 7)
      {
        LedPWM(BLUE, LED_PWM_75);
        LedPWM(CYAN, LED_PWM_35);
      }
      else if(UserApp1_BallPosition == 8 || UserApp1_BallPosition == 9)
      {
        LedPWM(CYAN, LED_PWM_35);
      }
      else if(UserApp1_BallPosition == 10)
      {
        LedPWM(CYAN, LED_PWM_35);
        LedPWM(GREEN, LED_PWM_75);
      }
      else if(UserApp1_BallPosition == 11)
      {
        LedPWM(GREEN, LED_PWM_75);
      }
      else if(UserApp1_BallPosition == 12)
      {
        LedPWM(GREEN, LED_PWM_75);
        LedPWM(YELLOW, LED_PWM_100);
      }
      else if(UserApp1_BallPosition == 13)
      {
        LedPWM(YELLOW, LED_PWM_100);
      }
      else if(UserApp1_BallPosition == 14 || UserApp1_BallPosition == 15)
      {
        LedPWM(YELLOW, LED_PWM_100);
        LedPWM(ORANGE, LED_PWM_100);
      }
      else if(UserApp1_BallPosition == 16)
      {
        LedPWM(ORANGE, LED_PWM_100);
      }
      else if(UserApp1_BallPosition == 17)
      {
        LedPWM(ORANGE, LED_PWM_100);
        LedPWM(RED, LED_PWM_100);
      }
      else
      {
        LedPWM(RED, LED_PWM_100);
      }
    } /* end LEDs */
    
    /* Ball location on LCD */
    else if(UserApp1_BallLevel <= 1)
    {
      AllLedsOff();
      UpdateGameScreen();
    } /* end Ball on LCD */
    
    /* Update ball position (left/right) */
    if(UserApp1_bBallRight)
    {
      if(UserApp1_BallPosition == 18)
      {
        UserApp1_bBallRight = FALSE;
        UserApp1_BallPosition--;
      }
      else
      {
        UserApp1_BallPosition++;
      }
    }
    else
    {
      if(UserApp1_BallPosition == 1)
      {
        UserApp1_bBallRight = TRUE;
        UserApp1_BallPosition++;
      }
      else
      {
        UserApp1_BallPosition--;
      }
    } /* end Ball Left/Right update */
    
    /* Update ball position (up/down) */
    if(UserApp1_bBallApproach)
    {
      if(UserApp1_BallLevel == 0)
      {
        UserApp1_bBallApproach = FALSE;
        UserApp1_BallLevel++;
      }
      else
      {
        UserApp1_BallLevel--;
      }
    }
    else
    {
      if(UserApp1_BallLevel == 6)
      {
        UserApp1_bBallApproach = TRUE;
        UserApp1_BallLevel--;
      }
      else
      {
        UserApp1_BallLevel++;
      }
    } /* end Ball Left/Right update */
  
  } /* end time dependent section */
  
  /* ---------- Ball Movement End ---------- */

} /* end UserApp1SM_1PlyrStart() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_GameOver(void)
{
  if(UserApp1_Time == G_u32SystemTime1s + 5)
  {
    AllLedsOff();
    LoadMainMenu();  
    
    UserApp1_StateMachine = UserApp1SM_MainMenu;
  }
} /* end UserApp1SM_GameOver() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
