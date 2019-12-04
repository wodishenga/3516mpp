
# target source
OBJS  := $(SMP_SRCS:%.c=%.o)

CFLAGS += $(COMM_INC)

MPI_LIBS += $(REL_LIB)/libhdmi.a
MPI_LIBS += /usr/local/lib/libpaho-mqtt3c.so.1
CFLAGS += -I/root/paho.mqtt.c/src

MPI_LIBS += /usr/local/lib/libfreetype.so.6
CFLAGS += -I/usr/local/include/freetype2/freetype

MPI_LIBS += /usr/local/lib/libSDL-1.2.so.0
CFLAGS += -I/usr/local/include/SDL

MPI_LIBS += /usr/local/lib/libSDL_ttf-2.0.so.0
CFLAGS += -I/usr/local/include/
.PHONY : clean all

all: $(TARGET)

$(TARGET):$(COMM_OBJ) $(OBJS)
	@$(CC) $(CFLAGS) -lpthread -lm -o $(TARGET_PATH)/$@ $^ -Wl,--start-group $(MPI_LIBS) $(SENSOR_LIBS) $(AUDIO_LIBA) $(REL_LIB)/libsecurec.a -Wl,--end-group

clean:
	@rm -f $(TARGET_PATH)/$(TARGET)
	@rm -f $(OBJS)
	@rm -f $(COMM_OBJ)

cleanstream:
	@rm -f *.h264
	@rm -f *.h265
	@rm -f *.jpg
	@rm -f *.mjp
	@rm -f *.mp4
