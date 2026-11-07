#include "app_button.h"
#include "bsp_button.h"
#include "stm32f1xx_hal.h"
typedef struct { bool raw,stable,event,holding; uint32_t changed,started; } Button_t;
static Button_t buttons[2];
void App_Button_Task(void) {
    bool raw[]={BSP_Button_IsPressed(),BSP_Button2_IsPressed()};
    uint32_t now=HAL_GetTick();
    for(unsigned i=0;i<2;i++) {
        Button_t *b=&buttons[i]; b->event=false;
        if(raw[i]!=b->raw) { b->raw=raw[i]; b->changed=now; }
        if((uint32_t)(now-b->changed)>=50 && b->stable!=b->raw) {
            b->stable=b->raw; b->holding=b->stable;
            if(b->stable) { b->event=true; b->started=now; }
        }
    }
}
static bool take(unsigned i) { bool e=buttons[i].event; buttons[i].event=false; return e; }
bool App_Button_IsPressed(void) { return take(0); }
bool App_Button2_IsPressed(void) { return take(1); }
bool App_Button_IsHeld(uint32_t ms) {
    Button_t *b=&buttons[0];
    if(b->holding && (uint32_t)(HAL_GetTick()-b->started)>=ms) { b->holding=false; return true; }
    return false;
}
