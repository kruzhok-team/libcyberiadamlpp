LIB_TARGET_STATIC := libcyberiadamlpp.a
LIB_TARGET_DYNAMIC := libcyberiadamlpp.so

ifeq ($(DYNAMIC), 1)
    LIB_TARGET := $(LIB_TARGET_DYNAMIC)
else
    LIB_TARGET := $(LIB_TARGET_STATIC)
endif

TEST_TARGET := cyberiadapp_test
LIB_SOURCES := cyberiadamlpp.cpp
TEST_SOURCES := test.cpp
LIB_OBJECTS := $(patsubst %.cpp, %.o, $(LIB_SOURCES))
TEST_OBJECTS := $(patsubst %.cpp, %.o, $(TEST_SOURCES))

ifeq ($(DEBUG), 1)
    CFLAGS := -Wall -Wshadow -Wconversion -fPIC -g3 -D__DEBUG__
    LFLAGS := 
else
    CFLAGS := -fPIC
    LFLAGS :=
endif

INCLUDE := -I. -I/usr/include/libxml2 -I./cyberiadaml
LIBS := -L/usr/lib -lxml2 -L./cyberiadaml -lcyberiadaml
TEST_LIBS := -L. -lcyberiadamlpp

$(LIB_TARGET): $(LIB_OBJECTS)
ifeq ($(DYNAMIC), 1)
	g++ -shared $(LIBS) $(LIB_OBJECTS) -o $@
else
	ar rcs $@ $(LIB_OBJECTS)
endif

$(TEST_TARGET): $(TEST_OBJECTS) $(LIB_TARGET) $(LIB_ORJECTS)
	g++ $(TEST_OBJECTS) -Wl,-\( $(LIBS) $(TEST_LIBS) -Wl,-\) -o $@

%.o: %.cpp
	g++ -c $< $(CFLAGS) $(INCLUDE) -o $@

clean:
	rm -f *~ *.o $(TARGET) $(TEST_TARGET) $(LIB_TARGET_STATIC) $(LIB_TARGET_DYNAMIC)

test: $(TEST_TARGET)

all: $(LIB_TARGET) $(TEST_TARGET)

.PHONY: all clean test
