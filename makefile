CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)

# message.o
main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o
	
tstCodeController.o: test/tstCodeController.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/tstCodeController.cpp -o build/tstCodeController.o
tstAlarmGenerator.o: test/tstAlarmGenerator.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/tstAlarmGenerator.cpp -o build/tstAlarmGenerator.o	
tstCounterr.o: test/tstCounterr.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/tstCounterr.cpp -o build/tstCounterr.o
tstDoorTimerr.o: test/tstDoorTimerr.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/tstDoorTimerr.cpp -o build/tstDoorTimerr.o
tstIndoorLightening.o: test/tstIndoorLightening.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/tstIndoorLightening.cpp -o build/tstIndoorLightening.o
tstOutdoorLightening.o: test/tstOutdoorLightening.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/tstOutdoorLightening.cpp -o build/tstOutdoorLightening.o



tests: tstCodeController.o tstAlarmGenerator.o tstCounterr.o tstDoorTimerr.o tstIndoorLightening.o tstOutdoorLightening.o
		$(CC) -g -o bin/CodeController_TEST build/tstCodeController.o 
		$(CC) -g -o bin/Counterr_TEST build/tstCounterr.o 
		$(CC) -g -o bin/AlarmGenerator_TEST build/tstAlarmGenerator.o 
		$(CC) -g -o bin/DoorTimerr_TEST build/tstDoorTimerr.o 
		$(CC) -g -o bin/IndoorLightening_TEST build/tstIndoorLightening.o 
		$(CC) -g -o bin/OutdoorLightening_TEST build/tstOutdoorLightening.o 

#TARGET TO COMPILE ONLY HOUSETOP SIMULATOR
simulator: main_top.o
	$(CC) -g -o bin/GARAGETOP build/main_top.o

	
#TARGET TO COMPILE EVERYTHING (HOUSETOP SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*
	
#joshil
