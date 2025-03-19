#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "mpu6050.h"

// I2C configuration
#define I2C_MASTER_SCL_IO           22    /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21    /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          100000     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0         /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0         /*!< I2C master doesn't need buffer */

// MPU6050 configuration
#define MPU6050_SENSOR_ADDR         0x68    /*!< Slave address of the MPU6050 sensor */
#define MPU6050_WHO_AM_I_REG_ADDR   0x75    /*!< Register address for WHO_AM_I */

// Function to initialize I2C master
static void i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// Function to initialize MPU6050 sensor
static mpu6050_handle_t mpu6050_init()
{
    mpu6050_handle_t sensor = mpu6050_create(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR);
    mpu6050_wake_up(sensor);
    mpu6050_config(sensor, ACCE_FS_2G, GYRO_FS_250DPS);
    return sensor;
}

// Task to read data from MPU6050 and print to the screen
void mpu6050_task(void *arg)
{
    mpu6050_handle_t sensor = mpu6050_init();
    mpu6050_acce_value_t acce_value;
    mpu6050_gyro_value_t gyro_value;

    while (1) {
        // Read accelerometer data
        mpu6050_get_acce(sensor, &acce_value);
        printf("Accelerometer: X: %.2f, Y: %.2f, Z: %.2f\n", acce_value.acce_x, acce_value.acce_y, acce_value.acce_z);

        // Read gyroscope data
        mpu6050_get_gyro(sensor, &gyro_value);
        printf("Gyroscope: X: %.2f, Y: %.2f, Z: %.2f\n", gyro_value.gyro_x, gyro_value.gyro_y, gyro_value.gyro_z);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // Initialize I2C master
    i2c_master_init();

    // Create task to read data from MPU6050
    xTaskCreate(mpu6050_task, "mpu6050_task", 2048, NULL, 10, NULL);
}
