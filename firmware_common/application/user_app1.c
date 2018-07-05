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

static u8 UserApp1_au8Set1Keys[11][6] = {
  {'F', 'B', 'D', 'A', 'E', 'C'},
  {'A', 'C', 'D', 'B', ' ', ' '},
  {'C', 'E', 'A', 'D', 'F', 'B'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'A', 'B', 'C', 'D', 'E', 'F'}
};

static u8 UserApp1_au8CorrectSequence[6];            /* Key sequence array */
static u8 UserApp1_u8CorrectSequenceIndex;           /* Tracks cursor within the key sequence array */
static u8 UserApp1_au8Sequence[6];                   /* Entered sequence array */
static u8 UserApp1_u8SequenceIndex;                  /* Tracks cursor within the entered sequence array */
static u8 UserApp1_u8CursorPosition;                 /* Cursor position on LCD */

static bool UserApp1_bConfig;                        /* Key sequence configured? */
static bool UserApp1_bFull;                          /* Full 6-char sequence entered? */
static bool UserApp1_bFinalBoard;                    /* Last board in the current round? */

static bool UserApp1_bSequenceTBE;                   /* Sequence to be edited? */
static bool UserApp1_bAB;                            /* A or B to added? */
static bool UserApp1_bCD;                            /* C or D to added? */
static bool UserApp1_bEF;                            /* E or F to added? */
static bool UserApp1_bOp;                            /* Show options menu? */

static u8 UserApp1_u8AttemptCounter;                 /* Stores number of attempts */
static u8 UserApp1_u8LockTimer;                      /* Timer used for duration of lockout */
static u32 UserApp1_u32SystemTimeStamp;              /* Timestamp used for creating timer */



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
  - NONE

Promises:
  - NONE
*/
void UserApp1Initialize(void)
{ 
  UserApp1_bConfig =        FALSE;
  UserApp1_bFinalBoard =    FALSE;
  UserApp1_bSequenceTBE =   FALSE;
  UserApp1_bAB =            FALSE;
  UserApp1_bCD =            FALSE;
  UserApp1_bEF =            FALSE;
  UserApp1_bOp =            FALSE;
  
  for(int i = 0; i < 6; i++)
  {
    UserApp1_au8CorrectSequence[i] = ' ';
    UserApp1_au8Sequence[i] = ' ';
  }
  
  UserApp1_u8CorrectSequenceIndex = 0;
  UserApp1_u8SequenceIndex = 0;
  UserApp1_u8AttemptCounter = 0;
  
  AllLedsOff();
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDCommand(LCD_HOME_CMD);
  UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
  UpdateLCD();
  
  LedOn(LCD_RED);
  LedOn(LCD_GREEN);
  LedOn(LCD_BLUE);
 
  /* If good initialization, set state to Config */
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

} /* end UserApp1RunActiveState() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: AllLedsOff()

Description:
Turns all LEDs off.

Requires:
  - NONE

Promises:
  - All LEDs will be turned off.
*/
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
  
} /* end AllLedsOff() */


