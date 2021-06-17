""" PuMApy
Root directory for the PuMApy package.
"""

# primitives
from pumapy.utilities.workspace import Workspace
from pumapy.utilities.logger import Logger, print_warning
from pumapy.utilities.timer import Timer
from pumapy.utilities.isosurface import generate_isosurface
from pumapy.utilities.property_maps import IsotropicConductivityMap, AnisotropicConductivityMap, ElasticityMap
from pumapy.utilities.boundary_conditions import ConductivityBC, ElasticityBC

# input/output
from pumapy.io.input import import_vti, import_3Dtiff, import_bin, import_weave_vtu
from pumapy.io.output import (export_vti, export_3Dtiff, export_bin, export_sparta_implicit_surfaces, export_stl,
                              export_weave_vtu)

# material properties
from pumapy.materialproperties.surfacearea import compute_surface_area
from pumapy.materialproperties.volumefraction import compute_volume_fraction
from pumapy.materialproperties.mean_intercept_length import compute_mean_intercept_length
from pumapy.materialproperties.orientation import compute_orientation_st, compute_angular_differences
from pumapy.materialproperties.conductivity import compute_thermal_conductivity, compute_electrical_conductivity
from pumapy.materialproperties.tortuosity import compute_continuum_tortuosity
from pumapy.materialproperties.elasticity import compute_elasticity, compute_stress_analysis
from pumapy.materialproperties.radiation import compute_radiation, compute_extinction_coefficients
from pumapy.materialproperties.permeability import compute_permeability

# filtering
from pumapy.filters.filters import (filter_median, filter_gaussian, filter_edt, filter_mean,
                                    filter_erode, filter_dilate, filter_opening, filter_closing)

# generation
from pumapy.generation.tpms import generate_tpms
from pumapy.generation.sphere import get_sphere
from pumapy.generation.random_spheres import generate_random_spheres
from pumapy.generation.generate_sphere import generate_sphere
from pumapy.generation.generate_2d_square_array import generate_2d_square_array
from pumapy.generation.random_fibers import generate_random_fibers
try:
    from pumapy.generation.weave_3mdcp.weave_3mdcp import generate_3mdcp
except ImportError:  # import it only if installed
    pass

# visualization
from pumapy.visualization.render import render_volume, render_contour, render_orientation, render_warp
from pumapy.visualization.render_multiphase import render_contour_multiphase
from pumapy.visualization.slicer import plot_slices, compare_slices

# segmentation
from pumapy.segmentation.label_tows import label_tows_fm
from pumapy.segmentation.porespace import identify_porespace, fill_closed_pores

# add wrapped puma cpp functions
import pumapy.utilities.libPuMA as cpp
