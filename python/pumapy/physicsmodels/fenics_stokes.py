import numpy as np
import dolfin as df
import sys
from pumapy.utilities.timer import Timer
from pumapy.utilities.generic_checks import check_ws_cutoff
from pumapy.utilities.logger import print_warning

flags = ["-O3", "-ffast-math", "-march=native"]
# df.parameters["form_compiler"]["quadrature_degree"] = 4
df.parameters["form_compiler"]["representation"] = "uflacs"
df.parameters["form_compiler"]["cpp_optimize"] = True
df.parameters["form_compiler"]["cpp_optimize_flags"] = " ".join(flags)
df.parameters["form_compiler"]["precision"] = 8


class Permeability:

    def __init__(self, workspace, direction, cutoff, side_bc, first_order, inf_sup, pressure_driven,
                 relative_tolerance, absolute_tolerance, maxiter, solver_type, prec_type, display_iter, export_path):
        self.ws = workspace.copy()
        self.direction = direction
        self.cutoff = cutoff
        self.side_bc = side_bc
        self.first_order = first_order
        self.pressure_driven = pressure_driven
        self.solver_type = solver_type
        self.prec_type = prec_type
        self.export_path = export_path
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.inf_sup = inf_sup

        if self.first_order:
            self.voxel_length = 1
        else:
            self.voxel_length = self.ws.voxel_length

        if self.pressure_driven:
            self.p_in = df.Constant(1.)
            self.bf = df.Constant((0, 0, 0))
        else:
            self.p_in = df.Constant(0.)
            self.bf = df.Constant((1, 0, 0))

        self.mesh = df.BoxMesh.create([df.Point(0, 0, 0), df.Point(self.ws.get_shape())],
                                      self.ws.get_shape(), df.CellType.Type.hexahedron)
        self.faces = df.MeshFunction("size_t", self.mesh, self.mesh.topology().dim() - 1)
        self.P = df.FiniteElement("CG", self.mesh.ufl_cell(), 1)
        self.V = df.VectorElement("CG", self.mesh.ufl_cell(), 1 if first_order else 2)
        self.W = None
        self.p = None
        self.u = None
        self.v = None
        self.q = None
        self.w = None
        self.bcs = []
        self.dx = None
        self.ds = None
        self.a = None
        self.L = None
        self.timer = Timer()
        if df.MPI.rank(df.MPI.comm_world) == 0:
            self.timer.start()

        self.keff = None
        self.pressure = None
        self.velocity = None

        prm = df.parameters['krylov_solver']
        prm["monitor_convergence"] = display_iter > 0
        prm["relative_tolerance"] = relative_tolerance
        prm["absolute_tolerance"] = absolute_tolerance
        prm["maximum_iterations"] = maxiter
        prm['nonzero_initial_guess'] = True
        df.set_log_level(display_iter)

    def compute(self):
        self.modify_domain()
        self.mark_domain()
        self.setup_bcs()
        self.construct_variational_form()
        self.solve()
        self.compute_effective_coefficient()

        if df.MPI.rank(df.MPI.comm_world) == 0:
            sys.stdout.write("Solved in {}s\n".format(str(self.timer.elapsed())))
            sys.stdout.flush()

    def modify_domain(self):
        self.ws.binarize_range(self.cutoff)

        if self.direction == "y":
            self.ws.matrix.transpose(1, 0, 2)
        elif self.direction == "z":
            self.ws.matrix.transpose(2, 1, 0)

    def setup_bcs(self):
        if self.side_bc == "p":
            if self.pressure_driven:
                self.W = df.FunctionSpace(self.mesh, self.V * self.P,
                                          constrained_domain=PeriodicBoundaryYZ(self.len_y, self.len_z))
            else:
                self.W = df.FunctionSpace(self.mesh, self.V * self.P,
                                          constrained_domain=PeriodicBoundaryXYZ(self.len_x, self.len_y, self.len_z))
        else:
            self.W = df.FunctionSpace(self.mesh, self.V * self.P)

        # imposing bc on W FunctionSpace, where W.sub(0) is velocity and W.sub(1) is pressure
        if self.side_bc == "fs":
            self.bcs.append(df.DirichletBC(self.W.sub(0).sub(1), df.Constant(0.), self.faces, 3))  # free slip y faces
            self.bcs.append(df.DirichletBC(self.W.sub(0).sub(2), df.Constant(0.), self.faces, 4))  # free slip z faces
        elif self.side_bc == "ns":
            self.bcs.append(df.DirichletBC(self.W.sub(0), df.Constant((0., 0., 0.)), self.faces, 3))  # no slip y faces
            self.bcs.append(df.DirichletBC(self.W.sub(0), df.Constant((0., 0., 0.)), self.faces, 4))  # no slip z faces

        self.bcs.append(df.DirichletBC(self.W.sub(1), df.Constant(0.), self.faces, 5))  # zero pressure inside
        self.bcs.append(df.DirichletBC(self.W.sub(0), df.Constant((0., 0., 0.)), self.faces, 5))  # zero velocity inside
        self.bcs.append(df.DirichletBC(self.W.sub(0), df.Constant((0., 0., 0.)), self.faces, 6))  # no slip on surfaces

    def construct_variational_form(self):
        self.u, self.p = df.TrialFunctions(self.W)
        self.v, self.q = df.TestFunctions(self.W)
        self.dx = df.Measure("dx", domain=self.mesh, subdomain_data=self.faces)  # Volume integration
        self.ds = df.Measure("ds", domain=self.mesh, subdomain_data=self.faces)  # Surface integration
        n = df.FacetNormal(self.mesh)

        self.a = (df.inner(df.grad(self.u), df.grad(self.v)) + self.q * df.div(self.u)) * self.dx
        self.L = - self.p_in * df.inner(n, self.v) * self.ds(1)

        if self.first_order:
            self.a += (- df.div(self.v) * self.p + self.inf_sup * df.inner(df.grad(self.q), df.grad(self.p))) * self.dx
            self.L += df.inner(self.v + self.inf_sup * df.grad(self.q), self.bf) * self.dx
        else:
            self.a += df.div(self.v) * self.p * self.dx  # +ve pressure to make system symmetric
            self.L += df.inner(self.bf, self.v) * self.dx

    def solve(self):
        w = df.Function(self.W)

        if self.first_order:
            solver_parameters = dict()
            if self.solver_type == 'direct':
                solver_parameters['linear_solver'] = 'umfpack'  # LU solver
            else:
                solver_parameters['linear_solver'] = self.solver_type
            if self.prec_type is not None:
                solver_parameters['preconditioner'] = self.prec_type
            df.solve(self.a == self.L, w, self.bcs, solver_parameters=solver_parameters)

        else:  # Taylor Hood elements
            if self.solver_type == "direct":
                df.solve(self.a == self.L, w, self.bcs)  # LU solver
            else:
                A, b = df.assemble_system(self.a, self.L, self.bcs)
                if self.prec_type is None:
                    solver = df.KrylovSolver(A, self.solver_type)
                else:  # construct ad-hoc preconditioner for TH method
                    solver = df.KrylovSolver(A, self.solver_type, self.prec_type)
                    b_prec = df.inner(df.grad(self.u), df.grad(self.v)) * self.dx + self.p * self.q * self.dx
                    P, _ = df.assemble_system(b_prec, self.L, self.bcs)  # assemble P preconditioner matrix
                    solver.set_operators(A, P)
                solver.solve(w.vector(), b)

        self.u, self.p = w.split(deepcopy=True)

        if not self.first_order:  # because we want to be able to use minres for TH
            self.p.vector()[:] *= -1

        x = self.mesh.coordinates()
        x[:, :] *= self.ws.voxel_length
        self.u.vector()[:] *= self.ws.voxel_length ** 2
        if not self.pressure_driven:
            self.p.vector()[:] *= self.ws.voxel_length ** 2

        if self.export_path is not None:
            with df.XDMFFile(self.export_path + '/mesh_facets.xdmf') as f:
                f.write(self.faces)
            df.File(self.export_path + "/velocity.pvd").write(self.u)
            df.File(self.export_path + "/pressure.pvd").write(self.p)

    def compute_effective_coefficient(self):

        self.keff = [df.assemble(self.u[0] * self.dx) * self.len_x,
                     df.assemble(self.u[1] * self.dx) * self.len_x,
                     df.assemble(self.u[2] * self.dx) * self.len_x]

        if self.direction == 'y':
            self.keff = [self.keff[1], self.keff[0], self.keff[2]]
        elif self.direction == 'z':
            self.keff = [self.keff[2], self.keff[1], self.keff[0]]

        # reconstruct pressure and velocity only when not running in parallel with MPI (cannot gather data to one proc)
        if (self.len_x + 1) * (self.len_y + 1) * (self.len_z + 1) == self.p.compute_vertex_values().size:
            self.pressure = self.p.compute_vertex_values().reshape(self.len_z + 1, self.len_y + 1, self.len_x + 1).transpose(2, 1, 0)
            self.velocity = self.u.compute_vertex_values().reshape(3, self.len_z + 1, self.len_y + 1, self.len_x + 1).transpose(3, 2, 1, 0)
            if self.direction == 'y':
                self.pressure = self.pressure.transpose(1, 0, 2)
                self.velocity = self.velocity.transpose(1, 0, 2, 3)[:, :, :, [1, 0, 2]]
            elif self.direction == 'z':
                self.pressure = self.pressure.transpose(2, 1, 0)
                self.velocity = self.velocity.transpose(2, 1, 0, 3)[:, :, :, [2, 1, 0]]

    def error_check(self):
        check_ws_cutoff(self.ws, self.cutoff)

        # solver type
        if not (self.solver_type == "bicgstab" or self.solver_type == "minres" or
                self.solver_type == "gmres" or self.solver_type == "direct"):
            print_warning("Unrecognized solver specified, defaulting to bicgstab.")
            self.solver_type = "bicgstab"
        if self.solver_type == "minres" and self.first_order:
            print_warning("Cannot solve body force with minres, defaulting to bicgstab.")
            self.solver_type = "bicgstab"

        # preconditioner type
        if not (self.prec_type == "amg" or self.prec_type == "sor" or
                self.prec_type == "ilu" or self.prec_type == "icc" or self.prec_type is None):
            print_warning("Unrecognized preconditioner specified, defaulting to amg.")
            self.prec_type = "amg"

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
        elif self.side_bc == "free slip" or self.side_bc == "Free Slip" or self.side_bc == "fs":
            self.side_bc = "fs"
        elif self.side_bc == "no slip" or self.side_bc == "No Slip" or self.side_bc == "ns":
            self.side_bc = "ns"
        else:
            raise Exception("Invalid side boundary conditions.")

    def log_input(self):
        self.ws.log.log_section("Computing Permeability")
        self.ws.log.log_line("Domain Size: " + str(self.ws.get_shape()))
        self.ws.log.write_log()

    def log_output(self):
        self.ws.log.log_section("Finished Permeability Calculation")
        self.ws.log.log_line("Permeability: " + "[" + str(self.keff) + "]")
        self.ws.log.write_log()

    def mark_domain(self):
        # Mesh facets marked as:
        # 0: void
        # 1: inflow x0
        # 2: outflow x1
        # 3: y0 and y1
        # 4: z0 and z1
        # 5: inside solid
        # 6: solid surface
        for f in df.facets(self.mesh):
            mp = f.midpoint()
            if mp[0].is_integer():
                i, j, k = int(mp[0]), int(np.floor(mp[1])), int(np.floor(mp[2]))
                if i == 0 or i == self.len_x:
                    self.faces[f] = 6 if self.ws[max(i - 1, 0), j, k] == 1 else (1 if i == 0 else 2)
                else:
                    if self.ws[i - 1, j, k] == 1 and self.ws[i, j, k] == 1:
                        self.faces[f] = 5
                    elif self.ws[i - 1, j, k] == 1:
                        self.faces[f] = 6
                    elif self.ws[i, j, k] == 1:
                        self.faces[f] = 6
            elif mp[1].is_integer():
                i, j, k = int(np.floor(mp[0])), int(mp[1]), int(np.floor(mp[2]))
                if j == 0 or j == self.len_y:
                    self.faces[f] = 6 if self.ws[i, max(j - 1, 0), k] == 1 else 3
                else:
                    if self.ws[i, j - 1, k] == 1 and self.ws[i, j, k] == 1:
                        self.faces[f] = 5
                    elif self.ws[i, j - 1, k] == 1:
                        self.faces[f] = 6
                    elif self.ws[i, j, k] == 1:
                        self.faces[f] = 6
            else:
                i, j, k = int(np.floor(mp[0])), int(np.floor(mp[1])), int(mp[2])
                if k == 0 or k == self.len_z:
                    self.faces[f] = 6 if self.ws[i, j, max(k - 1, 0)] == 1 else 4
                else:
                    if self.ws[i, j, k - 1] == 1 and self.ws[i, j, k] == 1:
                        self.faces[f] = 5
                    elif self.ws[i, j, k - 1] == 1:
                        self.faces[f] = 6
                    elif self.ws[i, j, k] == 1:
                        self.faces[f] = 6