/*--------------------------------------------------------------------------------------------------------------------
Function: UpdateLCD()

Description:
Updates LCD with messages that correspond to the current state and status of the
game.

Requires:
  - All booleans are correctly set according to the current state and status of the game

Promises:
  - The correct messages/menus will be displayed on the LCD
*/
void UpdateLCD(void)
{
  static u8 au8MainMenu1_Config[] = "Set sequence:       ";
  static u8 au8MainMenu1[] =        "UNSCRAMBLE ||       ";
  static u8 au8MainMenu2[] =        ">A,B  >C,D  >E,F  Op";
  static u8 au8AB[] =               ">A    >B        Back";
  static u8 au8CD[] =               ">C    >D        Back";
  static u8 au8EF[] =               ">E    >F        Back";
  static u8 au8Op[] =               "Entr  Del  Clr  Back";
  static u8 au8Full[] =             "Entr  Del  Clr      ";
  
  /* Line 1 of config main menu */
  if(!UserApp1_bConfig)
  {
    LCDMessage(LINE1_START_ADDR, au8MainMenu1_Config);
    LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8CorrectSequence);
  }
  /* Line 1 of standard main menu */
  else
  {
    LCDMessage(LINE1_START_ADDR, au8MainMenu1);
    LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8Sequence);
  }
  
  /* Line 2 for Enter Sequence Mode menus */
  if(!UserApp1_bFull)
  {
    if(!UserApp1_bSequenceTBE)
    {
      LCDMessage(LINE2_START_ADDR, au8MainMenu2);
    }
    else
    {
      if(UserApp1_bAB)
      {
        LCDMessage(LINE2_START_ADDR, au8AB);
      }
      if(UserApp1_bCD)
      {
        LCDMessage(LINE2_START_ADDR, au8CD);
      }
      if(UserApp1_bEF)
      {
        LCDMessage(LINE2_START_ADDR, au8EF);
      }
      if(UserApp1_bOp)
      {
        LCDMessage(LINE2_START_ADDR, au8Op);
      }
    }
  }
  
  /* Line 2 for Full Sequence Entered Mode menu */
  if(UserApp1_bFull)
  {
    LCDMessage(LINE2_START_ADDR, au8Full);
  }
  
  /* (draw cursor) */
  LCDCommand(LCD_ADDRESS_CMD | UserApp1_u8CursorPosition);
  
} /* end UpdateLCD() */


