CC = gcc
CFLAGS = 
DFLAGS = -lm -lpthread

# List all source files in the directory
SRCS = $(wildcard *.c)

# Remove the .c extension to get the executable names
EXECS = $(SRCS:.c=)


all: $(EXECS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(DFLAGS)


clean:
	rm -f $(EXECS) *~

	
# ~ are the emac files	
# $@-target
# $<- first dependency 
# $?- Out-of-Date Dependencies	
#  -f force
	
# target: dependecies
#		action	



