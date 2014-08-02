CC=	g++
OPT=	-std=c++11
LIBS=	-lm -lpthread
SRCS_NAIVE=	Naive_main.cpp Naive_chunk.cpp Naive_parser.cpp List.cpp Naive_basic.cpp
OBJS_NAIVE=	Naive_main.o Naive_chunk.o Naive_parser.o List.o Naive_basic.o
SRCS_VERIFY=	Verify_main.cpp Verify_chunk.cpp Verify_block.cpp Verify_lca.cpp Parser.cpp List.cpp MRCA_hash.cpp Prepare.cpp RMQmode.cpp Basic.cpp
OBJS_VERIFY=	Verify_main.o Verify_chunk.o Verify_block.o Verify_lca.o Parser.o List.o MRCA_hash.o Prepare.o RMQmode.o Basic.o
SRCS_BINARY=	Prepare.cpp Binary_main.cpp Binary_chunk.cpp Binary_block.cpp Binary_lca.cpp RMQmode.cpp MRCA_hash.cpp Parser.cpp List.cpp Basic.cpp
OBJS_BINARY=	Prepare.o Binary_main.o Binary_chunk.o Binary_block.o Binary_lca.o RMQmode.o MRCA_hash.o Parser.o List.o Basic.o

EXECUTABLE_NAIVE=	IBDdetection_naive
EXECUTABLE_VERIFY=	IBDdetection_verify
EXECUTABLE_BINARY=	IBDdetection_binary

# this is only a reminder if the user want to "make all"
all:
	@echo Please choose from \"make naive\", \"make binary\" and \"make verify\" to compile and link...


# this is the n^2 naive method (changed-pair detection) for IBD extraction
naive: main_naive clean mv_naive

$(OBJS_NAIVE): $(SRCS_NAIVE)
	$(CC) -c $*.cpp $(OPT)

main_naive: $(OBJS_NAIVE)
	$(CC) -o $(EXECUTABLE_NAIVE) $(OBJS_NAIVE) $(OPT) $(LIBS)

# this is the binary search method
binary: main_binary clean mv_binary

$(OBJS_BINARY): $(SRCS_BINARY)
	$(CC) -c $*.cpp $(OPT)

main_binary: $(OBJS_BINARY)
	$(CC) -o $(EXECUTABLE_BINARY) $(OBJS_BINARY) $(OPT) $(LIBS)

# this is the verify all trees verification algorithm
verify: main_verify clean mv_verify

$(OBJS_VERIFY): $(SRCS_VERIFY)
	$(CC) -c $*.cpp $(OPT)

main_verify: $(OBJS_VERIFY)
	$(CC) -o $(EXECUTABLE_VERIFY) $(OBJS_VERIFY) $(OPT) $(LIBS)


clean:
	-rm -f *.o

mv_naive:
	@chmod 755 $(EXECUTABLE_NAIVE)
	@mv $(EXECUTABLE_NAIVE) ../IBD_C_upgrade_test/
#	-@../IBD_C_upgrade_test/$(EXECUTABLE_NAIVE)

mv_verify:
	@chmod 755 $(EXECUTABLE_VERIFY)
	@mv $(EXECUTABLE_VERIFY) ../IBD_C_upgrade_test/
#	-@../IBD_C_upgrade_test/$(EXECUTABLE_VERIFY)

mv_binary:
	@chmod 755 $(EXECUTABLE_BINARY)
	@mv $(EXECUTABLE_BINARY) ../IBD_C_upgrade_test/
#	-@../IBD_C_upgrade_test/$(EXECUTABLE_BINARY)
