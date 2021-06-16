from pumapy.materialproperties.volumefraction import compute_volume_fraction
from pumapy import IsotropicConductivityMap
from pumapy.physicsmodels.isotropic_conductivity import IsotropicConductivity


def compute_continuum_tortuosity(workspace, cutoff, direction, side_bc='p', prescribed_bc=None,
                                 tolerance=1e-4, maxiter=10000, solver_type='cg', display_iter=True):
    """ Compute the tortuosity modelling the local conductivity as isotropic

    :param workspace: domain
    :type workspace: Workspace
    :param cutoff: to binarize domain
    :type cutoff: tuple(int, int)
    :param direction: direction for solve ('x','y', or 'z')
    :type direction: string
    :param side_bc: side boundary conditions (string) can be symmetric ('s'), periodic ('p') or dirichlet ('d')
    :type side_bc: string
    :param prescribed_bc: 3D array holding dirichlet BC
    :type prescribed_bc: ConductivityBC
    :param tolerance: tolerance for iterative solver
    :type tolerance: float
    :param maxiter: maximum Iterations for solver
    :type maxiter: int
    :param solver_type: solver type, options: 'cg', 'bicgstab', 'direct'
    :type solver_type: string
    :param display_iter: display iterations and residual
    :type display_iter: bool
    :return: tortuosity, diffusivity, porosity, concentration field
    :rtype: tuple(tuple(float, float, float), float, float, ndarrya)
    """

    cond_map = IsotropicConductivityMap()
    cond_map.add_material(cutoff, 1)

    if cutoff[0] > 0:
        cond_map.add_material((0, cutoff[0]-1),0)
    cond_map.add_material((cutoff[1]+1,32000),0)


    solver = IsotropicConductivity(workspace, cond_map, direction, side_bc, prescribed_bc,
                                   tolerance, maxiter, solver_type, display_iter)

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()

    porosity = compute_volume_fraction(workspace, cutoff)
    eta = [porosity / solver.keff[0], porosity / solver.keff[1], porosity / solver.keff[2]]

    return eta, solver.keff, porosity, solver.T
