CC  = gcc
CXX = g++ -std=c++11 -fno-elide-constructors

CXXFLAGS = -std=c++11 -I inc -Wall -Wextra
#DBGFLAGS = -D DEBUG -fsanitize=address -ggdb3

TARGET = app
SRCDIR = src/
OBJDIR = obj/
INCDIR = include/

SOURCES = main.cpp $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(foreach SRC, $(SOURCES), $(basename $(OBJDIR)$(notdir $(SRC))).o)
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS))

# .PHONY means these rules get executed even if
# files of those names exist.

all: build #dirs

debug: CXXFLAGS += -D DEBUG -fsanitize=address -ggdb3
debug: clean build

build: $(TARGET)

.PHONY: clean dirs

dirs:
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(SRCDIR)
	@ mkdir -p $(INCDIR)

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEPENDS)

$(OBJDIR)main.o: main.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@


#define BuildRule =
#$(OBJDIR)$(basename $(notdir $(1))).o: $(1)
#	echo $@
#	$(CXX) $< -c -o $@ $(CXXFLAGS)
#endef

#-include depends

#$(eval $(call BuildRule,main))

#$(foreach SRC, $(SOURCES), $(eval $(call BuildRule, $(SRC))))