#ifndef	COMPASS_H_
#define	COMPASS_H_

#include <semaphore.h>
#include <linux/spi/spidev.h>
#include "Sensor.h"
#include <math.h>
#include <stdint.h>
#define COMPASS	"COMPASS"

#define COMPASS_AVERAGE 5
#define COMPASS_AVERAGEA 50

/**
 * \class Compass
 * \brief compass data collection and analysis
 * 
 * Collects data from the compass and analyzes the data. Sends system 
 * messages to control actuators
 */
class Compass : public Sensor {
	public:
	
		class Vector3d{
		public:
			double X;
			double Y;
			double Z;
			Vector3d(){X=Y=Z=0;}
			Vector3d(double x,double y,double z){X=x;Y=y;Z=z;}
			Vector3d cross(Vector3d o){
				Vector3d output;
				output.X=Y*o.Z-Z*o.Y;
				output.Y=Z*o.X-X*o.Z;
				output.Z=X*o.Y-Y*o.X;
				return output;
			}
			Vector3d normalize(){
				double xyzNorm=sqrt(X*X+Y*Y+Z*Z);
				X/=xyzNorm;
				Y/=xyzNorm;
				Z/=xyzNorm;
				return Vector3d(X,Y,Z);
			}
			/** Output lat lon to cout **/
			friend std::ostream& operator<<(std::ostream& os, const Vector3d& dt){
				os << "(" << dt.X << "," << dt.Y << "," << dt.Z << ")";
			}
			/** Sums each of the components (lat lon) repsectivly **/
			Vector3d & operator+=(const Vector3d &other) {
				X+=other.X;
				Y+=other.Y;
				Z+=other.Z;
				return *this;
			}
			/** Devide by any class that can devide a double **/
			template<class T>
			Vector3d & operator/=(const T &other) {
				X/=other;
				Y/=other;
				Z/=other;
				return *this;
			}
		};

		/**
		 * \brief Compass default constructor
		 * 
		 * Sets subsystem parameters and sets up the collect/analysis task sync semaphore.
		 */
		Compass(ADC_DATA* adc_data_ptr);
		
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
		
		/**
		 * \brief collects data from the Compass
		 * 
		 * The collector function executes in the collector thread and executes indefinitely at a frequency of 20Hz.  
		 * It calls the data_grab to do the actual communication with the hardware to provide a layer of abstraction 
		 * between the software and specifics of the hardware. It can optionally average data prior to analysis if 
		 * the system is expected to be under heavy load and the tasks are not schedulable with the typical release 
		 * frequencies (averaging will have the effect that the analysis task will be released less frequently).
		 */
		void collector();
		
		/**
		 * \brief Performs the analysis of the data collected from the compass and commands the actuators.
		 * 
		 * The analysis function executes in the analysis thread and is synced with the collector task by the 
		 * collect_analysis_sync semaphore.  Depending on the difference between the current and desired headings,
		 * messages will be sent to the Steering subsystem using the system message queue to change course.
		 */
		void analysis();
		
		/**
		 * \brief Handles messages sent to the compass subsystem.
		 * 
		 * Performs tasks determined by the command part of the message.  For example the 
		 * command CPS_SET_HEADING is used to set the desired heading.  Messages could be 
		 * from command line interface or from another subsystem. 
		 */
		void handle_message(MESSAGE* message);
		
		/**
		 * \brief Reads in data from message to the compass.
		 * 
		 * Will read data into appropriate data type and then cast to a void* and return.  The type of 
		 * data received depends on the message command.  For example if trying to set the desired
		 * heading, a float is expected. This function will read the data in from standard in (using cin) 
		 * into an appropriate datatype for the given command and will return the data to the system message 
		 * handler for further processing.
		 */
		void* read_data(int command);
		
		/** A message that will, when sent, issue a command to the Steering subsystem to turn hard right */
		MESSAGE hard_right;
		
		/** A message that will, when sent, issue a command to the Steering subsystem to turn slightly right */
		MESSAGE slight_right;
		
		/** A message that will, when sent, issue a command to the Steering subsystem to go straight */
		MESSAGE straight;
		
		/** A message that will, when sent, issue a command to the Steering subsystem to turn hard left */
		MESSAGE hard_left;
		
		/** A message that will, when sent, issue a command to the Steering subsystem to turn slightly left */
		MESSAGE slight_left;
		
	protected:
		/**returns the heading corrected for tilt*/
		//float correct_heading(int Bx, int By, int Bz, int Ax, int Ay, int Az);
	
		/** store the adc data here for cross subsystem access */
		ADC_DATA* adc_data;
		/** SPI request for sonar data from the ADC */
		struct spi_ioc_transfer msg[1]; 
	
		/** The desired heading */
		float desired_heading;
		
		/** The last measured heading. Updated at 20Hz. */
		float meas_heading;
		
		/** The collect/analysis sync semaphore */
		sem_t collect_analysis_sync;
		
		/** The i2c file descriptor for the compass */
		int compass_fd;
		
		/** The i2c file descriptor for the compass */
		int accel_fd;
		
		/** The filename for the compass device. (will be a device in /dev). */
		char compass_filepath[40];
		
		/** The filename for the compass device. (will be a device in /dev). */
		char accel_filepath[40];
		
		/** the minimum subsystem priority that can change the compass heading */
		int min_priority;
		
		int data_grab_count;
		int16_t x;
		int16_t y;
		int16_t z;
		
		int subsys_priorities[NUM_SUBSYSTEMS+1];
		
		Vector3d adxlA[COMPASS_AVERAGEA];
		Vector3d magnA[COMPASS_AVERAGE];
		int compass_avg;
		int compass_avgA;
};

#endif
