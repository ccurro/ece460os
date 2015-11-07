all: hello helloWithBugs helloNoExit

hello: hello.s
	as hello.s -o hello.o
	ld hello.o -o hello

helloWithBugs: helloWithBugs.s
	as helloWithBugs.s -o helloWithBugs.o
	ld helloWithBugs.o -o helloWithBugs

helloNoExit: helloNoExit.s
	as helloNoExit.s -o helloNoExit.o
	ld helloNoExit.o -o helloNoExit