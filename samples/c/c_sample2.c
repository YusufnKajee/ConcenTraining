/*
COPYRIGHT 2018-2020  - PROPERTY OF TOBII AB
-------------------------------------
2018-2020 TOBII AB - KARLSROVAGEN 2D, DANDERYD 182 53, SWEDEN - All Rights Reserved.

NOTICE:  All information contained herein is, and remains, the property of Tobii AB and its suppliers, if any.
The intellectual and technical concepts contained herein are proprietary to Tobii AB and its suppliers and may be
covered by U.S.and Foreign Patents, patent applications, and are protected by trade secret or copyright law.
Dissemination of this information or reproduction of this material is strictly forbidden unless prior written
permission is obtained from Tobii AB.

*/

#include <stdio.h>
#include <inttypes.h> // for PRId64 and timestamps
#include <interaction_lib/interaction_lib_c.h>
#include <time.h>

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
    if (evt.id == 0){
        printf("top left corner ");
    }
    else if (evt.id == 1){
        printf("top right corner ");
    }
    else if (evt.id == 2){
        printf("bottom left corner ");
    }
    else if (evt.id == 3){
        printf("top right corner ");
    }
    else if (evt.id == 4){
        printf("top middle ");
    }
    else if (evt.id == 5){
        printf("left side ");
    }
    else if (evt.id == 6){
        printf("right side ");
    }
    else if (evt.id == 7){
        printf("bottom middle ");
    }
    else if (evt.id == 8){
        printf("middle area ");
    }
    else{
        printf("out of bounds ");
    }
    printf("Interactor: %" PRIu64 ", focused: %s\n",
           evt.id,
           evt.hasFocus ? "true" : "false");

    // printf("Interactor: %" PRIu64 ", focused: %s, timestamp: %" PRId64 " us\n",
    //        evt.id,
    //        evt.hasFocus ? "true" : "false",
    //        evt.timestamp_us);
    

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
    clock_t start = clock(), diff;
        
    //time set up end

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
    const IL_InteractorId idA = 0;
    const IL_InteractorId idB = 1;
    const IL_InteractorId idC = 2;
    const IL_InteractorId idD = 3;
    const IL_InteractorId idE = 4;
    const IL_InteractorId idF = 5;
    const IL_InteractorId idG = 6;
    const IL_InteractorId idH = 7;
    const IL_InteractorId idJ = 8;

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

    //time print out
    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    int sec = msec/1000;
    int milisec = msec%1000;

    // while (focus.count < max_focus_count)
    // {
    //     diff = clock() - start;
    //     msec = diff * 1000 / CLOCKS_PER_SEC;
    //     //printf("time: %d\n milliseconds", msec);
    //     IL_WaitAndUpdate(intlib, 1000);
    // }

    //max program runtime in seconds
    //30 = 30 seconds
    //180 = 3 minutes
    //1800 = 30 minutes
    //3600 = 1 hour
    int maxTime = 10;

    //run until program times out
    while (sec < maxTime)
    {
        diff = clock() - start;
        msec = diff * 1000 / CLOCKS_PER_SEC;
        sec = msec/1000;
        IL_WaitAndUpdate(intlib, 1000);
    }


    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
    //time print out end

    //printf("Interactor %" PRIu64 " got focused %zu times\n", focus.id, focus.count);

    // cleanup
    IL_DestroyInteractionLib(intlib);

    return 0;
}
