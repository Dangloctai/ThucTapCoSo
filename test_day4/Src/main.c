#include <system_stm32f4xx.h>
#include <stdint.h>
#include <timer.h>
#include <Ucglib.h>

static ucg_t ucg;

int main(void)
{
    SystemCoreClockUpdate();
    TimerInit();

    Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
    ucg_ClearScreen(&ucg);
    ucg_SetFont(&ucg, ucg_font_courB12_tf);
    ucg_SetColor(&ucg, 0, 0, 255, 0);
    ucg_SetRotate180(&ucg);
    ucg_DrawString (&ucg, 28, 70, 0, "Nhom4_TTCS");

    while(1)
    {
    	processTimerScheduler();
    }
}
