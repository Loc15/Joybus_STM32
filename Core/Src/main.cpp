#include <main.hpp>

#include <HardwareSerial.h>
#include <Nintendo64Controller.h>
#include <Nintendo64OverUart.h>

#include "Device.h"

HardwareSerial serial{nullptr};
HardwareSerial* JoyBusSerial = &serial;
Nintendo64OverUart JoyBusReader(JoyBusSerial);


class ControllerInput : public Host<Nintendo64OverUart> {
public:
    explicit ControllerInput(Nintendo64OverUart *controller) : Host(controller) {}

    void init() override {
        this->PhysicalController->Start();
    }

    void poll() override {
        this->PhysicalController->Poll();
    }

    void getData() override{
        HAL_Delay(1);
        this->PhysicalController->ReadControllerData(this->ControllerState);
        this->genericData.Buttons = this->ControllerState.Buttons;
        this->genericData.JoystickX = (uint16_t)this->ControllerState.JoystickX;
        this->genericData.JoystickY = (uint16_t)this->ControllerState.JoystickY;
    }

private:
    Nintendo64Controller::data_t ControllerState{};
};

void setup(UART_HandleTypeDef *joybus_uart){
    // Set uart data
    serial.set_uart_data(joybus_uart);
}

void loop(){
    // Link JoybusReader to ControllerInput Class
    ControllerInput host(&JoyBusReader);
    // Xinput controller class
    XinputController<ControllerInput> deviceController(&host);
    // Parent Class device init
    deviceController.init();
    // Parent Class device loop (The REAL LOOP)
    deviceController.loop();
};