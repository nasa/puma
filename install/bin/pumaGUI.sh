#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

conda activate puma

if [ ! -f "$PuMA_DIR"/gui/build/PuMA_V3_GUI ]; then
	chmod +x ../gui_installer.sh
    ../gui_installer.sh
fi

"$PuMA_DIR"/gui/build/PuMA_V3_GUI
