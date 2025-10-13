#include <main.hpp>

#include <iostream>
#include <string>

#include <HardwareSerial.h>
#include <Nintendo64Controller.h>
#include <Nintendo64OverUart.h>

static constexpr uint32_t UpdatePeriodMillis = 100;
HardwareSerial serial{nullptr};
HardwareSerial* JoyBusSerial = &serial;
Nintendo64OverUart JoyBusReader(JoyBusSerial);

Nintendo64Controller::data_t ControllerState{};
bool Connected = false;
std::string buffer;

UART_HandleTypeDef *huart2_p;

void setup(UART_HandleTypeDef *uart_p, UART_HandleTypeDef *joybus_uart)
{
    huart2_p = uart_p;
    serial.set_uart_data(joybus_uart);

    JoyBusReader.Start();
}

void loop(void)
{
    JoyBusReader.Poll();

    HAL_Delay(1);

    if (JoyBusReader.ReadControllerData(ControllerState))
    {
        if (!Connected)
        {
            Connected = true;
            buffer = "Found controller!\n\r";
            //HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);
            //Serial.println(F("Found controller!"));
        }

        //Serial.print(F("Buttons: 0b"));
        buffer = "Buttons: 0b";
        HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);
        for (uint8_t i = 0; i < sizeof(ControllerState.Buttons) * 8; i++)
        {
            //Serial.print((ControllerState.Buttons >> i) & 0b1);
            buffer = std::to_string((ControllerState.Buttons >> i) & 0b1);
            HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);
        }

        buffer = " " + std::to_string(ControllerState.JoystickX) + " ";
        HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);

        buffer = std::to_string(ControllerState.JoystickY);
        HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);

        // Imprime el salto de linea
        buffer = "\n\r";
        HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);

        HAL_Delay(UpdatePeriodMillis - 1);
    }
    else
    {
        if (Connected)
        {
            Connected = false;
            buffer = "No controller.";
            HAL_UART_Transmit(huart2_p, (uint8_t *)buffer.c_str(), buffer.size(), 100);
            //Serial.println(F("No controller."));
        }
        //HAL_Delay(UpdatePeriodMillis * 4);
    }
}