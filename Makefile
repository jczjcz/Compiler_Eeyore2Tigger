NOW_DIR = $(shell pwd)
BUILD_DIR ?= $(NOW_DIR)/build
SOURCE_DIR = $(NOW_DIR)/source
TARGET_EXEC = compiler
TARGET_DIR = $(BUILD_DIR)/$(TARGET_EXEC)



.PHONY: lc
lc:
	lex -o $(BUILD_DIR)/xcw_main.l.yy.cpp $(SOURCE_DIR)/tigger_scanner.l
	yacc -v --defines=$(BUILD_DIR)/xcw_parser.tab.h --output=$(BUILD_DIR)/xcw_parser.tab.c $(SOURCE_DIR)/tigger_parser.y
	g++ -o $(BUILD_DIR)/compiler -I source $(BUILD_DIR)/xcw_main.l.yy.cpp $(BUILD_DIR)/xcw_parser.tab.c