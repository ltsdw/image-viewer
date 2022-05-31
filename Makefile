TARGET=image-viewer
CXX=g++ -std=++17
CXXFLAGS="-Wp,-D_GLIBCXX_ASSERTIONS -MMD"
LD=g++
PKGCONFIG=`pkg-config gtkmm-3.0 --cflags --libs`
BUILD_DIR=build
SRC_DIR=src
OBJS=main.o image-viewer.o

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(LD) -o $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/main.o $(BUILD_DIR)/image-viewer.o $(PKGCONFIG)
	@rm -f $(BUILD_DIR)/{*.o, *.d}

main.o: $(SRC_DIR)/main.cpp
	$(CC) -c $(CXXFLAGS) $(PKGCONFIG) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

image-viewer.o: $(SRC_DIR)/image-viewer.cpp
	$(CC) -c $(CXXFLAGS) $(PKGCONFIG) $(SRC_DIR)/image-viewer.cpp -o $(BUILD_DIR)/image-viewer.o
