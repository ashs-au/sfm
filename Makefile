CC = gcc
CFLAGS = -O2 -I/opt/local/include -arch arm64 -arch x86_64 -mmacosx-version-min=10.13
LDFLAGS = -L/opt/local/lib -lsndfile -arch arm64 -arch x86_64

extract_markers: sfm.c
	$(CC) $(CFLAGS) sfm.c -o sfm $(LDFLAGS)