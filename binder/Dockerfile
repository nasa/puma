ARG BASE_IMAGE_TAG=latest
FROM ghcr.io/pyvista/pyvista:$BASE_IMAGE_TAG

COPY . ${HOME}
WORKDIR ${HOME}
#RUN conda env create --file binder/environment.yml
#RUN conda activate puma_binder
RUN pip install .