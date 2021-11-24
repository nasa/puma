from pumapy.utilities.workspace import Workspace
from pumapy.physicsmodels.boundary_conditions import ConductivityBC
from pumapy.physicsmodels.linear_solvers import PropertySolver
import numpy as np


class Conductivity(PropertySolver):

    def __init__(self, workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type, display_iter):
        allowed_solvers = ['direct', 'gmres', 'cg', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.cond_map = cond_map
        self.direction = direction
        self.side_bc = side_bc
        self.prescribed_bc = prescribed_bc

        self.keff = [-1., -1., -1.]
        self.solve_time = -1
        self.T = np.zeros([1, 1, 1])
        self.q = np.zeros([1, 1, 1, 3])

    def log_input(self):
        self.ws.log.log_section("Computing Conductivity")
        self.ws.log.log_line("Domain Size: " + str(self.ws.get_shape()))
        self.ws.log.log_line("Conductivity Map: ")
        for i in range(self.cond_map.get_size()):
            low, high, cond = self.cond_map.get_material(i)
            self.ws.log.log_line(
                "  - Material " + str(i) + "[" + str(low) + "," + str(high) + "," + str(cond) + "]")
            self.ws.log.log_line("Solver Type: " + str(self.solver_type))
        self.ws.log.log_line("Solver Tolerance: " + str(self.tolerance))
        self.ws.log.log_line("Max Iterations: " + str(self.maxiter))
        self.ws.log.write_log()

    def log_output(self):
        self.ws.log.log_section("Finished Conductivity Calculation")
        self.ws.log.log_line("Conductivity: " + "[" + str(self.keff) + "]")
        self.ws.log.log_line("Solver Time: " + str(self.solve_time))
        self.ws.log.write_log()

    def error_check(self):
        # ws checks
        if not isinstance(self.ws, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
        if self.ws.len_x() < 3 or self.ws.len_y() < 3 or self.ws.len_z() < 3:
            raise Exception("Workspace must be at least 3x3x3 for Conductivity solver.")

        # direction checks
        if self.direction == "x" or self.direction == "X":
            self.direction = "x"
        elif self.direction == "y" or self.direction == "Y":
            self.direction = "y"
        elif self.direction == "z" or self.direction == "Z":
            self.direction = "z"
        else:
            raise Exception("Invalid simulation direction.")

        # side_bc checks
        if self.side_bc == "periodic" or self.side_bc == "Periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc == "symmetric" or self.side_bc == "Symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        elif self.side_bc == "dirichlet" or self.side_bc == "Dirichlet" or self.side_bc == "d":
            self.side_bc = "d"
        else:
            raise Exception("Invalid side boundary conditions.")

        # prescribed_bc checks
        if self.prescribed_bc is not None:
            if not isinstance(self.prescribed_bc, ConductivityBC):
                raise Exception("prescribed_bc must be a puma.ConductivityBC.")
            if self.prescribed_bc.dirichlet.shape != self.ws.matrix.shape:
                raise Exception("prescribed_bc must be of the same size as the domain.")

            # rotate it
            if self.direction == 'y':
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet.transpose((1, 0, 2))
            elif self.direction == 'z':
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet.transpose((2, 1, 0))
            if np.any((self.prescribed_bc.dirichlet[[0, -1]] == np.Inf)):
                raise Exception("prescribed_bc must be defined on the direction sides")
