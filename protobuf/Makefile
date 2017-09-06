LIBDIR=/usr/local/lib

CXXFLAGS=-std=c++14 -g -Wall -O0 -pthread -I /usr/local/include -L $(LIBDIR)
LDFLAGS=-lprotobuf -lz -lpthread -Wl,-rpath=$(LIBDIR)

BINARIES=codec_test descriptor_test dispatcher_lite dispatcher
TARGETS=$(BINARIES)

# comment out following line if you have boost installed
TARGETS=codec_test descriptor_test


all: $(TARGETS)

whole: $(BINARIES)

codec_test: query.pb.h query.pb.cc codec.h codec_test.cc

descriptor_test: query.pb.h query.pb.cc codec.h descriptor_test.cc

dispatcher_lite: query.pb.h query.pb.cc dispatcher_lite.cc

dispatcher: query.pb.h query.pb.cc dispatcher.cc

$(BINARIES):
	g++ $(CXXFLAGS) $(filter %.cc,$^) -o $@ $(LDFLAGS)

query.pb.h query.pb.cc: query.proto
	/usr/local/bin/protoc --cpp_out . $<

test: codec_test
	./codec_test

clean:
	rm -f query.pb.*
	rm -f $(BINARIES)
