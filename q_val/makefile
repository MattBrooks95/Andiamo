COMP=		gfortran

FFLAGS=		-g 



hf_qvalue:		hf_qvalue.o dmass.o bamt16.o desep.o cqvalue.o \
		dqvalu.o
		$(COMP) $(FFLAGS) -o hf_qvalue hf_qvalue.o dmass.o bamt16.o \
		desep.o cqvalue.o dqvalu.f


hf_qvalue.o:	hf_qvalue.f makefile
		$(COMP) -c $(FFLAGS) hf_qvalue.f

cqvalue.o:		cqvalue.f makefile
		$(COMP) -c $(FFLAGS) cqvalue.f

dqvalu.o:		dqvalu.f makefile
		$(COMP) -c $(FFLAGS) dqvalu.f

dmass.o:		dmass.f makefile
		$(COMP) -c $(FFLAGS) dmass.f

bamt16.o:		bamt16.f makefile
		$(COMP) -c $(FFLAGS) bamt16.f

tofte2.o:	tofte2.f makefile
		$(COMP)  -c $(FFLAGS) tofte2.f

desep.o:		desep.f makefile
		$(COMP) -c $(FFLAGS) desep.f

clean:
	rm hf_qvalue *.o

install:	hf_qvalue
		cp hf_qvalue ~/bin
