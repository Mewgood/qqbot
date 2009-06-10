# Compiler
CC = g++
# Linker
LD = g++
# Linker flags
LDFLAGS = -I./ -lpthread
# Compiler flags
CFLAGS = -g -Wall -I./

DIRS = scripts

# Objects
OBJS = \
	main.o \
	heroin/bit_reader.o \
	heroin/bsha1.o \
	heroin/chat_server.o \
	heroin/check_revision.o \
	heroin/client.o \
	heroin/compression.o \
	heroin/container.o \
	heroin/d2cdkey.o \
	heroin/data_manager.o \
	heroin/game.o \
	heroin/game_server.o \
	heroin/item_action.o \
	heroin/item.o \
	heroin/item_data.o \
	heroin/item_string.o \
	heroin/npc.o \
	heroin/realm_server.o \
	heroin/skills.o \
	heroin/utility.o \
	nil/clipboard.o \
	nil/console.o \
	nil/crc32.o \
	nil/debug.o \
	nil/exception.o \
	nil/file.o \
	nil/graphics.o \
	nil/gui.o \
	nil/http.o \
	nil/ini.o \
	nil/math.o \
	nil/memory.o \
	nil/net.o \
	nil/random.o \
	nil/string.o \
	nil/thread.o \
	nil/time.o
LOBJS = main.o \
	bit_reader.o \
	bsha1.o \
	chat_server.o \
	check_revision.o \
	client.o \
	compression.o \
	container.o \
	d2cdkey.o \
	data_manager.o \
	game.o \
	game_server.o \
	item_action.o \
	item.o \
	item_data.o \
	item_string.o \
	npc.o \
	realm_server.o \
	utility.o \
	clipboard.o \
	console.o \
	crc32.o \
	debug.o \
	exception.o \
	file.o \
	graphics.o \
	gui.o \
	http.o \
	ini.o \
	math.o \
	memory.o \
	net.o \
	random.o \
	skills.o \
	string.o \
	thread.o \
	time.o
# Project name
PROG = qqbot
all: $(PROG)
clean:
	rm -f *.o $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(LOBJS) -o $(PROG)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<
