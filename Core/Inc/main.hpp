#ifndef __MAIN_HPP
#define __MAIN_HPP

#include <main.h>

#ifdef __cplusplus
extern "C" {
#endif

    void setup(UART_HandleTypeDef *uart_p, UART_HandleTypeDef *joybus_uart);

    void loop(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */