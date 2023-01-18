# Target name
TARGET := slideshow

# Build folder path
BUILD_DIR := build

# debug build?
DEBUG := 0
# optimization
OPT := -Og

WARNING := -Wall -Wextra

CFLAGS :=

ifeq ($(DEBUG), 1)
CFLAGS += -g
endif

CFLAGS += $(OPT)

LDFLAGS := -lglfw

CXX_SOURCES :=		\
./src/main.cpp		\
./src/shader.cpp	\
./src/texture.cpp	\
./src/mesh.cpp		\
./src/object.cpp	\
./src/camera.cpp	\
./src/scene.cpp		\
./src/stb_image.cpp	\
./src/image.cpp


C_SOURCES := \
./src/glad.c

INCLUDES := \
-I./include/

DEPENDS := $(addprefix $(BUILD_DIR)/,$(notdir $(patsubst %.c,%.d,$(C_SOURCES)) $(patsubst %.cpp,%.d,$(CXX_SOURCES))))

# list of objects
CXX_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.oxx)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))

C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS := $(CXX_OBJECTS) $(C_OBJECTS)

$(BUILD_DIR)/$(TARGET): $(OBJECTS) makefile
	$(CXX) $(WARNING) $(OBJECTS) $(LDFLAGS) -o $@

-include $(DEPENDS)

$(BUILD_DIR)/%.oxx: %.cpp makefile | $(BUILD_DIR)
	$(CXX) $(WARNING) -c $(CFLAGS) -MMD -MP $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: %.c makefile | $(BUILD_DIR)
	$(CC) $(WARNING) -c $(CFLAGS) -MMD -MP $(INCLUDES) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	-rm -fr $(BUILD_DIR)
.PHONY: clean