#include <Udon.hpp>
#include <array>

Udon::Led led(LED_BUILTIN);
Udon::CanBusTeensy<CAN1> bus;
Udon::LoopCycleController loopCtrl(1000);

std::array<Udon::CanWriter<Udon::Message::Motor>, 3> motors{
  { { bus, 0x01 },
    { bus, 0x02 },
    { bus, 0x03 } }
};

Udon::E220PadPS5 pad(
  { .serial = Serial2,
    .m0 = 2,
    .m1 = 3,
    .aux = 4,
    .channel = 1 });

int16_t lock_power = 50;
int16_t stick_power_r = 0;
int16_t stick_power_l = 0;
uint8_t vel_type = 0;

Udon::Message::Motor stop{
  .power = 0,
};

Udon::Message::Motor move{
  .power = lock_power,
};

Udon::Message::Motor back{
  .power = -lock_power,
};

Udon::Message::Motor stick_move_r{
  .power = stick_power_r,
};

Udon::Message::Motor stick_move_l{
  .power = stick_power_l,
};

void setup() {
  Serial.begin(115200);
  pad.begin();
  bus.begin();
  led.begin();
}

void loop() {
  bus.update();
  pad.update();

  if (pad.isOperable()) {
    stick_power_r = pad.getRightStick().y;
    stick_power_l = pad.getLeftStick().x;

    // 将来的にこの制御はカクつきなどの原因になり得ます
    // 他の速度抑制システムの実装を検討してください
    if (stick_power_r > 255) {
      stick_power_r = 250;
    } else if (stick_power_r < -255) {
      stick_power_r = -250;
    } else {
    }

    if (stick_power_l > 254) {
      stick_power_l = 250;
    } else if (stick_power_l < -254) {
      stick_power_l = -250;
    } else {
    }

    motors[0].setMessage(stick_move_r);
    motors[1].setMessage(stick_move_l);

    if (pad.getR2().press) {
      motors[2].setMessage(move);
    }
    if (pad.getL2().press) {
      motors[2].setMessage(back);
    }

    if (pad.getCircle().click) {
      vel_type++;

      switch (vel_type) {
        case 0:
          lock_power = 50;
          break;

        case 1:
          lock_power = 100;
          break;

        case 2:
          lock_power = 150;
          break;
      }
    }
  } else {
    for (int i = 0; i < 3; i++) {
      motors[i].setMessage(stop);
    }
  }

  led.flush();
  loopCtrl.update();
}
