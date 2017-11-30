/**
 * @file timer.h
 * @date 2015-12-18
 *
 * NOTE:
 * This file is generated by DAVE. Any manual modification done to this file will be lost when the code is regenerated.
 *
 * @cond
 ***********************************************************************************************************************
 * TIMER v4.1.10 - Configures the properties of CCU4 or CCU8 peripheral as a timer.
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2015-02-16:
 *     - Initial version<br>
 *
 * 2015-05-08:
 *     - "initialized" flag is updated in local init before starting the timer<br>
 *     - TIMER_GetTime() and TIMER_Clear() are added.<br>
 *     - Timer clear is removed from TIMER_Stop() API<br>
 *     - Additional condition check for timer state is added in TIMER_SetTimeInterval() API<br>
 *     - TIMER_CCU4_lShadowTransfer(), TIMER_CCU8_lShadowTransfer() are updated to use the mask from APP handler<br>
 *
 * 2015-05-22:
 *     -  API name changed
 *          a. TIMER_AcknowledgeInterrupt() --> TIMER_ClearEvent()<br>
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "timer.h"

/***********************************************************************************************************************
 * MACROS
 **********************************************************************************************************************/
#define TIMER_CMP_100_DUTY             ((uint16_t)0) /* Compare value for 100% duty cycle */
#define TIMER_RESOLUTION_SEC_TO_MICRO  (100000000U)  /* Convert the resolution from sec to usec, by dividing with the \
                                                      scale factor */
#define TIMER_PRESCALER_MAX            (15U)      /* Maximum prescaler values allowed */
#define TIMER_PERIOD_16BIT_MAX         (0xFFFFU)  /* Maximum period value */
#define TIMER_PERIOD_MIN               (0x1U)     /* Minimum period value */
#define TIMER_CLK_SCALE_FACTOR         (32U)      /* Scale factor used during calculation of the "TIMER_CLK_CONST_SCALED" */

/***********************************************************************************************************************
 * LOCAL DATA
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * LOCAL ROUTINES
 **********************************************************************************************************************/
#ifdef TIMER_CCU4_USED
TIMER_STATUS_t TIMER_CCU4_lInit(TIMER_t* const handle_ptr);
void TIMER_CCU4_lShadowTransfer(TIMER_t* const handle_ptr);
#endif

#ifdef TIMER_CCU8_USED
TIMER_STATUS_t TIMER_CCU8_lInit(TIMER_t* const handle_ptr);
void TIMER_CCU8_lShadowTransfer(TIMER_t* const handle_ptr);
#endif

 /**********************************************************************************************************************
 * API IMPLEMENTATION
 **********************************************************************************************************************/
 /* Returns the version of the TIMER APP.
 */
DAVE_APP_VERSION_t TIMER_GetAppVersion(void)
{
  DAVE_APP_VERSION_t version;

  version.major = TIMER_MAJOR_VERSION;
  version.minor = TIMER_MINOR_VERSION;
  version.patch = TIMER_PATCH_VERSION;

  return version;
}

/*
 * This function initializes a TIMER APP based on user configuration.
 *
 */
TIMER_STATUS_t TIMER_Init(TIMER_t* const handle_ptr)
{
  TIMER_STATUS_t status;

  XMC_ASSERT("TIMER_Init:handle_ptr NULL" , (handle_ptr != NULL));

  status = TIMER_STATUS_SUCCESS;
  /* Check for APP instance is initialized or not */
  if (false == handle_ptr->initialized)
  {
#ifdef TIMER_CCU4_USED
    if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
    {
      /* Configure CCU4 timer for the required time tick settings */
      status = TIMER_CCU4_lInit(handle_ptr);
    }
#endif

#ifdef TIMER_CCU8_USED
    if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
    {
      /* Configure CCU8 timer for the required time tick settings */
      status = TIMER_CCU8_lInit(handle_ptr);
    }
#endif
  }

  return (status);
}

/*
 * This function starts the timer to generate the events for the specified time_interval value
 */
