#ifndef ESMACAT_AMAX_4817_H
#define ESMACAT_AMAX_4817_H

/* Take 15 bits of the data (last bit is the sign) */
#define DATAMASK 0x7FFF

/* Sign is bit 15 (starting from zero) with the 15 bit data */
#define SIGNMASK 0x8000
#define SIGNBIT  15

/*****************************************************************************************
 * MACROS
 ****************************************************************************************/

/** @brief Unique vendor ID assigned by the EtherCAT Technology Group to Advantech*/
#define	ADVANTECH_AMAX_4817_VENDOR_ID	0x000013fe

/** @brief Unique product code for Advantech AMAX-4817 required for EtherCAT */
#define	ADVANTECH_AMAX_4817_PRODUCT_ID	0x00004817

/** @brief Total channels on the Advantech AMAX-4817 */
#define ADVANTECH_AMAX_4817_MAX_INPUT_INDEX 8

/*****************************************************************************************
 * ENUMERATIONS
 ****************************************************************************************/
enum advantech_err{

    //No error
    ADV_NO_ERR = 0,
    // Analog input error list
    ADV_ERR_ANALOG_INPUT_CHANNEL_INDEX_OUT_OF_RANGE = 1001,
    // Filter parameter out of range
    ADV_ERR_FILTER_PARAMETER_OUT_OF_RANGE = 2001,
};

/*****************************************************************************************
 * INCLUDES
 ****************************************************************************************/

#include "slave.h"

/*****************************************************************************************
 * CLASSES
 ****************************************************************************************/

/** Generic class that allows for access of information from the Advantech AMAX-4817 slave*/
class advantech_amax_4817: public esmacat_slave{

private:

    /** Contains the dummy output value */
    uint8_t	output_variable_0_Dummy;
    /** Contains the raw analog input values */
    uint16_t raw_analog_input_variable[ADVANTECH_AMAX_4817_MAX_INPUT_INDEX];
    /** Contains the previous filtered analog input values */
    float filtered_prev_analog_input_mV[ADVANTECH_AMAX_4817_MAX_INPUT_INDEX];

public:
    /** Contains a product code of the Advantech slave */
    uint32_t esmacat_slave_product_id = ADVANTECH_AMAX_4817_PRODUCT_ID;
    /** Contains a vendor ID of the Advantech slave */
    uint32_t esmacat_slave_vendor_id = ADVANTECH_AMAX_4817_VENDOR_ID;
    /** Containts error code */
    advantech_err error;

    advantech_amax_4817();

    uint32_t get_esmacat_product_id();
    uint32_t get_esmacat_vendor_id();
    void ecat_data_process(uint8_t* ec_slave_outputs,int oloop,uint8_t* ec_slave_inputs,int iloop);

    /** Read the raw analog input value from the ecat packet */
    uint16_t get_raw_analog_input(int channel_index, advantech_err& error);
    /** Converts raw analog input to analog input in mV (2's complement conversion) */
    float get_analog_input_mV(int channel_index, advantech_err& error);
    /** Reads the filtered analog input values */
    float get_filtered_analog_input_mV(int channel_index, float alpha, advantech_err& error);
    /** Writes the dummy output value */
    void set_output_variable_0_Dummy(uint8_t value);
    /** Backup function */
    float get_analog_input_mV_mask(int channel_index, advantech_err& error);

}; 

#endif
