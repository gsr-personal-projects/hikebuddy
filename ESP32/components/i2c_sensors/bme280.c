#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"

#include "hal_i2c.h"
#include "bme280.h"

static const char *TAG = "BME280";
static bme280_calib_data _bme280_calib;
static bme_ctrl_hum _humReg;
static bme_ctrl_meas _measReg;
static bme_config _configReg;
static int32_t t_fine = 0;
static float seaLevel = 100.0F;

void bme_init(void) {
    bool device_found = bme_test_access();
    if (!device_found) {
        return;
    }
    bme_device_reset();
    bme_read_coefficients();
    bme_set_sampling(MODE_NORMAL,
                     SAMPLING_X16,
                     SAMPLING_X16,
                     SAMPLING_X16,
                     FILTER_OFF,
                     STANDBY_MS_125);
    ESP_LOGD(TAG, "init done");
    return;
}

bool bme_test_access(void) {
    uint8_t sensorID, res;
    res = i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_CHIPID, &sensorID, 1);
    if (sensorID == 0x60) {ESP_LOGD(TAG, "BME chip found, returned valid ID: 0x%2x", sensorID);}
    else {ESP_LOGE(TAG, "BME chip found, returned invalid ID: 0x%2x", sensorID);}
    return sensorID == 0x60;
}

void bme_device_reset(void) {
    uint8_t res, status;
    res = i2c_write_reg(BME280_ADDRESS, BME280_REGISTER_SOFTRESET, 0xB6);
    while (1) {
        bool stat = bme_reading_calibration();
        if (stat == false) {break;}
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGD(TAG, "BME reset");
    return;
}

bool bme_reading_calibration(void) {
    uint8_t rStatus;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_STATUS, &rStatus, 1);
    ESP_LOGD(TAG, "BME reports status 0x%2x", rStatus);
    return (rStatus & (1 << 0)) != 0;
}

bool bme_measuring(void) {
    uint8_t rStatus;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_STATUS, &rStatus, 1);
/*    ESP_LOGD(TAG, "BME reports status 0x%2x", rStatus);*/
    return (rStatus & (1 << 3)) != 0;
}

void bme_read_coefficients(void) {
    uint16_t t1, t2, t3, p1, p2, p3, p4, p5, p6, p7, p8, p9, h2;
    
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_T1, &_bme280_calib.dig_T1, 2);
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_T2, &t2, 2);
    _bme280_calib.dig_T2 = (int16_t)t2;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_T3, &t3, 2);
    _bme280_calib.dig_T3 = (int16_t)t3;
/*    ESP_LOGD(TAG, "Calib data from T1 raw: 0x%4x (%d)", t1, t1);*/
/*    ESP_LOGD(TAG, "Calib data from T1: 0x%4x (%d)", _bme280_calib.dig_T1, _bme280_calib.dig_T1);*/
/*    ESP_LOGD(TAG, "Calib data from T2 raw: 0x%4x (%d)", t2, t2);*/
/*    ESP_LOGD(TAG, "Calib data from T2: 0x%4x (%d)", _bme280_calib.dig_T2, _bme280_calib.dig_T2);*/
/*    ESP_LOGD(TAG, "Calib data from T3: 0x%4x", _bme280_calib.dig_T3);*/

    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P1, &_bme280_calib.dig_P1, 2);
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P2, &p2, 2);
    _bme280_calib.dig_P2 = (int16_t)p2;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P3, &p3, 2);
    _bme280_calib.dig_P3 = (int16_t)p3;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P4, &p4, 2);
    _bme280_calib.dig_P4 = (int16_t)p4;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P5, &p5, 2);
    _bme280_calib.dig_P5 = (int16_t)p5;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P6, &p6, 2);
    _bme280_calib.dig_P6 = (int16_t)p6;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P7, &p7, 2);
    _bme280_calib.dig_P7 = (int16_t)p7;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P8, &p8, 2);
    _bme280_calib.dig_P8 = (int16_t)p8;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_P9, &p9, 2);
    _bme280_calib.dig_P9 = (int16_t)p9;
