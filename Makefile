CC = g++
FLAGS = -std=c++11 -Wall
TARGET = get_forecast_info
OBJS = main.o config_info.o forecast_xml_parser.o local_time.o tinyxml2.o forecast_xml_getter.o crypto.o CSBS_Packet.o
SRCDIR = Sources

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) -lcurl -lconfig++ -lcrypto -lssl
	rm -f *.o

main.o : $(SRCDIR)/main.cpp
	$(CC) -c $< $(FLAGS) -lcurl -lconfig++ -lpthread

config_info.o : $(SRCDIR)/config_info.cpp
	$(CC) -c $< $(FLAGS) -lconfig++

forecast_xml_parser.o : $(SRCDIR)/forecast_xml_parser.cpp
	$(CC) -c $< $(FLAGS)

local_time.o : $(SRCDIR)/local_time.cpp
	$(CC) -c $< $(FLAGS)

tinyxml2.o : $(SRCDIR)/tinyxml2.cpp
	$(CC) -c $< $(FLAGS)

forecast_xml_getter.o : $(SRCDIR)/forecast_xml_getter.cpp
	$(CC) -c $< $(FLAGS) -lcurl

crypto.o : $(SRCDIR)/rypto.cpp
	$(CC) -c $< -std=c++11  -lcrypto -lssl

CSBS_Packet.o :$(SRCDIR)/CSBS_Packet.cpp
	$(CC) -c $< -std=c++11

clean :
	rm -f $(OBJS) $(TARGET) $(wildcard XML/*.xml)
	
	
