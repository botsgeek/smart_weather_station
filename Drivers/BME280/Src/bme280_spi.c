#include <bme280_spi.h>
#include <stdbool.h>
#include <string.h>
#include <common_headers.h>
#define BME_DATA_STARTING_REGISTER 0xF7
#define BME_DATA_SIZE 8
#define BME_TABLE_STARTING_ADDRESS_ONE 0x88
#define BME_TABLE_STARTING_ADDRESS_TWO 0xE1
#define BME_TABLE_ONE_SIZE 25
#define BME_TABLE_TWO_SIZE 7
bme280_t *bme280_object;
static bme280_calib_data table_data;
struct bme280_t{
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
    bool initialized;
    bme280_calib_data calib_data;
    int32_t t_fine; 
};
static error_type_t bme280ReadSPIRegister(bme280_t * bme280_object, uint8_t address, uint8_t* buffer, uint8_t bytes_to_read);
static error_type_t bme280WriteSPIRegister(bme280_t * bme280_object, uint8_t address, uint8_t* buffer, uint8_t bytes_to_write);
static double BME280_compensate_T_double (int32_t adc_T);
static double BME280_compensate_P_double(int32_t adc_P);
static double BME280_compensate_H_double(int32_t adc_H);

bme280_t *bme280Create(const bme280_config_t *config){

    if(!config)
    {
        return NULL;
    }

    bme280_t *bme280_object = (bme280_t *) malloc(sizeof(bme280_t));
    if (!bme280_object){
        return NULL;
    }

    bme280_object->spi_handle = config->spi_handle;
    bme280_object->gpio_port = config->gpio_port;
    bme280_object->gpio_pin = config->gpio_pin;
    bme280_object->initialized = true;

    return bme280_object;
}

error_type_t bme280Init(bme280_t *bme280_object) {
    if (!bme280_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }

    error_type_t result = bme280ReadSPIRegister(bme280_object,BME_TABLE_STARTING_ADDRESS_ONE,&table_data,BME_TABLE_ONE_SIZE);
    if(result != HAL_OK)
    {
        return SYSTEM_FAILED;
    }
   
    uint8_t * table_two_offset = &table_data.dig_H2;
    result = bme280ReadSPIRegister(bme280_object,BME_TABLE_STARTING_ADDRESS_TWO,table_two_offset,BME_TABLE_TWO_SIZE);
    if(result != HAL_OK)
    {
        return SYSTEM_FAILED;
    }


    uint8_t ctrl_measure = 0b10010011;
    result = bme280WriteSPIRegister(bme280_object, BME280_REGISTER_CONTROL, &ctrl_measure, 1);
    if(result != HAL_OK)
    {
        return SYSTEM_FAILED;
    }

    uint8_t ctrl_measure_h = 0X04;
    result = bme280WriteSPIRegister(bme280_object, BME280_REGISTER_CONTROLHUMID, &ctrl_measure_h, 1);
    if(result != HAL_OK)
    {
        return SYSTEM_FAILED;
    }
   
    return SYSTEM_OK;
}

error_type_t bme280ReadData(bme280_t *bme280_object, float *temperature, float *pressure, float *humidity)

