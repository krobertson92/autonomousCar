#ifndef	SONAR_H_
#define	SONAR_H_

#include <semaphore.h>
#include <iostream>
#include <time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
 #include <stdint.h>
 #include <linux/spi/spidev.h>

#include "Sensor.h"
#include "shirtt.h"

#define SONAR	"SONAR"

/**
 * \class Sonar
 * \brief sonar data collection and analysis
 * 
 * Collects data from the sonar and analyzes the data. Based on analysis of 
 * collected data, sends messages (using the system message queue) to the
 * control actuators (Steering and Motor).
 */
class Sonar : public Sensor {
	public:
	
		/**
		 * \brief sonar default constructor
		 * 
		 * sets subsystem parameters and sets up the collect/analysis task sync semaphore.
		 */
		Sonar(ADC_DATA* adc_data_ptr);
		
		/**
		 * \brief grabs data from sonar
		 * 
		 * Gets data from sonar over SPI.  Returns a distance as a float.
		 */
		float data_grab();
		
		/**
		 * \brief Initializes the Sonar subsystem.
		 * 
		 * Sets up the SPI driver (to talk to the ADC) and sets up the ADC for sonar configuration.
		 */
		void init_sensor();
		
		/**
		 * \brief collects data from Sonar
		 * 
		 * The collector function executes in the collector thread and executes indefinitely at a frequency of 20Hz.  
		 * It calls the data_grab to do the actual communication with the hardware to provide a layer of abstraction 
		 * between the software and specifics of the hardware. It can optionally average data prior to analysis if 
		 * the system is expected to be under heavy load and the tasks are not schedulable with the typical release 
		 * frequencies (averaging will have the effect that the analysis task will be released less frequently).
		 */
		void collector();
		
		/**
		 * \brief Performs the analysis of the data collected from the sonar and commands the actuators.
		 * 
		 * The analysis function executes in the analysis thread and is synced with the collector task by the 
		 * collect_analysis_sync semaphore.  Depending on the distance reading from the sonar hardware,
		 * messages will be sent to the Motor and Steering subsystems using the system message queue to change course.
		 */
		void analysis();
		
		/**
		 * \brief Reads in data from message to Sonar.
		 * 
		 * Will read data into appropriate data type and then cast to a void* and return.  The type of 
		 * data received depends on the message command.  For example if trying to set the threashold distance,
		 * a float is expected. This function will read the data in from standard in (using cin) 
		 * into an appropriate datatype for the given command and will return the data to the system message 
		 * handler for further processing.
		 */
		void* read_data(int command);
		
		/**
		 * \brief Handles messages sent to the sonar subsystem.
		 * 
		 * Performs tasks determined by the command part of the message.  For example the 
		 * command SNR_SET_DIST_THR is used to set the distance threshold.  Messages could be 
		 * from command line interface or from another subsystem. 
		 */
		void handle_message(MESSAGE* message);
		
		/**
		 * \brief Resets the compass heading to its original value.
		 * 
		 * Waits an amount of time and then resets the compass heading to its value prior to obstacle
		 * avoidance (value stored in old_compass_heading).
		 */
		void reset_heading();
		
	protected:
	
		void setup_avoidance();
		
		/**
		 * \brief Avoids an obstacle that was detected by the sonar system.
		 * 
		 * turns 90 degrees left in response to an obstacle
		 */
		void avoid_obstacle();
		
		/**
		 * \brief Avoids an obstacle that was detected by the sonar system.
		 * 
		 * Will make the car reverse until it is out of range of the obstacle.
		 */
		void reverse_direction();
		
		/** store the adc data here for cross subsystem access */
		ADC_DATA* adc_data;
		/** The most recent sonar reading */
		float sonar_reading;
		/** Threshold distance for sonar avoidance.  If the sonar reading drops below this value, 
		 * the sonar subsystem will take control of the actuators to turn and avoid and obstacle */
		 float turn_threshold;
		 /** Threshold distance for sonar avoidance.  If the sonar reading drops below this value, 
		 * the sonar subsystem will take control of the actuators and REVERSE to avoid and obstacle */
		 float reverse_threshold;
		 /** SPI request for sonar data from the ADC */
		 struct spi_ioc_transfer msg[1]; 
		/** the colector/analysis sync semaphore used to sync the collector and analysis tasks */
		sem_t collect_analysis_sync;
		/** the obstacle avoidance/reset control semaphore */
		sem_t avoid_reset_control;
		/** The sonar file descriptor (SPI file descriptor) */
		int sonar_fd;
		/** filepath for the SPI device (in /dev) */
		char sonar_filepath[40];
		/** whether avoidance mode has been entered into */
		bool avoidance_mode;
		/** whether reverse mode has been entered into */
		bool reverse_mode;
		/** original compass heading prior to avoidance mode */
		float old_compass_heading;
		/** original motor speed prior to avoidance mode */
		char old_motor_speed[6];
		/** boolean to tell if the last time we reversed, we went left */
		bool last_reverse_left;
		/** message used for requesting data from compass subsystem */
		MESSAGE request_compass_data;
		/** message used for requesting data from motor subsystem*/
		MESSAGE request_motor_data;
		/** message used for changing the motor speed */
		MESSAGE change_speed;
		/** Message used for commanding other subsystems (usually compass)*/
		MESSAGE inter_subsys_command;
		/** message used to set and reset the compass min priority during obstacle avoidance */
		MESSAGE set_cps_prio;
		/** message used to set the motor direction */
		MESSAGE change_direction;
		/** message used to enable or disable a subsystem */
		MESSAGE en_or_dis_subsys;
		/** message used to center steering */
		MESSAGE steer_straight;
		/** reset_heading thread */
		pthread_t treset_heading;
		/** whether to print out data after each collection for debugging/testing */
		bool print_data;
};

#endif
