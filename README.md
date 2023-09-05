# MultiButton

## 简介

原作者[https://github.com/0x1abin/MultiButton]()

MultiButton 是一个小巧简单易用的事件驱动型按键驱动模块，可无限量扩展按键，按键事件的回调异步处理方式可以简化你的程序结构，去除冗余的按键处理硬编码，让你的按键业务逻辑更清晰。

## 使用方法

1.先申请一个按键结构

```c
struct Button button1;
```

2.初始化按键对象，绑定按键的GPIO电平读取接口**read_button_pin()** ，后一个参数设置有效触发电平

```c
button_init(&button1, read_button_pin, 0, 0);
```

3.注册按键事件

```c
button_attach(&button1, SINGLE_CLICK, Callback_SINGLE_CLICK_Handler);
button_attach(&button1, DOUBLE_CLICK, Callback_DOUBLE_Click_Handler);
...
```

4.设置长按时间和连按次数

```
//设置长按时间5S，默认为1S，这个方法可以改
button_set_long_ticks(&btn1, 5000);
//设置连按次数5次，默认为3，
 button_set_multiple_click(&btn1, 5);
```

5.启动按键

```c
button_start(&button1);
```

6.设置一个5ms间隔的定时器循环调用后台处理函数

```c
while(1) {
    ...
    if(timer_ticks == 5) {
        timer_ticks = 0;

        button_ticks();
    }
}
```

## 特性

MultiButton 使用C语言实现，基于面向对象方式设计思路，每个按键对象单独用一份数据结构管理：

```c
struct Button {
	uint16_t ticks;
	uint8_t  repeat: 4;
	uint8_t  event : 4;
	uint8_t  state : 3;
	uint8_t  debounce_cnt : 3;
	uint8_t  active_level : 1;
	uint8_t  button_level : 1;
	uint8_t  button_id;
	uint8_t  (*hal_button_Level)(uint8_t  button_id_);
	BtnCallback  cb[number_of_event];
	struct Button* next;
};
```

这样每个按键使用单向链表相连，依次进入 button_handler(struct Button* handle) 状态机处理，所以每个按键的状态彼此独立。

## 按键事件

 事件说明PRESS_DOWN按键按下，每次按下都触发PRESS_UP按键弹起，每次松开都触发PRESS_REPEAT重复按下触发，变量repeat计数连击次数SINGLE_CLICK单击按键事件DOUBLE_CLICK双击按键事件LONG_PRESS_START达到长按时间阈值时触发一次LONG_PRESS_HOLD长按期间一直触发

## Examples

```cpp
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

```
