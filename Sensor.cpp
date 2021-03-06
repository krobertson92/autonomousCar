#include <iostream>

#include "Sensor.h"
#include "shirtt.h"

#define SHUTDOWN_DEBUG


//Task Spawning ( called from Sensor::init() )
static void* collector_task(void* c) {
	setup_rt_task(10);
	Sensor* s = (Sensor*)c;
	s->collector();
}
static void* analysis_task(void* c) {
	setup_rt_task(11);
	Sensor* s = (Sensor*)c;
	s->analysis();
}

void Sensor::init(){
	init_sensor();
	if(pthread_create( &tCollector, NULL, &collector_task, (void *)(this)) != 0) {
		perror("Error creating collector thread for sensor! ");
	}
	if(pthread_create( &tAnalysis, NULL, &analysis_task, (void *)(this)) != 0) {
		perror("Error creating analysis thread for sensor! ");
	}
}

void Sensor::shutdown() {
	#ifdef SHUTDOWN_DEBUG
		std::cout << "Sensor Shutdown" << std::endl;
	#endif
	pthread_cancel(tCollector);
	pthread_cancel(tAnalysis);
}

Sensor::~Sensor(){
	#ifdef SHUTDOWN_DEBUG
		std::cout << "Sensor Destructor" << std::endl;
	#endif
}
