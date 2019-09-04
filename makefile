# object files, auto generated from source files
OBJDIR := .o
$(shell mkdir -p $(OBJDIR) >/dev/null)
TSTOBJDIR := $(OBJDIR)/test/dag
$(shell mkdir -p $(TSTOBJDIR) >/dev/null)

# dependency files, auto generated from source files
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
TSTDEPDIR := $(DEPDIR)/test/dag
$(shell mkdir -p $(TSTDEPDIR) >/dev/null)

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

all: tests


dep/HyperLogLog/MurmurHash3.o:
	$(MAKE) -C dep/HyperLogLog/MurmurHash3.o


tests: $(TSTOBJDIR)/tests.o $(TSTOBJDIR)/edge_tests.o $(EXTDEPS)
	$(LINK.o)


$(OBJDIR)/%.o : $(SRCDIR)/%.c
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cc
$(OBJDIR)/%.o : $(SRCDIR)/%.cc $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cxx
$(OBJDIR)/%.o : $(SRCDIR)/%.cxx $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(DEPDIR)/*.d TSTOBJDIR/*.d)
