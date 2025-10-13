//
// Created by loc15 on 5/09/25.
//

#ifndef JOYBUS_STM32_HARDWARESERIAL_H
#define JOYBUS_STM32_HARDWARESERIAL_H

#include <main.h>
#include <cstring>

class HardwareSerial {
    public:

    HardwareSerial(UART_HandleTypeDef *uart_data) : uart_data{uart_data}{

    }

    void set_uart_data(UART_HandleTypeDef *uart_data) {
        this->uart_data = uart_data;
    }

    void enable_DMA_circular_buffer(){
        HAL_UART_Receive_DMA(this->uart_data, this->UART_rxBuffer, this->buffer_size);
    }

    void begin(uint32_t Baudrate) {
        this->uart_data->Init.BaudRate = Baudrate;
        this->enable_DMA_circular_buffer();
    }

    void write(const uint8_t byte) {
        // Too slow between bytes after stop bit
        //HAL_UART_Transmit(uart_data, &byte, 1, 100);
        while (!(this->uart_data->Instance->ISR & USART_ISR_TXE_TXFNF));
        this->uart_data->Instance->TDR = byte;
    }

    uint8_t read() {
        //uint8_t byte{};
        //HAL_UART_Receive(uart_data, &byte, 1, 100);
        //return byte9;
        if (this->available()) {  // Check if there is data inside the buffer
            uint8_t data = this->UART_rxBuffer[this->UART_readIndex];

            // Update circular buffer read index
            this->UART_readIndex = (this->UART_readIndex + 1) % 64;

            return data;
        } else {
            return 0;  // No new data available
        }

    }

    bool available() {
        //return !(HAL_UART_GetState(uart_data) == HAL_UART_STATE_READY);
        //https://community.st.com/t5/stm32-mcus-embedded-software/how-to-check-the-uart-serial-buffer-if-they-are-available-using/td-p/217316
        this->UART_writeIndex = this->buffer_size - this->uart_data->hdmarx->Instance->CNDTR;
        if(this->UART_writeIndex != this->UART_readIndex){
            return true;
        }
        return false;
        //return (__HAL_UART_GET_FLAG(uart_data, UART_FLAG_RXNE) == SET);
    }

    void flush() {
        HAL_UART_DMAStop(this->uart_data);
        this->UART_writeIndex = 0;
        this->UART_readIndex = 0;
        memset(this->UART_rxBuffer, 0, this->buffer_size);
        this->enable_DMA_circular_buffer();
    }

    void end() {
        HAL_UART_DMAStop(this->uart_data);
    }

    void clearWriteError(){
        //Dummy function
    }

    uint8_t UART_rxBuffer[64];
    uint8_t UART_writeIndex;
    uint8_t UART_readIndex;

    private:
    UART_HandleTypeDef *uart_data;
    const uint8_t buffer_size = 64;

};

// Delay time function
void delay(int millis){
    HAL_Delay(millis);
}

#endif //JOYBUS_STM32_HARDWARESERIAL_H