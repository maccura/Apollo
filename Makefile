CC = g++
C_FLAGS = -g -Wall -D_DEBUG -std=c++17
C_FLAGS += #-fno-elide-constructors
INCLUDE = #-I/usr/local/include
LIBS = -L/usr/local/lib64
LIBS += -L/usr/local/lib -lpthread -latomic
LIBS += #-L/usr/local/lib -lboost_system -lboost_filesystem -lboost_serialization -lboost_unit_test_framework \
		-lboost_test_exec_monitor -lboost_atomic -lboost_chrono -lboost_thread -latomic -lpthread
LIBS += #-L/usr/local/lib64 -lcaf_core -lcaf_io

# wildcard对cpp文件进行展开
SRCS := $(wildcard *.cpp)

# 将SRCS中后缀.c替换成空, 即可执行程序的名字与cpp名字相同
TARGET  := $(patsubst %.cpp, %, $(SRCS))

OBJDIR = .

.cpp:
	$(CC) $(C_FLAGS) -o $@ $< $(INCLUDE) $(LIBS) 
# 可移动到指定的OBJDIR目录
# mv $@ $(OBJDIR)

all: $(TARGET)

clean:
	@rm -rf *.o $(TARGET)