/*--------------------------------------------------------------------------------------------------------------------
Function: EnterSequence(u8* au8Sequence, u8* u8Index)

Description:
Interfaces button presses on the board and manipulation of the sequence arrays to
allow for the sequence arrays to be edited. Correctly changes booleans based on the
button presses so that menus can be properly navigated and respective LCD updates 
will occur.

Requires:
  - au8Sequence pointer points to UserApp1_au8CorrectSequence if in Config state
  - au8Sequence pointer points to UserApp1_au8Sequence if in Activated state
  - u8Index pointer points to UserApp1_u8CorrectSequenceIndex if in Config state
  - u8Index pointer points to UserApp1_u8SequenceIndex if in Activatee state

Promises:
  - Button presses will result in the correct changes of booleans so that the action
    indicated on the LCD can be executed by the rest of the program.
*/
void EnterSequence(u8* au8Sequence, u8* u8Index)
{
  
  /* --- Enter Sequence Mode --- */
  
  if(!UserApp1_bFull)
  {
    /* Button 0 */
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      /* Handle button 0 press from main menu by going to A/B menu. */
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bAB = TRUE;
        UpdateLCD();
      }
      /* Handle button 0 press from:
      */
      else
      {
        /* A/B menu by inserting 'A' into the sequence. */
        if(UserApp1_bAB)
        {
          au8Sequence[*u8Index] = 'A';
          UserApp1_bAB = FALSE;
        }
        /* C/D menu by inserting 'C' into the sequence. */
        if(UserApp1_bCD)
        {
          au8Sequence[*u8Index] = 'C';
          UserApp1_bCD = FALSE;
        }
        /* E/F menu by inserting 'E' into the sequence. */
        if(UserApp1_bEF)
        {
          au8Sequence[*u8Index] = 'E';
          UserApp1_bEF = FALSE;
        }
        /* (increment index and cursor, reset bSequenceTBE and update LCD
            regardless of A/B, C/D, or E/F menu) */
        if(!UserApp1_bOp)
        {
          UserApp1_bSequenceTBE = FALSE;
          (*u8Index)++;
          UserApp1_u8CursorPosition++;
          UpdateLCD();
        }
        
        /* Options menu by setting the sequence and going to next state. */
        if(UserApp1_bOp)
        {
          /* If in Config state, go to Config2 */
          if(!UserApp1_bConfig)
          {
            UserApp1_StateMachine = UserApp1SM_Config2;
            UserApp1_bConfig = TRUE;
            UserApp1_bOp = FALSE;
            UserApp1_bSequenceTBE = FALSE;
            UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
            
            LedOff(LCD_RED);
            LedOff(LCD_GREEN);
            LedOn(LCD_BLUE);
            
            LCDMessage(LINE1_START_ADDR, "Final board?        ");
            LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8CorrectSequence);
            LCDMessage(LINE2_START_ADDR, "Yes   No            ");
          }
          /* If in Activated state, go to Compare */
          else
          {
            UserApp1_StateMachine = UserApp1SM_CompareSequence;
            (*u8Index)--; // Resolves issue when 5-char sequence is entered.
            UserApp1_bSequenceTBE = FALSE;
            UpdateLCD();
          }
        } 
      }
      
    } /* end Button 0 */
    
    /* Button 1 */
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      
      /* Handle button 1 press from main menu by going to C/D menu. */
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bCD = TRUE;
      }
      /* Handle button 1 press from:
      */
      else if(UserApp1_bSequenceTBE && !UserApp1_bOp)
      {
        /* A/B menu by inserting 'B' into the sequence. */
        if(UserApp1_bAB)
        {
          au8Sequence[*u8Index] = 'B';
          UserApp1_bAB = FALSE;
        }
        /* C/D menu by inserting 'D' into the sequence. */
        if(UserApp1_bCD)
        {
          au8Sequence[*u8Index] = 'D';
          UserApp1_bCD = FALSE;
        }
        /* E/F menu by inserting 'F' into the sequence. */
        if(UserApp1_bEF)
        {
          au8Sequence[*u8Index] = 'F';
          UserApp1_bEF = FALSE;
        }
        
        /* (increment index and cursor only for these menus) */
        (*u8Index)++;
        UserApp1_u8CursorPosition++;
        UserApp1_bSequenceTBE = FALSE;
      }
      else if(UserApp1_bSequenceTBE && UserApp1_bOp)
      {
        /* Options menu by deleting the previous instruction added to the sequence. */
        if(*u8Index > 0)
        {
          (*u8Index)--;
          UserApp1_u8CursorPosition--;
        }
        au8Sequence[*u8Index] = ' ';
        UserApp1_bOp = FALSE;
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 1 */
    
    /* Button 2 */
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      
      /* Handle button 2 press from main menu by going to E/F menu. */
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bEF = TRUE;
      }
      /* Handle button 2 press from:
      */
      else
      {
        /* Options menu by clearing all instructions added to the sequence. */
        if(UserApp1_bOp)
        {
          for(int i = 0; i < 6; i++)
          {
            au8Sequence[i] = ' ';
          }
          *u8Index = 0;
          UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
          UserApp1_bOp = FALSE;
        }
        /* All other menus by going back to the main menu. */
        else
        {
          UserApp1_bAB = FALSE;
          UserApp1_bCD = FALSE;
          UserApp1_bEF = FALSE;
        }
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 2 */
    
    /* Button 3 */
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      
      /* Handle button 3 press from main menu by going to Options menu. */
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bOp = TRUE;
      }
      /* Handle button 3 press from all other menus by going back to main menu. */
      else
      {
        UserApp1_bSequenceTBE = FALSE;
        UserApp1_bAB = FALSE;
        UserApp1_bCD = FALSE;
        UserApp1_bEF = FALSE;
        UserApp1_bOp = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 3 */
    
    /* If sequence has reached 6-chars, go to Full Sequence Entered Mode. */
    if(*u8Index >= 6)
    {
      UserApp1_bFull = TRUE;
      UpdateLCD();
    }
    
  } /* --- end Enter Sequence Mode --- */
  
  
  /* --- Full Sequence Entered Mode --- */
  
  if(UserApp1_bFull)
  {
    /* Button 0: ENTER (changes state) */
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      if(!UserApp1_bConfig)
      {
        UserApp1_StateMachine = UserApp1SM_Config2;
        UserApp1_bConfig = TRUE;
        UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
        
        LedOff(LCD_RED);
        LedOff(LCD_GREEN);
        LedOn(LCD_BLUE);
        
        LCDMessage(LINE1_START_ADDR, "Final board?        ");
        LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8CorrectSequence);
        LCDMessage(LINE2_START_ADDR, "Yes   No            ");
      }
      else
      {
        UserApp1_StateMachine = UserApp1SM_CompareSequence;
        UpdateLCD();
      }
      
      UserApp1_bFull = FALSE;
    } /* end Button 0 */
    
    /* Button 1: DELETE */
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      
      (*u8Index)--;
      au8Sequence[*u8Index] = ' ';
      UserApp1_u8CursorPosition--;
      
      UpdateLCD();
    } /* end Button 1 */
    
    /* Button 2: CLEAR */
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      
      for(int i = 0; i < 6; i++)
      {
        au8Sequence[i] = ' ';
      }
      *u8Index = 0;
      UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
      
      UpdateLCD();
    } /* end Button 2 */
    
    /* If sequence has become less than 6-chars, go back to Enter Sequence Mode. */
    if(*u8Index < 6)
    {
      UserApp1_bFull = FALSE;
      UpdateLCD();
    }
    
  } /* --- end Full Sequence Entered Mode --- */
  
} /* end EnterSequence() */



