#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include <font.h>
#include <string.h>

#include <esp_log.h>

#include <board.h>
#include <board_input.h>
#include <board_eink.h>
#include <board_eink_fb.h>
#include <board_pins.h>
#include <board_button.h>
#include <board_first_run.h>
#include <sha2017_ota.h>

#include "imgv2_sha.h"
#include "imgv2_menu.h"
#include "imgv2_nick.h"
#include "imgv2_weather.h"
#include "imgv2_test.h"

static const char *TAG = "main";

struct menu_item {
  const char *title;
  void (*handler)(void);
};

const struct menu_item demoMenu[] = {
    /*{"UART GPS", &uartdemo},
    {"I2C", &i2cdemo},
    {"uGFX demo", &demoUgfx},
    {"charging demo", &demoPower},*/
    {NULL, NULL},
};

void displayMenu(const char *menu_title, const struct menu_item *itemlist) {
	int num_items = 0;
	while (itemlist[num_items].title != NULL)
		num_items++;

	int scroll_pos = 0;
	int item_pos = 0;
	bool need_redraw = true;
	while (1) {
		/* draw menu */
		if (need_redraw) {
			// init buffer
			draw_font(board_eink_fb, 0, 0, BOARD_EINK_WIDTH, menu_title,
					FONT_16PX | FONT_INVERT | FONT_FULL_WIDTH | FONT_UNDERLINE_2);
			int i;
			for (i = 0; i < 7; i++) {
				int pos = scroll_pos + i;
				draw_font(board_eink_fb, 0, 16+16*i, BOARD_EINK_WIDTH,
						(pos < num_items) ? itemlist[pos].title : "",
						FONT_16PX | FONT_FULL_WIDTH |
						((pos == item_pos) ? 0 : FONT_INVERT));
			}

			board_eink_display(board_eink_fb,\
 DISPLAY_FLAG_LUT(BOARD_EINK_LUT_NORMAL) );
			need_redraw = false;
		}

		/* handle input */
		uint32_t button_id;
		if ((button_id = board_input_get_event(-1)) != 0)
		{
			if (button_id == BOARD_BUTTON_LEFT) {
				ets_printf("Button LEFT handling\n");
				return;
			}

			if (button_id == BOARD_BUTTON_SELECT) {
				ets_printf("Selected '%s'\n", itemlist[item_pos].title);
				if (itemlist[item_pos].handler != NULL)
					itemlist[item_pos].handler();

        ets_printf("Resetting display\n");
				// reset screen
				memset(board_eink_fb, 0xff, BOARD_EINK_WIDTH * BOARD_EINK_HEIGHT / 8);
				board_eink_display(board_eink_fb,\
 DISPLAY_FLAG_LUT(BOARD_EINK_LUT_NORMAL) | DISPLAY_FLAG_FULL_UPDATE);

				need_redraw = true;
				ets_printf("Button START handled\n");
				continue;
			}

			if (button_id == BOARD_BUTTON_LEFT) {
				if (item_pos > 0) {
					item_pos--;
					if (scroll_pos > item_pos)
						scroll_pos = item_pos;
					need_redraw = true;
				}
				ets_printf("Button LEFT handled\n");
			}

			if (button_id == BOARD_BUTTON_RIGHT) {
				if (item_pos + 1 < num_items) {
					item_pos++;
					if (scroll_pos + 6 < item_pos)
						scroll_pos = item_pos - 6;
					need_redraw = true;
				}
				ets_printf("Button RIGHT handled\n");
			}
		}
	}
}

void app_main(void) {
  ESP_LOGI(TAG, "Checking first run...\n");
	board_check_first_run();
  ESP_LOGI(TAG, "Successful return from first run!\n");
	board_init();

	esp_err_t err = board_eink_fb_init();
	assert( err == ESP_OK );
	// start with white screen
	memset(board_eink_fb, 0xff, BOARD_EINK_FB_LEN);
	board_eink_display(board_eink_fb, DISPLAY_FLAG_LUT(0));
  board_first_run();

  while (1)
  {
    esp_deep_sleep_start();
  }
}

void vPortCleanUpTCB ( void *pxTCB ) {
	// place clean up code here
}
