# object files, auto generated from source files
OBJDIR := .o
TSTOBJDIR := $(OBJDIR)/test/dag

# dependency files, auto generated from source files
DEPDIR := .d
TSTDEPDIR := $(DEPDIR)/test/dag

SRCDIR := src
TSTDIR := $(SRCDIR)/test/dag

# dependency object files needed for linker
EXTDEP := dep/HyperLogLog/MurmurHash3.o

CXX = g++
CC = $(CXX)
CXXFLAGS = -Werror -Wall -Wextra -Wconversion \
					 -O3 \
					 -std=c++17 \
					 -g \
					 -Idep/catch2/include\
					 -Idep/HyperLogLog/include\
					 -Idep/disjoint_set/include

LD = g++
# LDLIBS = -static-libstdc++


DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td


COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

LINK.o = $(LD) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: check
check: tests
	./tests

.PHONY: lint
lint:
	cpplint --extensions=tpp,cpp,hpp --recursive .

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR) $(DEPDIR)


dep/HyperLogLog/MurmurHash3.o:
	$(MAKE) -C dep/HyperLogLog/MurmurHash3.o


tests: $(TSTOBJDIR)/tests.o \
			 $(TSTOBJDIR)/edge_tests.o\
			 $(TSTOBJDIR)/implicit_event_graph.o\
			 $(TSTOBJDIR)/networks.o\
			 $(EXTDEPS)
	$(LINK.o)

BUILD_DIRS := $(DEPDIR) $(TSTDEPDIR) $(OBJDIR) $(TSTOBJDIR)
$(OBJDIR):
	$(shell mkdir -p $(OBJDIR))
$(TSTOBJDIR):
	$(shell mkdir -p $(TSTOBJDIR))
$(DEPDIR):
	$(shell mkdir -p $(DEPDIR))
$(TSTDEPDIR):
	$(shell mkdir -p $(TSTDEPDIR))


$(OBJDIR)/%.o : $(SRCDIR)/%.c
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d | $(BUILD_DIRS)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cc
$(OBJDIR)/%.o : $(SRCDIR)/%.cc $(DEPDIR)/%.d | $(BUILD_DIRS)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d | $(BUILD_DIRS)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cxx
$(OBJDIR)/%.o : $(SRCDIR)/%.cxx $(DEPDIR)/%.d | $(BUILD_DIRS)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(DEPDIR)/*.d TSTOBJDIR/*.d)
