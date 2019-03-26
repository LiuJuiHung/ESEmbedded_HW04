HW04
===
This is the hw04 sample. Please follow the steps below.

# Build the Sample Program

1. Fork this repo to your own github account.

2. Clone the repo that you just forked.

3. Under the hw04 dir, use:

	* `make` to build.

	* `make flash` to flash the binary file into your STM32F4 device.

	* `make clean` to clean the ouput files.

# Build Your Own Program

1. Edit or add files if you need to.

2. Make and run like the steps above.

3. Please avoid using hardware dependent C standard library functions like `printf`, `malloc`, etc.

# HW04 Requirements

1. Please practice to reference the user manuals mentioned in [Lecture 04], and try to use the user button (the blue one on the STM32F4DISCOVERY board).

2. After reset, the device starts to wait until the user button has been pressed, and then starts to blink the blue led forever.

3. Try to define a macro function `READ_BIT(addr, bit)` in reg.h for reading the value of the user button.

4. Push your repo to your github. (Use .gitignore to exclude the output files like object files, executable files, or binary files)

5. The TAs will clone your repo, build from your source code, and flash to see the result.

[Lecture 04]: http://www.nc.es.ncku.edu.tw/course/embedded/04/

--------------------

- [ ] **If you volunteer to give the presentation (demo) next week, check this.**

--------------------

HW04
===
###### tags: `嵌入式韌體開發與作業系統`

## User Button
### User button 電路圖
根據 [Discovery kit with STM32F407VG MCU (Page31 Figure 14. Peripherals/ Page34)](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F4DISCOVERY.pdf) 可知，User Button 是連接在 I/O PA0 上
![](https://i.imgur.com/xdLSQd0.png)


![](https://i.imgur.com/KeKwevX.png)

### GPIO 
#### [GPIO introduction (Page267)](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf) 介紹

![](https://i.imgur.com/umN6Oul.png)

#### GPIO Configurations
1. Section 8.3 described that each of the GPIOs has four 32-bit memory-mapped control registers:
    * MODER
    * OTYPER
    * OSPEEDR
    * PUPDR

2. Each GPIO has two 16-bit memory-mapped data registers: input and output data registers 
    * IDR
    * ODR

3.  set reset register (GPIOx_BSRR) is a 32-bit register
4.  locking mechanism
    * LCKR
5. Two registers are provided to select one out of the sixteen alternate function inputs/outputs available for each I/O .
    * AFRL
    * AFRH

6. GPIO 參數設定
![](https://i.imgur.com/hnyBjQf.png)
![](https://i.imgur.com/1LCruEp.png)

#### Input type
* 因為 `user button` 連接在 `PA0` 上，所以 `reg` 設置在 `0x4002 0000`

![](https://i.imgur.com/FRNtAbZ.png)

* I/O configuration 設定成 Input/floating
    * Input vs pull-up/pull-down
    ![](https://i.imgur.com/WtTGJLP.png)
    
```
// MODER = 00 
CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_MODER_OFFSET, MODERy_1_BIT(GPIO_PORTA));
CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_MODER_OFFSET, MODERy_0_BIT(GPIO_PORTA));

// PUPDR = 00 
CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(GPIO_PORTA));
CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(GPIO_PORTA));
```

#### Read Data
由 [section 6.4](http://www.nc.es.ncku.edu.tw/course/embedded/pdf/STM32F407_Reference_manual.pdf) 可知，讀取 Button 狀態，需讀取 PA0 故先查詢 PA0 之記憶體位置 

![](https://i.imgur.com/nMr6fZ4.png)

由上述得知其 offset 為 0x10 寫一個 Marco 計算 Reg 位置
```
#define GPIOx_IDR_OFFSET 0x10
#define IDRy_BIT(y) (y)
```
寫一個 Read_Bit Marco 取得特定位數 bit 資料
```
#define READ_BIT(addr, bit) ( ( REG(addr) >> bit ) &  UINT32_1 )
```

#### Program
* blink.c
    * LED init
    ```C=1
    void led_init(unsigned int led)
    {
        SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIO_EN_BIT(GPIO_PORTD));

        //MODER led pin = 01 => General purpose output mode
        CLEAR_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_MODER_OFFSET, MODERy_1_BIT(led));
        SET_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_MODER_OFFSET, MODERy_0_BIT(led));

        //OT led pin = 0 => Output push-pull
        CLEAR_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_OTYPER_OFFSET, OTy_BIT(led));

        //OSPEEDR led pin = 00 => Low speed
        CLEAR_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_1_BIT(led));
        CLEAR_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_0_BIT(led));

        //PUPDR led pin = 00 => No pull-up, pull-down
        CLEAR_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(led));
        CLEAR_BIT(GPIO_BASE(GPIO_PORTD) + GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(led));
    }
    ```
    
    * Button init
    ```C=1
    void button_init(void)
    {
        SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIO_EN_BIT(GPIO_PORTA));

        //MODER led pin = 00 => Input Floating mode
        CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_MODER_OFFSET, MODERy_1_BIT(0));
        CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_MODER_OFFSET, MODERy_0_BIT(0));

        //PUPDR led pin = 00 => No pull-up, pull-down
        CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(0));
        CLEAR_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(0));
    }
    ```
* blink.h
add `button_init()`
```
void button_init();
```

* main.c
```C=1
#include "blink.h"
#include "reg.h"
#include <stdint.h>

int main(void)
{
	button_init();
	while(! READ_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_IDR_OFFSET, IDRy_BIT(0)));    // wait for button trigger
	blink(LED_BLUE);
}
```

## Reference
1. [GPIO](http://wiki.csie.ncku.edu.tw/embedded/GPIO)
