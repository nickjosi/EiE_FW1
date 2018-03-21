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
#define M_GAME_TICK 500
#define M_STARTING_BALL_LEV 4
#define M_STARTING_PADD_POS 8



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

static u8 UserApp1_ScoreTENS[] = "0";
static u8 UserApp1_ScoreONES[] = "0";

static bool UserApp1_bBallHit;
static bool UserApp1_bGameOver;

static bool UserApp1_bSoundOn;
static bool UserApp1_bPaddSound;
static bool UserApp1_bGOSound;

static u32 UserApp1_Time;
static u32 UserApp1_SoundTimer;

static u8 UserApp1_LCDColour;

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
  UserApp1_LCDColour = 0;
  UserApp1_bSoundOn = TRUE;
  
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
  u8 au8UserApp1MainMenu2[] = "1PLYR 2PLYR        C";
  u8 au8UserApp1SoundOn[] = "S:ON";
  u8 au8UserApp1SoundOff[] = "S:OFF";
  //u8 au80123[] = "0123456789ABCDEF0123";
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
  
  //LCDMessage(LINE1_START_ADDR, au80123);
  LCDMessage(LINE1_START_ADDR, au8UserApp1MainMenu1);
  LCDMessage(LINE2_START_ADDR, au8UserApp1MainMenu2);
  if(UserApp1_bSoundOn)
  {
    LCDMessage(LINE2_START_ADDR + 13, au8UserApp1SoundOn);
  }
  else
  {
    LCDMessage(LINE2_START_ADDR + 13, au8UserApp1SoundOff);
  }
  
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
Function LoadGameScreen()
*/
void LoadGameScreen(void)
{ 
  //LCDCommand(LCD_CLEAR_CMD);
  //LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
  LCDMessage(LINE1_START_ADDR, "|                |  ");
  LCDMessage(LINE2_START_ADDR, "|                |");
  LCDMessage(LINE2_START_ADDR + 18, UserApp1_ScoreTENS);
  LCDMessage(LINE2_START_ADDR + 19, UserApp1_ScoreONES);
  //LCDMessage(LINE2_START_ADDR + UserApp1_PaddlePosition, "_");
  
  /* Ball location on LCD */
  /*
  if(UserApp1_BallLevel == 1)
  {
    LCDMessage(LINE1_START_ADDR + UserApp1_BallPosition, "o");
  }
  else if(UserApp1_BallLevel == 0)
  {
    if(UserApp1_PaddlePosition == UserApp1_BallPosition)
    {
      LCDMessage(LINE2_START_ADDR + UserApp1_BallPosition, "x");
    }
    else
    {
      LCDMessage(LINE2_START_ADDR + UserApp1_BallPosition, "o");
    }
  } */ /* end Ball on LCD */
} /* end LoadGameScreen() */

/*--------------------------------------------------------------------------------------------------------------------
Function UpdateLCDPaddle()
*/
void UpdateLCDPaddle(void)
{ 
  LCDMessage(LINE2_START_ADDR + UserApp1_PaddlePosition, "_");
} /* end UpdateLCDPaddle() */

/*--------------------------------------------------------------------------------------------------------------------
Function UpdateLCDBall()
*/
void UpdateLCDBall(void)
{ 
  /* Ball location on LCD */
  if(UserApp1_BallLevel == 1)
  {
    LCDMessage(LINE1_START_ADDR + UserApp1_BallPosition, "o");
  }
  else if(UserApp1_BallLevel == 0)
  {
    if(UserApp1_PaddlePosition == UserApp1_BallPosition)
    {
      LCDMessage(LINE2_START_ADDR + UserApp1_BallPosition, "x");
    }
    else
    {
      LCDMessage(LINE2_START_ADDR + UserApp1_BallPosition, "o");
    }
  } /* end Ball on LCD */
} /* end UpdateLCDBall() */

