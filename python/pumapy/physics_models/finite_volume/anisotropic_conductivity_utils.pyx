import numpy as np
DTYPE = np.float


def flatten_Kmat(int len_y, int len_z, double [:,:,:,:] Kmat, double [:,:,:] Kmat_flat):

    cdef int j, k, i2, j2, k2, counter

    for j in range(len_y - 1):
        for k in range(len_z - 1):
            counter = 0
            for k2 in range(2):
                for j2 in range(2):
                    for i2 in range(2):
                        Kmat_flat[counter:counter + 6, j, k] = Kmat[i2, j + j2, k + k2]
                        counter += 6


def index_at_p(index, size):
    if index == 0:
        return size - 1
    elif index == size:
        return 1
    return index


def index_at_s(index, size):
    if index == 0:
        return 1
    elif index == size:
        return size - 1
    return index


def pad_domain(unsigned short [:,:,:] ws_pad, double [:,:,:,:] orient_pad, unsigned short need_to_orient, int len_x, int len_y, int len_z, str side_bc):

    cdef int i, j, k

    if side_bc == 'p':
        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        ws_pad[i, j, k] = ws_pad[index_at_p(i, len_x - 1),
                                                 index_at_p(j, len_y - 1),
                                                 index_at_p(k, len_z - 1)]
        if need_to_orient:
            for i in range(len_x):
                for j in range(len_y):
                    for k in range(len_z):
                        orient_pad[i, j, k] = orient_pad[index_at_p(i, len_x - 1),
                                              index_at_p(j, len_y - 1),
                                              index_at_p(k, len_z - 1)]
    else:
        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        ws_pad[i, j, k] = ws_pad[index_at_s(i, len_x - 1),
                                                 index_at_s(j, len_y - 1),
                                                 index_at_s(k, len_z - 1)]
        if need_to_orient:
            for i in range(len_x):
                for j in range(len_y):
                    for k in range(len_z):
                        orient_pad[i, j, k] = orient_pad[index_at_s(i, len_x - 1),
                                              index_at_s(j, len_y - 1),
                                              index_at_s(k, len_z - 1)]


def add_nondiag(unsigned int [:] nondiag, int len_x, int len_y, int len_z, side_bc):

    cdef int i, j, k, counter = 0

    if side_bc == 'p':
        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = len_x * (len_y * index_at_p(k, len_z - 1) + index_at_p(j, len_y - 1)) + index_at_p(i, len_x - 1)
                        counter += 1
    else:
        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = len_x * (len_y * index_at_s(k, len_z - 1) + index_at_s(j, len_y - 1)) + index_at_s(i, len_x - 1)
                        counter += 1


