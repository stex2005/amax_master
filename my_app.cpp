/** @file
 * @brief This file contains the definition of the functions associated with the user-defined
 * application for the Esmacat slave project */
 /*****************************************************************************************
 * INCLUDES
 ****************************************************************************************/
#include "my_app.h"

/*****************************************************************************************
 * FUNCTIONS
 ****************************************************************************************/
/**
 * @brief Identifies the actual Esmacat slave sequence in the EtherCAT communication chain.
 */
void my_app::assign_slave_sequence(){
    // tell the master what type of slave is at which point in the chain
    assign_esmacat_slave_index(&ecat_amax,0);
}

/**
 * @brief Configure your Esmacat slave.
 * Link Esmacat slave object with the actual Esmacat slave in the EtherCAT communication chain.
 * Functions beginning with 'configure_slave' must only be executed in this function
 */
void my_app::configure_slaves(){


    // add initialization code here
    // Functions starting with "configure_slave" work only in configure_slaves() function
}

/** @brief Initialization that needs to happen on the first iteration of the loop
 */
void my_app::init()
{

    ecat_amax.error = ADV_NO_ERR;
}

/**
 * @brief Executes functions at the defined loop rate
 */
void my_app::loop(){
    // add functions below that are to be executed at the loop rate

    float data = ecat_amax.get_filtered_analog_input_mV(0,0.95,ecat_amax.error);

    if(loop_cnt%1==0)
    {
        cout << fixed << showpoint;
        cout << setprecision(2);
        cout << "\nReading at elapsed time = " << elapsed_time_ms
             << "\t" << ecat_amax.get_raw_analog_input(0,ecat_amax.error)
             << "\t"  << ecat_amax.get_analog_input_mV(0,ecat_amax.error)
             << "\t"  << data
             << "\t Error: "  << ecat_amax.error
             << endl;

    }

    if (loop_cnt > 1000000)
    {

        stop();
    }
}
