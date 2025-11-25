/*
 * ======================================================================
 * ADC_CONFIG_GUIDE.c
 * ======================================================================
 *
 * Description:
 * This file summarizes 4 different multi-channel reading methods for
 * STM32 ADC. To avoid repetition, it focuses only on the
 * *fundamental differences* between the methods.
 *
 * Created on: Nov 15, 2025
 * Author: hrnkr
 * ======================================================================
 */

/*
 * ======================================================================
 * COMMON SETTINGS FOR ALL METHODS (Default)
 * ======================================================================
 *
 * --- ADC General Settings ---
 * Scan Conversion Mode:     ENABLED (Multi-channel scan active)
 * Data Alignment:           ADC_DATAALIGN_RIGHT
 * Discontinuous Mode:       DISABLE
 * Overrun:                  ADC_OVR_DATA_PRESERVED
 * Low Power Auto Wait:      DISABLE
 * Oversampling:             DISABLE
 *
 * --- Channels & Sampling (Common) ---
 * ADC_IN0:                  Rank 1
 * ADC_IN1:                  Rank 2
 * ADC_IN4:                  Rank 3
 * ADC_IN5:                  Rank 4
 * Sampling Time (Common 1): 160.5 Cycles (For all channels)
 *
 * --- DMA Settings (For Methods 2, 3, and 4) ---
 * Direction:                Peripheral → Memory
 * Memory Increment:         ENABLE
 * Peripheral Increment:     DISABLE
 * Data Width (Per/Mem):     HALFWORD
 * Priority:                 LOW
 * ======================================================================
 */

/*
 * ======================================================================
 * METHOD 1: POLLING (Blocking with CPU)
 * ======================================================================
 * Description:
 * The CPU manually starts, waits for, and reads each channel conversion.
 * No DMA or Interrupts are used.
 *
 * --- Key Differences ---
 * DMA:                    DISABLED
 * ContinuousConvMode:     DISABLED
 * ExternalTrigConv:       ADC_SOFTWARE_START
 * EOCSelection:           ADC_EOC_SINGLE_CONV
 * (EOC flag set after every *single* channel conversion)
 *
 * --- Logic ---
 * Inside a loop, HAL_ADC_Start(), HAL_ADC_PollForConversion(),
 * and HAL_ADC_GetValue() are called for each channel.
 * ======================================================================
 */

/*
 * ======================================================================
 * METHOD 2: DMA (NORMAL MODE) - Single Scan
 * ======================================================================
 * Description:
 * ADC scans all channels *once* and writes results to RAM via DMA.
 * Once finished, DMA stops and must be restarted manually.
 *
 * --- Key Differences ---
 * DMA:                    ENABLED
 * ContinuousConvMode:     DISABLED (Scans only 1 sequence)
 * ExternalTrigConv:       ADC_SOFTWARE_START
 * EOCSelection:           ADC_EOC_SEQ_CONV
 * (EOC flag set after the entire *sequence* is finished)
 *
 * --- DMA Settings ---
 * Mode:                   DMA_NORMAL (Stops after one full transfer)
 * DMA Continuous Requests:DISABLED
 *
 * --- Logic ---
 * Started with HAL_ADC_Start_DMA().
 * The 'isADCFinished' flag is set inside HAL_ADC_ConvCpltCallback().
 * Must be restarted to read again.
 * ======================================================================
 */

/*
 * ======================================================================
 * METHOD 3: DMA (CIRCULAR MODE) - Continuous Conversion
 * ======================================================================
 * Description:
 * ADC scans channels *continuously*. When a sequence finishes,
 * it immediately starts the next one without stopping (no CPU intervention).
 *
 * --- Key Differences ---
 * DMA:                    ENABLED
 * ContinuousConvMode:     ENABLED (Self-triggers continuously)
 * ExternalTrigConv:       ADC_SOFTWARE_START
 * EOCSelection:           ADC_EOC_SEQ_CONV
 *
 * --- DMA Settings ---
 * Mode:                   DMA_CIRCULAR (Returns to the start of buffer)
 * DMA Continuous Requests:ENABLED
 *
 * --- Logic ---
 * Started *only once* with HAL_ADC_Start_DMA().
 * HAL_ADC_ConvCpltCallback() is called every time a sequence completes.
 * ======================================================================
 */

/*
 * ======================================================================
 * METHOD 4: EXTERNAL TRIGGER DMA (WITH TIMER)
 * ======================================================================
 * Description:
 * ADC conversion is triggered by hardware (Timer 1), not CPU software.
 * ADC performs 1 sequence scan only when the Timer signal arrives,
 * then waits for the next signal.
 *
 * --- Key Differences ---
 * DMA:                    ENABLED
 * ContinuousConvMode:     DISABLED (Waits for trigger)
 * ExternalTrigConv:       ADC_EXTERNALTRIG_T1_TRGO2 (Trigger source)
 * ExternalTriggerEdge:    RISINGEDGE
 * EOCSelection:           ADC_EOC_SEQ_CONV
 *
 * --- DMA Settings ---
 * Mode:                   DMA_CIRCULAR
 * DMA Continuous Requests:ENABLED
 * (NOTE: Must be ENABLED so ADC keeps the DMA request line open
 * for the next Timer trigger, even though ContConvMode is disabled.)
 *
 * --- Timer 1 Configuration (For 100ms Interval) ---
 * Source:                 Internal Clock (HCLK = 64 MHz)
 * Prescaler (PSC):        6400 - 1 (Value: 6399)
 * Counter Period (ARR):   1000 - 1 (Value: 999)
 * Trigger Event (TRGO2):  Update Event
 * (CRITICAL: ADC listens to TRGO2. "Update Event" sends a signal
 * every time the counter reaches the Period value.)
 *
 * --- Calculation (Why 100ms?) ---
 * Formula: Time = ((PSC + 1) * (ARR + 1)) / Timer_Clock
 *
 * Calculation:
 * 1. (6399 + 1) * (999 + 1) = 6400 * 1000 = 6,400,000 ticks
 * 2. 6,400,000 / 64,000,000 (64MHz) = 0.1 seconds
 * 3. Result = 100 ms
 *
 * --- Note (Difference: Method 3 vs 4) ---
 * *FUNDAMENTAL DIFFERENCE*:
 * - Method 3: "Continuous Mode" is ENABLED, so ADC runs *itself*
 * continuously at max speed.
 * - Method 4: "Continuous Mode" is DISABLED. ADC performs 1 sequence scan
 * *only when the Timer signal (trigger) arrives*.
 * ======================================================================
 */