TIMER_STATUS_t TIMER_Start(TIMER_t  *const handle_ptr)
{
  TIMER_STATUS_t status;

  XMC_ASSERT("TIMER_Start:handle_ptr NULL" , (handle_ptr != NULL));

  /* Check for APP instance is initialized or not */
  if (true == handle_ptr->initialized)
  {
#ifdef TIMER_CCU4_USED
    if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
    {
      /* Start the timer manually */
      XMC_CCU4_SLICE_StartTimer(handle_ptr->ccu4_slice_ptr);
    }
#endif

#ifdef TIMER_CCU8_USED
    if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
       {
      /* Start the timer manually */
      XMC_CCU8_SLICE_StartTimer(handle_ptr->ccu8_slice_ptr);
       }
#endif
    status = TIMER_STATUS_SUCCESS;
  }
  else
  {
    status = TIMER_STATUS_FAILURE;
  }

  return (status);
}

/*
 * This function stops and clears the timer
 */
TIMER_STATUS_t TIMER_Stop(TIMER_t  *const handle_ptr)
{
  TIMER_STATUS_t status;

  XMC_ASSERT("TIMER_Stop:handle_ptr NULL" , (handle_ptr != NULL));

  /* Check whether timer is initialized and in running state */
  if ((TIMER_GetTimerStatus(handle_ptr)) && (true == handle_ptr->initialized))
  {
#ifdef TIMER_CCU4_USED
    if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
    {
      /* Stops the timer */
      XMC_CCU4_SLICE_StopTimer(handle_ptr->ccu4_slice_ptr);
    }
#endif

#ifdef TIMER_CCU8_USED
    if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
       {
      /* Stops the timer */
      XMC_CCU8_SLICE_StopTimer(handle_ptr->ccu8_slice_ptr);
       }
#endif
    status = TIMER_STATUS_SUCCESS;
  }
  else
  {
    status = TIMER_STATUS_FAILURE;
  }

  return (status);
}

/*
 * This function returns the status of the timer
 */
bool TIMER_GetTimerStatus(TIMER_t  *const handle_ptr)
{
  bool status;

  XMC_ASSERT("TIMER_GetTimerStatus:handle_ptr NULL" , (handle_ptr != NULL));

  status = false;

#ifdef TIMER_CCU4_USED
  if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
  {
    /* Returns the current status of the timer */
    status = XMC_CCU4_SLICE_IsTimerRunning(handle_ptr->ccu4_slice_ptr);
  }
#endif

#ifdef TIMER_CCU8_USED
  if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
  {
    /* Returns the current status of the timer */
    status = XMC_CCU8_SLICE_IsTimerRunning(handle_ptr->ccu8_slice_ptr);
  }
#endif

  return (status);
}

/*
 * This function changes the PWM period which in turn changes the time tick interval value by checking that
 * the given time tick value is within supported range.
 */
