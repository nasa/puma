import numpy as np
cimport numpy as np

np.import_array()
DTYPE = np.int
ctypedef np.int_t DTYPE_t


def index_at_p(int index, int size):
    if index == -1:
        return size - 1
    elif index == size:
        return 0
    return index


def index_at_s(index, size):
    if index == -1:
        return 0
    elif index == size:
        return size - 1
    return index


def create_Ab_indices_cy(np.ndarray I_A, np.ndarray J_A, np.ndarray I_b, int counter_A, int counter_b,
                      int i_cv, int len_x, int len_y, int len_z, int len_xyz, str side_bc):

    cdef int j_cv, k_cv, i2, j2, k2, i_bc, j_bc, k_bc, counter, global_i
    cdef np.ndarray global_js = np.zeros(27, dtype=DTYPE)

    if side_bc == 'p':
        index_at = index_at_p
    else:
        index_at = index_at_s

    for j_cv in range(len_y):
        for k_cv in range(len_z):

            counter = 0
            for k2 in range(-1, 2):
                k_bc = index_at(k_cv + k2, len_z)
                for j2 in range(-1, 2):
                    j_bc = index_at(j_cv + j2, len_y)
                    for i2 in range(-1, 2):
                        i_bc = index_at(i_cv + i2, len_x)
                        global_js[counter] = len_x * (len_y * k_bc + j_bc) + i_bc
                        counter += 1

            # row of A matrix corresponding to divergence equation inside P cv
            global_i = len_x * (len_y * k_cv + j_cv) + i_cv

            # Append indices to assemble sparse A matrix
            # column of A corresponding to surrounding nodes (SW S SE W P E NW N NE) interacting with P cv
            I_A[counter_A:counter_A + 27] = global_i
            I_b[counter_b] = global_i
            J_A[counter_A:counter_A + 27] = global_js
            counter_A += 27
            counter_b += 1


def create_T_ivs_cy(double [:,:,:] T, double [:] tf, int i_cv, int len_x, int len_y, int len_z, int len_xyz, str side_bc,
                    np.ndarray t_sw, np.ndarray t_se, np.ndarray t_nw, np.ndarray t_ne,
                    np.ndarray t_tsw, np.ndarray t_tse, np.ndarray t_tnw, np.ndarray t_tne):

    cdef int j_cv, k_cv, i2, j2, k2, i_bc, j_bc, k_bc, dim, counter

    if side_bc == 'p':
        index_at = index_at_p
    else:
        index_at = index_at_s

    for j_cv in range(len_y):
        for k_cv in range(len_z):

            counter = 0
            for k2 in range(-1, 2):
                k_bc = index_at(k_cv + k2, len_z)
                for j2 in range(-1, 2):
                    j_bc = index_at(j_cv + j2, len_y)
                    for i2 in range(-1, 2):
                        i_bc = index_at(i_cv + i2, len_x)
                        tf[counter] = T[i_bc, j_bc, k_bc]
                        counter += 1

            t_sw[j_cv, k_cv] = [tf[0], tf[1], tf[3], tf[4], tf[9], tf[10], tf[12], tf[13]]
            t_se[j_cv, k_cv] = [tf[1], tf[2], tf[4], tf[5], tf[10], tf[11], tf[13], tf[14]]
            t_nw[j_cv, k_cv] = [tf[3], tf[4], tf[6], tf[7], tf[12], tf[13], tf[15], tf[16]]
            t_ne[j_cv, k_cv] = [tf[4], tf[5], tf[7], tf[8], tf[13], tf[14], tf[16], tf[17]]
            t_tsw[j_cv, k_cv] = [tf[9], tf[10], tf[12], tf[13], tf[18], tf[19], tf[21], tf[22]]
            t_tse[j_cv, k_cv] = [tf[10], tf[11], tf[13], tf[14], tf[19], tf[20], tf[22], tf[23]]
            t_tnw[j_cv, k_cv] = [tf[12], tf[13], tf[15], tf[16], tf[21], tf[22], tf[24], tf[25]]
            t_tne[j_cv, k_cv] = [tf[13], tf[14], tf[16], tf[17], tf[22], tf[23], tf[25], tf[26]]


