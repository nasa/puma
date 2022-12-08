class MaterialPropertyMap:
    def __init__(self):
        self._lowCutoffs = []
        self._highCutoffs = []
        self._properties = []

    def error_check(self, cutoff, conductivity):
        if not isinstance(cutoff, tuple) or len(cutoff) != 2:
            raise Exception("Cutoff has to be a tuple(int,int) specifying the low and high cutoffs")

        for i in range(len(self._lowCutoffs)):
            if (self._highCutoffs[i] >= cutoff[0] >= self._lowCutoffs[i]) \
                    or (self._highCutoffs[i] >= cutoff[1] >= self._lowCutoffs[i]):
                raise Exception("Invalid material range. The range overlaps an existing material range")

        check = False
        if isinstance(conductivity, tuple):
            if any(i < 0 for i in conductivity):
                check = True
        else:
            if conductivity < 0:
                check = True
        if check:
            raise Exception("Invalid conductivity. Must be positive")
        return conductivity

    def _append_inputs(self, cutoff, conductivity):
        self._lowCutoffs.append(cutoff[0])
        self._highCutoffs.append(cutoff[1])
        self._properties.append(conductivity)

    def get_size(self):
        return len(self._lowCutoffs)

    def get_material(self, i):
        if i >= len(self._lowCutoffs):
            raise Exception("Invalid index. Maximum size: " + str(self.get_size()))
        if i < 0:
            raise Exception("Invalid index. Must be >= 0")

        return self._lowCutoffs[i], self._highCutoffs[i], self._properties[i]

    def show(self):
        for i in range(len(self._lowCutoffs)):
            print('Range [{} - {}] = {}'.format(self._lowCutoffs[i], self._highCutoffs[i], self._properties[i]))
            if len(self._properties[i]) == 6:
                k = self._properties[i]
                print("[[", k[0], k[3], k[4], "]")
                print(" [", k[3], k[1], k[5], "]")
                print(" [", k[4], k[5], k[2], "]]")
            elif len(self._properties[i]) == 21:
                C = self._properties[i]
                print("[[", C[0], C[1], C[2], C[3], C[4], C[5], "]")
                print(" [", C[1], C[6], C[7], C[8], C[9], C[10], "]")
                print(" [", C[2], C[7], C[11], C[12], C[13], C[14], "]")
                print(" [", C[3], C[8], C[12], C[15], C[16], C[17], "]")
                print(" [", C[4], C[9], C[13], C[16], C[18], C[19], "]")
                print(" [", C[5], C[10], C[14], C[17], C[19], C[20], "]]")

class IsotropicConductivityMap(MaterialPropertyMap):
    def __init__(self):
        super().__init__()

    def add_material(self, cutoff, conductivity):
        conductivity = self.error_check(cutoff, conductivity)
        if isinstance(conductivity, bool):
            return
        self._append_inputs(cutoff, conductivity)


class AnisotropicConductivityMap(MaterialPropertyMap):
    def __init__(self):
        super().__init__()

    def add_material(self, cutoff, kxx, kyy, kzz, kxy, kxz, kyz):
        conductivity = (kxx, kyy, kzz, kxy, kxz, kyz)
        conductivity = self.error_check(cutoff, conductivity)
        if isinstance(conductivity, bool):
            return
        self._append_inputs(cutoff, conductivity)

    def add_isotropic_material(self, cutoff, k):
        conductivity = (k, k, k, 0., 0., 0.)
        conductivity = self.error_check(cutoff, conductivity)
        if isinstance(conductivity, bool):
            return
        self._append_inputs(cutoff, conductivity)

    def add_orthotropic_material(self, cutoff, kxx, kyy, kzz):
        conductivity = (kxx, kyy, kzz, 0., 0., 0.)
        conductivity = self.error_check(cutoff, conductivity)
        if isinstance(conductivity, bool):
            return
        self._append_inputs(cutoff, conductivity)

    def add_material_to_orient(self, cutoff, k_axial, k_radial):
        conductivity = (k_axial, k_radial)
        conductivity = self.error_check(cutoff, conductivity)
        if isinstance(conductivity, bool):
            return
        self._append_inputs(cutoff, conductivity)


