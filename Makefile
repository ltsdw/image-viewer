TARGET=image-viewer
INSTALL_DIR=/usr/bin
CXX=g++ -std=++17
CXXFLAGS="-Wp,-D_GLIBCXX_ASSERTIONS -MMD -lstdc++fs"
LD=g++
PKGCONFIG=`pkg-config gtkmm-3.0 --cflags --libs`
BUILD_DIR=build
SRC_DIR=src
OBJS=main.o image-viewer.o

$(TARGET): $(OBJS)
	$(LD) -o $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/main.o $(BUILD_DIR)/image-viewer.o $(PKGCONFIG)
	@rm -f $(BUILD_DIR)/{*.o, *.d}

main.o: $(SRC_DIR)/main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(PKGCONFIG) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o $(CXXFLAGS)

image-viewer.o: $(SRC_DIR)/image-viewer.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(PKGCONFIG) $(SRC_DIR)/image-viewer.cpp -o $(BUILD_DIR)/image-viewer.o $(CXXFLAGS)

install:
	cp $(BUILD_DIR)/$(TARGET) $(INSTALL_DIR)
	chmod o+rx $(INSTALL_DIR)/$(TARGET)

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET)

