#-*-makefile-*-

ifndef RNDOBJ
RNDOBJ=obj
endif

RNDSRCS = Uniform.cc\
	Poisson.cc\
	NegExp.cc\
	DiscUnif.cc\
	Normal.cc\
	Binomial.cc\
	MLCG.cc\
	gcd.cc\
	RNG.cc\
	Random.cc

RNDOBJS = $(RNDOBJ)/Uniform.o\
	$(RNDOBJ)/Poisson.o\
	$(RNDOBJ)/NegExp.o\
	$(RNDOBJ)/DiscUnif.o\
	$(RNDOBJ)/Normal.o\
	$(RNDOBJ)/Binomial.o\
	$(RNDOBJ)/RNG.o\
	$(RNDOBJ)/gcd.o\
	$(RNDOBJ)/MLCG.o\
	$(RNDOBJ)/Random.o


all: obj $(RNDOBJ)/libRnd.a

obj:
	mkdir $(RNDOBJ)

# C++ compiler flags (-g -O2 -Wall)
CCCFLAGS = -g

# compiler
CCC = g++

$(RNDOBJ)/libRnd.a: $(RNDOBJS)
	-rm -f $(RNDOBJ)/libRnd.a
	$(AR) cr $(RNDOBJ)/libRnd.a $^
# Rule to make all the TimeWarp .o files
$(RNDOBJ)/%.o : %.cc
	$(CCC) -c \
	$(CCCFLAGS) \
	$< -o $@

depend:
	makedepend -p$(RNDOBJ)/ -- $(CCCFLAGS) -- $(DEPENDINC) $(RNDSRCS) 

cleandep:
	../bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile

clean:
	rm -f *~ \#*\#\
	rm -rf $(RNDOBJ)

# DO NOT DELETE THIS LINE -- make depend depends on it.
