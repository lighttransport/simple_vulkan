TARGET := vulkan_test

SRCDIR := ./src
OBJDIR := ./obj
BINDIR := ./bin
INCDIRS := ./include /usr/include /usr/include/c++/v1 /usr/local/include
DEFINES := 
LDFLAGS := -lvulkan -lc++ -lGLEW -lglfw3 -lGLU -lGL -lX11 -lXrandr -lXi -lXxf86vm -lpthread -lXcursor -lXinerama -ldl -llayer_utils -lVkLayer_device_limits -lVkLayer_draw_state -lVkLayer_image -lVkLayer_mem_tracker -lVkLayer_object_tracker -lVkLayer_param_checker -lVkLayer_swapchain -lVkLayer_threading -lVkLayer_unique_objects



DEBUG := 1
COMMONFLAGS := -w -std=c++14 
COMPILER := clang++

SOURCES = $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o))) 
DEPENDS = $(OBJECTS:.o=.d)

FLAGS := $(COMMONFLAGS)
FLAGS := $(FLAGS) $(addprefix -I,$(INCDIRS))
FLAGS := $(FLAGS) $(addprefix -D,$(DEFINES))
FLAGS := $(FLAGS) $(LDFLAGS)
ifdef DEBUG
	FLAGS := $(FLAGS) -g
endif

.SUFFIXES:

$(BINDIR)/$(TARGET) : $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo Linking $@
	$(COMPILER) -o $@ $^ $(LDFLAGS)
	#$(BINDIR)/$(TARGET)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@echo Compiling $<
	$(COMPILER) -o $@ -c $< -MMD -MP $(FLAGS)

-include $(DEPENDS)

.PHONY: clean all directory

all: clean $(TARGET)

clean:
	@echo Cleaning
	@rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

run:
	$(BINDIR)/$(TARGET)
