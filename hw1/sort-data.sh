#!/bin/bash

ext=".txt"
str="_sorted"
sort -k 1 -n "$1$ext" > "$1$str".txt
