#include "esmacat_amax.h"

advantech_amax_4817::advantech_amax_4817(){

    output_variable_0_Dummy=0;
    // Reset errors
    error = ADV_NO_ERR;
    // Reset filtered data upon startup
    std::fill(filtered_prev_analog_input_mV, filtered_prev_analog_input_mV+ADVANTECH_AMAX_4817_MAX_INPUT_INDEX, 0.0f);
}

/** @brief Private function- Data exchange process that is specific to the Advantech slave
 *
 * *
 * @param ec_slave_outputs Pointer to the start of the outputs for slave in consideration
 * @param oloop No. of output bytes
 * @param ec_slave_inputs Pointer to the start of the inputs for slave in consideration
 * @param iloop No. of input bytes
 **/
void advantech_amax_4817::ecat_data_process(uint8_t* ec_slave_outputs,int oloop,uint8_t* ec_slave_inputs,int iloop){
    unsigned char input_variable[16];
    unsigned char output_variable[1];
    input_variable[0] = *(ec_slave_inputs+0);
    input_variable[1] = *(ec_slave_inputs+1);
    input_variable[2] = *(ec_slave_inputs+2);
    input_variable[3] = *(ec_slave_inputs+3);
    input_variable[4] = *(ec_slave_inputs+4);
    input_variable[5] = *(ec_slave_inputs+5);
    input_variable[6] = *(ec_slave_inputs+6);
    input_variable[7] = *(ec_slave_inputs+7);
    input_variable[8] = *(ec_slave_inputs+8);
    input_variable[9] = *(ec_slave_inputs+9);
    input_variable[10] = *(ec_slave_inputs+10);
    input_variable[11] = *(ec_slave_inputs+11);
    input_variable[12] = *(ec_slave_inputs+12);
    input_variable[13] = *(ec_slave_inputs+13);
    input_variable[14] = *(ec_slave_inputs+14);
    input_variable[15] = *(ec_slave_inputs+15);
    raw_analog_input_variable[0] = +(input_variable[0] << 0)+(input_variable[1] << 8);
    raw_analog_input_variable[1] = +(input_variable[2] << 0)+(input_variable[3] << 8);
    raw_analog_input_variable[2] = +(input_variable[4] << 0)+(input_variable[5] << 8);
    raw_analog_input_variable[3] = +(input_variable[6] << 0)+(input_variable[7] << 8);
    raw_analog_input_variable[4] = +(input_variable[8] << 0)+(input_variable[9] << 8);
    raw_analog_input_variable[5] = +(input_variable[10] << 0)+(input_variable[11] << 8);
    raw_analog_input_variable[6] = +(input_variable[12] << 0)+(input_variable[13] << 8);
    raw_analog_input_variable[7] = +(input_variable[14] << 0)+(input_variable[15] << 8);
    output_variable[0] = 0;
    output_variable[0] =  +((output_variable_0_Dummy >> 0) & 0x00ff);
    *(ec_slave_outputs+0) = output_variable[0];
}

/** @brief Reads raw analog input value and provides error handling
 * @param channel_index Index of the ADC channel to be read
 * @param error Contains the error message to demonstrate success of data acquisition
 * @return Raw analog input value of the input channel
 */
uint16_t advantech_amax_4817::get_raw_analog_input(int channel_index, advantech_err& error)
{
    uint16_t raw_value = 0;
    if(error == ADV_NO_ERR)
    {
        if (channel_index < 0 || channel_index > ADVANTECH_AMAX_4817_MAX_INPUT_INDEX ){
            error = ADV_ERR_ANALOG_INPUT_CHANNEL_INDEX_OUT_OF_RANGE;
        }
        else{
            raw_value = raw_analog_input_variable[channel_index];
            error =  ADV_NO_ERR;
        }
    }
    return raw_value;
}

/** @brief Reads analog input value in mV and provides error handling
 * @param channel_index Index of the ADC channel to be read
 * @param error Contains the error message to demonstrate success of data acquisition
 * @return Voltage reading (mV) of the input channel
 *
 * Note: Working only for +- 10V range.
 *       Index: 2180:01 Value:0 = +-10V range (2's complement conversion)
 *                      Value:1 = 0-10V range (scaling conversion)
 */
