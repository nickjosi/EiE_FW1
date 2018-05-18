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

static u8 UserApp1_au8CorrectSequence[6];
static u8 UserApp1_u8CorrectSequenceIndex;
static u8 UserApp1_au8Sequence[6];
static u8 UserApp1_u8SequenceIndex;
static u8 UserApp1_u8CursorPosition;

static bool UserApp1_bConfig;
static bool UserApp1_bFull;

static bool UserApp1_bSequenceTBE;
static bool UserApp1_bAB;
static bool UserApp1_bCD;
static bool UserApp1_bEF;
static bool UserApp1_bOp;

static u8 UserApp1_u8AttemptCounter;
static u8 UserApp1_u8LockTimer;
static u32 UserApp1_u32SystemTimeStamp;



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
  UserApp1_bConfig =        FALSE;
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
  static u8 au8MainMenu1_Config[] = "Set sequence:       ";
  static u8 au8MainMenu1[] =        "UNSCRAMBLE ||       ";
  static u8 au8MainMenu2[] =        ">A,B  >C,D  >E,F  Op";
  static u8 au8AB[] =               ">A    >B        Back";
  static u8 au8CD[] =               ">C    >D        Back";
  static u8 au8EF[] =               ">E    >F        Back";
  static u8 au8Op[] =               "Entr  Del  Clr  Back";
  static u8 au8Full[] =             "ENTR  DEL  CLR      ";
  
  if(!UserApp1_bConfig)
  {
    LCDMessage(LINE1_START_ADDR, au8MainMenu1_Config);
    LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8CorrectSequence);
  }
  else
  {
    LCDMessage(LINE1_START_ADDR, au8MainMenu1);
    LCDMessage(LINE1_START_ADDR + 14, UserApp1_au8Sequence);
  }
  
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
  
  if(UserApp1_bFull)
  {
    LCDMessage(LINE2_START_ADDR, au8Full);
  }
  
  LCDCommand(LCD_ADDRESS_CMD | UserApp1_u8CursorPosition);
}
/* --- end UpdateLCD() --- */

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Config(void)
{
  /*
  UserApp1_au8CorrectSequence[0] = 'A';
  UserApp1_au8CorrectSequence[1] = 'B';
  UserApp1_au8CorrectSequence[2] = 'C';
  UserApp1_au8CorrectSequence[3] = 'D';
  UserApp1_au8CorrectSequence[4] = 'E';
  UserApp1_au8CorrectSequence[5] = 'F';
  */
  
  /* --- Enter Sequence Mode --- */
  
  if(!UserApp1_bFull)
  {
    /* Button 0 */
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bAB = TRUE;
      }
      
      else
      {
        if(UserApp1_bAB)
        {
          UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = 'A';
          UserApp1_bAB = FALSE;
        }
        if(UserApp1_bCD)
        {
          UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = 'C';
          UserApp1_bCD = FALSE;
        }
        if(UserApp1_bEF)
        {
          UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = 'E';
          UserApp1_bEF = FALSE;
        }
        
        UserApp1_u8CorrectSequenceIndex++;
        UserApp1_u8CursorPosition++;
        
        if(UserApp1_bOp)
        {
          //add code for ENTER here
          UserApp1_StateMachine = UserApp1SM_Unactivated;
          UserApp1_bConfig = TRUE;
          UserApp1_bOp = FALSE;
          UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
        }
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 0 */
    
    /* Button 1 */
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bCD = TRUE;
      }
      else if(UserApp1_bSequenceTBE && !UserApp1_bOp)
      {
        if(UserApp1_bAB)
        {
          UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = 'B';
          UserApp1_bAB = FALSE;
        }
        if(UserApp1_bCD)
        {
          UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = 'D';
          UserApp1_bCD = FALSE;
        }
        if(UserApp1_bEF)
        {
          UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = 'F';
          UserApp1_bEF = FALSE;
        }
        
        UserApp1_u8CorrectSequenceIndex++;
        UserApp1_u8CursorPosition++;
        UserApp1_bSequenceTBE = FALSE;
      }
      else if(UserApp1_bSequenceTBE && UserApp1_bOp)
      {
        if(UserApp1_u8CorrectSequenceIndex > 0)
        {
          UserApp1_u8CorrectSequenceIndex--;
          UserApp1_u8CursorPosition--;
        }
        UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = ' ';
        UserApp1_bOp = FALSE;
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 1 */
    
    /* Button 2 */
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bEF = TRUE;
      }
      else
      {
        if(UserApp1_bOp)
        {
          for(int i = 0; i < 6; i++)
          {
            UserApp1_au8CorrectSequence[i] = ' ';
          }
          UserApp1_u8CorrectSequenceIndex = 0;
          UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
          UserApp1_bOp = FALSE;
        }
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 2 */
    
    /* Button 3 */
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bOp = TRUE;
      }
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
    
    if(UserApp1_u8CorrectSequenceIndex >= 6)
    {
      UserApp1_bFull = TRUE;
      UpdateLCD();
    }
    
  } /* --- end Enter Sequence Mode --- */
  
  
  /* --- Full Sequence Entered Mode --- */
  
  if(UserApp1_bFull)
  {
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      //add code for ENTER here
      UserApp1_StateMachine = UserApp1SM_Unactivated;
      UserApp1_bConfig = TRUE;
      UserApp1_bFull = FALSE;
      UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
      
      UpdateLCD();
    }
    
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      
      UserApp1_u8CorrectSequenceIndex--;
      UserApp1_au8CorrectSequence[UserApp1_u8CorrectSequenceIndex] = ' ';
      UserApp1_u8CursorPosition--;
      
      UpdateLCD();
    }
    
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      
      for(int i = 0; i < 6; i++)
      {
        UserApp1_au8CorrectSequence[i] = ' ';
      }
      UserApp1_u8CorrectSequenceIndex = 0;
      UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
      
      UpdateLCD();
    }
    
    if(UserApp1_u8CorrectSequenceIndex < 6)
    {
      UserApp1_bFull = FALSE;
      UpdateLCD();
    }
    
  } /* --- end Full Sequence Entered Mode --- */
  
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
  /* --- Enter Sequence Mode --- */
  
  if(!UserApp1_bFull)
  {
    /* Button 0 */
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bAB = TRUE;
      }
      
      else
      {
        if(UserApp1_bAB)
        {
          UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = 'A';
          UserApp1_bAB = FALSE;
        }
        if(UserApp1_bCD)
        {
          UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = 'C';
          UserApp1_bCD = FALSE;
        }
        if(UserApp1_bEF)
        {
          UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = 'E';
          UserApp1_bEF = FALSE;
        }
        
        UserApp1_u8SequenceIndex++;
        UserApp1_u8CursorPosition++;
        
        if(UserApp1_bOp)
        {
          //add code for ENTER here
          UserApp1_StateMachine = UserApp1SM_CompareSequence;
        }
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 0 */
    
    /* Button 1 */
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bCD = TRUE;
      }
      else if(UserApp1_bSequenceTBE && !UserApp1_bOp)
      {
        if(UserApp1_bAB)
        {
          UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = 'B';
          UserApp1_bAB = FALSE;
        }
        if(UserApp1_bCD)
        {
          UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = 'D';
          UserApp1_bCD = FALSE;
        }
        if(UserApp1_bEF)
        {
          UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = 'F';
          UserApp1_bEF = FALSE;
        }
        
        UserApp1_u8SequenceIndex++;
        UserApp1_u8CursorPosition++;
        UserApp1_bSequenceTBE = FALSE;
      }
      else if(UserApp1_bSequenceTBE && UserApp1_bOp)
      {
        if(UserApp1_u8SequenceIndex > 0)
        {
          UserApp1_u8SequenceIndex--;
          UserApp1_u8CursorPosition--;
        }
        UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = ' ';
        UserApp1_bOp = FALSE;
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 1 */
    
    /* Button 2 */
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bEF = TRUE;
      }
      else
      {
        if(UserApp1_bOp)
        {
          for(int i = 0; i < 6; i++)
          {
            UserApp1_au8Sequence[i] = ' ';
          }
          UserApp1_u8SequenceIndex = 0;
          UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
          UserApp1_bOp = FALSE;
        }
        UserApp1_bSequenceTBE = FALSE;
      }
      
      UpdateLCD();
      
    } /* end Button 2 */
    
    /* Button 3 */
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      
      if(!UserApp1_bSequenceTBE)
      {
        UserApp1_bSequenceTBE = TRUE;
        UserApp1_bOp = TRUE;
      }
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
    
    if(UserApp1_u8SequenceIndex >= 6)
    {
      UserApp1_bFull = TRUE;
      UpdateLCD();
    }
    
  } /* --- end Enter Sequence Mode --- */
  
  
  /* --- Full Sequence Entered Mode --- */
  
  if(UserApp1_bFull)
  {
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      //add code for ENTER here
      UserApp1_StateMachine = UserApp1SM_CompareSequence;
      
      //UpdateLCD();
    }
    
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      
      UserApp1_u8SequenceIndex--;
      UserApp1_au8Sequence[UserApp1_u8SequenceIndex] = ' ';
      UserApp1_u8CursorPosition--;
      
      UpdateLCD();
    }
    
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      
      for(int i = 0; i < 6; i++)
      {
        UserApp1_au8Sequence[i] = ' ';
      }
      UserApp1_u8SequenceIndex = 0;
      UserApp1_u8CursorPosition = LINE1_START_ADDR + 14;
      
      UpdateLCD();
    }
    
    if(UserApp1_u8SequenceIndex < 6)
    {
      UserApp1_bFull = FALSE;
      UpdateLCD();
    }
    
  } /* --- end Full Sequence Entered Mode --- */
  
} /* end UserApp1SM_Activated() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_CompareSequence(void)
{
  static bool bCorrect = TRUE;
  
  for(int i = 0; i < 6; i++)
  {
    if(UserApp1_au8Sequence[i] != UserApp1_au8CorrectSequence[i])
    {
      bCorrect = FALSE;
    }
  }
  
  if(bCorrect)
  {
    LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
    LCDMessage(LINE1_START_ADDR, "Correct!            ");
    LCDMessage(LINE2_START_ADDR, "Next board activated");
    
    AllLedsOff();
    LedOn(GREEN);
    
    UserApp1_StateMachine = UserApp1SM_Correct;
  }
  else
  {
    LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
    LCDMessage(LINE1_START_ADDR, "Incorrect!          ");
    LCDMessage(LINE2_START_ADDR, "Locked out for:     ");
    
    AllLedsOff();
    LedOn(RED);
    
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
/* Wait for ??? */
static void UserApp1SM_Correct(void)
{
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    
    UserApp1Initialize();
    UserApp1_StateMachine = UserApp1SM_Config;
  }
} /* end UserApp1SM_Correct() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Locked(void)
{
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
