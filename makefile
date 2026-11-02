CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -O2
PKG_CONFIG ?= pkg-config
GL_CFLAGS = $(shell $(PKG_CONFIG) --cflags glew glfw3 2>/dev/null)
GL_LIBS = $(shell $(PKG_CONFIG) --libs glew glfw3 2>/dev/null)

ifeq ($(shell uname -s),Darwin)
PLATFORM_LIBS = -framework OpenGL
else
PLATFORM_LIBS = -lGL
endif

TARGET = baikonur
SOURCES = src/main.cpp src/Camera.cpp src/Rocket.cpp src/Shader.cpp src/Mesh.cpp src/Geometry.cpp src/Texture.cpp src/Scene.cpp src/Hud.cpp
OBJECTS = $(SOURCES:src/%.cpp=build/%.o)
TEST_SOURCES = tests/core_tests.cpp src/Camera.cpp src/Rocket.cpp src/Geometry.cpp

.PHONY: all run clean check-gl test
all: $(TARGET)

check-gl:
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { echo "pkg-config is missing; see README.md"; exit 1; }
	@$(PKG_CONFIG) --exists glew glfw3 || { echo "glfw or glew is missing; see README.md"; exit 1; }

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GL_LIBS) $(PLATFORM_LIBS)

build/%.o: src/%.cpp | check-gl
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(GL_CFLAGS) -Iinclude -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

build/core_tests: $(TEST_SOURCES) $(wildcard include/*.h)
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Iinclude $(TEST_SOURCES) -o $@

test: build/core_tests
	./build/core_tests

clean:
	rm -rf build $(TARGET)

-include $(OBJECTS:.o=.d)
