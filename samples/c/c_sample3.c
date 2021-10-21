
#include <stdio.h>
#include <inttypes.h> // for PRId64 and timestamps
#include <interaction_lib/interaction_lib_c.h>
#include <time.h>

//global flag variable for keeping track of user eye state
int flag = 0;

//these time variables are global only so that i can update the maxTime in both main and the core method
int sec = 0;
int sec2 = 0;
int msec = 0;
int msec2 = 0;
int maxTime = 0;

// this struct is used to maintain a focus count (see below)
typedef struct focus_context
{
    IL_InteractorId id;
    size_t          count;
} focus_context;

// this callback is used for the gaze focus event subscription below
// print event data to std out when called and count the number of consecutive focus events
void gaze_focus_callback(IL_GazeFocusEvent evt, void* context)
{
    //id == 9 refers to the whole screen. 0-8 are segments of the screen. 
    if (evt.id == 9){
        //hasFocus refers to if the screen is being looked at or not. 
        if(evt.hasFocus){
            printf("You're looking at the screen \n");
            flag = 1;

            //increase total time by the time spent looking away
            maxTime = maxTime + sec2;

            //function to see how much time was added, for testing
            //printf("time added: %d seconds\n", sec2);
        }
        else if(!evt.hasFocus){
            printf("you looked away \n");
            flag = 2;
        }

    }

    

    if (evt.hasFocus)
    {        
        focus_context* focus = context;
        focus->count = focus->id == evt.id ? focus->count + 1 : 1;
        focus->id = evt.id;
    }
}

int main(int argc, char* argv[])
{
    //time set up
    clock_t start = clock(), diff, diff2;
        
    // create the interaction library
    IL_Context* intlib = NULL;
    IL_CreateInteractionLib(&intlib, IL_FieldOfUse_Interactive);

    // assume single screen with size 2560x1440 and use full screen (not window local) coordinates
    const float width  = 2560.0f;
    const float height = 1440.0f;
    const float offset = 0.0f;

    IL_CoordinateTransformAddOrUpdateDisplayArea(intlib, width, height, 0, 0, 0, 0, NULL);
    IL_CoordinateTransformSetOriginOffset(intlib, offset, offset);

    // setup ids and rectangles that define the interactors we want to use (one in each corner of the screen),
    // then add them to the interaction library
    // only interactor 9 to cover the entire screen is being used in this version. 
    const IL_InteractorId idA = 0;
    const IL_InteractorId idB = 1;
    const IL_InteractorId idC = 2;
    const IL_InteractorId idD = 3;
    const IL_InteractorId idE = 4;
    const IL_InteractorId idF = 5;
    const IL_InteractorId idG = 6;
    const IL_InteractorId idH = 7;
    const IL_InteractorId idJ = 8;
    const IL_InteractorId idK = 9;

    const float size = 500.0f;
    const IL_Rectangle rectA = {            0,             0, size, size };
    const IL_Rectangle rectB = { width - size,             0, size, size };
    const IL_Rectangle rectC = {            0, height - size, size, size };
    const IL_Rectangle rectD = { width - size, height - size, size, size };
    //E will be between A and B
    const IL_Rectangle rectE = {         size,             0, (width - size - size), size };
    //F will be between A and C
    const IL_Rectangle rectF = {         0,             size, size, (height - size - size) };
    //G will be between B and D
    const IL_Rectangle rectG = { width - size,             size, size, (height - size - size) };
    //H will be between C and D
    const IL_Rectangle rectH = {         size, height - size, (width - size - size), size };
    //J will be the middle area
    const IL_Rectangle rectJ = { size, size, (width - size - size), (height - size - size) };
    //K covers the entire screen
    const IL_Rectangle rectK = { 0, 0, width,  height};

    const float z = 0.0f;

    
    IL_BeginInteractorUpdates(intlib);

    IL_AddOrUpdateInteractor(intlib, idA, rectA, z);
    IL_AddOrUpdateInteractor(intlib, idB, rectB, z);
    IL_AddOrUpdateInteractor(intlib, idC, rectC, z);
    IL_AddOrUpdateInteractor(intlib, idD, rectD, z);
    IL_AddOrUpdateInteractor(intlib, idE, rectE, z);
    IL_AddOrUpdateInteractor(intlib, idF, rectF, z);
    IL_AddOrUpdateInteractor(intlib, idG, rectG, z);
    IL_AddOrUpdateInteractor(intlib, idH, rectH, z);
    IL_AddOrUpdateInteractor(intlib, idJ, rectJ, z);
    IL_AddOrUpdateInteractor(intlib, idK, rectK, z);

    IL_CommitInteractorUpdates(intlib);

    // subscribe to gaze focus events using the callback defined above
    // it prints event data to std out when called and counts the number of consecutive focus events
    focus_context focus = { IL_EMPTYINTERACTORID, 0 };

    IL_SubscribeGazeFocusEvents(intlib, gaze_focus_callback, &focus);

    // setup and maintain device connection, wait for device data between events and 
    // update interaction library to trigger all callbacks
    // stop after 3 consecutive focus events on the same interactor
    printf("Starting interaction library update loop.\n");

    const size_t max_focus_count = 3;

    //max program runtime in seconds
    //30 = 30 seconds
    //180 = 3 minutes
    //1800 = 30 minutes
    //3600 = 1 hour
    maxTime = 10;

    //run until program times out
    while (sec < maxTime)
    {
        //if they are not looking, increase max time
        //flag is set to 2 whenever eyes are not detected on the screen and back to 1 when eyes are back on screen
        if (flag == 2){
            
            //calculate the amount of time spent looking away
            diff2 = clock() - diff;
            msec2 = diff2 * 1000 / CLOCKS_PER_SEC;
            sec2 = msec2/1000;

            //send a signal to the vibration motor
            //if time looking away is between 15.0 seconds and 15.025 seconds. 
            //millisecond upperbound set so that it doesn't spam 
            if (sec2 == 15 && msec2 < 15025){
                //printf("%d \n", msec2);
                printf("you've looked away too long, prepare to get bonked\n");
                //how to send signal to vibration motor question mark
            }

            //this calls the program itself
            IL_WaitAndUpdate(intlib, 1000);
        }
        //if they are looking, keep checking how much time has passed to compare to max time
        else{

            //calculate total time since beginning of the program
            diff = clock() - start;
            msec = diff * 1000 / CLOCKS_PER_SEC;
            sec = msec/1000;

            //this calls the program itself
            IL_WaitAndUpdate(intlib, 1000);
        }
        
    }

    //total time the program ran for
    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
    
    // cleanup
    IL_DestroyInteractionLib(intlib);

    return 0;
}
