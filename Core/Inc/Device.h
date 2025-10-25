//
// Created by loc15 on 19/10/25.
//

#ifndef JOYBUS_STM32_DEVICE_H
#define JOYBUS_STM32_DEVICE_H

#include "main.h"
#include "xinput_driver.h"
#include "Nintendo64Controller.h"

#define GAMEPAD_JOYSTICK_MID 0

uint8_t endpoint_in = 0;
uint8_t endpoint_out = 0;
uint8_t xinput_out_buffer[XINPUT_OUT_SIZE] = {};

typedef struct {
    uint16_t Buttons;
    uint16_t JoystickX;
    uint16_t JoystickY;
} ControllerGenericData_t;

template <typename T>
class Host{
public:
    Host(T *controller) : PhysicalController(controller){};
    virtual void init() = 0;
    virtual void poll() = 0;
    virtual void getData() = 0;

    ControllerGenericData_t genericData{};

protected:
    T *PhysicalController;
};

template <typename DeviceController>
class Device {
public:
    Device(){
        tud_init(BOARD_TUD_RHPORT);
    }

    void initDevice(){
        static_cast<DeviceController *>(this)->init();
    }

    void getHostReportDevice(){
        static_cast<DeviceController *>(this)->getHostReport();
    }

    static inline uint32_t board_millis(void) {
        return HAL_GetTick();
    }

    void sendReportData() {

        // Poll every 1ms
        const uint32_t interval_ms = 1;
        static uint32_t start_ms = 0;

        if (board_millis() - start_ms < interval_ms) return;  // not enough time
        start_ms += interval_ms;

        if (tud_suspended())
            tud_remote_wakeup();

        static_cast<DeviceController *>(this)->sendReport();
    }

    void loop(){
        while(1){
            tud_task();  // tinyusb device task
            this->getHostReportDevice();
            this->sendReportData();
        }

    }


private:

};

template <typename HostController>
class XinputController : public Device<XinputController<HostController>>{
public:
    XinputController(HostController *host) : host(host){};

    void init(){
        host->init();
    }

    void getHostReport(){
        host->poll();
        host->getData();
        //Converter the data from N64 to Xinput
        this->report.buttons2 = ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::A)) & 0x1 ? XBOX_MASK_A : 0) |
                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::B)) & 0x1 ? XBOX_MASK_B : 0) |
                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::CDown)) & 0x1 ? XBOX_MASK_X : 0) |
                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::CLeft)) & 0x1 ? XBOX_MASK_Y : 0) |
                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::CUp)) & 0x1 ? XBOX_MASK_LB : 0) |
                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::CRight)) & 0x1 ? XBOX_MASK_RB : 0);

        this->report.buttons1 = ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::Up)) & 0x1 ? XBOX_MASK_UP : 0) |
                                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::Left)) & 0x1 ? XBOX_MASK_LEFT : 0) |
                                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::Right)) & 0x1 ? XBOX_MASK_RIGHT : 0) |
                                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::Down)) & 0x1 ? XBOX_MASK_DOWN : 0) |
                                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::Start)) & 0x1 ? XBOX_MASK_START : 0) |
                                ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::Z)) & 0x1 ? XBOX_MASK_BACK : 0);

        this->report.lt = ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::L)) & 0x1 ? 0xFF : 0);
        this->report.rt = ((this->host->genericData.Buttons >> static_cast<uint8_t>(Nintendo64Controller::ButtonsEnum::R)) & 0x1 ? 0xFF : 0);
        this->report.lx = this->host->genericData.JoystickX << 10;
        this->report.ly = this->host->genericData.JoystickY << 10;
    }

    void sendReport(){
        send_xinput_report(&(this->report), sizeof(XInputReport));
    }

private:
    XInputReport report = {
            .report_id = 0,
            .report_size = XINPUT_ENDPOINT_SIZE,
            .buttons1 = 0,
            .buttons2 = 0,
            .lt = 0,
            .rt = 0,
            .lx = GAMEPAD_JOYSTICK_MID,
            .ly = GAMEPAD_JOYSTICK_MID,
            .rx = GAMEPAD_JOYSTICK_MID,
            .ry = GAMEPAD_JOYSTICK_MID,
            ._reserved = { },
    };

    HostController *host;
};


#endif //JOYBUS_STM32_DEVICE_H


//Curiously Recurring Template Pattern