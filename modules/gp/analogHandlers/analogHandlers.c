#include "analogHandlers.h"
#include "../../devicePins/device_pins.h"
#include <string.h>
#include "hardware/adc.h"

/* ADC Functionality */
uint8_t gpio_analog_init                (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_deinit              (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_sampleOnce          (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_getResult           (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_sampleOnceB         (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_startSampler        (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_setSamplerChannels  (const uint8_t pinID, uint8_t channelsMask, uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_readSampler         (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t gpio_analog_stopSampler         (const uint8_t pinID,                       uint8_t* const responseBuffer, uint8_t* const errorBuffer);

/* Helper Functionality */
static inline bool sampler_is_running   (void);
static inline bool sampler_is_busy      (void);
static inline bool pin_is_init_as_adc   (uint8_t pinID);

uint8_t gpio_analog_commandStringHandler(uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t     command         = commandString[1]; /* Read the analog command */
    uint8_t     pinID           = commandString[2]; /* Read the pin ID */
    uint8_t     responseSize    = 0u;

    /* Set the error into an unknown state to allow catching unforseen issues */
    *errorBuffer = ADC_E_UNKNOWN;

    if( /* Check for the pin ID to  overlap with the ADC channel pins (pins 26-28 for RP2040) */
        (ADC_MIN_PIN_ID >= pinID) ||
        (ADC_MAX_PIN_ID <= pinID)
    )
    {
        /* Pin index is invalid */
        memcpy(responseBuffer, "Pin index invalid", sizeof("Pin index invalid"));
        responseSize  = sizeof("Pin index invalid");
        
        *errorBuffer = ADC_E_PIN_INDEX_INVALID;
    }
    else
    {
        switch(command)
        {
            case ADC_C_INIT_PIN:
                responseSize = gpio_analog_init        (pinID, responseBuffer, errorBuffer);
            break;

            case ADC_C_DEINIT_PIN:
                responseSize = gpio_analog_deinit      (pinID, responseBuffer, errorBuffer);
            break;

            case ADC_C_SAMPLE_ONCE:
                responseSize = gpio_analog_sampleOnce  (pinID, responseBuffer, errorBuffer);
            break;
            
            case ADC_C_SAMPLE_ONCE_BLOCKING:
                responseSize = gpio_analog_sampleOnceB (pinID, responseBuffer, errorBuffer);
            break;
            
            case ADC_C_GET_RESULT:
                responseSize = gpio_analog_getResult   (pinID, responseBuffer, errorBuffer);
            break;

            case ADC_C_START_SAMPLER:
            case ADC_C_READ_SAMPLER:
            case ADC_C_STOP_SAMPLER:
                memcpy(responseBuffer, "Not implemented", sizeof("Not implemented"));
                responseSize  = sizeof("Not implemented");
                // responseSize = gpio_analog_startSampler(pinID, responseBuffer, errorBuffer);
                // responseSize = gpio_analog_readSampler (pinID, responseBuffer, errorBuffer);            
                // responseSize = gpio_analog_stopSampler (pinID, responseBuffer, errorBuffer);
            break;

            default:
            
                memcpy(responseBuffer, "ADC Command invalid", sizeof("ADC Command invalid"));
                responseSize  = sizeof("ADC Command invalid");
            
                *errorBuffer = ADC_E_COMMAND_UNKNOWN;
        }
    }
    
    return responseSize;
}

uint8_t gpio_analog_init(const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t responseSize  = 0u;

    /* Check if the pin is already initialized */
    if(true == pin_is_init_as_adc(pinID))
    {
        /* Pin is already initialized */
        memcpy(responseBuffer, "ADC Pin already initialized", sizeof("ADC Pin already initialized"));
        responseSize  = sizeof("ADC Pin already initialized");
        
        *errorBuffer = ADC_E_ALREADY_INITIALIZED;
    }
    else
    {
        adc_gpio_init(pinID);

        pinConfigs[pinID].pinFunction       = PIN_FUNCTION_ANALOG;
        pinConfigs[pinID].pinSubfunction    = ANALOG_SUB_NONE;
        pinConfigs[pinID].pinInitialized    = true;

        memcpy(responseBuffer, "ADC Pin Initialized", sizeof("ADC Pin Initialized"));
        responseSize  = sizeof("ADC Pin Initialized");
        
        *errorBuffer = ADC_E_OK;
    }

    return responseSize;
}

uint8_t gpio_analog_deinit(const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t responseSize  = 0u;

    /* Check if the pin is initialized */
    if(false == pin_is_init_as_adc(pinID))
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "ADC Pin not initialized", sizeof("ADC Pin not initialized"));
        responseSize  = sizeof("ADC Pin not initialized");
        
        *errorBuffer = ADC_E_NOT_INITIALIZED;
    }
    else
    /* Check if the continuous sampler is not started */
    if(true == sampler_is_running())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler is running", sizeof("ADC sampler is running"));
        responseSize  = sizeof("ADC sampler is running");
        
        *errorBuffer = ADC_E_SAMPLER_RUNNING;
    }
    /* Check if a previous conversion is still running */
    if(true == sampler_is_busy())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler is busy", sizeof("ADC sampler is busy"));
        responseSize  = sizeof("ADC sampler is busy");
        
        *errorBuffer = ADC_E_SAMPLER_BUSY;
    }
    else
    {
        pinConfigs[pinID].pinFunction       = PIN_FUNCTION_NONE;
        pinConfigs[pinID].pinSubfunction    = 0u;
        pinConfigs[pinID].pinInitialized    = false;

        memcpy(responseBuffer, "ADC Pin Initialized", sizeof("ADC Pin Initialized"));
        responseSize  = sizeof("ADC Pin Initialized");
        
        *errorBuffer = ADC_E_OK;
    }

    return responseSize;
}

uint8_t gpio_analog_sampleOnce      (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    const uint8_t   adcChannel      = pinID - ADC_MIN_PIN_ID;
    uint8_t         responseSize    = 0u;

    /* Check if the pin is initialized */
    if(false == pin_is_init_as_adc(pinID))
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "ADC Pin not initialized", sizeof("ADC Pin not initialized"));
        responseSize  = sizeof("ADC Pin not initialized");
        
        *errorBuffer = ADC_E_NOT_INITIALIZED;
    }
    else
    /* Check if the continuous sampler is not started */
    if(true == sampler_is_running())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_RUNNING;
    }
    /* Check if a previous conversion is still running */
    if(true == sampler_is_busy())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_BUSY;
    }
    else
    {
        adc_select_input(adcChannel);
        hw_set_bits(&adc_hw->cs, ADC_CS_START_ONCE_BITS);

        memcpy(responseBuffer, "Aquiring one ADC sample", sizeof("Aquiring one ADC sample"));
        responseSize  = sizeof("Aquiring one ADC sample");
        
        *errorBuffer = ADC_E_OK;
    }

    return responseSize;
}