/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for good initialization / return from Correct state */
static void UserApp1SM_Config(void)
{
  /* Interfaces game facilitator to set UserApp1_au8CorrectSequence */
  EnterSequence(UserApp1_au8CorrectSequence, &UserApp1_u8CorrectSequenceIndex);
  
} /* end UserApp1SM_Config() */
 


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for configuration 1 */
static void UserApp1SM_Config2(void)
{
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOff(LCD_BLUE);
    
    UserApp1_bFinalBoard = TRUE;
    UserApp1_StateMachine = UserApp1SM_Unactivated;
    UpdateLCD();
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOff(LCD_BLUE);
    
    UserApp1_StateMachine = UserApp1SM_Unactivated;
    UpdateLCD();
  }
  if(WasButtonPressed(BUTTON2) || WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON2);
    ButtonAcknowledge(BUTTON3);
  }
  
  
} /* end UserApp1SM_Config2() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for configuration 2 */
static void UserApp1SM_Unactivated(void)
{
  
  /* ADD CODE HERE for how this state should be handled */
  UserApp1_StateMachine = UserApp1SM_Activated;
  
} /* end UserApp1SM_Unactivated() */
  


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for activation / return from Locked state */
static void UserApp1SM_Activated(void)
{
  /* Interfaces players to enter UserApp1_au8Sequence */
  EnterSequence(UserApp1_au8Sequence, &UserApp1_u8SequenceIndex);
  
} /* end UserApp1SM_Activated() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for sequence to be entered from Activated state */
static void UserApp1SM_CompareSequence(void)
{
  static bool bCorrect = TRUE;
  
  /* Compare key sequence with entered sequence.
        - set bCorrect to false if two unlike chars are found
  */
  for(int i = 0; i < 6; i++)
  {
    if(UserApp1_au8Sequence[i] != UserApp1_au8CorrectSequence[i])
    {
      bCorrect = FALSE;
    }
  } /* end comparison */
  
  /* If correct, display message and transition to Correct state. */ 
  if(bCorrect)
  {
    LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
    if(UserApp1_bFinalBoard)
    {
      LCDMessage(LINE1_START_ADDR, "Unscrambled! You've ");
      LCDMessage(LINE2_START_ADDR, "completed the round!");
    }
    else
    {
      LCDMessage(LINE1_START_ADDR, "Unscrambled!        ");
      LCDMessage(LINE2_START_ADDR, "Next board activated");
    }
    
    AllLedsOff();
    LedOn(GREEN);
    
    LedOff(LCD_RED);
    LedOn(LCD_GREEN);
    LedOff(LCD_BLUE);
    
    UserApp1_StateMachine = UserApp1SM_Correct;
  }
  /* If incorrect, display message, start timer and transition to Locked state. */
  else
  {
    LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
    LCDMessage(LINE1_START_ADDR, "Incorrect!          ");
    LCDMessage(LINE2_START_ADDR, "Locked out for:     ");
    
    AllLedsOff();
    LedOn(RED);
    
    LedOn(LCD_RED);
    LedOff(LCD_GREEN);
    LedOff(LCD_BLUE);
    
    UserApp1_u8AttemptCounter++;
    UserApp1_u8LockTimer = UserApp1_u8AttemptCounter * 10;
    
    u8 u8TimerTensTEMP = (UserApp1_u8LockTimer / 10) + 48;
    u8 u8TimerOnesTEMP = (UserApp1_u8LockTimer % 10) + 48;
    u8* au8TimerTens = &u8TimerTensTEMP;
    u8* au8TimerOnes = &u8TimerOnesTEMP;
    LCDMessage(LINE2_START_ADDR + 18, au8TimerTens);
    LCDMessage(LINE2_START_ADDR + 19, au8TimerOnes);
    
    UserApp1_u32SystemTimeStamp = G_u32SystemTime1ms;
    bCorrect = TRUE;
    
    UserApp1_StateMachine = UserApp1SM_Locked;
  }
  
} /* end UserApp1SM_CompareSequence() */
  


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for correct sequence to be found by Compare state */
static void UserApp1SM_Correct(void)
{
  /* TEMPORARY: Press any button to re-initialize and restart game */
  if(WasButtonPressed(BUTTON0) || WasButtonPressed(BUTTON1) || WasButtonPressed(BUTTON2) || WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON1);
    ButtonAcknowledge(BUTTON2);
    ButtonAcknowledge(BUTTON3);
    
    UserApp1Initialize();
    UserApp1_StateMachine = UserApp1SM_Config;
  }
} /* end UserApp1SM_Correct() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for incorrect sequence to be found by Compare state */
static void UserApp1SM_Locked(void)
{
  /* Ensure that any button presses in this state have no effect*/
  ButtonAcknowledge(BUTTON0);
  ButtonAcknowledge(BUTTON1);
  ButtonAcknowledge(BUTTON2);
  ButtonAcknowledge(BUTTON3);
  
  if((G_u32SystemTime1ms - UserApp1_u32SystemTimeStamp) >= 1000)
  {
    UserApp1_u32SystemTimeStamp = G_u32SystemTime1ms;
    UserApp1_u8LockTimer--;
    
    u8 u8TimerTensTEMP = (UserApp1_u8LockTimer / 10) + 48;
    u8 u8TimerOnesTEMP = (UserApp1_u8LockTimer % 10) + 48;
    u8* au8TimerTens = &u8TimerTensTEMP;
    u8* au8TimerOnes = &u8TimerOnesTEMP;
    LCDMessage(LINE2_START_ADDR + 18, au8TimerTens);
    LCDMessage(LINE2_START_ADDR + 19, au8TimerOnes);
  }
  
  if(UserApp1_u8LockTimer <= 0)
  {
    UserApp1_bSequenceTBE = FALSE;
    UserApp1_bAB = FALSE;
    UserApp1_bCD = FALSE;
    UserApp1_bEF = FALSE;
    UserApp1_bOp = FALSE;
    for(int i = 0; i < 6; i++)
    {
      UserApp1_au8Sequence[i] = ' ';
    }
    UserApp1_u8SequenceIndex = 0;
    
    AllLedsOff();
    
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    LedOff(LCD_BLUE);
    
    LCDCommand(LCD_CLEAR_CMD);
    LCDCommand(LCD_HOME_CMD);
    UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
    LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
    
    UpdateLCD();
    
    UserApp1_StateMachine = UserApp1SM_Activated;
  }
  
} /* end UserApp1SM_Locked() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  AllLedsOff();
  LedOn(BLUE);
  LedOn(GREEN);
  LedOn(YELLOW);
  LedOn(RED);
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
