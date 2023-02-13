# Target name
TARGET := slideshow

# Build folder path
BUILD_DIR := build

# Debug build?
DEBUG := 0
# Optimization
OPT := -Og

WARNING := -Wall -Wextra

CFLAGS :=

ifeq ($(DEBUG), 1)
CFLAGS += -g
endif

CFLAGS += $(OPT)

LDFLAGS := -lglfw -fopenmp

CXX_SOURCES :=			\
./src/main.cpp			\
./src/stb_image.cpp		\
./src/image.cpp			\
./src/image_renderer.cpp\
./src/file_getter.cpp	\
./src/effect.cpp

C_SOURCES := \
./src/glad.c

INCLUDES := 				\
-I./include/				\
-I./include/third_party/

# Generic from here

DEPENDS := $(addprefix $(BUILD_DIR)/,$(notdir $(patsubst %.c,%.d,$(C_SOURCES)) $(patsubst %.cpp,%.d,$(CXX_SOURCES))))

# list of objects
CXX_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(addsuffix .o, $(CXX_SOURCES))))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))

C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(addsuffix .o, $(C_SOURCES))))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS := $(CXX_OBJECTS) $(C_OBJECTS)

$(BUILD_DIR)/$(TARGET): $(OBJECTS) makefile
	$(CXX) $(WARNING) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@

-include $(DEPENDS)

$(BUILD_DIR)/%.cpp.o: %.cpp makefile | $(BUILD_DIR)
	$(CXX) $(WARNING) -c $(CFLAGS) $(LDFLAGS) -MMD -MP $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.c.o: %.c makefile | $(BUILD_DIR)
	$(CC) $(WARNING) -c $(CFLAGS) $(LDFLAGS) -MMD -MP $(INCLUDES) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	-rm -fr $(BUILD_DIR)
.PHONY: clean