uint8_t gpio_analog_getResult       (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    const uint8_t   adcChannel      = pinID - ADC_MIN_PIN_ID;
    uint8_t         responseSize    = 0u;
    uint16_t        adcSample       = UINT16_MAX;

    /* Check if the pin is already initialized */
    if(false == pin_is_init_as_adc(pinID))
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "ADC Pin not initialized", sizeof("ADC Pin not initialized"));
        responseSize  = sizeof("ADC Pin not initialized");
        
        *errorBuffer = ADC_E_NOT_INITIALIZED;
    }
    else
    /* Check if the continuous sampler is not started */
    if(true == sampler_is_running())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_RUNNING;
    }
    /* Check if a previous conversion is still running */
    if(true == sampler_is_busy())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler if busy", sizeof("ADC sampler if busy"));
        responseSize  = sizeof("ADC sampler if busy");
        
        *errorBuffer = ADC_E_SAMPLER_BUSY;
    }
    else
    /* Check if previously selected channel matches the user expectation */
    if(adcChannel != adc_get_selected_input())
    {
        /* Sampling result belongs to a different ADC channel */
        memcpy(responseBuffer, "ADC result channel mismatch", sizeof("ADC result channel mismatch"));
        responseSize  = sizeof("ADC result channel mismatch");
        
        *errorBuffer = ADC_E_CHANNEL_MISMATCH;
    }
    else
    {
        adcSample = adc_read();

        memcpy(responseBuffer, "Aquiring one ADC sample", sizeof("Aquiring one ADC sample"));
        responseSize  = sizeof("Aquiring one ADC sample");
        
        responseBuffer[responseSize]        = (uint8_t)(adcSample & 0xffu);
        responseBuffer[responseSize + 1]    = (uint8_t)(adcSample >> 0x8u);
        
        responseSize += 2u;

        *errorBuffer = ADC_E_OK;
    }

    return responseSize;
}

