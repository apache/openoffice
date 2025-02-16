#!/bin/sh

# Seems to print out warning messages to the console before finishing successfully
# Example
# WARNING: Cannot decode file using encoding "utf-8": ./main/writerfilter/qa/documents/multimerge2.docx
# WARNING: Trying next encoding "iso-8859-1"

codespell --skip='./extras' . | cut -f2 -d' ' | tr A-Z a-z | sort | uniq > .github/linters/codespell.txt
