/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky style version.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware are defined in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 200 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, toggles an LED.  The 'block
 * time' parameter passed to the queue receive function specifies that the
 * task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 200 milliseconds, the queue receive
 * task leaves the Blocked state every 200 milliseconds, and therefore toggles
 * the LED every 200 milliseconds.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Standard demo includes. */
#include "partest.h"
#include <stdio.h>

/* Priorities at which the tasks are created. */
#define T1_PRIORITY (tskIDLE_PRIORITY + 2)
#define T2_PRIORITY (tskIDLE_PRIORITY + 1)

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS (pdMS_TO_TICKS(10))

/*
 * Called by main when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1 in
 * main.c.
 */
char msgBuffer[100][20];
int CurrentIdx = 0;
void main_lab(void);
void printMessage();
/*-----------------------------------------------------------*/

static void task_1(void *parameters)
{
    /* Remove compiler warning about unused parameter. */
    taskArgs *param = (taskArgs *)parameters;
    xTaskSetDeadline(param->period);
    xTaskSetCompTime(param->compTime);

    /* Initialise xNextWakeTime - this only needs to be done once. */
    // TickType_t xNextWakeTime;
    // xNextWakeTime = xTaskGetTickCount();

    TickType_t start;
    TickType_t end;
    TickType_t todelay;
    // pxCurrentTCB->deadline = param->period;
    start = 0;
    for (;;)
    {
        while (xTaskGetCompTime() > 0)
        {
        }
        if ((int)xTaskGetTickCount() > 20)
            printMessage();
        end = xTaskGetTickCount();
        todelay = (param->period) - (end - start);
        start = start + param->period;
        taskDISABLE_INTERRUPTS();
        // Reset compTime
        xTaskSetCompTime(param->compTime);
        xTaskSetDeadline(xTaskGetTickCount() + todelay);
        xTaskSetResetFlagUser(1);
        taskENABLE_INTERRUPTS();

        if (todelay > 0)
            vTaskDelay(todelay);
        else if (todelay == 0 && (xTaskGetResetFlagUser() == 1))
            xTaskSetResetFlagUser(0);
    }
}

static void task_2(void *parameters)
{
    /* Remove compiler warning about unused parameter. */
    taskArgs *param = (taskArgs *)parameters;
    xTaskSetDeadline(param->period);
    xTaskSetCompTime(param->compTime);

    /* Initialise xNextWakeTime - this only needs to be done once. */

    TickType_t start;
    TickType_t end;
    TickType_t todelay;
    // TickType_t xNextWakeTime;
    // xNextWakeTime = xTaskGetTickCount();

    start = 0;
    for (;;)
    {
        while (xTaskGetCompTime() > 0)
        {
        }
        if ((int)xTaskGetTickCount() > 20)
            printMessage();
        end = xTaskGetTickCount();
        todelay = (TickType_t)(param->period) - (end - start);
        start = start + param->period;
        taskDISABLE_INTERRUPTS();
        // Reset compTime
        xTaskSetCompTime(param->compTime);
        xTaskSetDeadline(xTaskGetTickCount() + todelay);
        xTaskSetResetFlagUser(1);
        taskENABLE_INTERRUPTS();

        if (todelay > 0)
            vTaskDelay(todelay);
        else if (todelay == 0 && (xTaskGetResetFlagUser() == 1))
            xTaskSetResetFlagUser(0);

        // if (todelay == 0)
        //     xTaskSetResetFlagUser(0);
    }
}

void main_lab(void)
{
    taskArgs args[2];
    args[0].compTime = 1;
    args[0].period = 3;
    args[1].compTime = 3;
    args[1].period = 5;
    //    TickType_t xNextWakeTime;
    //    xNextWakeTime = xTaskGetTickCount();
    xTaskCreate(task_1, "T1", configMINIMAL_STACK_SIZE, (void *)&args[0], T1_PRIORITY, NULL);
    xTaskCreate(task_2, "T2", configMINIMAL_STACK_SIZE, (void *)&args[1], T1_PRIORITY, NULL);
    sprintf(&msgBuffer[CurrentIdx++], "T1 c:%d p:%d\n", args[0].compTime, args[0].period);
    sprintf(&msgBuffer[CurrentIdx++], "T2 c:%d p:%d\n", args[1].compTime, args[1].period);
    vTaskStartScheduler();
}
/*-----------------------------------------------------------*/

void printMessage()
{
    static int i = 0;
    for (; i < CurrentIdx; i++)
        printf("%s", msgBuffer[i]);

    while (1)
        ;
}
