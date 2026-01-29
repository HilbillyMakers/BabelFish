#include "pwmHandlers.h"
#include "../../devicePins/device_pins.h"
#include <string.h>

#include "hardware/pwm.h"

#define DEFAULT_WRAP_SIZE       1024u
#define DEFAULT_COUNTER_DIVISOR 100u

#define dutyCycleShift(channel)             (0 < channel ? PWM_CH0_CC_B_LSB  : PWM_CH0_CC_A_LSB )
#define dutyCycleMask(channel)              (0 < channel ? PWM_CH0_CC_B_BITS : PWM_CH0_CC_A_BITS)
#define dutyCycleBits(dutyCycle, channel)   (dutyCycle << dutyCycleShift(channel))

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
    PWM_E_INVALID_FREQ_DIV,
    PWM_E_CHANNEL_A_IN_USE,

    PWM_E_UNKNOWN
};
struct FrequencyCounter
{
    bool                active;
    volatile uint32_t *counter;
} frequencyCounters[] = 
{
    {
        .active     = false,
        .counter    = &pwm_hw->slice[0].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[1].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[2].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[3].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[4].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[5].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[6].ctr,
    },
    {
        .active     = false,
        .counter    = &pwm_hw->slice[7].ctr,
    },
};

uint8_t gpio_pwm_initPin      (const uint8_t pinID, const bool pinDirection,    uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_deinitPin    (const uint8_t pinID,                             uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_get_wrap     (const uint8_t pinID,                             uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_set_wrap     (const uint8_t pinID, const uint16_t wrapSize,    uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_get_dutyCycle(const uint8_t pinID,                             uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_set_dutyCycle(const uint8_t pinID, const uint16_t dutyCycle,   uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_get_frequency(const uint8_t pinID,                             uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_pwm_set_frequency(const uint8_t pinID, const uint16_t frequency,   uint8_t* const responseBuffer, uint8_t* const errorBuffer);

uint8_t gpio_pwm_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t     command         = commandString[1];
    uint8_t     pinID           = commandString[2];
    uint8_t     argument        = 0u;
    uint8_t     responseSize    = 0u;
    uint16_t    argument16      = 0u;

    *errorBuffer = PWM_E_UNKNOWN;

    if(PIN_NUMBER < pinID)
    {
        *errorBuffer = PWM_E_PIN_INDEX_INVALID;
        /* Pin index is invalid */
        memcpy(responseBuffer, "Pin index invalid", sizeof("Pin index invalid"));
        responseSize  = sizeof("Pin index invalid");
    }
    else
    {

        switch(command)
        {
            case PWM_INIT_PIN:
                argument        = commandString[3];
                responseSize    = gpio_pwm_initPin      (pinID, argument,   responseBuffer, errorBuffer);
            break;
    
            case PWM_DEINIT_PIN:
                responseSize    = gpio_pwm_deinitPin    (pinID,             responseBuffer, errorBuffer);
            break;
    
            case PWM_GET_WRAP_SIZE:
                responseSize    = gpio_pwm_get_wrap     (pinID,             responseBuffer, errorBuffer);
            break;
    
            case PWM_SET_WRAP_SIZE:
                argument16      = (uint16_t)(commandString[3] | commandString[4] << 8);
                responseSize    = gpio_pwm_set_wrap     (pinID, argument16, responseBuffer, errorBuffer);
            break;
    
            case PWM_GET_DUTY_CYCLE:
                responseSize    = gpio_pwm_get_dutyCycle(pinID,             responseBuffer, errorBuffer);
            break;
    
            case PWM_SET_DUTY_CYCLE:
                argument16      = (uint16_t)(commandString[3] | commandString[4] << 8);
                responseSize    = gpio_pwm_set_dutyCycle(pinID, argument16, responseBuffer, errorBuffer);
            break;
    
            case PWM_GET_FREQUENCY:
                responseSize    = gpio_pwm_get_frequency(pinID,             responseBuffer, errorBuffer);
            break;
    
            case PWM_SET_FREQUENCY:
                argument16      = (uint16_t)(commandString[3] | commandString[4] << 8);
                responseSize    = gpio_pwm_set_frequency(pinID, argument16, responseBuffer, errorBuffer);
            break;
        }
    }

    return responseSize;
}

uint8_t gpio_pwm_initPin      (const uint8_t pinID, const bool pinDirection, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{

    uint8_t     responseSize  = 0u;
    uint8_t     slice_num     = UINT8_MAX;
    uint8_t     channel       = UINT8_MAX;
    pwm_config  cfg           = pwm_get_default_config();

    *errorBuffer = PWM_E_UNKNOWN;

    /* Check if the pin is already initialized */
    if(
        (PIN_FUNCTION_NONE != pinConfigs[pinID].pinFunction   ) ||
        (true              == pinConfigs[pinID].pinInitialized)
    )
    {
        *errorBuffer = PWM_E_ALREADY_INITIALIZED;
        /* Pin is already initialized */
        memcpy(responseBuffer, "PWM Pin already initialized", sizeof("PWM Pin already initialized"));
        responseSize  = sizeof("PWM Pin already initialized");
    }

    /* Check if the pin ID is valid */
    if(
        (PWM_E_UNKNOWN == *errorBuffer)
    )
    {
        slice_num = pwm_gpio_to_slice_num (pinID);
        channel   = pwm_gpio_to_channel   (pinID);

        /* Check if the pin is in output mode */
        if(PWM_SUB_OUT == pinDirection)
        {
            pwm_config_set_wrap(&cfg, DEFAULT_WRAP_SIZE);
        }
        else
        /* Check if the input pin ID is on channel B */
        if(
            (PWM_SUB_IN == pinDirection ) &&
            (PWM_CHAN_B == channel      )
        )
        {
            /* As the counter A is needed for checking the increment counter, block the assignment of channel A for other functionality */
            if(
                (PIN_FUNCTION_NONE != pinConfigs[pinID - 1].pinFunction     ) &&
                (PIN_FUNCTION_PWM  == pinConfigs[pinID - 1].pinFunction     )
            )
            {
                /* WARNING: as the clock is set to only run during high periods of input, channel A is unusable for regular output */
                pwm_config_set_clkdiv_mode  (&cfg, PWM_DIV_B_HIGH);
                /* Duty cycle is measured once every 100 cycles, to allow 0-100% output */
                pwm_config_set_clkdiv       (&cfg, DEFAULT_COUNTER_DIVISOR);
                /* Set the counter as active */
                frequencyCounters[slice_num].active = true;
                ///TODO: Set a separate clock source, running at maximum frequency, for maximum measurement resolution
            }
            else
            {
                memcpy(responseBuffer, "PWM Channel A is in use", sizeof("PWM Channel A is in use"));
                responseSize  = sizeof("PWM Channel A is in use");
                *errorBuffer  = PWM_E_CHANNEL_A_IN_USE;
            }
        }
        else
        {
            memcpy(responseBuffer, "PWM Input Pin Invalid", sizeof("PWM Input Pin Invalid"));
            responseSize  = sizeof("PWM Input Pin Invalid");
            *errorBuffer  = PWM_E_INPUT_PIN_CHANNEL_INVALID;
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
        responseSize  = sizeof("PWM Pin Initialized");
        
        *errorBuffer = PWM_E_OK;
    }

    return responseSize;
}

uint8_t gpio_pwm_deinitPin    (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
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
        responseSize  = sizeof("Pin Deinitialized");

        *errorBuffer = PWM_E_OK;
    }
    else
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
        
        *errorBuffer = PWM_E_DEINIT_FAILED;
    }

    return responseSize;
}

uint8_t gpio_pwm_get_wrap(const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    const uint8_t slice_num     = pwm_gpio_to_slice_num (pinID);
          uint8_t responseSize  = 0u;

    if(true               != pinConfigs[pinID].pinInitialized )
    {
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
    }
    else
    if(PIN_FUNCTION_PWM   != pinConfigs[pinID].pinFunction    )
    {
        memcpy(responseBuffer, "Pin function not PWM", sizeof("Pin function not PWM"));
        responseSize  = sizeof("Pin function not PWM");
    }
    else
    if(PWM_SUB_OUT        != pinConfigs[pinID].pinSubfunction )
    {
        memcpy(responseBuffer, "PWM not output", sizeof("PWM not output"));
        responseSize  = sizeof("PWM not output");
    }
    else
    {
        ((uint16_t *)responseBuffer)[0] = pwm_hw->slice[slice_num].top;
    }

    return responseSize;
}

uint8_t gpio_pwm_set_wrap     (const uint8_t pinID, const uint16_t wrapSize, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    const uint8_t slice_num     = pwm_gpio_to_slice_num (pinID);
          uint8_t responseSize  = 0u;

    if(true               != pinConfigs[pinID].pinInitialized )
    {
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
    }
    else
    if(PIN_FUNCTION_PWM   != pinConfigs[pinID].pinFunction    )
    {
        memcpy(responseBuffer, "Pin function not PWM", sizeof("Pin function not PWM"));
        responseSize  = sizeof("Pin function not PWM");
    }
    else
    if(PWM_SUB_OUT        != pinConfigs[pinID].pinSubfunction )
    {
        memcpy(responseBuffer, "PWM not output", sizeof("PWM not output"));
        responseSize  = sizeof("PWM not output");
    }
    else
    {
        pwm_hw->slice[slice_num].top = wrapSize;
    }

    return responseSize;
}

uint8_t gpio_pwm_get_dutyCycle(const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
          uint8_t   responseSize    = 0u;
    const bool      channel         = (pinID & 0b1u);
    const uint8_t   dutyCycleRShift = ((0 < channel) ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB);
    const uint8_t   slice_num       = (pwm_gpio_to_slice_num(pinID));

    if(true               != pinConfigs[pinID].pinInitialized )
    {
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
    }
    else
    if(PIN_FUNCTION_PWM   != pinConfigs[pinID].pinFunction    )
    {
        memcpy(responseBuffer, "Pin function not PWM", sizeof("Pin function not PWM"));
        responseSize  = sizeof("Pin function not PWM");
    }
    else
    if(PWM_SUB_OUT        != pinConfigs[pinID].pinSubfunction )
    {
        memcpy(responseBuffer, "PWM not output", sizeof("PWM not output"));
        responseSize  = sizeof("PWM not output");
    }
    else
    {
        ((uint16_t *)responseBuffer)[0] = pwm_hw->slice[slice_num].cc >> dutyCycleRShift;
    }

    return responseSize;
}
  
uint8_t gpio_pwm_set_dutyCycle(const uint8_t pinID, const uint16_t dutyCycle, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
          uint8_t   responseSize    = 0u;
    const uint8_t   slice_num       = pwm_gpio_to_slice_num(pinID);
    const bool      channel         = (pinID & 0b1u);

    if(true               != pinConfigs[pinID].pinInitialized )
    {
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
    }
    else
    if(PIN_FUNCTION_PWM   != pinConfigs[pinID].pinFunction    )
    {
        memcpy(responseBuffer, "Pin function not PWM", sizeof("Pin function not PWM"));
        responseSize  = sizeof("Pin function not PWM");
    }
    else
    if(PWM_SUB_OUT        != pinConfigs[pinID].pinSubfunction )
    {
        memcpy(responseBuffer, "PWM not output", sizeof("PWM not output"));
        responseSize  = sizeof("PWM not output");
    }
    else
    {
        hw_write_masked(&pwm_hw->slice[slice_num].cc, dutyCycleBits(dutyCycle, channel), dutyCycleMask(channel));
        memcpy(responseBuffer, "New duty cycle set", sizeof("New duty cycle set"));
        responseSize  = sizeof("New duty cycle set");
    }

    return responseSize;
}

uint8_t gpio_pwm_get_frequency(const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t responseSize    = 0u;
    uint8_t slice_num       = pwm_gpio_to_slice_num (pinID);

    if(true             != pinConfigs[pinID].pinInitialized)
    {
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
    }
    else
    if(PIN_FUNCTION_PWM != pinConfigs[pinID].pinFunction)
    {
        memcpy(responseBuffer, "Pin function not PWM", sizeof("Pin function not PWM"));
        responseSize  = sizeof("Pin function not PWM");
    }
    else
    {
        ((uint16_t *)responseBuffer)[0] = (uint16_t)(pwm_hw->slice[slice_num].div & UINT16_MAX);
    
        memcpy(responseBuffer, "Read current frequency divider", sizeof("Read current frequency divider"));
        responseSize =  sizeof("Read current frequency divider");
    }

    return responseSize;
}

uint8_t gpio_pwm_set_frequency(const uint8_t pinID, const uint16_t frequency, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
          uint8_t responseSize    = 0u;
    const uint8_t slice_num       = pwm_gpio_to_slice_num (pinID);
    const uint8_t integerPart     = frequency >> 0x08u;
    const uint8_t fractionaryPart = frequency &  0xFFu;

    if(true             != pinConfigs[pinID].pinInitialized)
    {
        memcpy(responseBuffer, "Pin not initialized", sizeof("Pin not initialized"));
        responseSize  = sizeof("Pin not initialized");
    }
    else
    if(PIN_FUNCTION_PWM != pinConfigs[pinID].pinFunction)
    {
        memcpy(responseBuffer, "Pin function not PWM", sizeof("Pin function not PWM"));
        responseSize  = sizeof("Pin function not PWM");
    }
    else
    if((0x00u < integerPart) && (0x0Fu > fractionaryPart))
    {
        pwm_hw->slice[slice_num].div = ((uint16_t)(integerPart << PWM_CH0_DIV_INT_LSB) | (uint16_t)(fractionaryPart));

        memcpy(responseBuffer, "Frequency set", sizeof("Frequency set"));
        responseSize  = sizeof("Frequency set");
        *errorBuffer = PWM_E_OK;
    }
    else
    {
        memcpy(responseBuffer, "Frequency invalid", sizeof("Frequency invalid"));
        responseSize  = sizeof("Frequency invalid");
        *errorBuffer = PWM_E_INVALID_FREQ_DIV;
    }
    
    return responseSize;
}
