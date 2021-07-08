cd "${0%/*}" || exit 1    # Run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"  # this is required for conda

# this env activation only lasts inside bash script
conda activate puma

# Build and Run
mkdir -p build
cd build
cmake -D CONDA_PREFIX=$CONDA_PREFIX ..
make -j