uint8_t gpio_analog_sampleOnceB     (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    const uint8_t   adcChannel      = pinID - ADC_MIN_PIN_ID;
    uint8_t         responseSize    = 0u;
    uint16_t        adcSample       = UINT16_MAX;

    /* Check if the pin is initialized */
    if(false == pin_is_init_as_adc(pinID))
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "ADC Pin not initialized", sizeof("ADC Pin not initialized"));
        responseSize  = sizeof("ADC Pin not initialized");
        
        *errorBuffer = ADC_E_NOT_INITIALIZED;
    }
    else
    /* Check if the continuous sampler is not started */
    if(true == sampler_is_running())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_RUNNING;
    }
    /* Check if a previous conversion is still running */
    if(true == sampler_is_busy())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_BUSY;
    }
    else
    {
        adc_select_input(adcChannel);
        adcSample = adc_read();

        memcpy(responseBuffer, "Aquiring one ADC sample", sizeof("Aquiring one ADC sample"));
        responseSize  = sizeof("Aquiring one ADC sample");
        
        responseBuffer[responseSize]        = (uint8_t)(adcSample & 0xffu);
        responseBuffer[responseSize + 1]    = (uint8_t)(adcSample >> 0x8u);
        
        responseSize += 2u;

        *errorBuffer = ADC_E_OK;
    }

    return responseSize;
}

uint8_t gpio_analog_startSampler    (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    const uint8_t   adcChannel      = pinID - ADC_MIN_PIN_ID;
    uint8_t         responseSize    = 0u;

    /* Check if the pin is initialized */
    if(false == pin_is_init_as_adc(pinID))
    {
        /* Pin is not initialized */
        memcpy(responseBuffer, "ADC Pin not initialized", sizeof("ADC Pin not initialized"));
        responseSize  = sizeof("ADC Pin not initialized");
        
        *errorBuffer = ADC_E_NOT_INITIALIZED;
    }
    else
    /* Check if the continuous sampler is not started */
    if(true == sampler_is_running())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_RUNNING;
    }
    /* Check if a previous conversion is still running */
    if(true == sampler_is_busy())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler already running", sizeof("ADC sampler already running"));
        responseSize  = sizeof("ADC sampler already running");
        
        *errorBuffer = ADC_E_SAMPLER_BUSY;
    }
    else
    {
        adc_select_input(adcChannel);
        adc_run(true);

        memcpy(responseBuffer, "ADC Pin Initialized", sizeof("ADC Pin Initialized"));
        responseSize  = sizeof("ADC Pin Initialized");
        
        *errorBuffer = ADC_E_OK;
    }


    return responseSize;
}

uint8_t gpio_analog_readSampler     (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t responseSize = 0u;
    ///TODO: implement DMA sample transfer

    return responseSize;
}

uint8_t gpio_analog_stopSampler     (const uint8_t pinID, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t responseSize = 0u;

    /* Check if the continuous sampler is started */
    if(false == sampler_is_running())
    {
        /* Continuous sampler already running */
        memcpy(responseBuffer, "ADC sampler not running", sizeof("ADC sampler not running"));
        responseSize  = sizeof("ADC sampler not running");
        
        *errorBuffer = ADC_E_SAMPLER_NOT_RUNNING;
    }
    else
    {
        adc_run(false);

        memcpy(responseBuffer, "ADC Pin Initialized", sizeof("ADC Pin Initialized"));
        responseSize  = sizeof("ADC Pin Initialized");
        
        *errorBuffer = ADC_E_OK;
    }


    return responseSize;
}

bool sampler_is_running(void)
{
    return (0 < (adc_hw->cs & ADC_CS_START_MANY_BITS));
}

bool sampler_is_busy(void)
{
    return (0 == (adc_hw->cs & ADC_CS_READY_BITS));
}

bool pin_is_init_as_adc(uint8_t pinID)
{
    return pinConfigs[pinID].pinFunction == PIN_FUNCTION_ANALOG;
}