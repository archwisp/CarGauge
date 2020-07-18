#include <Arduino.h>

class OilTempSensor {
    int sensor_pin;
    float vin;
    float adc_resolution;
    float vtweak;
    double buffer;
    int R1 = 1000;

    public:
    OilTempSensor(int sensor_pin, float vin, float adc_resolution, float vtweak) {
        this->sensor_pin = sensor_pin;
        this->vin = vin;
        this->adc_resolution = adc_resolution;
        this->vtweak = vtweak;
    }

    float getTemp() {
        buffer = analogRead(this->sensor_pin);
        buffer = buffer * (vin / this->adc_resolution) + this->vtweak;
        buffer = R1 / ((vin / buffer) -1);
        float s1 = 4000.0, s2 = 310.0;
        buffer = 1.0/((log(buffer/R1)/s1)+(1.0/s2))-271.0;
        return buffer;
    }
};

