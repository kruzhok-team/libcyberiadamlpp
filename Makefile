LIB_TARGET_STATIC := libcyberiadamlpp.a
LIB_TARGET_DYNAMIC := libcyberiadamlpp.so

ifeq ($(DYNAMIC), 1)
    LIB_TARGET := $(LIB_TARGET_DYNAMIC)
else
    LIB_TARGET := $(LIB_TARGET_STATIC)
endif

TESTS_DIR := tests
MAIN_TARGET := cyberiadapp
LIB_SOURCES := cyberiadamlpp.cpp
MAIN_SOURCES := main.cpp
LIB_OBJECTS := $(patsubst %.cpp, %.o, $(LIB_SOURCES))
MAIN_OBJECTS := $(patsubst %.cpp, %.o, $(MAIN_SOURCES))
TEST_SOURCES := $(wildcard $(addsuffix /*.cpp, $(TESTS_DIR)))
TEST_OBJECTS := $(patsubst %.cpp, %.o, $(TEST_SOURCES))
TEST_TARGETS := $(patsubst %.cpp, %.test, $(TEST_SOURCES))

ifeq ($(DEBUG), 1)
    CFLAGS := -Wall -Wshadow -Wconversion -fPIC -g3 -D__DEBUG__
    LFLAGS := 
else
    CFLAGS := -fPIC
    LFLAGS :=
endif

INCLUDE := -I. -I/usr/include/libxml2 -I./cyberiadaml
LIBS := -L/usr/lib -lxml2 -L./cyberiadaml -lcyberiadaml
MAIN_LIBS := -L. -lcyberiadamlpp

$(LIB_TARGET): $(LIB_OBJECTS)
ifeq ($(DYNAMIC), 1)
	g++ -shared $(LIBS) $(LIB_OBJECTS) -o $@
else
	ar rcs $@ $(LIB_OBJECTS)
endif

$(MAIN_TARGET): $(MAIN_OBJECTS) $(LIB_TARGET) $(LIB_ORJECTS)
	g++ $(MAIN_OBJECTS) -Wl,-\( $(LIBS) $(MAIN_LIBS) -Wl,-\) -o $@

%.test: %.o $(LIB_TARGET)
	g++ $< -Wl,-\( $(LIBS) $(MAIN_LIBS) -Wl,-\) -o $@

%.o: %.cpp
	g++ -c $< $(CFLAGS) $(INCLUDE) -o $@

clean:
	rm -f *~ *.o $(TARGET) $(MAIN_TARGET) $(LIB_TARGET_STATIC) $(LIB_TARGET_DYNAMIC)
	rm -f $(TESTS_DIR)/*~ $(TESTS_DIR)/*.o $(TESTS_DIR)/*.test.graphml $(TESTS_DIR)/*.test.txt $(TEST_TARGETS) 

main: $(MAIN_TARGET)

tests: $(TEST_TARGETS)
	@echo >/dev/null

all: $(LIB_TARGET) $(MAIN_TARGET) $(TEST_TARGETS)

.PHONY: all clean main tests
