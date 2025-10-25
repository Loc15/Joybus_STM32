# STM32 JOYBUS

A N64 input to Xinput converter.

- This project was made using a NUCLEO-U083RC.
- The idea was using a [JoybusOverUart library made by GitMoDu](https://github.com/GitMoDu/JoybusOverUart),
this is why it's necessary to do the wiring explained in that repository.
- The USART4 was used as data input (A0 TX and A1 RX)

## Building
```
git clone https://github.com/Loc15/Joybus_STM32.git
cd Joybus_STM32/
git submodule update --init
mkdir build
cd build
make
```