/*    ESP_LOGD(TAG, "Calib data from P1: 0x%4x", _bme280_calib.dig_P1);*/
/*    ESP_LOGD(TAG, "Calib data from P2: 0x%4x", _bme280_calib.dig_P2);*/
/*    ESP_LOGD(TAG, "Calib data from P3: 0x%4x", _bme280_calib.dig_P3);*/
/*    ESP_LOGD(TAG, "Calib data from P4: 0x%4x", _bme280_calib.dig_P4);*/
/*    ESP_LOGD(TAG, "Calib data from P5: 0x%4x", _bme280_calib.dig_P5);*/
/*    ESP_LOGD(TAG, "Calib data from P6: 0x%4x", _bme280_calib.dig_P6);*/
/*    ESP_LOGD(TAG, "Calib data from P7: 0x%4x", _bme280_calib.dig_P7);*/
/*    ESP_LOGD(TAG, "Calib data from P8: 0x%4x", _bme280_calib.dig_P8);*/
/*    ESP_LOGD(TAG, "Calib data from P9: 0x%4x", _bme280_calib.dig_P9);*/

    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H1, &_bme280_calib.dig_H1, 1);
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H2, &h2, 2);
    _bme280_calib.dig_H2 = (int16_t)h2;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H3, &_bme280_calib.dig_H3, 1);
    uint8_t h6;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H6, &h6, 1);
    _bme280_calib.dig_H6 = (int)h6;
    uint8_t e4, e5, e6;
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H4, &e4, 1);
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H5, &e5, 1);
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_DIG_H5+1, &e6, 1);
    uint16_t h4 = (e4 << 4) | (e5 & 0xF);
    uint16_t h5 = (e6 << 4) | (e5 >> 4);
    _bme280_calib.dig_H4 = h4;//(h4 >> 8) | (h4 << 8);
    _bme280_calib.dig_H5 = h5;//(h5 >> 8) | (h5 << 8);
    ESP_LOGD(TAG, "Calib data from H1: 0x%4x", _bme280_calib.dig_H1);
    ESP_LOGD(TAG, "Calib data from H2: 0x%4x", _bme280_calib.dig_H2);
    ESP_LOGD(TAG, "Calib data from H3: 0x%4x", _bme280_calib.dig_H3);
    ESP_LOGD(TAG, "Calib data from H4: 0x%4x", _bme280_calib.dig_H4);
    ESP_LOGD(TAG, "Calib data from H5: 0x%4x", _bme280_calib.dig_H5);
    ESP_LOGD(TAG, "Calib data from H6: 0x%4x", _bme280_calib.dig_H6);
    return;
}

void bme_set_sampling(bme_sensor_mode mode, bme_sensor_sampling tempSampling,
                      bme_sensor_sampling pressSampling, bme_sensor_sampling humSampling,
                      bme_sensor_filter filter, bme_standby_duration duration) {
    _measReg.mode     = mode;
    _measReg.osrs_t   = tempSampling;
    _measReg.osrs_p   = pressSampling;
    
    _humReg.osrs_h    = humSampling;
    _configReg.filter = filter;
    _configReg.t_sb   = duration;
    
    // you must make sure to also set REGISTER_CONTROL after setting the
    // CONTROLHUMID register, otherwise the values won't be applied (see DS 5.4.3)
    i2c_write_reg(BME280_ADDRESS, BME280_REGISTER_CONTROLHUMID, _humReg.osrs_h);
    i2c_write_reg(BME280_ADDRESS, BME280_REGISTER_CONFIG, (_configReg.t_sb << 5) | (_configReg.filter << 2));
    i2c_write_reg(BME280_ADDRESS, BME280_REGISTER_CONTROL, (_measReg.osrs_t << 5) | (_measReg.osrs_p << 2) | _measReg.mode);
    ESP_LOGD(TAG, "sampling rates updated");
}