class ElasticityMap(MaterialPropertyMap):
    def __init__(self):
        super().__init__()

    def add_material(self, cutoff, C11, C12, C13, C14, C15, C16, C22, C23, C24, C25, C26,
                     C33, C34, C35, C36, C44, C45, C46, C55, C56, C66):

        elasticity = (C11, C12, C13, C14, C15, C16,
                      C22, C23, C24, C25, C26,
                      C33, C34, C35, C36,
                      C44, C45, C46,
                      C55, C56,
                      C66)
        elasticity = self.error_check(cutoff, elasticity)
        if isinstance(elasticity, bool):
            return
        self._append_inputs(cutoff, elasticity)

    def add_isotropic_material(self, cutoff, E_youngmod, nu_poissrat):
        Lambda = (nu_poissrat * E_youngmod) / ((1 + nu_poissrat) * (1 - 2 * nu_poissrat))
        mu = E_youngmod / (2 * (1 + nu_poissrat))

        elasticity = (Lambda + 2 * mu, Lambda, Lambda, 0., 0., 0.,
                      Lambda + 2 * mu, Lambda,     0., 0., 0.,
                      Lambda + 2 * mu,     0.,     0., 0.,
                      2 * mu,              0.,     0.,
                      2 * mu,              0.,
                      2 * mu)

        elasticity = self.error_check(cutoff, elasticity)
        if isinstance(elasticity, bool):
            return
        self._append_inputs(cutoff, elasticity)

    def add_orthotropic_material(self, cutoff, E_x, E_y, E_z, nu_yz, nu_xz, nu_xy, G_yz, G_xz, G_xy):
        nu_yx = (nu_xy * E_y) / E_x
        nu_zx = (nu_xz * E_z) / E_x
        nu_zy = (nu_yz * E_z) / E_y

        den = (1 - nu_xy * nu_yx - nu_yz * nu_zy - nu_xz * nu_zx - 2 * nu_xy * nu_yz * nu_zx) / (E_x * E_y * E_z)

        elasticity = ((1 - nu_yz * nu_zy) / (E_y * E_z * den), (nu_yx + nu_zx * nu_yz) / (E_y * E_z * den), (nu_zx + nu_yx * nu_zy) / (E_y * E_z * den), 0, 0, 0,
                      (1 - nu_zx * nu_xz) / (E_z * E_x * den), (nu_zy + nu_zx * nu_xy) / (E_z * E_x * den), 0, 0, 0,
                      (1 - nu_xy * nu_yx) / (E_x * E_y * den), 0, 0, 0,
                      2 * G_yz, 0, 0,
                      2 * G_xz, 0,
                      2 * G_xy)

        elasticity = self.error_check(cutoff, elasticity)
        if isinstance(elasticity, bool):
            return
        self._append_inputs(cutoff, elasticity)

    def add_transverse_isotropic_material(self, cutoff, E_a, E_t, nu_a, nu_t, G_a):
        E_x = E_a
        E_y = E_t
        E_z = E_t
        nu_xy = nu_a
        nu_xz = nu_a
        nu_yz = nu_t

        nu_yx = (nu_xy * E_y) / E_x
        nu_zx = (nu_xz * E_z) / E_x
        nu_zy = (nu_yz * E_z) / E_y

        den = (1 - nu_xy * nu_yx - nu_yz * nu_zy - nu_xz * nu_zx - 2 * nu_xy * nu_yz * nu_zx) / (E_x * E_y * E_z)

        c11 = (1 - nu_yz * nu_zy) / (E_y * E_z * den)
        c12 = (nu_yx + nu_zx * nu_yz) / (E_y * E_z * den)
        c22 = (1 - nu_zx * nu_xz) / (E_z * E_x * den)
        c23 = (nu_zy + nu_zx * nu_xy) / (E_z * E_x * den)
        c66 = G_a

        elasticity = (c11, c12, c12, 0, 0, 0,
                      c22, c23, 0, 0, 0,
                      c22, 0, 0, 0,
                      (c22 - c23) / 2, 0, 0,
                      c66, 0,
                      c66)

        elasticity = self.error_check(cutoff, elasticity)
        if isinstance(elasticity, bool):
            return
        self._append_inputs(cutoff, elasticity)

    def add_material_to_orient(self, cutoff, E_axial, E_radial, nu_poissrat_12, nu_poissrat_23, G12):
        elasticity = (E_axial, E_radial, nu_poissrat_12, nu_poissrat_23, G12)
        elasticity = self.error_check(cutoff, elasticity)
        if isinstance(elasticity, bool):
            return
        self._append_inputs(cutoff, elasticity)

    def show(self):
        print("Material elasticity as [low-high cutoffs] = elasticity:")
        for i in range(len(self._lowCutoffs)):
            print('[{} - {}] = '.format(self._lowCutoffs[i], self._highCutoffs[i], self._properties[i]))
            if len(self._properties[i]) == 5:
                print('{}'.format(self._properties[i]))
            else:
                first_elast, last_elast = (0, 6)
                for i2 in range(6):
                    print('{}'.format(self._properties[i][first_elast:last_elast]))
                    first_elast = last_elast
                    last_elast += 5 - i2