def divP(int i, int len_x, int len_y, int len_z, unsigned char [:,:,:] dir_vox, unsigned int [:] j_indices, double [:] values, double [:,:,:,:,:] Emat):

    cdef int j, k
    cdef unsigned long long counter_j, counter_v
    cdef double [:,:] E_sw, E_se, E_nw, E_ne, E_tsw, E_tse, E_tnw, E_tne
    counter_j = 0
    counter_v = 0

    for j in range(1, len_y - 1):
        for k in range(1, len_z - 1):

            # When dirichlet voxel skip node
            if not dir_vox[i, j, k]:

                # Computing x and y divergence equations for P control volume
                E_sw = Emat[0, j - 1, k - 1]
                E_se = Emat[1, j - 1, k - 1]
                E_nw = Emat[0, j, k - 1]
                E_ne = Emat[1, j, k - 1]
                E_tsw = Emat[0, j - 1, k]
                E_tse = Emat[1, j - 1, k]
                E_tnw = Emat[0, j, k]
                E_tne = Emat[1, j, k]

                values[counter_v + 0] = -E_sw[3, 0] - E_sw[7, 0] - E_sw[11, 0]
                values[counter_v + 1] = E_se[3, 0] - E_sw[3, 1] - E_se[6, 0] - E_sw[7, 1] - E_se[10, 0] - E_sw[11, 1]
                values[counter_v + 2] = E_se[3, 1] - E_se[6, 1] - E_se[10, 1]
                values[counter_v + 3] = E_nw[7, 0] - E_nw[2, 0] - E_nw[9, 0] - E_sw[3, 2] - E_sw[7, 2] - E_sw[11, 2]
                values[counter_v + 4] = E_ne[2, 0] - E_nw[2, 1] + E_ne[6, 0] - E_ne[8, 0] + E_nw[7, 1] - E_nw[9, 1] + E_se[3, 2] - E_sw[3, 3] - E_se[6, 2] - E_sw[7, 3] - E_se[10, 2] - E_sw[11, 3]
                values[counter_v + 5] = E_ne[2, 1] + E_ne[6, 1] - E_ne[8, 1] + E_se[3, 3] - E_se[6, 3] - E_se[10, 3]
                values[counter_v + 6] = E_nw[7, 2] - E_nw[2, 2] - E_nw[9, 2]
                values[counter_v + 7] = E_ne[2, 2] - E_nw[2, 3] + E_ne[6, 2] - E_ne[8, 2] + E_nw[7, 3] - E_nw[9, 3]
                values[counter_v + 8] = E_ne[2, 3] + E_ne[6, 3] - E_ne[8, 3]
                values[counter_v + 9] = E_tsw[11, 0] - E_sw[7, 4] - E_sw[11, 4] - E_tsw[1, 0] - E_tsw[5, 0] - E_sw[3, 4]
                values[counter_v + 10] = E_se[3, 4] - E_sw[3, 5] - E_se[6, 4] - E_sw[7, 5] - E_se[10, 4] - E_sw[11, 5] + E_tse[1, 0] - E_tsw[1, 1] - E_tse[4, 0] - E_tsw[5, 1] + E_tse[10, 0] + E_tsw[11, 1]
                values[counter_v + 11] = E_se[3, 5] - E_se[6, 5] - E_se[10, 5] + E_tse[1, 1] - E_tse[4, 1] + E_tse[10, 1]
                values[counter_v + 12] = E_nw[7, 4] - E_nw[2, 4] - E_nw[9, 4] - E_sw[3, 6] - E_sw[7, 6] - E_sw[11, 6] - E_tnw[0, 0] - E_tsw[1, 2] + E_tnw[5, 0] - E_tsw[5, 2] + E_tnw[9, 0] + E_tsw[11, 2]
                values[counter_v + 13] = E_ne[2, 4] - E_nw[2, 5] + E_ne[6, 4] - E_ne[8, 4] + E_nw[7, 5] - E_nw[9, 5] + E_se[3, 6] - E_sw[3, 7] - E_se[6, 6] - E_sw[7, 7] - E_se[10, 6] - E_sw[11, 7] + E_tne[0, 0] + E_tse[1, 2] - E_tnw[0, 1] - E_tsw[     1, 3] + E_tne[4, 0] - E_tse[4, 2] + E_tnw[5, 1] - E_tsw[5, 3] + E_tne[8, 0] + E_tse[10, 2] + E_tnw[     9, 1] + E_tsw[11, 3]
                values[counter_v + 14] = E_ne[2, 5] + E_ne[6, 5] - E_ne[8, 5] + E_se[3, 7] - E_se[6, 7] - E_se[10, 7] + E_tne[0, 1] + E_tse[1, 3] + E_tne[4, 1] - E_tse[4, 3] + E_tne[8, 1] + E_tse[10, 3]
                values[counter_v + 15] = E_nw[7, 6] - E_nw[2, 6] - E_nw[9, 6] - E_tnw[0, 2] + E_tnw[5, 2] + E_tnw[9, 2]
                values[counter_v + 16] = E_ne[2, 6] - E_nw[2, 7] + E_ne[6, 6] - E_ne[8, 6] + E_nw[7, 7] - E_nw[9, 7] + E_tne[0, 2] - E_tnw[0, 3] + E_tne[4, 2] + E_tnw[5, 3] + E_tne[8, 2] + E_tnw[9, 3]
                values[counter_v + 17] = E_ne[2, 7] + E_ne[6, 7] - E_ne[8, 7] + E_tne[0, 3] + E_tne[4, 3] + E_tne[8, 3]
                values[counter_v + 18] = E_tsw[11, 4] - E_tsw[5, 4] - E_tsw[1, 4]
                values[counter_v + 19] = E_tse[1, 4] - E_tsw[1, 5] - E_tse[4, 4] - E_tsw[5, 5] + E_tse[10, 4] + E_tsw[11, 5]
                values[counter_v + 20] = E_tse[1, 5] - E_tse[4, 5] + E_tse[10, 5]
                values[counter_v + 21] = E_tnw[5, 4] - E_tsw[1, 6] - E_tnw[0, 4] - E_tsw[5, 6] + E_tnw[9, 4] + E_tsw[11, 6]
                values[counter_v + 22] = E_tne[0, 4] + E_tse[1, 6] - E_tnw[0, 5] - E_tsw[1, 7] + E_tne[4, 4] - E_tse[4, 6] + E_tnw[5, 5] - E_tsw[     5, 7] + E_tne[8, 4] + E_tse[10, 6] + E_tnw[9, 5] + E_tsw[11, 7]
                values[counter_v + 23] = E_tne[0, 5] + E_tse[1, 7] + E_tne[4, 5] - E_tse[4, 7] + E_tne[8, 5] + E_tse[10, 7]
                values[counter_v + 24] = E_tnw[5, 6] - E_tnw[0, 6] + E_tnw[9, 6]
                values[counter_v + 25] = E_tne[0, 6] - E_tnw[0, 7] + E_tne[4, 6] + E_tnw[5, 7] + E_tne[8, 6] + E_tnw[9, 7]
                values[counter_v + 26] = E_tne[0, 7] + E_tne[4, 7] + E_tne[8, 7]

                # Extra check in case all divergence values are 0 (to avoid singularity in Amat)
                if np.sum(np.abs(values[counter_v:counter_v + 27])) == 0:
                    dir_vox[i, j, k] = True
                    values[counter_v:counter_v + 27] = np.NAN
                else:
                    for k2 in range(-1, 2):
                        for j2 in range(-1, 2):
                            for i2 in range(-1, 2):
                                j_indices[counter_j] = len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                                counter_j += 1

                    counter_v += 27


