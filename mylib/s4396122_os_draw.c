#include "s4396122_os_draw.h"

int OS_DRAW_CANVAS_OFFSET_X = OS_DRAW_CANVAS_ORIGINAL_OFFSET_X;
int OS_DRAW_CANVAS_OFFSET_Y = OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y;

Queue *drawList;
SemaphoreHandle_t s4396122_SemaphoreDrawList;
Queue *mouseQueue;
SemaphoreHandle_t s4396122_SemaphoreMouseQueue;
SemaphoreHandle_t s4396122_SemaphoreUpdateDisplay;
int charLast;
USBD_HandleTypeDef hidDevice;
int leftMouseState;
int lastMouseX;
int lastMouseY;
enum MouseColor currentColor;
enum MouseType currentType;
struct DrawCmd tempChar;

char s4396122_os_draw_number_to_segment[] = {
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
char s4396122_os_draw_letter_to_segment[] = {
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
    0x54,   // l
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

void s4396122_DrawerTask() {
    
    while (1) {

        if (s4396122_SemaphoreUpdateDisplay != NULL && xSemaphoreTake(s4396122_SemaphoreUpdateDisplay, 1))
            s4396122_os_draw_redraw();

        if (s4396122_SemaphoreMouseQueue != NULL && xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
            while (s4396122_util_queue_size(mouseQueue) > 0) {
                struct MouseCommand *cmd = s4396122_util_queue_pop(mouseQueue);
                uint8_t buff[4];
                buff[0] = cmd->leftMouse;
                buff[1] = (char) cmd->xMovement;
                buff[2] = (char) cmd->yMovement;
                buff[3] = 0;
                USBD_HID_SendReport(&hidDevice, buff, 4);
                free(cmd);
                vTaskDelay(10);
            }
            xSemaphoreGive(s4396122_SemaphoreMouseQueue);
        }

        vTaskDelay(100);
    }
}

void s4396122_os_draw_init() {
    s4396122_SemaphoreDrawList = xSemaphoreCreateMutex();
    s4396122_SemaphoreMouseQueue = xSemaphoreCreateMutex();
    s4396122_SemaphoreUpdateDisplay = xSemaphoreCreateMutex();
    xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
    leftMouseState = 0;
    currentColor = BLACK;
    currentType = PEN;
    tempChar.mode = 0;
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        drawList = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
    if (s4396122_SemaphoreMouseQueue != NULL && xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
        mouseQueue = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }

    USBD_Init(&hidDevice, &HID_Desc, 0);
    USBD_RegisterClass(&hidDevice, &USBD_HID);
    USBD_Start(&hidDevice);
}

void s4396122_os_draw_add_char(int x, int y, char c) {
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        struct DrawCmd *d = malloc(sizeof(struct DrawCmd));
        d->mode = OS_DRAW_CHAR_MODE;
        d->c.x = x;
        d->c.y = y;
        d->c.c = c;
        d->color = currentColor;
        d->type = currentType;
        s4396122_util_queue_push(drawList, d);
        xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_add_poly(int x, int y, int length, int degree) {
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        struct DrawCmd *d = malloc(sizeof(struct DrawCmd));
        d->mode = OS_DRAW_POLY_MODE;
        d->p.x = x;
        d->p.y = y;
        d->p.length = length;
        d->p.degree = degree;
        d->color = currentColor;
        d->type = currentType;
        s4396122_util_queue_push(drawList, d);
        xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_add_line(int x1, int y1, int x2, int y2) {
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(x1, y1);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_move_mouse(x2, y2);
    s4396122_os_draw_mouse_button(0);
}

void s4396122_os_draw_change_pen_color(enum MouseColor c) {
    currentColor = c;
    switch(c) {
        case BLACK:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_BLACK_X, OS_DRAW_COLOR_BLACK_Y);
            break;
        case WHITE:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_WHITE_X, OS_DRAW_COLOR_WHITE_Y);
            break;
        case RED:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_RED_X, OS_DRAW_COLOR_RED_Y);
            break;
        case BLUE:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_BLUE_X, OS_DRAW_COLOR_BLUE_Y);
            break;
        case ORANGE:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_ORANGE_X, OS_DRAW_COLOR_ORANGE_Y);
            break;
    }
}

