# Variables used to construct compile commands
CC=g++         # Compiler
LIBS=-lm       # Referenced libraries

# Creating object files
wp_test: src/WP_test_driver.o src/WeightProcessor.o
	$(CC) src/WP_test_driver.o src/WeightProcessor.o -o wp_test $(LIBS)

wp_sim: src/WP_simulation_driver.o src/WeightProcessor.o
	$(CC) src/WP_simulation_driver.o src/WeightProcessor.o -o wp_sim $(LIBS)

# Compiling src/WeightProcessor.cpp into object file
WeightProcessor.o: src/WeightProcessor.cpp src/WeightProcessor.h
	$(CC) -c src/WeightProcessor.cpp

# Compiling WP_test_driver.cpp into object file
WP_test_driver.o: src/WP_test_driver.cpp
	$(CC) -c src/WP_test_driver.cpp

# Compiling WP_simulation_driver.cpp into object file
WP_simulation_driver.o: src/WP_simulation_driver.cpp
	$(CC) -c src/WP_simulation_driver.cpp

# Deletes all the object code files and the executable
clean:
	@rm -f src/*.o
	@rm wp_test
	@rm wp_sim

# Moves the relevant binaries and/or libraries to the correct folders   
install:
	@mv wp_test ~/bin
	@mv wp_sim ~/bin