{

    if (bme280_object == NULL || temperature == NULL || pressure == NULL || humidity == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    uint8_t buff[BME_DATA_SIZE];
    uint8_t temp_msb, temp_lsb, temp_xlsb,pres_msb,pres_lsb,pres_xlsb,hum_msb,hum_lsb;
    int32_t actual_temp;
    int32_t actual_pres;
    int32_t actual_hum;
    error_type_t result = bme280ReadSPIRegister(bme280_object,BME_DATA_STARTING_REGISTER,buff,sizeof(buff));
    if(result != SYSTEM_OK)
    {
        return result;
    }

    pres_msb = buff[0];
    pres_lsb = buff[1];
    pres_xlsb = buff[2];
    temp_msb = buff[3];
    temp_lsb = buff[4];
    temp_xlsb = buff[5];
    hum_msb = buff[6];
    hum_lsb = buff[7];
   
    printf("temperature is: %ld\n",actual_temp);
    printf("pressure is: %ld\n", actual_pres);
    printf("humidity is: %ld\n", actual_hum);
    actual_temp = temp_msb << 12 | temp_lsb << 4 | (temp_xlsb & 0xF0);
    actual_pres = pres_msb << 12 | pres_lsb << 4 | (pres_xlsb & 0xF0);
    actual_hum = hum_msb << 12 | hum_lsb << 4;
   

    
    *temperature = BME280_compensate_T_double(actual_temp);
    printf("bme data obtained: %f\n",temperature);

   *pressure = BME280_compensate_P_double(actual_pres);
    printf("bme data obtained: %f\n",pressure);

   *humidity = BME280_compensate_H_double(actual_hum);
    printf("bme data obtained: %f\n",humidity);
    return SYSTEM_OK;
}

int32_t t_fine;
static double BME280_compensate_T_double (int32_t adc_T)
{
	double var1, var2, T;
 	var1 = (((double)adc_T) /16384.0 - ((double)table_data.dig_T1) / 1024.0) * ((double)table_data.dig_T2);
    var2 =((((double)adc_T) /131072.0  - ((double)table_data.dig_T1) /8192.0) * 
         (((double)adc_T) /131072.0 - ((double)table_data.dig_T1)/ 8192.0)) * ((double)table_data.dig_T3);
    t_fine = (int32_t) (var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}

static double BME280_compensate_P_double(int32_t adc_P)
{
    double var1, var2, p;
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)table_data.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)table_data.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)table_data.dig_P4) * 65536.0);
    var1 = (((double)table_data.dig_P3) * var1 * var1 / 524288.0 + ((double)table_data.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)table_data.dig_P1);

    if (var1 == 0.0)
    {
        return 0;
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)table_data.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)table_data.dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)table_data.dig_P7)) / 16.0;
    return p;
}

static double BME280_compensate_H_double(int32_t adc_H)
{
	double var_H;
	var_H = (((double)t_fine) - 76800.0);
	var_H = (adc_H - (((double)table_data.dig_H4) * 64.0 + ((double)table_data.dig_H5) / 16384.0 *
     var_H)) *
	(((double)table_data.dig_H2) / 65536.0 * (1.0 + ((double)table_data.dig_H6) / 67108864.0 * var_H *
	(1.0 + ((double)table_data.dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)table_data.dig_H1) * var_H / 524288);

	if(var_H > 100.0)
	{
		var_H = 100.0;
	}
	else if(var_H < 0.0)
	{
		var_H = 0.0;
	}
   
    return var_H;
}


error_type_t bme280DeInit(bme280_t *bme280_object){
    if (bme280_object)
    {
        bme280_object->initialized = false;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}

error_type_t bme280Destroy(bme280_t **bme280_object) {
    if (*bme280_object)
    {
        free(*bme280_object);
        *bme280_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}

error_type_t bme280ReadSPIRegister(bme280_t * bme280_object, uint8_t address, uint8_t* buffer, uint8_t bytes_to_read)
{
   if (!bme280_object || !buffer) {
        return SYSTEM_NULL_PARAMETER;
    }
    address = address | 0x80;
    HAL_GPIO_WritePin(bme280_object->gpio_port, bme280_object->gpio_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef result = HAL_SPI_Transmit(bme280_object->spi_handle, &address, 1,
                                         1000);
    result = HAL_SPI_Receive(bme280_object->spi_handle, buffer, bytes_to_read,
                                         1000);
    HAL_GPIO_WritePin(bme280_object->gpio_port, bme280_object->gpio_pin, GPIO_PIN_SET);
    if(result != HAL_OK){
        return SYSTEM_FAILED;
    }
    return SYSTEM_OK;
}

error_type_t bme280WriteSPIRegister(bme280_t * bme280_object, uint8_t address, uint8_t* buffer, uint8_t bytes_to_write)
{
   if (!bme280_object || !buffer) {
        return SYSTEM_NULL_PARAMETER;
    }
    address = address & 0x7F;
    HAL_GPIO_WritePin(bme280_object->gpio_port, bme280_object->gpio_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef result = HAL_SPI_Transmit(bme280_object->spi_handle, &address, 1,
                                         1000);
    result = HAL_SPI_Transmit(bme280_object->spi_handle, buffer, bytes_to_write,
                                         1000);
    HAL_GPIO_WritePin(bme280_object->gpio_port, bme280_object->gpio_pin, GPIO_PIN_SET);
    if(result != HAL_OK){
        return SYSTEM_FAILED;
    }
    return SYSTEM_OK;
}
