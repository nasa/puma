#!/bin/bash
## Change test/CMakeList.txt with the new header and source files.
## Usage: ./createCMakeLists_test.sh

cd "${0%/*}" || exit 1    # Run from this directory
current_dir=$PWD
cd ..

# Initialization
list_cpp="" # list of the source files
list_pub_h="" # list of the public header files

# Source files
i=0
for f in `find test -name *.cpp`
do
     if [ $i -eq 0 ]; then
         list_cpp=$f
     else
         list_cpp=$list_cpp";"$f
     fi
     i=$((i+1))
done

printf "# source files \n" > $current_dir/CMakeLists.txt
printf "SET (SOURCES \n" >> $current_dir/CMakeLists.txt
printf $list_cpp >> $current_dir/CMakeLists.txt
printf "\nPARENT_SCOPE\n)" >> $current_dir/CMakeLists.txt

# Public header files 
i=0
for f in `find test -name *.h`
do
     if [ $i -eq 0 ]; then
	 list_pub_h=$f
     else
	 list_pub_h=$list_pub_h";"$f
     fi
     i=$((i+1))
done

printf "\n\n" >> $current_dir/CMakeLists.txt
printf "# public header files \n" >> $current_dir/CMakeLists.txt
printf "SET (PUB_HEADERS \n" >> $current_dir/CMakeLists.txt
printf $list_pub_h >> $current_dir/CMakeLists.txt
printf "\nPARENT_SCOPE\n)" >> $current_dir/CMakeLists.txt

echo "Creates $current_dir/CMakeLists.txt"