void s4396122_os_draw_change_pen_type(enum MouseType t) {
    currentType = t;
    switch(t) {
        case RECTANGLE:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_RECTANGLE_X, OS_DRAW_RECTANGLE_Y);
            break;
        case PEN:
            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_PEN_X, OS_DRAW_PEN_Y);
            break;
    }
}

void s4396122_os_draw_redraw() {
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        enum MouseColor origColor = currentColor;
        enum MouseType origType = currentType;
        Queue *newDrawList = s4396122_util_queue_create();
        while (s4396122_util_queue_size(drawList) > 0) {
            struct DrawCmd *c = s4396122_util_queue_pop(drawList);
            s4396122_util_queue_push(newDrawList, c);
            s4396122_os_draw_change_pen_color(c->color);
            s4396122_os_draw_change_pen_type(c->type);
            if (c->mode == OS_DRAW_CHAR_MODE) {
                int origX = c->c.x * OS_DRAW_LINE_WIDTH + OS_DRAW_CANVAS_OFFSET_X;
                int origY = c->c.y * OS_DRAW_LINE_HEIGHT + OS_DRAW_CANVAS_OFFSET_Y;
                if (c->c.c & 0x01)
                    s4396122_os_draw_add_line(origX, origY, origX + OS_DRAW_LINE_LENGTH, origY);
                if (c->c.c & 0x02)
                    s4396122_os_draw_add_line(origX + OS_DRAW_LINE_LENGTH, origY, origX + OS_DRAW_LINE_LENGTH, origY + OS_DRAW_LINE_LENGTH);
                if (c->c.c & 0x04)
                    s4396122_os_draw_add_line(origX, origY, origX, origY + OS_DRAW_LINE_LENGTH);
                if (c->c.c & 0x08)
                    s4396122_os_draw_add_line(origX, origY + OS_DRAW_LINE_LENGTH, origX + OS_DRAW_LINE_LENGTH, origY + OS_DRAW_LINE_LENGTH);
                if (c->c.c & 0x10)
                    s4396122_os_draw_add_line(origX, origY + OS_DRAW_LINE_LENGTH, origX, origY + 2 * OS_DRAW_LINE_LENGTH);
                if (c->c.c & 0x20)
                    s4396122_os_draw_add_line(origX + OS_DRAW_LINE_LENGTH, origY + OS_DRAW_LINE_LENGTH, origX + OS_DRAW_LINE_LENGTH, origY + 2 * OS_DRAW_LINE_LENGTH);
                if (c->c.c & 0x40)
                    s4396122_os_draw_add_line(origX, origY + 2 * OS_DRAW_LINE_LENGTH, origX + OS_DRAW_LINE_LENGTH, origY + 2 * OS_DRAW_LINE_LENGTH);
            } else if (c->mode == OS_DRAW_POLY_MODE) {
                double origX = c->p.x + OS_DRAW_CANVAS_OFFSET_X;
                double origY = c->p.y + OS_DRAW_CANVAS_OFFSET_Y;
                double angle = 2 * M_PI / c->p.degree;
                s4396122_os_draw_mouse_button(0);
                s4396122_os_draw_move_mouse(100, 100);
                s4396122_os_draw_move_mouse(origX, origY * OS_DRAW_CANVAS_MULTI_FACTOR);
                s4396122_os_draw_mouse_button(1);
                for (int i = 0; i < c->p.degree; i++) {
                    origX += c->p.length * sin(angle * i);
                    origY += c->p.length * cos(angle * i);
                    s4396122_os_draw_move_mouse(origX, origY * OS_DRAW_CANVAS_MULTI_FACTOR);
                }
                s4396122_os_draw_mouse_button(0);
            }
        }
        s4396122_util_queue_free(drawList);
        drawList = newDrawList;

        if (tempChar.mode != 0) {
            s4396122_os_draw_change_pen_color(tempChar.color);
            s4396122_os_draw_change_pen_type(tempChar.type);
            int origX = tempChar.c.x * OS_DRAW_LINE_WIDTH + OS_DRAW_CANVAS_OFFSET_X;
            int origY = tempChar.c.y * OS_DRAW_LINE_HEIGHT + OS_DRAW_CANVAS_OFFSET_Y;
            if (tempChar.c.c & 0x01)
                s4396122_os_draw_add_line(origX, origY, origX + OS_DRAW_LINE_LENGTH, origY);
            if (tempChar.c.c & 0x02)
                s4396122_os_draw_add_line(origX + OS_DRAW_LINE_LENGTH, origY, origX + OS_DRAW_LINE_LENGTH, origY + OS_DRAW_LINE_LENGTH);
            if (tempChar.c.c & 0x04)
                s4396122_os_draw_add_line(origX, origY, origX, origY + OS_DRAW_LINE_LENGTH);
            if (tempChar.c.c & 0x08)
                s4396122_os_draw_add_line(origX, origY + OS_DRAW_LINE_LENGTH, origX + OS_DRAW_LINE_LENGTH, origY + OS_DRAW_LINE_LENGTH);
            if (tempChar.c.c & 0x10)
                s4396122_os_draw_add_line(origX, origY + OS_DRAW_LINE_LENGTH, origX, origY + 2 * OS_DRAW_LINE_LENGTH);
            if (tempChar.c.c & 0x20)
                s4396122_os_draw_add_line(origX + OS_DRAW_LINE_LENGTH, origY + OS_DRAW_LINE_LENGTH, origX + OS_DRAW_LINE_LENGTH, origY + 2 * OS_DRAW_LINE_LENGTH);
            if (tempChar.c.c & 0x40)
                s4396122_os_draw_add_line(origX, origY + 2 * OS_DRAW_LINE_LENGTH, origX + OS_DRAW_LINE_LENGTH, origY + 2 * OS_DRAW_LINE_LENGTH);
        }

        s4396122_os_draw_change_pen_color(origColor);
        s4396122_os_draw_change_pen_type(origType);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_reset() {
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(OS_DRAW_RECTANGLE_X, OS_DRAW_RECTANGLE_Y);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(-OS_DRAW_CANVAS_ORIGINAL_OFFSET_X, -OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y);
    s4396122_os_draw_mouse_button(2);
    s4396122_os_draw_move_mouse(OS_DRAW_CANVAS_ORIGINAL_OFFSET_X, OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y);
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(OS_DRAW_PEN_X, OS_DRAW_PEN_Y);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_mouse_button(0);
}

void s4396122_os_draw_remove_top() {
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        Queue *newDrawList = s4396122_util_queue_create();
        while (s4396122_util_queue_size(drawList) > 1) {
            s4396122_util_queue_push(newDrawList, s4396122_util_queue_pop(drawList));
        }
        if (s4396122_util_queue_size(drawList) > 0) {
            free(s4396122_util_queue_pop(drawList));
        }
        drawList = newDrawList;
        s4396122_os_draw_reset();
        /*s4396122_os_draw_redraw();*/
        xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
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

void s4396122_os_draw_add_temp_char(char c) {
    tempChar.mode = OS_DRAW_CHAR_MODE;
    s4396122_os_draw_reset();
    xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
    tempChar.c.c = c;
}

void s4396122_os_draw_move_temp_char(int x, int y) {
    s4396122_os_draw_reset();
    xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
    tempChar.c.x += x;
    tempChar.c.y += y;
}

void s4396122_os_draw_commit_temp_char() {
    if (tempChar.mode != 0) {
        if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
            struct DrawCmd *d = malloc(sizeof(struct DrawCmd));
            d->mode = OS_DRAW_CHAR_MODE;
            d->c = tempChar.c;
            d->color = tempChar.color;
            d->type = tempChar.type;
            d->c.x++;
            tempChar.mode = 0;
            s4396122_util_queue_push(drawList, d);
            xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
            xSemaphoreGive(s4396122_SemaphoreDrawList);
        }
    }
}

void s4396122_os_draw_move_origin(int x, int y) {
    s4396122_os_draw_reset();
    xSemaphoreGive(s4396122_SemaphoreUpdateDisplay);
    OS_DRAW_CANVAS_OFFSET_X += x;
    OS_DRAW_CANVAS_OFFSET_Y += y;
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
