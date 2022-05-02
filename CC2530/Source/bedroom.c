#include "ZComDef.h"
#include "OSAL.h"
#include "sapi.h"
#include "smartHome.h"
#include "dht11.h"
#include "stdio.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "io.h"
#include "string.h"

#define NUM_IN_CMD_SENSORDEVICE 1
#define NUM_OUT_CMD_SENSORDEVICE 2
#define DATALENGTH 2
#define CONTROLNUM 1
uint16 AUTO = 0;
uint16 ID = 1;
uint8 DEVICENAME[2];
uint8 DHT[DATALENGTH];
uint16 CON;
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
    DHT11(&DHT[0], &DHT[1]);
    if ((int)DHT[1] < 30 && AUTO == 1)
    {
      OPEN_FUN1();
    }
    else if ((int)DHT[1] >= 30 && AUTO == 1)
    {
      CLOSE_FUN1();
    }
    FUNS_STATE(&CON);
    if (CON == 1)
    {
      HalLedSet(HAL_LED_2, HAL_LED_MODE_ON);
    }
    else
    {
      HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
    }
    sprintf(DEVICEINFO, "%c%c,%d,%d,%d,%d,%d,%d,\n", DEVICENAME[0], DEVICENAME[1], DATALENGTH, DHT[0], DHT[1], CONTROLNUM, CON, AUTO);
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
    DEVICENAME[0] = 'A';
    DEVICENAME[1] = ID + 48;
    SENSIO_Init();
    HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
    HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
    DHT11(&DHT[0], &DHT[1]);
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
      OPEN_FUN1();
    }
    else if (pData[3] == '1' && pData[5] == '0')
    {
      CLOSE_FUN1();
    }
    else if (osal_memcmp(pData, "AUTO", 4))
    {
      AUTO = 1;
    }
    else if (osal_memcmp(pData, "NOAU", 4))
    {
      AUTO = 0;
    }
  }
}
