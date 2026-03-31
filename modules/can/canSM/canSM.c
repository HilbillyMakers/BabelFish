#include "canSM.h"

canSM_error_e canSM_init(canSM_t *canSM)
{
    canSM_error_e error = E_CAN_SM_UNKNOWN;

    return error;
}

canSM_error_e canSM_run(canSM_t *canSM)
{
    canSM_error_e error = E_CAN_SM_UNKNOWN;
    bool loop = true;

    while(loop)
    {
        switch(canSM->state)
        {
            case S_CAN_SM_UNITINIALIZED:
                
                canSM->state = S_CAN_SM_UNKNOWN;
                loop = true;
                /* Report error */
            break;

            case S_CAN_SM_INITIALIZED:
                
                canSM->state = S_CAN_SM_RUNNING;
                loop = true;
            break;

            case S_CAN_SM_RUNNING:
                
                canSM->state = S_CAN_SM_RUNNING;
                loop = false;
            break;

            default:
                canSM->state = S_CAN_SM_UNKNOWN;
                loop = false;
                /* Report error */
        }
    }

    return error;
}