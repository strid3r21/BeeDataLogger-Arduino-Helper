#ifndef _BDL_H
#define _BDL_H

#include <Arduino.h>
#include <esp_adc_cal.h>
#include <soc/adc_channel.h>


#define VBAT_ADC_CHANNEL ADC1_GPIO1_CHANNEL
#define VBAT_GPIO 1
#define green 0,150,0
#define red  150,0,0
#define blue 0,0,150
#define pink 255,20,147
#define purple 148,0,211
#define yellow 242,239,61
#define white 255,255,255
#define aqua 0,255,255
#define orange 255,165,0
#define off 0,0,0



class BDL {
  public:
    BDL() : brightness(255) {}

    void begin() {
      // RGB_PWR in on LDO2 so we can turn it completely off during deep sleep
      pinMode(RGB_PWR, OUTPUT);
      #if ESP_ARDUINO_VERSION_MAJOR < 3
    rmt = rmtInit(RGB_DATA, RMT_TX_MODE, RMT_MEM_64);
    rmtSetTick(rmt, 25);
#else
    rmtInit(RGB_DATA, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, 10000000);
#endif

      #if ESP_ARDUINO_VERSION_MAJOR < 3
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_2_5, ADC_WIDTH_BIT_12, 0,
                             &adc_cal);
    adc1_config_channel_atten(VBAT_ADC_CHANNEL, ADC_ATTEN_DB_2_5);
#else
    analogSetPinAttenuation(VBAT_GPIO, ADC_2_5db);
#endif
      pinMode(VBUS_SENSE, INPUT);
	  digitalWrite(LDO2, HIGH);
    }

    void setLDO2Power(bool on) {
      digitalWrite(LDO2, on);
    }


    void setPixelPower(bool on) {
      digitalWrite(RGB_PWR, on);
    }

    void setPixelColor(uint8_t r, uint8_t g, uint8_t b) {
      pixel_color[0] = g;
      pixel_color[1] = r;
      pixel_color[2] = b;
      writePixel();
    }
	

    void setPixelColor(uint32_t rgb) {
      setPixelColor(rgb >> 16, rgb >> 8, rgb);
    }

    void setPixelBrightness(uint8_t brightness) {
      this->brightness = brightness;
      writePixel();
    }

     void writePixel() {
    setPixelPower(true);
    while (micros() - next_rmt_write < 350) {
      yield();
    }
    int index = 0;
    for (auto chan : pixel_color) {
      uint8_t value = chan * (brightness + 1) >> 8;
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
#if ESP_ARDUINO_VERSION_MAJOR < 3
          rmt_data[index].level0 = 1;
          rmt_data[index].duration0 = 32; 
          rmt_data[index].level1 = 0;
          rmt_data[index].duration1 = 18; 
        } else {
          rmt_data[index].level0 = 1;
          rmt_data[index].duration0 = 16; 
          rmt_data[index].level1 = 0;
          rmt_data[index].duration1 = 34; 
#else
          rmt_data[index].level0 = 1;
          rmt_data[index].duration0 = 8;
          rmt_data[index].level1 = 0;
          rmt_data[index].duration1 = 4;
        } else {
          rmt_data[index].level0 = 1;
          rmt_data[index].duration0 = 4;
          rmt_data[index].level1 = 0;
          rmt_data[index].duration1 = 8;
#endif
        }
        index++;
      }
    }
#if ESP_ARDUINO_VERSION_MAJOR < 3
    rmtWrite(rmt, rmt_data, 3 * 8);
#else
    rmtWrite(RGB_DATA, rmt_data, 3 * 8, RMT_WAIT_FOR_EVER);
#endif
    next_rmt_write = micros();
  }

    static uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
      return (r << 16) | (g << 8) | b;
    }

    static uint32_t colorWheel(uint8_t pos) {
      if (pos < 85) {
        return color(255 - pos * 3, pos * 3, 0);
      } else if (pos < 170) {
        pos -= 85;
        return color(0, 255 - pos * 3, pos * 3);
      } else {
        pos -= 170;
        return color(pos * 3, 0, 255 - pos * 3);
      }
    }

    float getBatteryVoltage() {
      #if ESP_ARDUINO_VERSION_MAJOR < 3
    uint32_t raw = adc1_get_raw(VBAT_ADC_CHANNEL);
    uint32_t millivolts = esp_adc_cal_raw_to_voltage(raw, &adc_cal);
#else
    uint32_t millivolts = analogReadMilliVolts(VBAT_GPIO);
#endif
    const uint32_t upper_divider = 442;
    const uint32_t lower_divider = 160;
    return (float)(upper_divider + lower_divider) / lower_divider / 1000 *
           millivolts;
  }
	
	float getADCVoltage(int pinNumber){
		int _pin = pinNumber;
		pinMode(_pin, INPUT);
		float userPin = analogRead(_pin);
		float voltage_value = (userPin * 3.3 ) / (4095);
		return(voltage_value);
	}


    bool getVbusPresent() {
      return digitalRead(VBUS_SENSE);
    }

  private:
  #if ESP_ARDUINO_VERSION_MAJOR < 3
  rmt_obj_t *rmt;
#endif
   
    rmt_data_t rmt_data[3 * 8];
    unsigned long next_rmt_write;
    uint8_t pixel_color[3];
    uint8_t brightness;
    esp_adc_cal_characteristics_t adc_cal;
};

#endif
