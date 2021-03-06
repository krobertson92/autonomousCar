#ifndef	COMPASS_H_
#define	COMPASS_H_

#include <semaphore.h>

/**
 * \class ADC
 * \brief compass data collection and analysis
 * 
 * Collects data from the compass and analyzes the data. Sends system 
 * messages to control actuators
 */
class ADC {
	public:
	
		/**
		 * \brief Compass default constructor
		 * 
		 * Sets subsystem parameters and sets up the collect/analysis task sync semaphore.
		 */
		ADC();
		
		/**
		 * \brief grabs data from compass
		 * 
		 * Gets data from the compass over I2C. Returns the current heading in degrees as a float.
		 */
		float data_grab();
		
		/**
		 * \brief Initializes the Compass.
		 * 
		 * Sets up the I2C driver and sets up the compass hardware to continuously spit out data at 20Hz.
		 */
		void init_sensor();
	protected:
		/** The collect/analysis sync semaphore */
		sem_t collect_analysis_sync;
		
		/** The i2c file descriptor for the compass */
		int spi_fd;
		
		/** The filename for the compass device. (will be a device in /dev). */
		char spi_filepath[40];
};

#endif
