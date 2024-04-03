# Variables used to construct compile commands
CC=g++         # Compiler
LIBS=-lm       # Referenced libraries

# Creating object files
wp_test: WP_test_driver.o WeightProcessor.o
	$(CC) WP_test_driver.o WeightProcessor.o -o wp_test $(LIBS)

# Compiling WeightProcessor.cpp into object file
WeightProcessor.o: WeightProcessor.cpp WeightProcessor.h
	$(CC) -c WeightProcessor.cpp

# Compiling WP_test_driver.cpp into object file
WP_test_driver.o: WP_test_driver.cpp
	$(CC) -c WP_test_driver.cpp

# Deletes all the object code files and the executable
clean:
	@rm -f *.o
	@rm wp_test

# Moves the relevant binaries and/or libraries to the correct folders   
install:
	@mv wp_test ~/bin