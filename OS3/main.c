/*
***********************************************************************************************
* INCLUDE FILES
***********************************************************************************************
*/


#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h> //main header file for mc3i
#include  <bsp_clk.h>
#include  <bsp_int.h>
#include  <bsp_os.h>
#include  <bsp_led.h>
#include  <bsp_uart.h>
#include <stdio.h>
#include  "os_app_hooks.h"
#include  "app_cfg.h" //congigures the application
/*
***********************************************************************************************
* LOCAL GLOBAL VARIABLES
***********************************************************************************************
*/

#define APP_TASK_START_STK_SIZE 50;

static OS_TCB StartupTaskTCB;//TCB for start up task
static OS_TCB AppTask1_TCB;//TCB for first task
static OS_TCB AppTask2_TCB;//TCB for second task
static OS_MUTEX AppMutex;//mutex kernel object
static OS_Q AppQ;//message queue kernel object; see uC/OS-III book for more info
static CPU_STK StartupTaskStk[128];//Stack size of startup task
static CPU_STK AppTask1_Stk[128];//stack size first task
static CPU_STK AppTask2_Stk[128];//stack size second task
/*
***********************************************************************************************
* FUNCTION PROTOTYPES
***********************************************************************************************
*/
static void StartupTask (void *p_arg);
static void AppTask1 (void *p_arg);
static void AppTask2 (void *p_arg);

void main (void)
{
  OS_ERR  os_err;
  
  
  BSP_ClkInit();                                              /* Initialize the main clock                            */
  BSP_IntInit();                                              /* Initialize interrupt vector table                    */
  BSP_OS_TickInit();                                          /* Initialize kernel tick timer                         */
  
  Mem_Init();                                                 /* Initialize Memory Managment Module                   */
  CPU_IntDis();                                               /* Disable all Interrupts                               */
  CPU_Init();                                                 /* Initialize the uC/CPU services                       */
  
  OSInit(&os_err);                                            /* Initialize uC/OS-III                                 */
  if (os_err != OS_ERR_NONE) {
    while (1);
  }
  
  //create mutex;  
  OSMutexCreate(
                &AppMutex,
                "My App Mutex",
                &os_err
                  );
  
  //create message queue
  OSQCreate(
            &AppQ,
            "My App Queue",
            10u,
            &os_err
              );
  
  App_OS_SetAllHooks();                                       /* Set all applications hooks                           */
  
  OSTaskCreate(&StartupTaskTCB,                               /* Create the startup task                              */
               "Startup Task",
               StartupTask,
               0u,
               APP_CFG_STARTUP_TASK_PRIO,
               &StartupTaskStk[0u],
               StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE / 10u],
               APP_CFG_STARTUP_TASK_STK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
  if (os_err != OS_ERR_NONE) {
    while (1);
  }
  
  OSStart(&os_err);                                           /* Start multitasking (i.e. give control to uC/OS-III)  */
  
  while (DEF_ON) {                                            /* Should Never Get Here.                               */
    ;
  }
  
}

static  void  StartupTask (void *p_arg)
{
  OS_ERR  os_err;
  (void)p_arg;
  OS_TRACE_INIT();                                            /* Initialize the uC/OS-III Trace recorder              */
  BSP_OS_TickEnable();                                        /* Enable the tick timer and interrupt                  */
  BSP_LED_Init();                                             /* Initialize LEDs                                      */
  BSP_UART_Init();                                            /* Initialize UART                                      */
  BSP_UART_Start(115200u);                                    /* Start UART at 115200 baud rate                       */
  
#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&os_err);                            /* Compute CPU capacity with no task running            */
#endif
  
#ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();
#endif
  
  
  
  
  //Create first task
  OSTaskCreate(&AppTask1_TCB,
               "App Task 1",
               AppTask1,
               0u,
               APP_TASK_1_STARTUP_TASK_PRIO,
               &AppTask1_Stk[0u],
               AppTask1_Stk[APP_CFG_APP_TASK_1_STK_SIZE/10u],
               APP_CFG_APP_TASK_1_STK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
  
  //Create second task
  OSTaskCreate(&AppTask2_TCB,
               "App Task 2",
               AppTask2,
               0u,
               APP_TASK_2_STARTUP_TASK_PRIO,
               &AppTask2_Stk[0u],
               AppTask2_Stk[APP_CFG_APP_TASK_2_STK_SIZE/10u],
               APP_CFG_APP_TASK_2_STK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
  
  
  
  while (1) {
    BSP_LED_Toggle(0);
    OSTimeDlyHMSM((CPU_INT16U) 0,
                  (CPU_INT16U) 0,
                  (CPU_INT16U) 0,
                  (CPU_INT32U)100,
                  (OS_OPT )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR *)&os_err);
  }  
  
}


static void AppTask2 (void *p_arg){
  OS_ERR  os_err;
  while(1){
    BSP_LED_Toggle(LED6);
    OSTimeDlyHMSM(0u, 0u, 0u, 400u, OS_OPT_TIME_HMSM_STRICT, &os_err);
  }
  
}

static void AppTask1 (void *p_arg){
  
  OS_ERR  os_err;
  while(1){
    BSP_LED_Toggle(LED4);
    OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
  }
  
}







