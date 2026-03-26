CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -I C:/msys64/ucrt64/include
LDFLAGS  := -L C:/msys64/ucrt64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
TARGET   := CarRental.exe

SRC := main.cpp        \
       Date.cpp        \
       Person.cpp      \
       User.cpp        \
       VehicleBase.cpp \
       RentalAgreement.cpp \
       DataStore.cpp \
       App.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	del $(TARGET) 2>nul || true