void bme_forced_measure() {
    if (_measReg.mode == MODE_FORCED) {
        // set to forced mode, i.e. "take next measurement"
        i2c_write_reg(BME280_ADDRESS, BME280_REGISTER_CONTROL, (_measReg.osrs_t << 5) | (_measReg.osrs_p << 2) | _measReg.mode);
        // wait until measurement has been completed, otherwise we would read
        // the values from the last measurement
            while (1) {
            bool stat = bme_measuring();
            if (stat == false) {break;}
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

float read_temperature(void) {
    int32_t var1, var2;
    uint8_t data[3];
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_TEMPDATA, &data, 3);
    int32_t adc_T = data[0] << 16 | data[1] << 8 | data[2];
/*    uint8_t fa;*/
/*    i2c_read_reg(BME280_ADDRESS, 0xFA, &fa, 1);*/
/*    ESP_LOGD(TAG, "temperature measurement raw msb: 0x%2x", data[0]);*/
/*    uint8_t fb;*/
/*    i2c_read_reg(BME280_ADDRESS, 0xFB, &fb, 1);*/
/*    ESP_LOGD(TAG, "temperature measurement raw lsb: 0x%2x", data[1]);*/
/*    uint8_t fc;*/
/*    i2c_read_reg(BME280_ADDRESS, 0xFC, &fc, 1);*/
/*    ESP_LOGD(TAG, "temperature measurement raw xlsb: 0x%2x", data[2]);*/
/*    ESP_LOGD(TAG, "temperature measurement returned raw: 0x%4x", adc_T);*/
    if (adc_T == 0x800000) // value in case temp measurement was disabled
        return 0;
/*    ESP_LOGD(TAG, "temperature raw          : %d", adc_T);*/
    adc_T >>= 4;
/*    ESP_LOGD(TAG, "temperature right shifted: %d", adc_T);*/

    var1 = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
            ((int32_t)_bme280_calib.dig_T2)) >> 11;
/*    ESP_LOGD(TAG, "temperature var1         : %d", var1);*/
             
    var2 = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
              ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
            ((int32_t)_bme280_calib.dig_T3)) >> 14;
/*    ESP_LOGD(TAG, "temperature var2         : %d", var2);*/

    t_fine = var1 + var2;
/*    ESP_LOGD(TAG, "t_fine                   : %d", t_fine);*/

    float T = (t_fine * 5 + 128) >> 8;
    ESP_LOGD(TAG, "temperature: %.2f C", T/100);
    return T/100;
}

float read_pressure(void) {
    int64_t var1, var2, p;

/*    read_temperature(); // must be done first to get t_fine*/
    
    uint8_t data[3];
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_PRESSUREDATA, &data, 3);
    int32_t adc_P = data[0] << 16 | data[1] << 8 | data[2];
/*    ESP_LOGD(TAG, "pressure measurement returned raw: 0x%4x", adc_P);*/
    if (adc_P == 0x800000) // value in case pressure measurement was disabled
        return 0;
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
           ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);
    ESP_LOGD(TAG, "pressure   : %.2f kPa", (float)p/256000);
    return (float)p/256;
}

float read_humidity(void) {
/*    read_temperature(); // must be done first to get t_fine*/
    
    uint8_t data[2];
    i2c_read_reg(BME280_ADDRESS, BME280_REGISTER_HUMIDDATA, &data, 2);
    int32_t adc_H = data[0] << 8 | data[1];
/*    ESP_LOGD(TAG, "humidity measurement returned raw: 0x%4x", adc_H);*/
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return 0;
        
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
                    (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)_bme280_calib.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float h = (v_x1_u32r>>12);
    ESP_LOGD(TAG, "humidity   : %.2f %% RH", h/1024.0);
    return  h / 1024.0;
}

float read_altitude(void) {
    float atmospheric = read_pressure() / 100.0F;
    float altitude = 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
    ESP_LOGD(TAG, "altitude   : %.2f", altitude);
    return altitude;
}

void set_base_pressure(void) {
    seaLevel = read_pressure() / 100.0F;
}

float sea_level_for_altitude(float altitude, float pressure) {
    return pressure / pow(1.0 - (altitude/44330.0), 5.255);
}
