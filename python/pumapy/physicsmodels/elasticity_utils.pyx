import numpy as np
cimport numpy as np

np.import_array()
DTYPE = np.int
ctypedef np.int_t DTYPE_t


def index_at_p_pad(int index, int size):
    if index == 0:
        return size - 1
    elif index == size:
        return 1
    return index


def index_at_s_pad(index, size):
    if index == 0:
        return 1
    elif index == size:
        return size - 1
    return index


def pad_domain_cy(unsigned short [:,:,:] ws_pad, double [:,:,:,:] orient_pad, unsigned short need_to_orient, int len_x, int len_y, int len_z, str side_bc):

    cdef int i, j, k

    if side_bc == 'p':
        index_at_pad = index_at_p_pad
    else:
        index_at_pad = index_at_s_pad

    # if side_bc == 'p':
    for i in range(len_x + 2):
        for j in range(len_y + 2):
            for k in range(len_z + 2):
                if i == 0 or i == len_x + 1 or j == 0 or j == len_y + 1 or k == 0 or k == len_z + 1:
                    ws_pad[i, j, k] = ws_pad[index_at_pad(i, len_x + 1),
                                             index_at_pad(j, len_y + 1),
                                             index_at_pad(k, len_z + 1)]
    if need_to_orient:
        for i in range(len_x + 2):
            for j in range(len_y + 2):
                for k in range(len_z + 2):
                    if i == 0 or i == len_x + 1 or j == 0 or j == len_y + 1 or k == 0 or k == len_z + 1:
                        orient_pad[i, j, k] = orient_pad[index_at_pad(i, len_x + 1),
                                                         index_at_pad(j, len_y + 1),
                                                         index_at_pad(k, len_z + 1)]


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
            for d, dim in enumerate([0, len_xyz, 2 * len_xyz]):
                I_A[counter_A:counter_A + 81] = dim + global_i
                I_b[counter_b] = dim + global_i
                J_A[counter_A:counter_A + 81] = np.hstack((global_js, len_xyz + global_js, 2 * len_xyz + global_js))
                counter_A += 81
                counter_b += 1


def create_u_ivs_cy(double [:,:,:,:] u, double [:] uf, int i_cv, int len_x, int len_y, int len_z, int len_xyz, str side_bc,
                    np.ndarray u_sw, np.ndarray u_se, np.ndarray u_nw, np.ndarray u_ne,
                    np.ndarray u_tsw, np.ndarray u_tse, np.ndarray u_tnw, np.ndarray u_tne):

    cdef int j_cv, k_cv, i2, j2, k2, i_bc, j_bc, k_bc, dim, counter

    if side_bc == 'p':
        index_at = index_at_p
    else:
        index_at = index_at_s

    for j_cv in range(len_y):
        for k_cv in range(len_z):

            counter = 0
            for dim in range(3):
                for k2 in range(-1, 2):
                    k_bc = index_at(k_cv + k2, len_z)
                    for j2 in range(-1, 2):
                        j_bc = index_at(j_cv + j2, len_y)
                        for i2 in range(-1, 2):
                            i_bc = index_at(i_cv + i2, len_x)
                            uf[counter] = u[i_bc, j_bc, k_bc, dim]
                            counter += 1

            u_sw[j_cv, k_cv] = [uf[0], uf[1], uf[3], uf[4], uf[9], uf[10], uf[12], uf[13],
                                uf[27], uf[28], uf[30], uf[31], uf[36], uf[37], uf[39], uf[40],
                                uf[54], uf[55], uf[57], uf[58], uf[63], uf[64], uf[66], uf[67]]

            u_se[j_cv, k_cv] = [uf[1], uf[2], uf[4], uf[5], uf[10], uf[11], uf[13], uf[14],
                                uf[28], uf[29], uf[31], uf[32], uf[37], uf[38], uf[40], uf[41],
                                uf[55], uf[56], uf[58], uf[59], uf[64], uf[65], uf[67], uf[68]]

            u_nw[j_cv, k_cv] = [uf[3], uf[4], uf[6], uf[7], uf[12], uf[13], uf[15], uf[16],
                                uf[30], uf[31], uf[33], uf[34], uf[39], uf[40], uf[42], uf[43],
                                uf[57], uf[58], uf[60], uf[61], uf[66], uf[67], uf[69], uf[70]]

            u_ne[j_cv, k_cv] = [uf[4], uf[5], uf[7], uf[8], uf[13], uf[14], uf[16], uf[17],
                                uf[31], uf[32], uf[34], uf[35], uf[40], uf[41], uf[43], uf[44],
                                uf[58], uf[59], uf[61], uf[62], uf[67], uf[68], uf[70], uf[71]]

            u_tsw[j_cv, k_cv] = [uf[9], uf[10], uf[12], uf[13], uf[18], uf[19], uf[21], uf[22],
                                 uf[36], uf[37], uf[39], uf[40], uf[45], uf[46], uf[48], uf[49],
                                 uf[63], uf[64], uf[66], uf[67], uf[72], uf[73], uf[75], uf[76]]

            u_tse[j_cv, k_cv] = [uf[10], uf[11], uf[13], uf[14], uf[19], uf[20], uf[22], uf[23],
                                 uf[37], uf[38], uf[40], uf[41], uf[46], uf[47], uf[49], uf[50],
                                 uf[64], uf[65], uf[67], uf[68], uf[73], uf[74], uf[76], uf[77]]

            u_tnw[j_cv, k_cv] = [uf[12], uf[13], uf[15], uf[16], uf[21], uf[22], uf[24], uf[25],
                                 uf[39], uf[40], uf[42], uf[43], uf[48], uf[49], uf[51], uf[52],
                                 uf[66], uf[67], uf[69], uf[70], uf[75], uf[76], uf[78], uf[79]]

            u_tne[j_cv, k_cv] = [uf[13], uf[14], uf[16], uf[17], uf[22], uf[23], uf[25], uf[26],
                                 uf[40], uf[41], uf[43], uf[44], uf[49], uf[50], uf[52], uf[53],
                                 uf[67], uf[68], uf[70], uf[71], uf[76], uf[77], uf[79], uf[80]]
