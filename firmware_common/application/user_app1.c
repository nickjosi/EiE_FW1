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
  PWMAudioSetFrequency(BUZZER1, 500);
 
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
  //OCTAVE FREQUENCIES
  static u16 au16C2Freqs[] = {65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 124};
  static u16 au16C3Freqs[] = {131, 139, 147, 156, 166, 175, 185, 196, 208, 220, 233, 247};
  static u16 au16C4Freqs[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};
  static u16 au16C5Freqs[] = {523, 554, 587, 622, 659, 699, 740, 784, 831, 880, 932, 988};
  static u16 *pu16Freq = &au16C4Freqs[0];
  
  //INPUT BUFFER ARRAY AND INPUT CHAR COUNT
  static u8 au8InputBuffer[3];
  u8 u8CharCount;
  
  //VARIABLES FOR PLAYING THE NOTES
  static bool bPlay = FALSE;
  static u32 u32Timer = 0;
  
  u8CharCount = DebugScanf(au8InputBuffer);
  if(u8CharCount > 0)
  { 
    bPlay = TRUE;
    u32Timer = 0; //reset timer each time a key is pressed
    
    //set the frequency according to which key is pressed
    if(au8InputBuffer[0] == 'q') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[0]);
    }
    else if(au8InputBuffer[0] == 'w') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[1]);
    }
    else if(au8InputBuffer[0] == 'e') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[2]);
    }
    else if(au8InputBuffer[0] == 'r') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[3]);
    }
    else if(au8InputBuffer[0] == 'a') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[4]);
    }
    else if(au8InputBuffer[0] == 's') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[5]);
    }
    else if(au8InputBuffer[0] == 'd') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[6]);
    }
    else if(au8InputBuffer[0] == 'f') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[7]);
    }
    else if(au8InputBuffer[0] == 'z') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[8]);
    }
    else if(au8InputBuffer[0] == 'x') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[9]);
    }
    else if(au8InputBuffer[0] == 'c') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[10]);
    }
    else if(au8InputBuffer[0] == 'v') {
      PWMAudioSetFrequency(BUZZER1, pu16Freq[11]);
    }
    //if an unassigned key is pressed, play nothing
    else {
      bPlay = FALSE;
    }
  }
  
  
  if(bPlay) {
    PWMAudioOn(BUZZER1);
    u32Timer++;
    
    //a note will play for 100ms unless interrupted by another key press
    if(u32Timer == 100) {
      bPlay = FALSE;
    }
  }
  else {
    PWMAudioOff(BUZZER1);
  }
  
  //CHANGE OCTAVE by moving the pointer to the respective array
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    pu16Freq = &au16C2Freqs[0];
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    pu16Freq = &au16C3Freqs[0];
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    pu16Freq = &au16C4Freqs[0];
  }
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    pu16Freq = &au16C5Freqs[0];
  }
  
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
