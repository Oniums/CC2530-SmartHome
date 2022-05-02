#include "ZComDef.h"
#include "OSAL.h"
#include "sapi.h"
#include "smartHome.h"
#include "dht11.h"
#include "stdio.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "steer.h"
#include "string.h"
#include "mq2.h"

#define NUM_IN_CMD_SENSORDEVICE 1
#define NUM_OUT_CMD_SENSORDEVICE 2

uint16 AUTO = 0;
uint16 ID = 1;
uint8 DEVICENAME[2];
uint16 FUN;
uint16 MQ2;
const cId_t SensorDeviceInputCommandList[NUM_IN_CMD_SENSORDEVICE] =
    {TOGGLE_CMD_ID};
const cId_t SensorDeviceOutputCommandList[NUM_OUT_CMD_SENSORDEVICE] =
    {ENDJOINNET_CMD_ID, DATA_SEND_CMD_ID};
const SimpleDescriptionFormat_t zb_SimpleDesc =
    {
        ENDPOINT_ID_SMARTHOME,
        PROFILE_ID_SMARTHOME,
        DEVICE_ID_SENSOR,
        DEVIDE_VERSION_ID,
        0,
        NUM_IN_CMD_SENSORDEVICE,
        (cId_t *)SensorDeviceInputCommandList,
        NUM_OUT_CMD_SENSORDEVICE,
        (cId_t *)SensorDeviceOutputCommandList};

//用来处理用户的自定义事件
void zb_HandleOsalEvent(uint16 event)
{
  if (event & DATA_GET)
  {
    uint8 DEVICEINFO[50];
    uint16 BEEP = 0;
    MQ2 = ReadGasData();
    if (MQ2 >= 400)
    {
      BEEP = 1;
      warming();
    }
    else
    {
      BEEP = 0;
      no_warm();
    }
    FUN = STATE();
    sprintf(DEVICEINFO, "%c%c,%d,%d,%d,%d,%d,\n", DEVICENAME[0], DEVICENAME[1], 1, MQ2, 2, FUN, BEEP);
    zb_SendDataRequest(0X0, DATA_SEND_CMD_ID,
                       osal_strlen(DEVICEINFO), DEVICEINFO, 0, FALSE, AF_DEFAULT_RADIUS);
  }
}
//用来处理按键消息
void zb_HandleKeys(uint8 shift, uint8 keys)
{
}
//当构建网络或加入网络成功时被调用
void zb_StartConfirm(uint8 status)
{
  if (status == ZSUCCESS)
  {
    STEER_INIT();
    DEVICENAME[0] = 'B';
    DEVICENAME[1] = ID + 48;
    HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
    HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
    zb_SendDataRequest(0X0, ENDJOINNET_CMD_ID,
                       osal_strlen(DEVICENAME), DEVICENAME, 0, FALSE, AF_DEFAULT_RADIUS);
    osal_start_reload_timer(sapi_TaskID, DATA_GET, 1000);
  }
}
//当数据包被接收方收到时调用
void zb_SendDataConfirm(uint8 handle, uint8 status)
{
}

void zb_BindConfirm(uint16 commandId, uint8 status)
{
}
//其它节点绑定到该节点时调用
void zb_AllowBindConfirm(uint16 source)
{
  (void)source;
}
//用户任务通过zb_FindDeviceRequest通过节点的物理地址获取节点的网络地址时的调用
void zb_FindDeviceConfirm(uint8 searchType, uint8 *searchKey, uint8 *result)
{
}

/******************************************************************************
 * @fn          zb_ReceiveDataIndication
 *
 * @brief       The zb_ReceiveDataIndication callback function is called
 *              asynchronously by the ZigBee stack to notify the application
 *              when data is received from a peer device.
 *
 * @param       source - The short address of the peer device that sent the data
 *              command - The commandId associated with the data
 *              len - The number of bytes in the pData parameter
 *              pData - The data sent by the peer device
 *
 * @return      none
 */
void zb_ReceiveDataIndication(uint16 source, uint16 command, uint16 len, uint8 *pData)
{
  if (command == TOGGLE_CMD_ID)
  {
    if (pData[3] == '1' && pData[5] == '1')
    {
      open();
    }
    else if (pData[3] == '1' && pData[5] == '0')
    {
      close();
    }
    else if (pData[0] == 'A' && pData[1] == 'U' && pData[0] == 'T' && pData[1] == 'O')
    {
      AUTO = 1;
    }
    else if (pData[0] == 'N' && pData[1] == 'O' && pData[0] == 'A' && pData[1] == 'U')
    {
      AUTO = 0;
    }
  }
}
