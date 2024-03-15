#ifndef __DS3231_H___
#define __DS3231_H___

I2C_HandleTypeDef hi2c_DS3231;

	typedef struct
	{


	}ds3231_config_t;

	typedef struct
	{

	}ds3231_time_t;

	 error_type_t ds3231Init(ds3231_t *ds3231_object);
	 error_type_t ds3231Read()

#endif
