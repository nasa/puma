cd "${0%/*}" || exit 1    # Run from this directory
set -e  # exit when any command fails

eval "$(conda shell.bash hook)"  # this is required for conda
conda activate puma

mkdir -p build
mkdir -p out
cd build
mkdir -p out
cmake -D FILENAME=$1 -D CONDA_PREFIX=$CONDA_PREFIX -D CMAKE_INSTALL_PREFIX=. ..
make
make install


../build/$1
