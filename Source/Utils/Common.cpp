#include "../../Include/Utils/Common.h"

/* Global variables common to grid and game class
*/
/* In case of random pattern input, this is set in the
 * pattern generation function.
 * When user inputs cell pattern, this is set in the 
 * process input function
*/
volatile bool cellInitialized = false;
/* mouse click position
*/
double xPos, yPos;
/* this is needed so that we operate only once in the loop.
 * Also, we compute cellX, cellY only if this boolean is 
 * set
*/
volatile bool mouseClicked = false;
/* call back function that is registered to be called upon
 * mouse click
*/