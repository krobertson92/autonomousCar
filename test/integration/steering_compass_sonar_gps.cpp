#include <iostream>

#include "../../SUBSYS_COMMANDS.h"

int main() {
	//forward full speed for 5 seconds
	
	//std::cout << "en_subsys " << SUBSYS_GPS << std::endl;
	std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_ENABLE << std::endl;
	std::cout << "en_subsys " << SUBSYS_COMPASS << std::endl;
	std::cout << "en_subsys " << SUBSYS_STEERING << std::endl;
	std::cout << "en_subsys " << SUBSYS_SONAR << std::endl;

	std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_DISPLAY << std::endl;


	for(int i=0;i<10;i++){
		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALAT <<  " 40.00459"  << std::endl;
		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALON << "-105.25573" << std::endl;//25591
		std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_ADDWAYDATARUN << std::endl;

		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALAT <<  " 40.00410"  << std::endl;
		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALON << "-105.25573" << std::endl;
		std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_ADDWAYDATARUN << std::endl;

		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALAT <<  " 40.00459"  << std::endl;
		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALON << "-105.25573" << std::endl;//25591
		std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_ADDWAYDATARUN << std::endl;

		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALAT <<  " 40.00459"  << std::endl;
		std::cout << "subsys 1 " << SUBSYS_GPS << " " << GPS_ADDWAYDATALON << "-105.25630" << std::endl;
		std::cout << "subsys 0 " << SUBSYS_GPS << " " << GPS_ADDWAYDATARUN << std::endl;
	
	}
	
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec += 20000;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	
	//std::cout << "subsys 0 " << SUBSYS_MOTOR << " " << MOT_STOP << std::endl;
	
	std::cout << "exit" << std::endl;
	
}
