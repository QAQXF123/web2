CXX = g++
CFLAGS = -std=c++14 -O3 -Wall -g 

TARGET = server
OBJS = ../src/log/*.cpp ../src/socket/*.cpp ../src/buffer/*.cpp \
       ../src/http/*.cpp ../src/server/*.cpp \
       ../src/buffer/*.h ../src/main.cpp  ../src/epoll/*.cpp \
	   ../src/database/main/table_manager.cpp \

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../$(TARGET)  -pthread
clean:
	rm -f ../(TARGET)