/*--------------------------------------------------------------------------------------------------------------------
Function MenuSound()
*/
void MenuSound(void)
{
  if(UserApp1_bSoundOn && G_u32SystemTime1ms <= UserApp1_SoundTimer + 100)
  {
    PWMAudioSetFrequency(BUZZER1, 550);
    PWMAudioSetFrequency(BUZZER2, 550);
    PWMAudioOn(BUZZER1);
    PWMAudioOn(BUZZER2);
  }
  else
  {
    PWMAudioOff(BUZZER1);
    PWMAudioOff(BUZZER2);
  }
} /* end MeunSound() */

/*--------------------------------------------------------------------------------------------------------------------
Function GameSound()
*/
void GameSound(void)
{
  if(UserApp1_bSoundOn)
  {
    if(UserApp1_bPaddSound)
    {
      PWMAudioSetFrequency(BUZZER1, 459);
      PWMAudioSetFrequency(BUZZER2, 459);
      PWMAudioOn(BUZZER1);
      PWMAudioOn(BUZZER2);
      if(G_u32SystemTime1ms >= UserApp1_SoundTimer + 100)
      {
        PWMAudioOff(BUZZER1);
        PWMAudioOff(BUZZER2);
        UserApp1_bPaddSound = FALSE;
      }
    }
    
    if(UserApp1_bGOSound)
    {
      PWMAudioSetFrequency(BUZZER1, 490);
      PWMAudioSetFrequency(BUZZER2, 490);
      PWMAudioOn(BUZZER1);
      PWMAudioOn(BUZZER2);
      if(G_u32SystemTime1ms >= UserApp1_SoundTimer + 500)
      {
        PWMAudioOff(BUZZER1);
        PWMAudioOff(BUZZER2);
        UserApp1_bGOSound = FALSE;
      }
    }
  }
} /* end GameSound() */


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
  MenuSound();
  /* ----- BUTTON0 -----  */
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    UserApp1_BallLevel = M_STARTING_BALL_LEV;
    UserApp1_BallPosition = (G_u32SystemTime1ms % 8) + 3;
    UserApp1_bBallRight = TRUE;
    UserApp1_bBallApproach = TRUE;
    UserApp1_PaddlePosition = M_STARTING_PADD_POS;
   
    //UserApp1_ScoreTENS = "1";
    //UserApp1_ScoreONES = "4";
    
    UserApp1_bBallHit = FALSE;
    UserApp1_bGameOver = FALSE;
    UserApp1_bPaddSound = FALSE;
    UserApp1_bGOSound = FALSE;
    
    LoadGameScreen();
    UpdateLCDPaddle();
      
    UserApp1_Time = G_u32SystemTime1ms;
    
    UserApp1_StateMachine = UserApp1SM_1PlyrStart;
  } /* end BUTTON0 */
  
  
  /* ----- BUTTON2 ----- */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    
    if(!UserApp1_bSoundOn)
    {
      UserApp1_bSoundOn = TRUE;
      UserApp1_SoundTimer = G_u32SystemTime1ms;
    }
    else
    {
      UserApp1_bSoundOn = FALSE;
    }
    
    LoadMainMenu();
  } /* end BUTTON2 */
  
  
  /* ----- BUTTON3 ----- */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    
    UserApp1_LCDColour++;
    if(UserApp1_LCDColour == 6)
    {
      UserApp1_LCDColour = 0;
    }
  }
    
  if(UserApp1_LCDColour == 0) //WHITE
  {
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
  }
  else if(UserApp1_LCDColour == 1) //GREEN
  {
    LedOff(LCD_RED);
    LedOn(LCD_GREEN);
    LedOff(LCD_BLUE);
  }
  else if(UserApp1_LCDColour == 2) //MAGENTA
  {
    LedOn(LCD_RED);
    LedOff(LCD_GREEN);
    LedOn(LCD_BLUE);
  }
  else if(UserApp1_LCDColour == 3) //CYAN
  {
    LedOff(LCD_RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
  }
  else if(UserApp1_LCDColour == 4) //YELLOW
  {
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOff(LCD_BLUE);
  }
  else if(UserApp1_LCDColour == 5) //OFF
  {
    LedOff(LCD_RED);
    LedOff(LCD_GREEN);
    LedOff(LCD_BLUE);
  } /* end BUTTON3 */
  
} /* end UserApp1SM_MainMenu() */
  

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_1PlyrStart(void)
{
  GameSound();
  /* Check for ball contact */
  if(UserApp1_BallLevel == 0 && UserApp1_BallPosition == UserApp1_PaddlePosition)
  {
    UserApp1_bBallHit = TRUE;
    
    if(G_u32SystemTime1ms > UserApp1_SoundTimer + (2 * M_GAME_TICK))
    {
      UserApp1_SoundTimer = G_u32SystemTime1ms;
      UserApp1_bPaddSound = TRUE;
    }
  } /* end Check Ball Contact */
  
  
  /* ---------- Paddle Movement ---------- */
  
  /* BUTTON0 moves the paddle left one position */
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);

    /* Handle the case where the paddle is all the way to the left */
    if(UserApp1_PaddlePosition > 1)
    {
      UserApp1_PaddlePosition--;
      
      /* Update display with new paddle position */
      //LoadGameScreen();
      UpdateLCDPaddle();
    }
  } /* end BUTTON0 */
  
  
  /* BUTTON3 moves the cursor right one position */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);

    /* Handle the case where the paddle is all the way to the right */
    if(UserApp1_PaddlePosition < 16)
    {
      UserApp1_PaddlePosition++;

      /* Update display with new paddle position */
      LoadGameScreen();
      UpdateLCDPaddle();
    }
  } /* end BUTTON3 */
  
  
  /* BUTTON1 or BUTTON2 exits to the main menu */
  if(WasButtonPressed(BUTTON1) || WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON1);
    ButtonAcknowledge(BUTTON2);

    AllLedsOff();
    LoadMainMenu();  
    
    UserApp1_StateMachine = UserApp1SM_MainMenu;
  } /* end BUTTON 2 */
  
  /* ---------- Paddle Movement End ---------- */
  
  
  
  /* ---------- Ball Movement ----------*/
  
  if(G_u32SystemTime1ms >= UserApp1_Time + M_GAME_TICK)
  {
    UserApp1_Time = G_u32SystemTime1ms;
    
    /* Check for missed ball */
    if(UserApp1_BallLevel == 0)
    {
      if(UserApp1_bBallHit == FALSE)
      {
        LedOn(LCD_RED);
        LedOff(LCD_GREEN);
        LedOff(LCD_BLUE);
        LCDCommand(LCD_CLEAR_CMD);
        LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
        LCDMessage(LINE1_START_ADDR, "======= GAME =======");
        LCDMessage(LINE2_START_ADDR, "======= OVER =======");
        
        UserApp1_bGameOver = TRUE;
        if(G_u32SystemTime1ms > UserApp1_SoundTimer + (2 * M_GAME_TICK))
        {
          UserApp1_SoundTimer = G_u32SystemTime1ms;
          UserApp1_bGOSound = TRUE;
        }
        UserApp1_StateMachine = UserApp1SM_GameOver;
      }
      else 
      {
        UserApp1_bBallHit = FALSE;
      }
    }
    
    if(!UserApp1_bGameOver)
    {
      /* Update ball position (left/right) */
      if(UserApp1_bBallRight)
      {
        if(UserApp1_BallPosition == 16)
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
      
      /* Update ball level (up/down) */
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
        if(UserApp1_BallLevel == 4)
        {
          AllLedsOff();
          UserApp1_bBallApproach = TRUE;
          
          if(G_u32SystemTime1ms > UserApp1_SoundTimer + (2 * M_GAME_TICK))
          {
            UserApp1_SoundTimer = G_u32SystemTime1ms;
            UserApp1_bPaddSound = TRUE;
          }
          
          u8 temp_rand = G_u32SystemTime1ms % 3;
          if(temp_rand % 2 == 0)
          {
            UserApp1_bBallRight = TRUE;
          }
          else
          {
            UserApp1_bBallRight = FALSE;
          }
        }
        else
        {
          UserApp1_BallLevel++;
        }
      } /* end Ball Level update */
    
      /* Ball location to be indicated by LEDs */
      if(UserApp1_BallLevel > 1 && UserApp1_BallLevel != 4)
      {
        AllLedsOff();
        //LoadGameScreen();
        UpdateLCDBall();
        
        if(UserApp1_BallPosition == 1)
        {
          LedPWM(WHITE, LED_PWM_50);
        }
        else if(UserApp1_BallPosition == 2)
        {
          LedPWM(WHITE, LED_PWM_25);
          LedPWM(PURPLE, LED_PWM_25);
        }
        else if(UserApp1_BallPosition == 3)
        {
          LedPWM(PURPLE, LED_PWM_50);
        }
        else if(UserApp1_BallPosition == 4)
        {
          LedPWM(PURPLE, LED_PWM_25);
          LedPWM(BLUE, LED_PWM_25);
        }
        else if(UserApp1_BallPosition == 5)
        {
          LedPWM(PURPLE, LED_PWM_10);
          LedPWM(BLUE, LED_PWM_50);
        }
        else if(UserApp1_BallPosition == 6)
        {
          LedPWM(BLUE, LED_PWM_75);
        }
        else if(UserApp1_BallPosition == 7)
        {
          LedPWM(BLUE, LED_PWM_35);
          LedPWM(CYAN, LED_PWM_35);
        }
        else if(UserApp1_BallPosition == 8)
        {
          LedPWM(CYAN, LED_PWM_60);
        }
         else if(UserApp1_BallPosition == 9)
        {
          LedPWM(CYAN, LED_PWM_40);
          LedPWM(GREEN, LED_PWM_35);
        }
        else if(UserApp1_BallPosition == 10)
        {
          LedPWM(CYAN, LED_PWM_20);
          LedPWM(GREEN, LED_PWM_60);
        }
        else if(UserApp1_BallPosition == 11)
        {
          LedPWM(GREEN, LED_PWM_75);
        }
        else if(UserApp1_BallPosition == 12)
        {
          LedPWM(GREEN, LED_PWM_40);
          LedPWM(YELLOW, LED_PWM_70);
        }
        else if(UserApp1_BallPosition == 13)
        {
          LedPWM(YELLOW, LED_PWM_100);
        }
        else if(UserApp1_BallPosition == 14)
        {
          LedPWM(YELLOW, LED_PWM_70);
          LedPWM(ORANGE, LED_PWM_40);
        }
        else if(UserApp1_BallPosition == 15)
        {
          LedPWM(YELLOW, LED_PWM_40);
          LedPWM(ORANGE, LED_PWM_70);
        }
        else if(UserApp1_BallPosition == 16)
        {
          LedPWM(ORANGE, LED_PWM_100);
        }
      } /* end LEDs */
      
      /* Ball location on LCD */
      else if(UserApp1_BallLevel <= 1)
      {
        AllLedsOff();
        //LoadGameScreen();
        UpdateLCDBall();
      } /* end Ball on LCD */
      
    } /* end if(!GameOver) */
    
  } /* end time dependent section */
  
  /* ---------- Ball Movement End ---------- */

} /* end UserApp1SM_1PlyrStart() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_GameOver(void)
{
  GameSound();
  
  if(G_u32SystemTime1ms >= UserApp1_Time + 3000
     || WasButtonPressed(BUTTON1) || WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON1);
    ButtonAcknowledge(BUTTON2);
    ButtonAcknowledge(BUTTON3);
    LoadMainMenu();
    
    UserApp1_StateMachine = UserApp1SM_MainMenu;
  }
} /* end UserApp1SM_GameOver() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
