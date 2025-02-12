#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "unistd.h"

#define I2C_MASTER_SCL_IO           GPIO_NUM_9
#define I2C_MASTER_SDA_IO           GPIO_NUM_8
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

#define SLAVE_ADDRESS_LCD 0x4E>>1

//esp_err_t err;

#define I2C_NUM I2C_NUM_0

void lcd_send_cmd (char cmd);
void lcd_send_data (char data);
void lcd_clear (void);
void lcd_put_cur(int row, int col);
void lcd_init (void);
void lcd_send_string (char *str);
void lcd_function(void);