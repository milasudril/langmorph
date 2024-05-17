#!/usr/bin/python3
import sys

lines = []
for line in sys.stdin:
	lines.append(line.strip())

for line_a in lines:
	for line_b in lines:
		print(line_a + line_b)
