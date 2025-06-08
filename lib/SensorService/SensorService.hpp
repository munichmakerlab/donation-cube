#ifndef SENSOR_SERVICE_HPP
#define SENSOR_SERVICE_HPP

#include <Arduino.h>

#include "Config.h"

class SensorService {
    private:
        uint8_t sensorPin;
        uint8_t sensorState = HIGH;

    public:
        SensorService(uint8_t pin) : sensorPin(pin), sensorState(HIGH) {}

        bool risingEdge();
        bool fallingEdge();
        bool isActive();

        void setup();
        void loop();
};

#endif // SENSOR_SERVICE_HPP