class PeriodicBoundaryYZ(df.SubDomain):

    def __init__(self, len_y, len_z):
        df.SubDomain.__init__(self)
        self.len_y = len_y
        self.len_z = len_z

    def inside(self, x, on_boundary):
        return (int(x[1]) == 0 and not int(x[2]) == self.len_z) or (int(x[2]) == 0 and not int(x[1]) == self.len_y)

    def map(self, x, y):
        y[0] = x[0]
        y[1] = 0 if int(x[1]) == self.len_y else x[1]
        y[2] = 0 if int(x[2]) == self.len_z else x[2]


class PeriodicBoundaryXYZ(df.SubDomain):

    def __init__(self, len_x, len_y, len_z):
        df.SubDomain.__init__(self)
        self.len_x = len_x
        self.len_y = len_y
        self.len_z = len_z

    def inside(self, x, on_boundary):
        return (int(x[0]) == 0 and not int(x[1]) == self.len_y and not int(x[2]) == self.len_z) or \
               (int(x[1]) == 0 and not int(x[0]) == self.len_x and not int(x[2]) == self.len_z) or \
               (int(x[2]) == 0 and not int(x[0]) == self.len_x and not int(x[1]) == self.len_y)

    def map(self, x, y):
        y[0] = 0 if int(x[0]) == self.len_x else x[0]
        y[1] = 0 if int(x[1]) == self.len_y else x[1]
        y[2] = 0 if int(x[2]) == self.len_z else x[2]
