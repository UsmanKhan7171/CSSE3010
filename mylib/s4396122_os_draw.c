#include "s4396122_os_draw.h"

Iter *drawList;
SemaphoreHandle_t s4396122_SemaphoreDrawList;
Queue *mouseQueue;
SemaphoreHandle_t s4396122_SemaphoreMouseQueue;
int charLast;
USBD_HandleTypeDef hidDevice;
int leftMouseState;
int lastMouseX;
int lastMouseY;

void s4396122_DrawerTask() {
    
    while (1) {
        if (s4396122_SemaphoreMouseQueue != NULL && xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
            if (s4396122_util_queue_size(mouseQueue) > 0) {
                struct MouseCommand *cmd = s4396122_util_queue_pop(mouseQueue);
                uint8_t buff[4];
                buff[0] = cmd->leftMouse;
                buff[1] = (char) cmd->xMovement;
                buff[2] = (char) cmd->yMovement;
                buff[3] = 0;
                USBD_HID_SendReport(&hidDevice, buff, 4);
                free(cmd);
            }
            xSemaphoreGive(s4396122_SemaphoreMouseQueue);
        }
        vTaskDelay(100);
    }
}

void s4396122_os_draw_init() {
    s4396122_SemaphoreDrawList = xSemaphoreCreateMutex();
    s4396122_SemaphoreMouseQueue = xSemaphoreCreateMutex();
    leftMouseState = 0;
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        drawList = s4396122_util_iter_create();
        charLast = 2;
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
    if (s4396122_SemaphoreMouseQueue != NULL && xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
        mouseQueue = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }
    char s4396122_os_draw_number_to_segment[10] = {
        0x77,   // 0
        0x22,   // 1
        0x5B,   // 2
        0x6B,   // 3
        0x2E,   // 4
        0x6D,   // 5
        0x7D,   // 6
        0x27,   // 7
        0x7F,   // 8
        0x6F    // 9
    };
    char s4396122_os_draw_letter_to_segment[26] = {
        0x3F,   // a
        0x7C,   // b
        0x58,   // c
        0x7A,   // d
        0x5D,   // e
        0x1D,   // f
        0x6F,   // g
        0x3C,   // h
        0x22,   // i
        0x72,   // j
        0x1C,   // k
        0x34,   // l
        0x38,   // m
        0x38,   // n
        0x78,   // o
        0x1F,   // p
        0x2F,   // q
        0x18,   // r
        0x6D,   // s
        0x15,   // t
        0x70,   // u
        0x30,   // v
        0x70,   // w
        0x48,   // x
        0x6E,   // y
        0x5B    // z
    };

    USBD_Init(&hidDevice, &HID_Desc, 0);
    USBD_RegisterClass(&hidDevice, &USBD_HID);
    USBD_Start(&hidDevice);
}

void s4396122_os_draw_add_char(int x, int y, char c) {
    int d = c | (y << 8) | (x << 16) | (0 << 31);

    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        s4396122_util_iter_add_tail(drawList, d);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_add_poly(int x, int y, int length, int degree) {
    int d = length | (degree << 8) | (y << 16) | (x << 24) | (1 << 31);

    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        s4396122_util_iter_add_tail(drawList, d);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_change_pen(char c) {

}

void s4396122_os_draw_redraw() {

}

void s4396122_os_draw_remove_top() {
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        s4396122_util_iter_remove_tail(drawList);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_move_mouse(int xMovement, int yMovement) {
    if (s4396122_SemaphoreMouseQueue != NULL && xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
        struct MouseCommand *cmd = malloc(sizeof(struct MouseCommand));
        cmd->leftMouse = leftMouseState;
        cmd->xMovement = xMovement;
        cmd->yMovement = yMovement;
        lastMouseX = xMovement;
        lastMouseY = yMovement;
        s4396122_util_queue_push(mouseQueue, cmd);
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }
}

void s4396122_os_draw_mouse_button(int leftMouse) {
    if (s4396122_SemaphoreMouseQueue != NULL && xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
        leftMouseState = leftMouse;
        struct MouseCommand *cmd = malloc(sizeof(struct MouseCommand));
        cmd->leftMouse = leftMouseState;
        cmd->xMovement = lastMouseX;
        cmd->yMovement = lastMouseY;
        s4396122_util_queue_push(mouseQueue, cmd);
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }
}

// Taken from hid/usbd_desc
USBD_DescriptorsTypeDef HID_Desc =
{
  USBD_HID_DeviceDescriptor,
  USBD_HID_LangIDStrDescriptor, 
  USBD_HID_ManufacturerStrDescriptor,
  USBD_HID_ProductStrDescriptor,
  USBD_HID_SerialStrDescriptor,
  USBD_HID_ConfigStrDescriptor,
  USBD_HID_InterfaceStrDescriptor,
  
};
/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t hUSBDDeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END;
/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END;

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;
/**
* @brief  USBD_HID_DeviceDescriptor 
*         return the device descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_DeviceDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  *length = sizeof(hUSBDDeviceDesc);
  return hUSBDDeviceDesc;
}
/**
* @brief  USBD_HID_LangIDStrDescriptor 
*         return the LangID string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_LangIDStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  *length =  sizeof(USBD_LangIDDesc);  
  return USBD_LangIDDesc;
}
/**
* @brief  USBD_HID_ProductStrDescriptor 
*         return the product string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_ProductStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed == 0)
  {   
    USBD_GetString ((uint8_t *)USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}
/**
* @brief  USBD_HID_ManufacturerStrDescriptor 
*         return the manufacturer string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_ManufacturerStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  USBD_GetString ((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}
/**
* @brief  USBD_HID_SerialStrDescriptor 
*         return the serial number string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_SerialStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed  == USBD_SPEED_HIGH)
  {    
    USBD_GetString ((uint8_t *)USBD_SERIALNUMBER_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_SERIALNUMBER_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}
/**
* @brief  USBD_HID_ConfigStrDescriptor 
*         return the configuration string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_ConfigStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed  == USBD_SPEED_HIGH)
  {  
    USBD_GetString ((uint8_t *)USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;  
}
/**
* @brief  USBD_HID_InterfaceStrDescriptor 
*         return the interface string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_InterfaceStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed == 0)
  {
    USBD_GetString ((uint8_t *)USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;  
}