TIMER_STATUS_t TIMER_SetTimeInterval(TIMER_t  *const handle_ptr, uint32_t time_interval)
{
  TIMER_STATUS_t status;
  uint32_t lfrequency;
  uint32_t lprescaler;

  XMC_ASSERT("TIMER_SetTimeInterval:handle_ptr NULL" , (handle_ptr != NULL));

  status = TIMER_STATUS_FAILURE;

  if (false == TIMER_GetTimerStatus(handle_ptr))
  {
    /* check for time_interval range */
    if ((time_interval >= handle_ptr->timer_min_value_us) && (time_interval <= handle_ptr->timer_max_value_us))
    {
      /* Initialize the prescaler */
      lprescaler = 0U;
      while (time_interval > (handle_ptr->timer_max_value_us >> (TIMER_PRESCALER_MAX - lprescaler)))
      {
        lprescaler++;
      }
#ifdef TIMER_CCU4_USED
      if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
      {
        lfrequency = handle_ptr->global_ccu4_handler->module_frequency;
        handle_ptr->ccu4_slice_config_ptr->prescaler_initval = lprescaler;
        /* Calculate the period register for the required time_interval value */
        handle_ptr->period_value = (uint16_t)((((uint64_t)time_interval * lfrequency) >> \
                                               handle_ptr->ccu4_slice_config_ptr->prescaler_initval) / \
                                              TIMER_RESOLUTION_SEC_TO_MICRO);
        /* Actual timer period values is Period_reg_val+1U */
        if (handle_ptr->period_value > TIMER_PERIOD_MIN)
        {
          (handle_ptr->period_value)--;
        }
        /* Update the prescaler */
        XMC_CCU4_SLICE_SetPrescaler(handle_ptr->ccu4_slice_ptr, handle_ptr->ccu4_slice_config_ptr->prescaler_initval);
        /* update period, compare and prescaler values */
        TIMER_CCU4_lShadowTransfer(handle_ptr);
        /* Update the status */
        status = TIMER_STATUS_SUCCESS;
      }
#endif

#ifdef TIMER_CCU8_USED
      if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
      {
        handle_ptr->ccu8_slice_config_ptr->prescaler_initval = lprescaler;
        lfrequency = handle_ptr->global_ccu8_handler->module_frequency;
        /* Calculate the period register for the required time_interval value */
        handle_ptr->period_value = (uint16_t)((((uint64_t)time_interval * lfrequency) >> \
                                               handle_ptr->ccu8_slice_config_ptr->prescaler_initval) / \
                                              TIMER_RESOLUTION_SEC_TO_MICRO);
        /* Actual timer period values is Period_reg_val+1U */
        if (handle_ptr->period_value > TIMER_PERIOD_MIN)
        {
          (handle_ptr->period_value)--;
        }
        /* Update the prescaler */
        XMC_CCU8_SLICE_SetPrescaler(handle_ptr->ccu8_slice_ptr, handle_ptr->ccu8_slice_config_ptr->prescaler_initval);
        /* update period, compare and prescaler values */
        TIMER_CCU8_lShadowTransfer(handle_ptr);
        /* Update the status */
        status = TIMER_STATUS_SUCCESS;
      }
#endif
    }
  }
  return (status);
}

/*
 * This function reads the timer event(period match interrupt) status of the given timer
 */