float advantech_amax_4817::get_analog_input_mV(int channel_index, advantech_err& previous_error)
{
    uint16_t raw_analog_input;
    advantech_err error = ADV_NO_ERR;
    float analog_input_mV;

    if(previous_error == ADV_NO_ERR)
    {
        raw_analog_input = get_raw_analog_input(channel_index,error);
        if (error  == ADV_NO_ERR)
        {   // If the raw value is greater than 0x7FFF, it's negative. Convert from 2's complement.
            int16_t signed_value = (raw_analog_input > 0x7FFF) ? -((int16_t)(~raw_analog_input) + 1) : (int16_t)raw_analog_input;
            // Calculate the voltage. The max value (0x7FFF, or 32767) corresponds to +10V,
            // and the min value (0x8000, or -32768) corresponds to -10V.
            analog_input_mV = (signed_value / float(32767)) * 10.0 * 1000;
            previous_error = ADV_NO_ERR;
        }
        else
        {   //if error then set value to 0 and set the error
            analog_input_mV  = 0;
            previous_error = error;
        }
    }
    else
    {
        analog_input_mV = 0;
    }
    return analog_input_mV;
}

/** @brief Reads filtered analog input value in mV and provides error handling
 * @param channel_index Index of the ADC channel to be read
 *        alpha Filter parameter for exponential filtering (0-1)
 * @param error Contains the error message to demonstrate success of data acquisition
 * @return Filtered voltage reading (mV) of the input channel
 */
float advantech_amax_4817::get_filtered_analog_input_mV(int channel_index, float alpha, advantech_err& error)
{
    advantech_err previous_error = ADV_NO_ERR;
    float analog_input_mV,filtered_analog_input_mV;

    if(error == ADV_NO_ERR)
    {
        analog_input_mV = get_analog_input_mV(channel_index,previous_error);
        if (previous_error  == ADV_NO_ERR)
        {
            if(alpha < 0.0f || alpha > 1.0f)
            {
                filtered_analog_input_mV = 0;
                error = ADV_ERR_FILTER_PARAMETER_OUT_OF_RANGE;
            }
            else
            {
                // Exponential filter y(k) = a*y(k-1) + (1-a)*x(k)
                filtered_analog_input_mV = alpha*filtered_prev_analog_input_mV[channel_index] + (1-alpha)*analog_input_mV;
                // Uncomment to remove negative values
                // if(filtered_analog_input_mV < 0.0) filtered_analog_input_mV = 0.0;
            }
        }
        else
        {   //if error then set value to 0 and set the error
            filtered_analog_input_mV  = 0;
            error = previous_error;
        }
    }
    else
    {
        filtered_analog_input_mV  = 0;
    }

    // Update previous reading
    filtered_prev_analog_input_mV[channel_index] = filtered_analog_input_mV;

    return filtered_analog_input_mV;
}

/** @brief Reads the product id of the slave
 * @return Product ID of the slave
 */
uint32_t advantech_amax_4817::get_esmacat_product_id()
{
    return esmacat_slave_product_id;
}

/** @brief Returns the vendor ID of the slave
 * @return Vendor ID of the slave
 */
uint32_t advantech_amax_4817::get_esmacat_vendor_id()
{
    return esmacat_slave_vendor_id;
}

/** @brief Backup function to read analog input value in mV and provides error handling
 * @param channel_index Index of the ADC channel to be read
 * @param error Contains the error message to demonstrate success of data acquisition
 * @return Voltage reading (mV) of the input channel
 *
 * Note: Working only for +- 10V range.
 *       Index: 2180:01 Value:0 = +-10V range (2's complement conversion)
 *                      Value:1 = 0-10V range (scaling conversion)
 */
float advantech_amax_4817::get_analog_input_mV_mask(int channel_index, advantech_err& error)
{
    uint16_t raw_analog_input;
    advantech_err result_of_get_raw_analog_input;
    float analog_input_mV;

    raw_analog_input = get_raw_analog_input(channel_index,result_of_get_raw_analog_input);
    if (result_of_get_raw_analog_input  == ADV_NO_ERR)
    {
        int16_t temp, sign;
        int16_t signed_value;

        sign  = (int16_t)((raw_analog_input & SIGNMASK) >> SIGNBIT);

        if (sign)
        {
            temp = (~raw_analog_input) & DATAMASK;
            signed_value = (short)(temp * -1);
        }
        else
        {
            temp = (raw_analog_input & DATAMASK);
            signed_value = temp;
        }

        // Calculate the voltage. The max value (0x7FFF, or 32767) corresponds to +10V,
        // and the min value (0x8000, or -32768) corresponds to -10V.
        analog_input_mV = (signed_value / float(32767)) * 10.0 * 1000;
        error = ADV_NO_ERR;
    }
    else
    {   //if error then set value to 0 and set the error
        analog_input_mV  = 0;
        error = result_of_get_raw_analog_input;
    }
    return analog_input_mV;
}
