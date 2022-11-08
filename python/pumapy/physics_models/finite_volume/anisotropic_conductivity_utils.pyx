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