bool TIMER_GetInterruptStatus(TIMER_t * const handle_ptr)
{
  bool status;
  XMC_ASSERT("TIMER_GetInterruptStatus:handle_ptr NULL" , (handle_ptr != NULL));
  status = false;
#ifdef TIMER_CCU4_USED
  if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
  {
    /* Reads the interrupt status */
    status = XMC_CCU4_SLICE_GetEvent(handle_ptr->ccu4_slice_ptr, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  }
#endif

#ifdef TIMER_CCU8_USED
  if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
  {
    /* Reads the interrupt status */
    status = XMC_CCU8_SLICE_GetEvent(handle_ptr->ccu8_slice_ptr, XMC_CCU8_SLICE_IRQ_ID_PERIOD_MATCH);
  }
#endif
  return (status);
}

/*
 * This function clears the period match interrupt status of the given timer.
 */
void TIMER_ClearEvent(TIMER_t *const handle_ptr)
{
  XMC_ASSERT("TIME_CCU_AcknowledgeInterrupt:handle_ptr NULL" , (handle_ptr != NULL));

#ifdef TIMER_CCU4_USED
  if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
  {
    /* clears the timer event(period match interrupt) */
    XMC_CCU4_SLICE_ClearEvent(handle_ptr->ccu4_slice_ptr, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  }
#endif

#ifdef TIMER_CCU8_USED
  if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
  {
    /* clears the timer event(period match interrupt) */
    XMC_CCU8_SLICE_ClearEvent(handle_ptr->ccu8_slice_ptr, XMC_CCU8_SLICE_IRQ_ID_PERIOD_MATCH);
  }
#endif
}

/*
 * This function returns the current time value
 */
uint32_t TIMER_GetTime(TIMER_t *const handle_ptr)
{
  uint32_t ltimer_val;
  uint32_t lprescaler;
  uint32_t ltime_val;

  XMC_ASSERT("TIMER_GetTimerStatus:handle_ptr NULL" , (handle_ptr != NULL));
  ltime_val = 0U;

#ifdef TIMER_CCU4_USED
  if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
  {
    /* Added one to according to the edge aligned mode */
    ltimer_val = (uint32_t)XMC_CCU4_SLICE_GetTimerValue(handle_ptr->ccu4_slice_ptr) + 1U;
    lprescaler = handle_ptr->ccu4_slice_config_ptr->prescaler_initval;

    /* calculate the time value in micro seconds and scaled with 100 */
    ltime_val = (uint32_t)((uint64_t)((uint64_t)ltimer_val * (uint64_t)TIMER_CLK_CONST_SCALED) >> \
                           (TIMER_CLK_SCALE_FACTOR - lprescaler));
  }
#endif

#ifdef TIMER_CCU8_USED
  if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
  {
    /* Added one to according to the edge aligned mode */
    ltimer_val = (uint32_t)XMC_CCU8_SLICE_GetTimerValue(handle_ptr->ccu8_slice_ptr) + 1U;
    lprescaler = handle_ptr->ccu8_slice_config_ptr->prescaler_initval;

    /* calculate the time value in micro seconds and scaled with 100 */
    ltime_val = (uint32_t)((uint64_t)((uint64_t)ltimer_val * (uint64_t)TIMER_CLK_CONST_SCALED) >> \
                           (TIMER_CLK_SCALE_FACTOR - lprescaler));
  }
#endif

  return ltime_val;
}

/*
 * Clear the timer
 */
TIMER_STATUS_t TIMER_Clear(TIMER_t *const handle_ptr)
{
  TIMER_STATUS_t status;

  XMC_ASSERT("TIMER_Clear:handle_ptr NULL" , (handle_ptr != NULL));

  /* Check for APP instance is initialized or not */
  if (true == handle_ptr->initialized)
  {
#ifdef TIMER_CCU4_USED
    if (TIMER_MODULE_CCU4 == handle_ptr->timer_module)
    {
      /* Clear the timer register */
      XMC_CCU4_SLICE_ClearTimer(handle_ptr->ccu4_slice_ptr);
    }
#endif

#ifdef TIMER_CCU8_USED
    if (TIMER_MODULE_CCU8 == handle_ptr->timer_module)
    {
      /* Clear the timer register */
      XMC_CCU8_SLICE_ClearTimer(handle_ptr->ccu8_slice_ptr);
    }
#endif
    status = TIMER_STATUS_SUCCESS;
  }
  else
  {
    status = TIMER_STATUS_FAILURE;
  }

  return (status);
}

/******************************************** PRIVATE FUNCTION DEFINITIONS ********************************************/
#ifdef TIMER_CCU4_USED
/*
 * This function configures timer ccu4 timer with required time tick value
 */
TIMER_STATUS_t TIMER_CCU4_lInit(TIMER_t* const handle_ptr)
{
  TIMER_STATUS_t status;
  /* Initialize the global registers */
  status = (TIMER_STATUS_t)GLOBAL_CCU4_Init(handle_ptr->global_ccu4_handler);

  /* Enable the clock for selected timer */
  XMC_CCU4_EnableClock(handle_ptr->global_ccu4_handler->module_ptr, handle_ptr->ccu4_slice_number);
  /* Configure the timer with required settings */
  XMC_CCU4_SLICE_CompareInit(handle_ptr->ccu4_slice_ptr, handle_ptr->ccu4_slice_config_ptr);
  /* programs the timer period and compare register according to time interval value and do the shadow transfer */
  TIMER_CCU4_lShadowTransfer(handle_ptr);

#ifdef  TIMER_INTERRUPT
  if (true == handle_ptr->period_match_enable)
  {
    /* Binds a period match event to an NVIC node  */
    XMC_CCU4_SLICE_SetInterruptNode(handle_ptr->ccu4_slice_ptr, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH,
                                    handle_ptr->ccu4_period_match_node);
    /* Enables a timer(period match) event  */
    XMC_CCU4_SLICE_EnableEvent(handle_ptr->ccu4_slice_ptr, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  }
#endif
  /* Clears the timer register */
  XMC_CCU4_SLICE_ClearTimer(handle_ptr->ccu4_slice_ptr);

  /* update the initialization flag as true for particular instance*/
  handle_ptr->initialized = true;

  /* Check whether the start of the timer is enabled during initialization or not */
  if (handle_ptr->start_control == true)
  {
    /* Start the timer */
    XMC_CCU4_SLICE_StartTimer(handle_ptr->ccu4_slice_ptr);
  }

  return (status);
}

/*
 * This function configures timer period and compare values and triggers the shadow transfer operation
 */
void TIMER_CCU4_lShadowTransfer(TIMER_t* const handle_ptr)
{
  /* programs the timer period register according to time interval value */
  XMC_CCU4_SLICE_SetTimerPeriodMatch(handle_ptr->ccu4_slice_ptr, handle_ptr->period_value);
  /* programs the timer compare register for 50% duty cycle */
  XMC_CCU4_SLICE_SetTimerCompareMatch(handle_ptr->ccu4_slice_ptr, TIMER_CMP_100_DUTY);
  /* Transfers value from shadow timer registers to actual timer registers */
  XMC_CCU4_EnableShadowTransfer(handle_ptr->global_ccu4_handler->module_ptr, handle_ptr->shadow_mask);
}
#endif

#ifdef TIMER_CCU8_USED
/*
 * This function configures timer ccu8 timer with required time tick value
 */
TIMER_STATUS_t TIMER_CCU8_lInit(TIMER_t* const handle_ptr)
{
  TIMER_STATUS_t status;
  /* Initialize the global registers */
  status = (TIMER_STATUS_t)GLOBAL_CCU8_Init(handle_ptr->global_ccu8_handler);

  /* Enable the clock for selected timer */
  XMC_CCU8_EnableClock(handle_ptr->global_ccu8_handler->module_ptr, handle_ptr->ccu8_slice_number);
  /* Configure the timer with required settings */
  XMC_CCU8_SLICE_CompareInit(handle_ptr->ccu8_slice_ptr, handle_ptr->ccu8_slice_config_ptr);
  /* programs the timer period and compare register according to time interval value and do the shadow transfer */
  TIMER_CCU8_lShadowTransfer(handle_ptr);

#ifdef  TIMER_INTERRUPT
  if (true == handle_ptr->period_match_enable)
  {
    /* Binds a period match event to an NVIC node  */
    XMC_CCU8_SLICE_SetInterruptNode(handle_ptr->ccu8_slice_ptr, XMC_CCU8_SLICE_IRQ_ID_PERIOD_MATCH,
                                    handle_ptr->ccu8_period_match_node);
    /* Enables a timer(period match) event  */
    XMC_CCU8_SLICE_EnableEvent(handle_ptr->ccu8_slice_ptr, XMC_CCU8_SLICE_IRQ_ID_PERIOD_MATCH);
  }
#endif
  /* Clears the timer register */
  XMC_CCU8_SLICE_ClearTimer(handle_ptr->ccu8_slice_ptr);

  /* update the initialization flag as true for particular instance*/
  handle_ptr->initialized = true;

  /* Check whether the start of the timer is enabled during initialization or not */
  if (handle_ptr->start_control == true)
  {
    /* Start the timer */
    XMC_CCU8_SLICE_StartTimer(handle_ptr->ccu8_slice_ptr);
  }

  return (status);
}

/*
 * This function configures timer period and compare values and triggers the shadow transfer operation
 */
void TIMER_CCU8_lShadowTransfer(TIMER_t* const handle_ptr)
{
  /* programs the timer period register according to time interval value */
  XMC_CCU8_SLICE_SetTimerPeriodMatch(handle_ptr->ccu8_slice_ptr, handle_ptr->period_value);
  /* programs the timer compare register for 50% duty cycle in compare channel 1*/
  XMC_CCU8_SLICE_SetTimerCompareMatch(handle_ptr->ccu8_slice_ptr,
                                      XMC_CCU8_SLICE_COMPARE_CHANNEL_1,
                                      TIMER_CMP_100_DUTY);
  /* Transfers value from shadow timer registers to actual timer registers */
  XMC_CCU8_EnableShadowTransfer(handle_ptr->global_ccu8_handler->module_ptr, handle_ptr->shadow_mask);
}
#endif
