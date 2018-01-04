# This is the makefile for Project 5.
# @author David Lovato, dalovato
CC = gcc
CFLAGS = -g -Wall -std=c99
un-utf8: codepoints.o
clean:
				rm -f un-utf8 un-utf8.o
				rm -f codepoints codepoints.o
				rm -f output.txt
