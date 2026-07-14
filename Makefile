CC = clang
CFLAGS = -O2 -I/opt/local/include
LDFLAGS = -L/opt/local/lib -lsndfile

UNAME_S := ($shell uname -s)

ifeq ($(UNAME_S)), Darwin)
	MACOS_MAJOR := $(shell sw_vers -productVersion | cut -d. -f2)
	
	OLD_CLANG := $(shell [ $(MACOS_MINOR) -le 15 ] && echo yes || echo no)
else 
	OLD_CLANG := no
endif

ifeq ($(OLD_CLANG), yes)
	CFLAGS += -arch x86_64
	LDFLAGS += -arch x86_64
else 
	CFLAGS += -arch arm64 -arch x86_64
	LDFLAGS += -arch arm64 -arch x86_64
endif

sfm: sfm.c
	$(CC) $(CFLAGS) sfm.c -o sfm $(LDFLAGS)
