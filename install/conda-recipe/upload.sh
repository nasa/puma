#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory

anaconda login

array=( 3.5 3.6 3.7 3.8 3.9 )

sha256_code=$(curl -sL https://gitlab.com/jcfergus/puma-dev/-/archive/main/puma-dev-main.tar.gz | openssl sha256)
sha256_code="${sha256_code:9}"
echo "  sha256: "$sha256_code >> meta.yaml

echo "Building conda package ..."
for i in "${array[@]}"
do
	echo "python "$i
	conda-build --python $i .
done

sed -i '' -e '$ d' meta.yaml

echo "Uploading conda package done!"
find $CONDA_PREFIX/conda-bld/ -name *.tar.bz2 | while read file
do
    echo $file
    anaconda upload $file
done
