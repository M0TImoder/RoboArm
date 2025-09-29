#include <Udon.hpp>
#include <array>

static Udon::CanBusSpi bus;
Udon::PicoWDT wdt(2000);
Udon::Led led(LED_BUILTIN);

std::array<Udon::Motor3, 3> drivers = {
  Udon::Motor3(0, 2, 1), 
  Udon::Motor3(3, 5, 4), 
  Udon::Motor3(6, 8, 7)
};

std::array<Udon::CanReader<Udon::Message::Motor>, 3> motors{
  { { bus, 0x01 },
    { bus, 0x02 },
    { bus, 0x03 } }
};

void setup() {
  led.begin();
  bus.begin();
  
  for (auto& driver : drivers)
  {
    driver.begin();
  }
}

void loop() {
  wdt.update();
  bus.update();

  for(int i = 0; i < 4; i++)
  {
    if (const auto message = motors[i].getMessage())
    {
      int16_t power = message->power;
      drivers[i].move(- power);
      drivers[i].show();
    }
    else
    {
      drivers[i].stop();
    }
  }

  led.flush();

  // bus.show();
  // Serial.println();
}
