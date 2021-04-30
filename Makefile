program_name := actor

CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -ansi -O2
linker_flags =
debug_flags = -g -DDEBUG

source_dir := src
include_dir := include
bin_dir := bin
install_dir := /usr/local/bin

source_files := $(wildcard $(addsuffix /*.cpp, $(source_dir) ) )
object_files := $(notdir $(source_files) )
object_files := $(object_files:.cpp=.o)
object_files := $(addprefix $(bin_dir)/, $(object_files))

VPATH := $(source_dir) $(include_dir) $(bin_dir)

all: $(program_name)

install: $(program_name)
	cp $(program_name) $(install_dir)/

$(program_name): $(object_files)
	$(CXX) $^ ${linker_flags} -o $@

$(bin_dir)/%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -I$(include_dir) -I$(source_dir) -o $@

debug: $(source_files)
	$(CXX) $^ $(CXXFLAGS) $(debug_flags) -I$(include_dir) -o $@

clean:
	$(RM) $(bin_dir)/*.o $(program_name) debug tags

uninstall:
	$(RM) $(install_dir)/$(program_name)
tags:
	ctags $(source_dir)/* $(include_dir)/*

.PHONY: all clean install uninstall tags
