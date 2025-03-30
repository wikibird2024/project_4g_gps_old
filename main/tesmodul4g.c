
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

#define UART_NUM UART_NUM_1
#define BUF_SIZE 1024
#define AT_TIMEOUT_MS 3000 // Tăng timeout để đảm bảo nhận phản hồi đầy đủ

static const char *TAG = "EC800K";
static char uart_buffer[BUF_SIZE];

void uart_init() {
  uart_config_t uart_config = {.baud_rate = 115200,
                               .data_bits = UART_DATA_8_BITS,
                               .parity = UART_PARITY_DISABLE,
                               .stop_bits = UART_STOP_BITS_1,
                               .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
  ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
  ESP_ERROR_CHECK(
      uart_set_pin(UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
  uart_flush(UART_NUM);
  vTaskDelay(pdMS_TO_TICKS(200)); // Đợi ổn định UART
}

bool send_at_command(const char *command) {
  memset(uart_buffer, 0, BUF_SIZE);
  uart_flush(UART_NUM);
  ESP_LOGI(TAG, "Sending: %s", command);
  uart_write_bytes(UART_NUM, command, strlen(command));
  uart_write_bytes(UART_NUM, "\r\n", 2);
  int len = uart_read_bytes(UART_NUM, uart_buffer, BUF_SIZE - 1,
                            pdMS_TO_TICKS(AT_TIMEOUT_MS));
  if (len > 0) {
    uart_buffer[len] = '\0';
    ESP_LOGI(TAG, "Response: %s", uart_buffer);
    return true;
  } else {
    ESP_LOGW(TAG, "No response received!");
    return false;
  }
}

void init_module() {
  ESP_LOGI(TAG, "Initializing EC800K...");
  send_at_command("AT");
  send_at_command("ATE0"); // Tắt echo để tránh phản hồi lặp lại
  send_at_command("AT+CPIN?"); // Kiểm tra SIM
}

void get_network_info() {
  send_at_command("AT+COPS?");
  send_at_command("AT+CSQ");
  send_at_command("AT+CREG?");
}

void enable_gps() {
  send_at_command("AT+QGPSCFG=\"autogps\",1"); // Bật tự động GPS
  if (send_at_command("AT+QGPS?")) {
    if (!strstr(uart_buffer, "+QGPS: 1")) {
      ESP_LOGW(TAG, "GPS is NOT enabled! Enabling now...");
      send_at_command("AT+QGPS=1");
    }
  }
}

void check_gps_status() {
  ESP_LOGI(TAG, "Checking GPS status...");
  if (!send_at_command("AT+QGPS?")) {
    ESP_LOGW(TAG, "Failed to check GPS status!");
    return;
  }

  if (!strstr(uart_buffer, "+QGPS: 1")) {
    ESP_LOGW(TAG, "GPS is NOT enabled!");
    return;
  }

  if (!send_at_command("AT+QGPSGNMEA=\"GSV\"") ||
      !strstr(uart_buffer, "$GPGSV")) {
    ESP_LOGW(TAG, "No satellites detected!");
  }
}

void get_gps_location() {
  if (!send_at_command("AT+QGPSLOC?")) {
    ESP_LOGW(TAG, "Failed to get GPS location.");
    return;
  }

  if (strstr(uart_buffer, "CME ERROR: 516")) {
    ESP_LOGW(TAG, "Weak GPS signal or no fix.");
  } else if (strstr(uart_buffer, "+QGPSLOC:")) {
    ESP_LOGI(TAG, "GPS location: %s", uart_buffer);
  }
}

void app_main() {
  esp_log_level_set("*", ESP_LOG_INFO);
  uart_init();
  init_module();
  enable_gps();

  while (1) {
    ESP_LOGI(TAG, "=== NETWORK & GPS UPDATE ===");
    get_network_info();
    check_gps_status();
    get_gps_location();
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
