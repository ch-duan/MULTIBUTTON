#include "multi_button.h"
#include <map>
enum Button_IDs {
  btn1_id,
  btn2_id,
};

struct Button btn1;
struct Button btn2;

std::map<uint8_t, std::string> btn_evt = {
    {PRESS_DOWN, "PRESS_DOWN"},
    {PRESS_UP, "PRESS_UP"},
    {PRESS_REPEAT, "PRESS_REPEAT"},
    {SINGLE_CLICK, "SINGLE_CLICK"},
    {MULTIPLE_CLICK, "MULTIPLE_CLICK"},
    {LONG_PRESS_START, "LONG_PRESS_START"},
    {LONG_PRESS_HOLD, "LONG_PRESS_HOLD"},
    {number_of_event, "number_of_event"},
    {NONE_PRESS, "NONE_PRESS"}};

void btn_event_cb(void *btn) {
  Button *button = (Button *)btn;
  printf("button%d:%s\r\n", button->button_id, btn_evt[button->event].c_str());
}

uint8_t read_button_GPIO(uint8_t button_id) {
  // you can share the GPIO read function with multiple Buttons
  switch (button_id) {
  case btn1_id:
    return HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
  case btn2_id:
    return HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin);
  default:
    return 0;
  }
}

int main() {
  button_init(&btn1, read_button_GPIO, 0, btn1_id);
  button_init(&btn2, read_button_GPIO, 0, btn2_id);

  button_attach(&btn1, PRESS_DOWN, btn_event_cb);
  button_attach(&btn1, PRESS_UP, btn_event_cb);
  button_attach(&btn1, PRESS_REPEAT, btn_event_cb);
  button_attach(&btn1, SINGLE_CLICK, btn_event_cb);
  button_attach(&btn1, MULTIPLE_CLICK, btn_event_cb);
  button_attach(&btn1, LONG_PRESS_START, btn_event_cb);
  button_attach(&btn1, LONG_PRESS_HOLD, btn_event_cb);

  button_attach(&btn2, PRESS_DOWN, btn_event_cb);
  button_attach(&btn2, PRESS_UP, btn_event_cb);
  button_attach(&btn2, PRESS_REPEAT, btn_event_cb);
  button_attach(&btn2, SINGLE_CLICK, btn_event_cb);
  button_attach(&btn2, MULTIPLE_CLICK, btn_event_cb);
  button_attach(&btn2, LONG_PRESS_START, btn_event_cb);
  button_attach(&btn2, LONG_PRESS_HOLD, btn_event_cb);

	//设置长按时间5S
	button_set_long_ticks(&btn1, 5000);
	
	//设置连按次数5次
  button_set_multiple_click(&btn1, 5);

  button_start(&btn1);
  button_start(&btn2);

  // make the timer invoking the button_ticks() interval 5ms.
  // This function is implemented by yourself.
  __timer_start(button_ticks, 0, 5);

  while (1) {
  }
}
