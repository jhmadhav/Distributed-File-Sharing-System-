#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

CC = gcc

CFLAGS = -Wall -g

INCLUDES = -I/.

#LFLAGS = -L/home/newhall/lib  -L../lib

#LIBS = -lmylib -lm

SRCS = hemamadh_proj1.c

OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = hemamadh_proj1

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean
#.PHONY: all clean count

all:    $(MAIN)
	

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

#depend: $(SRCS)
		#makedepend $(INCLUDES) $^
