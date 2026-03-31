typedef enum
{
    S_CAN_SM_UNITINIALIZED,
    S_CAN_SM_INITIALIZED,
    S_CAN_SM_RUNNING,

    S_CAN_SM_UNKNOWN
} canSM_state_e;

typedef struct 
{
    canSM_state_e state;

} canSM_t;

typedef enum
{
    E_CAN_SM_OK,
    E_CAN_SM_UNINITIALIZED,

    E_CAN_SM_UNKNOWN,
    
} canSM_error_e;