def fill_flux(int i, int len_x, int len_y, int len_z, double [:,:,:] T, double [:,:,:,:,:] Emat,
              double [:,:,:,:] E_sw, double [:,:,:,:] E_se, double [:,:,:,:] E_nw, double [:,:,:,:] E_ne,
              double [:,:,:,:] E_tsw, double [:,:,:,:] E_tse, double [:,:,:,:] E_tnw, double [:,:,:,:] E_tne,
              double [:,:,:] T_sw, double [:,:,:] T_se, double [:,:,:] T_nw, double [:,:,:] T_ne,
              double [:,:,:] T_tsw, double [:,:,:] T_tse, double [:,:,:] T_tnw, double [:,:,:] T_tne):
    cdef int j, k

    for j in range(1, len_y - 1):
        for k in range(1, len_z - 1):
            # Eight IVs
            T_sw[j - 1, k - 1, 0] = T[0, j - 1, k - 1]
            T_sw[j - 1, k - 1, 1] = T[1, j - 1, k - 1]
            T_sw[j - 1, k - 1, 2] = T[0, j, k - 1]
            T_sw[j - 1, k - 1, 3] = T[1, j, k - 1]
            T_sw[j - 1, k - 1, 4] = T[0, j - 1, k + 1]
            T_sw[j - 1, k - 1, 5] = T[1, j - 1, k + 1]
            T_sw[j - 1, k - 1, 6] = T[0, j, k + 1]
            T_sw[j - 1, k - 1, 7] = T[1, j, k]

            T_se[j - 1, k - 1, 0] = T[1, j - 1, k - 1]
            T_se[j - 1, k - 1, 1] = T[2, j - 1, k - 1]
            T_se[j - 1, k - 1, 2] = T[1, j, k - 1]
            T_se[j - 1, k - 1, 3] = T[2, j, k - 1]
            T_se[j - 1, k - 1, 4] = T[1, j - 1, k]
            T_se[j - 1, k - 1, 5] = T[2, j - 1, k]
            T_se[j - 1, k - 1, 6] = T[1, j, k]
            T_se[j - 1, k - 1, 7] = T[2, j, k + 1]

            T_nw[j - 1, k - 1, 0] = T[0, j, k - 1]
            T_nw[j - 1, k - 1, 1] = T[1, j, k - 1]
            T_nw[j - 1, k - 1, 2] = T[0, j + 1, k - 1]
            T_nw[j - 1, k - 1, 3] = T[1, j + 1, k - 1]
            T_nw[j - 1, k - 1, 4] = T[0, j, k]
            T_nw[j - 1, k - 1, 5] = T[1, j, k]
            T_nw[j - 1, k - 1, 6] = T[0, j + 1, k]
            T_nw[j - 1, k - 1, 7] = T[1, j + 1, k]

            T_ne[j - 1, k - 1, 0] = T[1, j, k - 1]
            T_ne[j - 1, k - 1, 1] = T[2, j, k - 1]
            T_ne[j - 1, k - 1, 2] = T[1, j + 1, k - 1]
            T_ne[j - 1, k - 1, 3] = T[2, j + 1, k - 1]
            T_ne[j - 1, k - 1, 4] = T[1, j, k]
            T_ne[j - 1, k - 1, 5] = T[2, j, k]
            T_ne[j - 1, k - 1, 6] = T[1, j + 1, k]
            T_ne[j - 1, k - 1, 7] = T[2, j + 1, k]

            T_tsw[j - 1, k - 1, 0] = T[0, j - 1, k + 1]
            T_tsw[j - 1, k - 1, 1] = T[1, j - 1, k]
            T_tsw[j - 1, k - 1, 2] = T[0, j, k]
            T_tsw[j - 1, k - 1, 3] = T[1, j, k]
            T_tsw[j - 1, k - 1, 4] = T[0, j - 1, k + 1]
            T_tsw[j - 1, k - 1, 5] = T[1, j - 1, k + 1]
            T_tsw[j - 1, k - 1, 6] = T[0, j, k + 1]
            T_tsw[j - 1, k - 1, 7] = T[1, j, k + 1]

            T_tse[j - 1, k - 1, 0] = T[1, j - 1, k]
            T_tse[j - 1, k - 1, 1] = T[2, j - 1, k]
            T_tse[j - 1, k - 1, 2] = T[1, j, k]
            T_tse[j - 1, k - 1, 3] = T[2, j, k]
            T_tse[j - 1, k - 1, 4] = T[1, j - 1, k + 1]
            T_tse[j - 1, k - 1, 5] = T[2, j - 1, k + 1]
            T_tse[j - 1, k - 1, 6] = T[1, j, k + 1]
            T_tse[j - 1, k - 1, 7] = T[2, j, k + 1]

            T_tnw[j - 1, k - 1, 0] = T[0, j, k]
            T_tnw[j - 1, k - 1, 1] = T[1, j, k]
            T_tnw[j - 1, k - 1, 2] = T[0, j + 1, k]
            T_tnw[j - 1, k - 1, 3] = T[1, j + 1, k]
            T_tnw[j - 1, k - 1, 4] = T[0, j, k + 1]
            T_tnw[j - 1, k - 1, 5] = T[1, j, k + 1]
            T_tnw[j - 1, k - 1, 6] = T[0, j + 1, k + 1]
            T_tnw[j - 1, k - 1, 7] = T[1, j + 1, k + 1]

            T_tne[j - 1, k - 1, 0] = T[1, j, k]
            T_tne[j - 1, k - 1, 1] = T[2, j, k]
            T_tne[j - 1, k - 1, 2] = T[1, j + 1, k]
            T_tne[j - 1, k - 1, 3] = T[2, j + 1, k]
            T_tne[j - 1, k - 1, 4] = T[1, j, k + 1]
            T_tne[j - 1, k - 1, 5] = T[2, j, k + 1]
            T_tne[j - 1, k - 1, 6] = T[1, j + 1, k + 1]
            T_tne[j - 1, k - 1, 7] = T[2, j + 1, k + 1]

            E_sw[j - 1, k - 1] = Emat[0, j - 1, k - 1]
            E_se[j - 1, k - 1] = Emat[1, j - 1, k - 1]
            E_nw[j - 1, k - 1] = Emat[0, j, k - 1]
            E_ne[j - 1, k - 1] = Emat[1, j, k - 1]
            E_tsw[j - 1, k - 1] = Emat[0, j - 1, k]
            E_tse[j - 1, k - 1] = Emat[1, j - 1, k]
            E_tnw[j - 1, k - 1] = Emat[0, j, k]
            E_tne[j - 1, k - 1] = Emat[1, j, k]
