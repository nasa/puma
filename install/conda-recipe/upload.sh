#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory

# install the following packages before running this script:
# conda install conda-build anaconda-client
conda build purge-all
anaconda login

array=( 3.6 3.7 3.8 )

sha256_code=$(curl -sL https://gitlab.com/jcfergus/puma-dev/-/archive/main/puma-dev-main.tar.gz | openssl sha256)
sha256_code="${sha256_code:9}"
echo "  sha256: "$sha256_code >> meta.yaml

echo "Building conda package ..."
for i in "${array[@]}"
do
	echo "python "$i
	conda-build --python $i .
done

if [ "$(uname)" == "Darwin" ]; then
	sed -i '' -e '$ d' meta.yaml
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	sed -i '$ d' meta.yaml
fi

echo "Uploading conda package done!"
find $CONDA_PREFIX/conda-bld/ -name *.tar.bz2 | while read file
do
    echo $file
    anaconda upload $file --force
done
