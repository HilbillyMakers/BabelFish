#include "pwmHandlers.h"
#include "../../usb_device_pins.h"
#include <string.h>

#include "hardware/pwm.h"

enum
{
    PWM_INIT_PIN,
    PWM_DEINIT_PIN,
    PWM_GET_WRAP_SIZE,
    PWM_SET_WRAP_SIZE,
    PWM_GET_DUTY_CYCLE,
    PWM_SET_DUTY_CYCLE,
    PWM_GET_FREQUENCY,
    PWM_SET_FREQUENCY,
};

enum
{
    PWM_E_OK,
    PWM_E_INIT_FAILED,
    PWM_E_ALREADY_INITIALIZED,
    PWM_E_DEINIT_FAILED,
    PWM_E_PIN_INDEX_INVALID,
    PWM_E_INPUT_PIN_CHANNEL_INVALID,

    PWM_E_UNKNOWN
};

uint8_t gpio_pwm_initPin      (uint8_t pinID, bool pinDirection,  uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_deinitPin    (uint8_t pinID,                     uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_get_wrap     (uint8_t pinID,                     uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_set_wrap     (uint8_t pinID, uint16_t wrapSize,  uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_get_dutyCycle(uint8_t pinID,                     uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_set_dutyCycle(uint8_t pinID, uint16_t dutyCycle, uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_get_frequency(uint8_t pinID,                     uint8_t *responseBuffer, uint8_t *errorBuffer);
uint8_t gpio_pwm_set_frequency(uint8_t pinID, uint16_t frequency, uint8_t *responseBuffer, uint8_t *errorBuffer);


uint8_t gpio_pwm_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t command         = commandString[1];
    uint8_t pinID           = commandString[2];
    uint8_t argument        = 0u;
    uint8_t responseSize    = 0u;

    *errorBuffer = PWM_E_UNKNOWN;

    switch(command)
    {
        case PWM_INIT_PIN:
            argument        = commandString[3];
            responseSize    = gpio_pwm_initPin      (pinID, argument, responseBuffer, errorBuffer);
        break;

        case PWM_DEINIT_PIN:
            responseSize    = gpio_pwm_deinitPin    (pinID,           responseBuffer, errorBuffer);
        break;

        case PWM_GET_WRAP_SIZE:
            responseSize    = gpio_pwm_get_wrap     (pinID,           responseBuffer, errorBuffer);
        break;

        case PWM_SET_WRAP_SIZE:
            responseSize    = gpio_pwm_set_wrap     (pinID, argument, responseBuffer, errorBuffer);
        break;

        case PWM_GET_DUTY_CYCLE:
            responseSize    = gpio_pwm_get_dutyCycle(pinID,           responseBuffer, errorBuffer);
        break;

        case PWM_SET_DUTY_CYCLE:
            responseSize    = gpio_pwm_set_dutyCycle(pinID, argument, responseBuffer, errorBuffer);
        break;

        case PWM_GET_FREQUENCY:
            responseSize    = gpio_pwm_get_frequency(pinID,           responseBuffer, errorBuffer);
        break;

        case PWM_SET_FREQUENCY:
            responseSize    = gpio_pwm_set_frequency(pinID, argument, responseBuffer, errorBuffer);
        break;
    }

    return responseSize;
}

uint8_t gpio_pwm_initPin      (uint8_t pinID, bool pinDirection, uint8_t *responseBuffer, uint8_t *errorBuffer)
{

    uint8_t     responseSize  = 0u;
    int8_t      slice_num     = -1;
    int8_t      channel       = -1;
    pwm_config  cfg           = pwm_get_default_config();

    *errorBuffer == PWM_E_UNKNOWN;

    if(PIN_FUNCTION_NONE != pinConfigs[pinID].pinInitialized)
    {
        *errorBuffer = PWM_E_ALREADY_INITIALIZED;
    }

    if(
        (PWM_E_UNKNOWN == *errorBuffer) &&
        (PIN_NUMBER    >  pinID)
    )
    {
        *errorBuffer = PWM_E_PIN_INDEX_INVALID;
    }

    if(PWM_E_UNKNOWN == *errorBuffer)
    {

        *errorBuffer = PWM_E_PIN_INDEX_INVALID;
        slice_num = pwm_gpio_to_slice_num (pinID);
        channel   = pwm_gpio_to_channel   (pinID);
        if(
            (PWM_SUB_IN == pinDirection) &&
            (PWM_CHAN_B != channel)
        )
        {
            /* Only the PWM B pins can be used as inputs */
            *errorBuffer = PWM_E_INPUT_PIN_CHANNEL_INVALID;
        }
        else 
        if (PWM_SUB_IN == pinDirection)
        {
            ///TODO: as the counter A is needed for checking the increment counter, block the assignment of channel A for other functionality
            pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
            ///INFO: Duty cycle is measured once every 100 cycles, to allow 0-100% output
            pwm_config_set_clkdiv(&cfg, 100);
        }
        else
        {
            ///INFO: default wrap is set to 1000
            pwm_config_set_wrap(&cfg, 1000);
        }
    }

    if(PWM_E_UNKNOWN == *errorBuffer)
    {
        pwm_init          (slice_num, &cfg, true);
        gpio_set_function (pinID, GPIO_FUNC_PWM);

        pinConfigs[pinID].pinFunction       = PIN_FUNCTION_PWM;
        pinConfigs[pinID].pinSubfunction    = pinDirection;
        pinConfigs[pinID].pinInitialized    = true;

        memcpy(responseBuffer, "PWM Pin Initialized", sizeof("PWM Pin Initialized"));
        responseSize = sizeof("PWM Pin Initialized");
        
        *errorBuffer = PWM_E_OK;
    }
    else
    {
        /* Pin is already initialized */
        memcpy(responseBuffer, "PWM Pin already initialized", sizeof("PWM Pin already initialized"));
        responseSize = sizeof("PWM Pin already initialized");

        *errorBuffer = PWM_E_INIT_FAILED;
    }

    return responseSize;
}

uint8_t gpio_pwm_deinitPin    (uint8_t pinID,                 uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    if(
          (true               == pinConfigs[pinID].pinInitialized ) &&
          (PIN_FUNCTION_PWM   == pinConfigs[pinID].pinFunction    )
    )
    {
        /*  */
        gpio_deinit (pinID);

        pinConfigs[pinID].pinInitialized    = false;
        pinConfigs[pinID].pinFunction       = PIN_FUNCTION_NONE;
        pinConfigs[pinID].pinSubfunction    = 0u;

        memcpy(responseBuffer, "Pin Deinitialized", sizeof("Pin Deinitialized"));
        responseSize = sizeof("Pin Deinitialized");

        *errorBuffer = PWM_E_OK;
    }
    else
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize = sizeof("Pin not initialized");
        
        *errorBuffer = PWM_E_DEINIT_FAILED;
    }

    return responseSize;
}

uint8_t gpio_pwm_get_wrap     (uint8_t pinID,                 uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize  = 0u;
    ///TODO: read "top" member variable
    uint8_t response      = 0u;
    uint8_t slice_num     = pwm_gpio_to_slice_num (pinID);
    
    response = pwm_hw->slice[slice_num].top;


    
    return responseSize;
}

uint8_t gpio_pwm_set_wrap     (uint8_t pinID, uint16_t wrapSize,  uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize  = 0u;
    uint8_t slice_num     = pwm_gpio_to_slice_num (pinID);

    pwm_hw->slice[slice_num].top;


    return responseSize;
}

uint8_t gpio_pwm_get_dutyCycle(uint8_t pinID,                 uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;



    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");

    return responseSize;
}
  
uint8_t gpio_pwm_set_dutyCycle(uint8_t pinID, uint16_t dutyCycle, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;
    


    pwm_set_gpio_level(pinID, dutyCycle);

    return responseSize;
}

uint8_t gpio_pwm_get_frequency(uint8_t pinID,                 uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");
    return responseSize;
}

uint8_t gpio_pwm_set_frequency(uint8_t pinID, uint16_t frequency, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t responseSize = 0u;

    memcpy(responseBuffer, "Functionality not implemented", sizeof("Functionality not implemented"));
    responseSize = sizeof("Functionality not implemented");
    return responseSize;
}
