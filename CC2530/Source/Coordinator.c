#include "ZComDef.h"
#include "osal.h"
#include "sapi.h"
#include "hal_uart.h"
#include "smartHome.h"
#include "stdio.h"
#include "hal_led.h"
#include "string.h"
#include "bh1750.h"
#include "coordinator_io.h"

#define NUM_IN_CMD_COORINATOR 2
#define NUM_OUT_CMD_COORINATOR 1

uint16 L_VAL;
uint16 L_STATE = 0;
uint16 AUTO = 0;
uint8 COORINFO[10];
void *DeviceNode(void);
uint16 Light(void);
void Receive(uint8 port, uint8 event);
const cId_t coordinatorInputCommandList[NUM_IN_CMD_COORINATOR] =
    {ENDJOINNET_CMD_ID, DATA_SEND_CMD_ID};
const cId_t coordinatorOutputCommandList[NUM_OUT_CMD_COORINATOR] =
    {TOGGLE_CMD_ID};

struct device_list
{
  struct device_list *next;
  unsigned char DEVICEINFO[50];
  unsigned char DEVICENAME[2];
  unsigned char ADDR[2];
  unsigned int HEART;
};

void *DeviceNode(void)
{
  return osal_mem_alloc(sizeof(struct device_list));
}

static struct device_list DeviceHeader = {NULL};

const SimpleDescriptionFormat_t zb_SimpleDesc =
    {
        ENDPOINT_ID_SMARTHOME,
        PROFILE_ID_SMARTHOME,
        DEVICE_ID_COORDINATOR,
        DEVIDE_VERSION_ID,
        0,
        NUM_IN_CMD_COORINATOR,
        (cId_t *)coordinatorInputCommandList,
        NUM_OUT_CMD_COORINATOR,
        (cId_t *)coordinatorOutputCommandList};

//用来处理用户的自定义事件
void zb_HandleOsalEvent(uint16 event)
{
  if (event & DEVICE_HEART)
  {
    L_VAL = Light();
    if (AUTO == 1)
    {
      if (L_VAL < 300)
      {
        HalLedSet(HAL_LED_2, HAL_LED_MODE_ON);
      }
      else if (L_VAL >= 300)
      {
        HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
      }
    }
    L_STATE=LIGHT_STATE();
    sprintf(COORINFO, "COOR,%d,%d", L_VAL, L_STATE);
    struct device_list *p = DeviceHeader.next;
    HalUARTWrite(0, COORINFO, osal_strlen(COORINFO));
    HalUARTWrite(0, ";", osal_strlen(";"));
    while (p != NULL)
    {
      if (p->HEART == 0)
      {
        p = p->next;
        continue;
      }
      p->HEART--;
      if (p->HEART == 0)
      {
        p = p->next;
        continue;
      }
      HalUARTWrite(0, p->DEVICEINFO, osal_strlen(p->DEVICEINFO));
      HalUARTWrite(0, ";", osal_strlen(";"));
      p = p->next;
    }
  }
}
//用来处理按键消息
void zb_HandleKeys(uint8 shift, uint8 keys)
{
}
//当构建网络或加入网络成功时被调用
void zb_StartConfirm(uint8 status)
{
  HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
  HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
  halUARTCfg_t uartcfg;
  uartcfg.baudRate = HAL_UART_BR_115200;
  uartcfg.flowControl = FALSE;
  uartcfg.callBackFunc = Receive;
  HalUARTOpen(0, &uartcfg);
  L_STATE = 0;
  if (status == ZSUCCESS)
  {
    osal_start_reload_timer(sapi_TaskID, DEVICE_HEART, 2000);
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
  if (command == ENDJOINNET_CMD_ID)
  {
    struct device_list *p = DeviceHeader.next;
    while (p != NULL)
    {
      if (osal_memcmp(pData, p->DEVICENAME, len) == TRUE)
        break;
      else
      {
        p = p->next;
      }
    }
    if (p == NULL) //新节点加入
    {
      struct device_list *np = (struct device_list *)DeviceNode();
      osal_memcpy(np->ADDR, &source, 2);
      osal_memcpy(np->DEVICENAME, pData, len);
      np->HEART = HEART_BEAT_MAX_COUNT;
      np->next = DeviceHeader.next; //头插
      DeviceHeader.next = np;
      p = np;
    }
    else
    {
      osal_memcpy(p->ADDR, &source, 2);
    }
  }
  else if (command == DATA_SEND_CMD_ID)
  {
    struct device_list *p = DeviceHeader.next;
    while (p != NULL)
    {
      if (pData[0] == p->DEVICENAME[0] && pData[1] == p->DEVICENAME[1])
        break;
      else
      {
        p = p->next;
      }
    }
    if (p != NULL)
    {
      osal_memcpy(p->DEVICEINFO, pData, len);
      p->HEART = HEART_BEAT_MAX_COUNT;
    }
  }
}

void Receive(uint8 port, uint8 event)
{
  uint16 dstAddr;
  if (event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
  {
    uint8 CMD[6];
    HalUARTRead(port, CMD, 10);
    struct device_list *p = DeviceHeader.next;
    while (p != NULL)
    {
      if (CMD[0] == p->DEVICENAME[0] && CMD[1] == p->DEVICENAME[1])
        break;
      else
      {
        p = p->next;
      }
    }
    if (p != NULL)
    {
      osal_memcpy(&dstAddr, p->ADDR, 2);
      zb_SendDataRequest(dstAddr, TOGGLE_CMD_ID,
                         6, CMD, 0, FALSE, AF_DEFAULT_RADIUS);
    }
    if (CMD[0] == 'Z')
    {
      if (CMD[2] == '1')
      {
        LIGHT_OFF();
      }
      else if (CMD[2] == '0')
      {
        LIGHT_ON();
      }
    }
    if (osal_memcmp(CMD, "AUTO", 4))
    {
      AUTO = 1;
      zb_SendDataRequest(ZB_BROADCAST_ADDR, TOGGLE_CMD_ID,
                         6, CMD, 0, FALSE, AF_DEFAULT_RADIUS);
    }
    else if (osal_memcmp(CMD, "NOAU", 4))
    {
      AUTO = 0;
      zb_SendDataRequest(ZB_BROADCAST_ADDR, TOGGLE_CMD_ID,
                         6, CMD, 0, FALSE, AF_DEFAULT_RADIUS);
    }
  }
}
uint16 Light(void)
{
  uint32 w;
  LIGHT_INIT();
  w = get_light() / 1.2;
  uint16 lightData = w;
  return lightData;
}
