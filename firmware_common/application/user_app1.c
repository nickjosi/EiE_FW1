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
  /* Set all LEDs to the dimmest state we have (0% duty cycle) */
  LedPWM(WHITE, LED_PWM_0);
  LedPWM(PURPLE, LED_PWM_0);
  LedPWM(BLUE, LED_PWM_0);
  LedPWM(CYAN, LED_PWM_0);
  LedPWM(GREEN, LED_PWM_0);
  LedPWM(YELLOW, LED_PWM_0);
  LedPWM(ORANGE, LED_PWM_0);
  LedPWM(RED, LED_PWM_0);
 
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
  /* Counter for each step */
  static u16 u16StepCount = 0;
  /* Variable to indicate which stage to implement */
  static u8 u8Stage = 0;
  /* Counter for each time through stage 0 */
  static u8 u8Stage0Count = 1;
  /* Switch to control fade in (0) vs. fade out (1) */
  static u8 u8FadeState = 0;
  /* Variable to control the current LED during wave */
  static u8 u8WaveState = 1;
  /* Switch to control wave direction */
  static u8 u8WaveDir = 0;
  /* White LED state variable to help with transitions between stage 0 and 2 */
  static u8 u8WhiteOn = 1;
  /* Set initial to 0% duty cycle */
  static LedRateType eCurrentRate = LED_PWM_0;
  
  u16StepCount++;
  if(u16StepCount == 20) //step every 20 ms
  {
    u16StepCount = 0;
    
    //STAGE 0
    // White, blue, green, and orange LEDs fade in and out together
    if(u8Stage == 0)
    {
      if(u8FadeState == 0) //Fade in
      {
        eCurrentRate++;
        /* White LED must be controlled differently due to transition to/from Stage 2 */
        if(u8Stage0Count != 0 || (u8Stage0Count == 0 && eCurrentRate >= LED_PWM_20))
          LedPWM(WHITE, eCurrentRate);
        LedPWM(BLUE, eCurrentRate);
        LedPWM(GREEN, eCurrentRate);
        LedPWM(ORANGE, eCurrentRate);
        
        /* When LEDs reach full brightness, switch to fade out */
        if(eCurrentRate == LED_PWM_100)
          u8FadeState = 1;
      }
      
      else if(u8FadeState == 1) //Fade out
      {
        eCurrentRate--;
        /* White LED must be controlled differently due to transition to/from Stage 2 */
        if(u8Stage0Count != 2 || (u8Stage0Count == 2 && eCurrentRate >= LED_PWM_20))
          LedPWM(WHITE, eCurrentRate);
        LedPWM(BLUE, eCurrentRate);
        LedPWM(GREEN, eCurrentRate);
        LedPWM(ORANGE, eCurrentRate);
        
        /* When LEDs reach 0% duty cycle, move to stage 1 or 2 */
        if(eCurrentRate == LED_PWM_0)
        {
          u8Stage0Count++;
          u8FadeState = 0;
          if(u8Stage0Count == 3) //Once stage 0 has run 3 times, transition to stage 2 (wave sequence)
          {
            u8Stage0Count = 0;
            u8Stage = 2;
          }
          else //If stage 0 has not run 3 times, go to stage 1
            u8Stage = 1;
        }
      }
    }
    
    //STAGE 1
    // Purple, cyan, yellow, and red LEDs fade in and out together
    else if(u8Stage == 1)
    {
      if(u8FadeState == 0) //Fade in
      {
        eCurrentRate++;
        LedPWM(PURPLE, eCurrentRate);
        LedPWM(CYAN, eCurrentRate);
        LedPWM(YELLOW, eCurrentRate);
        LedPWM(RED, eCurrentRate);
        
        /* When LEDs reach full brightness, switch to fade out */
        if(eCurrentRate == LED_PWM_100)
          u8FadeState = 1;
      }
      else if(u8FadeState == 1) //Fade out
      {
        eCurrentRate--;
        LedPWM(PURPLE, eCurrentRate);
        LedPWM(CYAN, eCurrentRate);
        LedPWM(YELLOW, eCurrentRate);
        LedPWM(RED, eCurrentRate);
        
        /* When LEDs reach 0% duty cycle, move back to stage 0 */
        if(eCurrentRate == LED_PWM_0)
        {
          u8FadeState = 0;
          u8Stage = 0;
        }
      }
    }

    //STAGE 2
    // Wave sequence
    else if(u8Stage == 2)
    {
      if(u8FadeState == 0) //Fade in
      {
        eCurrentRate++;
        /* According to u8WaveState, only one LED will fade in */
        if(u8WaveState == 1)
        {
          /* Only fade in when white LED is off */
          /* This is in order to create smooth transitions with stage 0 */
          if(u8WhiteOn == 0)
            LedPWM(WHITE, eCurrentRate);
        }
        else if(u8WaveState == 2)
          LedPWM(PURPLE, eCurrentRate);
        else if(u8WaveState == 3)
          LedPWM(BLUE, eCurrentRate);
        else if(u8WaveState == 4)
          LedPWM(CYAN, eCurrentRate);
        else if(u8WaveState == 5)
          LedPWM(GREEN, eCurrentRate);
        else if(u8WaveState == 6)
          LedPWM(YELLOW, eCurrentRate);
        else if(u8WaveState == 7)
          LedPWM(ORANGE, eCurrentRate);
        else
          LedPWM(RED, eCurrentRate);
        
        /* Once the LED reaches 20% duty cycle, switch to fade out */
        if(eCurrentRate == LED_PWM_20)
          u8FadeState = 1;
      }
      else if(u8FadeState == 1) //Fade out
      {
        eCurrentRate--;
        /* According to u8WaveState, only the LED that is on will fade out */
        if(u8WaveState == 1)
        {
          /* Only fade out when white LED is already on */
          /* This is in order to create smooth transitions with stage 0*/
          if(u8WhiteOn == 1)
            LedPWM(WHITE, eCurrentRate);
        }
        else if(u8WaveState == 2)
          LedPWM(PURPLE, eCurrentRate);
        else if(u8WaveState == 3)
          LedPWM(BLUE, eCurrentRate);
        else if(u8WaveState == 4)
          LedPWM(CYAN, eCurrentRate);
        else if(u8WaveState == 5)
          LedPWM(GREEN, eCurrentRate);
        else if(u8WaveState == 6)
          LedPWM(YELLOW, eCurrentRate);
        else if(u8WaveState == 7)
          LedPWM(ORANGE, eCurrentRate);
        else
          LedPWM(RED, eCurrentRate);
        
        /* Once the LED reaches 0% duty cycle, either:
              a. move to fade in for next LED
              b. change direction of wave
              c. begin stage 0                       */
        if(eCurrentRate == LED_PWM_0)
        {
          u8FadeState = 0;
          
          //Left-to-right direction
          if(u8WaveDir == 0)
          {
            u8WaveState++; //move to the next LED to the right
            
            /* Once the red LED has faded in and out, reverse direction of wave 
            beginning with the orange LED */
            if(u8WaveState == 9)
            {
              u8WaveDir = 1;
              u8WaveState = 7;
              
              /* Set to off so that the white LED will fade in during next wave direction */
              u8WhiteOn = 0;
            }
          }
          
          //Right-to-left direction
          else if(u8WaveDir == 1)
          {
            u8WaveState--; //move to the next LEd to the left
            
            /* Once the white LED has faded in, reset wave direction, wave state, and
            the white LED state variable, and begin stage 0 */
            if(u8WaveState == 0)
            {
              u8WaveDir = 0;
              u8WaveState = 1;
              u8WhiteOn = 1;
              u8Stage = 0;
            }
          }
        }
      }
    }
    
    
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