def assign_prescribed_bc_cy(np.ndarray not_dir_x, np.ndarray not_dir_y, np.ndarray not_dir_z, np.ndarray Td,
                            np.ndarray dirichlet_bc_xfaces, np.ndarray dirichlet_bc_yfaces, np.ndarray dirichlet_bc_zfaces,
                            int len_x, int len_y, int len_z, int i_iv):
    cdef int j_iv, k_iv, k2, k2_v, j2, j2_v, i2, i2_v, i_cv, counter_not_dir

    for j_iv in range(len_y + 1):
        for k_iv in range(len_z + 1):

            # gather IV properties cell by cell
            counter_not_dir = 0
            for k2, k2_v in enumerate([-1, 0]):
                k_cv = k_iv + k2_v
                for j2, j2_v in enumerate([-1, 0]):
                    j_cv = j_iv + j2_v
                    for i2, i2_v in enumerate([-1, 0]):
                        i_cv = i_iv + i2_v

                        if i_cv == -1:
                            if not np.isnan(dirichlet_bc_xfaces[0, j_iv, k_iv]):
                                not_dir_x[counter_not_dir, j_iv, k_iv] = 0
                        if i_cv == len_x:
                            if not np.isnan(dirichlet_bc_xfaces[1, j_iv, k_iv]):
                                not_dir_x[counter_not_dir, j_iv, k_iv] = 0

                        if j_cv == -1:
                            if not np.isnan(dirichlet_bc_yfaces[0, i_iv, k_iv]):
                                not_dir_y[counter_not_dir, j_iv, k_iv] = 0
                        if j_cv == len_y:
                            if not np.isnan(dirichlet_bc_yfaces[1, i_iv, k_iv]):
                                not_dir_y[counter_not_dir, j_iv, k_iv] = 0

                        if k_cv == -1:
                            if not np.isnan(dirichlet_bc_zfaces[0, i_iv, j_iv]):
                                not_dir_z[counter_not_dir, j_iv, k_iv] = 0
                        if k_cv == len_z:
                            if not np.isnan(dirichlet_bc_zfaces[1, i_iv, j_iv]):
                                not_dir_z[counter_not_dir, j_iv, k_iv] = 0

                        counter_not_dir += 1

            # Td arranged as:
            # Pe: 0  Ne: 1  Te: 2   TNe: 3
            # Pn: 4  En: 5  Tn: 6   TEn: 7
            # Pt: 8  Et: 9  Nt: 10  NEt: 11
            # not_dir_xyz arranged as:
            # P: 0   E: 1   N: 2   NE: 3
            # T: 4  TE: 5  TN: 6  TNE: 7

            if j_iv == 0:
                if not np.isnan(dirichlet_bc_yfaces[0, i_iv, k_iv]):
                    Td[[4, 5, 6, 7], j_iv, k_iv] = dirichlet_bc_yfaces[0, i_iv, k_iv]

            if j_iv == len_y:
                if not np.isnan(dirichlet_bc_yfaces[1, i_iv, k_iv]):
                    Td[[4, 5, 6, 7], j_iv, k_iv] = dirichlet_bc_yfaces[1, i_iv, k_iv]

            if k_iv == 0:
                if not np.isnan(dirichlet_bc_zfaces[0, i_iv, j_iv]):
                    Td[[8, 9, 10, 11], j_iv, k_iv] = dirichlet_bc_zfaces[0, i_iv, j_iv]

            elif k_iv == len_z:
                if not np.isnan(dirichlet_bc_zfaces[1, i_iv, j_iv]):
                    Td[[8, 9, 10, 11], j_iv, k_iv] = dirichlet_bc_zfaces[1, i_iv, j_iv]

    if i_iv == 0:
        mask_assigned_dir = ~np.isnan(dirichlet_bc_xfaces[0, :, :])
        Td[0, mask_assigned_dir] = dirichlet_bc_xfaces[0, mask_assigned_dir]
        Td[[1, 2, 3]] = Td[0]

    elif i_iv == len_x:
        mask_assigned_dir = ~np.isnan(dirichlet_bc_xfaces[1, :, :])
        Td[0, mask_assigned_dir] = dirichlet_bc_xfaces[1, mask_assigned_dir]
        Td[[1, 2, 3]] = Td[0]
