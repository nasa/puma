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
        print("Material conductivity as [low-high cutoffs] = conductivity:")
        for i in range(len(self._lowCutoffs)):
            print('[{} - {}] = {}'.format(self._lowCutoffs[i], self._highCutoffs[i], self._properties[i]))


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
