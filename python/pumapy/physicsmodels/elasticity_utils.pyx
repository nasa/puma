import numpy as np

DTYPE = np.float

def flatten_Cmat(int i, int len_y, int len_z, double [:,:,:,:] Cmat, double [:,:,:] Cmat_flat):

    cdef int j, k, i2, j2, k2, counter

    for j in range(len_y - 1):
        for k in range(len_z - 1):
            counter = 0
            for k2 in range(2):
                for j2 in range(2):
                    for i2 in range(2):
                        Cmat_flat[counter:counter + 21, j, k] = Cmat[i2, j + j2, k + k2]
                        counter += 21


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


def add_nondiag(unsigned int [:] nondiag, signed char [:] nondiag1s, int len_x, int len_y, int len_z, side_bc):

    cdef int i, j, k, counter = 0

    if side_bc == 'p':
        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = len_x * (len_y * index_at_p(k, len_z - 1) + index_at_p(j, len_y - 1)) + index_at_p(i, len_x - 1)
                        counter += 1

        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = len_x * len_y * len_z + \
                                           len_x * (len_y * index_at_p(k, len_z - 1) + index_at_p(j, len_y - 1)) + index_at_p(i, len_x - 1)
                        counter += 1

        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = 2 * len_x * len_y * len_z + \
                                           len_x * (len_y * index_at_p(k, len_z - 1) + index_at_p(j, len_y - 1)) + index_at_p(i, len_x - 1)
                        counter += 1
    else:
        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = len_x * (len_y * index_at_s(k, len_z - 1) + index_at_s(j, len_y - 1)) + index_at_s(i, len_x - 1)
                        if not (i == 0 or i == len_x - 1):
                            nondiag1s[counter] = -1
                        counter += 1

        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = len_x * len_y * len_z + \
                                           len_x * (len_y * index_at_s(k, len_z - 1) + index_at_s(j, len_y - 1)) + index_at_s(i, len_x - 1)
                        if not (j == 0 or j == len_y - 1):
                            nondiag1s[counter] = -1
                        counter += 1

        for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if i == 0 or i == len_x-1 or j == 0 or j == len_y-1 or k == 0 or k == len_z-1:
                        nondiag[counter] = 2 * len_x * len_y * len_z + \
                                           len_x * (len_y * index_at_s(k, len_z - 1) + index_at_s(j, len_y - 1)) + index_at_s(i, len_x - 1)
                        if not (k == 0 or k == len_z - 1):
                            nondiag1s[counter] = -1
                        counter += 1


def divP(int i, int len_x, int len_y, int len_z, unsigned char [:,:,:, :] dir_cv, unsigned int [:] j_indices, double [:] values, double [:,:,:,:,:] Emat):

    cdef int j, k
    cdef unsigned long long counter_j, counter_v, len_xyz
    cdef double [:,:] E_sw, E_se, E_nw, E_ne, E_tsw, E_tse, E_tnw, E_tne
    counter_j = 0
    counter_v = 0
    len_xyz = len_x * len_y * len_z

    # divP_x
    for j in range(1, len_y - 1):
        for k in range(1, len_z - 1):

            # When dirichlet voxel skip node
            if not dir_cv[i, j, k, 0]:

                # Computing x and y divergence equations for P control volume
                E_sw = Emat[0, j - 1, k - 1]
                E_se = Emat[1, j - 1, k - 1]
                E_nw = Emat[0, j, k - 1]
                E_ne = Emat[1, j, k - 1]
                E_tsw = Emat[0, j - 1, k]
                E_tse = Emat[1, j - 1, k]
                E_tnw = Emat[0, j, k]
                E_tne = Emat[1, j, k]

                values[counter_v +  0] = - E_sw[9, 0] - E_sw[22, 0] - E_sw[35, 0]
                values[counter_v +  1] = E_se[9, 0] - E_sw[9, 1] - E_se[19, 0] - E_sw[22, 1] - E_se[32, 0] - E_sw[35, 1]
                values[counter_v +  2] = E_se[9, 1] - E_se[19, 1] - E_se[32, 1]
                values[counter_v +  3] = E_nw[22, 0] - E_nw[6, 0] - E_nw[29, 0] - E_sw[9, 2] - E_sw[22, 2] - E_sw[35, 2]
                values[counter_v +  4] = E_ne[6, 0] - E_nw[6, 1] + E_ne[19, 0] + E_nw[22, 1] - E_ne[26, 0] - E_nw[29, 1] + E_se[9, 2] - E_sw[9, 3] - E_se[19, 2] - E_sw[22, 3] - E_se[32, 2] - E_sw[35, 3]
                values[counter_v +  5] = E_ne[6, 1] + E_ne[19, 1] - E_ne[26, 1] + E_se[9, 3] - E_se[19, 3] - E_se[32, 3]
                values[counter_v +  6] = E_nw[22, 2] - E_nw[6, 2] - E_nw[29, 2]
                values[counter_v +  7] = E_ne[6, 2] - E_nw[6, 3] + E_ne[19, 2] + E_nw[22, 3] - E_ne[26, 2] - E_nw[29, 3]
                values[counter_v +  8] = E_ne[6, 3] + E_ne[19, 3] - E_ne[26, 3]
                values[counter_v +  9] = E_tsw[35, 0] - E_sw[22, 4] - E_sw[35, 4] - E_tsw[3, 0] - E_tsw[16, 0] - E_sw[9, 4]
                values[counter_v + 10] = E_se[9, 4] - E_sw[9, 5] - E_se[19, 4] - E_sw[22, 5] - E_se[32, 4] + E_tse[3, 0] - E_sw[35, 5] - E_tsw[3, 1] - E_tse[13, 0] - E_tsw[16, 1] + E_tse[32, 0] + E_tsw[35, 1]
                values[counter_v + 11] = E_se[9, 5] - E_se[19, 5] - E_se[32, 5] + E_tse[3, 1] - E_tse[13, 1] + E_tse[32, 1]
                values[counter_v + 12] = E_nw[22, 4] - E_nw[6, 4] - E_nw[29, 4] - E_sw[9, 6] - E_sw[22, 6] - E_tnw[0, 0] - E_sw[35, 6] - E_tsw[3, 2] + E_tnw[16, 0] - E_tsw[16, 2] + E_tnw[29, 0] + E_tsw[35, 2]
                values[counter_v + 13] = E_ne[6, 4] - E_nw[6, 5] + E_ne[19, 4] + E_nw[22, 5] - E_ne[26, 4] - E_nw[29, 5] + E_se[9, 6] - E_sw[9, 7] - E_se[19, 6] - E_sw[22, 7] - E_se[32, 6] + E_tne[0, 0] - E_tnw[0, 1] + E_tse[3, 2] - E_sw[35, 7] - E_tsw[3, 3] + E_tne[13, 0] - E_tse[13, 2] + E_tnw[16, 1] - E_tsw[16, 3] + E_tne[26, 0] + E_tnw[29, 1] + E_tse[32, 2] + E_tsw[35, 3]
                values[counter_v + 14] = E_ne[6, 5] + E_ne[19, 5] - E_ne[26, 5] + E_se[9, 7] - E_se[19, 7] - E_se[32, 7] + E_tne[0, 1] + E_tse[3, 3] + E_tne[13, 1] - E_tse[13, 3] + E_tne[26, 1] + E_tse[32, 3]
                values[counter_v + 15] = E_nw[22, 6] - E_nw[6, 6] - E_nw[29, 6] - E_tnw[0, 2] + E_tnw[16, 2] + E_tnw[29, 2]
                values[counter_v + 16] = E_ne[6, 6] - E_nw[6, 7] + E_ne[19, 6] + E_nw[22, 7] - E_ne[26, 6] - E_nw[29, 7] + E_tne[0, 2] - E_tnw[0, 3] + E_tne[13, 2] + E_tnw[16, 3] + E_tne[26, 2] + E_tnw[29, 3]
                values[counter_v + 17] = E_ne[6, 7] + E_ne[19, 7] - E_ne[26, 7] + E_tne[0, 3] + E_tne[13, 3] + E_tne[26, 3]
                values[counter_v + 18] = E_tsw[35, 4] - E_tsw[16, 4] - E_tsw[3, 4]
                values[counter_v + 19] = E_tse[3, 4] - E_tsw[3, 5] - E_tse[13, 4] - E_tsw[16, 5] + E_tse[32, 4] + E_tsw[35, 5]
                values[counter_v + 20] = E_tse[3, 5] - E_tse[13, 5] + E_tse[32, 5]
                values[counter_v + 21] = E_tnw[16, 4] - E_tsw[3, 6] - E_tnw[0, 4] - E_tsw[16, 6] + E_tnw[29, 4] + E_tsw[35, 6]
                values[counter_v + 22] = E_tne[0, 4] - E_tnw[0, 5] + E_tse[3, 6] - E_tsw[3, 7] + E_tne[13, 4] - E_tse[13, 6] + E_tnw[16, 5] - E_tsw[16, 7] + E_tne[26, 4] + E_tnw[29, 5] + E_tse[32, 6] + E_tsw[35, 7]
                values[counter_v + 23] = E_tne[0, 5] + E_tse[3, 7] + E_tne[13, 5] - E_tse[13, 7] + E_tne[26, 5] + E_tse[32, 7]
                values[counter_v + 24] = E_tnw[16, 6] - E_tnw[0, 6] + E_tnw[29, 6]
                values[counter_v + 25] = E_tne[0, 6] - E_tnw[0, 7] + E_tne[13, 6] + E_tnw[16, 7] + E_tne[26, 6] + E_tnw[29, 7]
                values[counter_v + 26] = E_tne[0, 7] + E_tne[13, 7] + E_tne[26, 7]
                values[counter_v + 27] = - E_sw[9, 8] - E_sw[22, 8] - E_sw[35, 8]
                values[counter_v + 28] = E_se[9, 8] - E_se[19, 8] - E_se[32, 8] - E_sw[9, 9] - E_sw[22, 9] - E_sw[35, 9]
                values[counter_v + 29] = E_se[9, 9] - E_se[19, 9] - E_se[32, 9]
                values[counter_v + 30] = E_nw[22, 8] - E_nw[6, 8] - E_nw[29, 8] - E_sw[9, 10] - E_sw[22, 10] - E_sw[35, 10]
                values[counter_v + 31] = E_ne[6, 8] + E_ne[19, 8] - E_ne[26, 8] - E_nw[6, 9] + E_se[9, 10] - E_sw[9, 11] + E_nw[22, 9] - E_nw[29, 9] - E_se[19, 10] - E_sw[22, 11] - E_se[32, 10] - E_sw[35, 11]
                values[counter_v + 32] = E_ne[6, 9] + E_ne[19, 9] + E_se[9, 11] - E_ne[26, 9] - E_se[19, 11] - E_se[32, 11]
                values[counter_v + 33] = E_nw[22, 10] - E_nw[6, 10] - E_nw[29, 10]
                values[counter_v + 34] = E_ne[6, 10] - E_nw[6, 11] + E_ne[19, 10] + E_nw[22, 11] - E_ne[26, 10] - E_nw[29, 11]
                values[counter_v + 35] = E_ne[6, 11] + E_ne[19, 11] - E_ne[26, 11]
                values[counter_v + 36] = E_tsw[35, 8] - E_tsw[16, 8] - E_tsw[3, 8] - E_sw[9, 12] - E_sw[22, 12] - E_sw[35, 12]
                values[counter_v + 37] = E_tse[3, 8] - E_tse[13, 8] + E_tse[32, 8] - E_tsw[3, 9] + E_se[9, 12] - E_sw[9, 13] - E_se[19, 12] - E_tsw[16, 9] - E_sw[22, 13] - E_se[32, 12] - E_sw[35, 13] + E_tsw[35, 9]
                values[counter_v + 38] = E_tse[3, 9] + E_se[9, 13] - E_tse[13, 9] - E_se[19, 13] - E_se[32, 13] + E_tse[32, 9]
                values[counter_v + 39] = E_tnw[16, 8] - E_tnw[0, 8] - E_nw[6, 12] + E_tnw[29, 8] - E_tsw[3, 10] - E_sw[9, 14] + E_nw[22, 12] - E_nw[29, 12] - E_tsw[16, 10] - E_sw[22, 14] - E_sw[35, 14] + E_tsw[35, 10]
                values[counter_v + 40] = E_ne[6, 12] - E_nw[6, 13] + E_tne[0, 8] - E_tnw[0, 9] + E_tne[13, 8] + E_tne[26, 8] + E_ne[19, 12] + E_tse[3, 10] - E_tsw[3, 11] + E_se[9, 14] - E_sw[9, 15] + E_nw[22, 13] - E_ne[26, 12] - E_tse[13, 10] - E_nw[29, 13] - E_se[19, 14] + E_tnw[16, 9] - E_tsw[16, 11] - E_sw[22, 15] - E_se[32, 14] + E_tnw[29, 9] - E_sw[35, 15] + E_tse[32, 10] + E_tsw[35, 11]
                values[counter_v + 41] = E_ne[6, 13] + E_tne[0, 9] + E_ne[19, 13] + E_tse[3, 11] + E_se[9, 15] - E_ne[26, 13] + E_tne[13, 9] - E_tse[13, 11] - E_se[19, 15] + E_tne[26, 9] - E_se[32, 15] + E_tse[32, 11]
                values[counter_v + 42] = E_nw[22, 14] - E_tnw[0, 10] - E_nw[6, 14] - E_nw[29, 14] + E_tnw[16, 10] + E_tnw[29, 10]
                values[counter_v + 43] = E_ne[6, 14] - E_nw[6, 15] + E_tne[0, 10] - E_tnw[0, 11] + E_ne[19, 14] + E_nw[22, 15] - E_ne[26, 14] + E_tne[13, 10] - E_nw[29, 15] + E_tnw[16, 11] + E_tne[26, 10] + E_tnw[29, 11]
                values[counter_v + 44] = E_ne[6, 15] + E_tne[0, 11] + E_ne[19, 15] - E_ne[26, 15] + E_tne[13, 11] + E_tne[26, 11]
                values[counter_v + 45] = E_tsw[35, 12] - E_tsw[16, 12] - E_tsw[3, 12]
                values[counter_v + 46] = E_tse[3, 12] - E_tsw[3, 13] - E_tse[13, 12] - E_tsw[16, 13] + E_tse[32, 12] + E_tsw[35, 13]
                values[counter_v + 47] = E_tse[3, 13] - E_tse[13, 13] + E_tse[32, 13]
                values[counter_v + 48] = E_tnw[16, 12] - E_tsw[3, 14] - E_tnw[0, 12] - E_tsw[16, 14] + E_tnw[29, 12] + E_tsw[35, 14]
                values[counter_v + 49] = E_tne[0, 12] - E_tnw[0, 13] + E_tse[3, 14] - E_tsw[3, 15] + E_tne[13, 12] - E_tse[13, 14] + E_tnw[16, 13] - E_tsw[16, 15] + E_tne[26, 12] + E_tnw[29, 13] + E_tse[32, 14] + E_tsw[35, 15]
                values[counter_v + 50] = E_tne[0, 13] + E_tse[3, 15] + E_tne[13, 13] - E_tse[13, 15] + E_tne[26, 13] + E_tse[32, 15]
                values[counter_v + 51] = E_tnw[16, 14] - E_tnw[0, 14] + E_tnw[29, 14]
                values[counter_v + 52] = E_tne[0, 14] - E_tnw[0, 15] + E_tne[13, 14] + E_tnw[16, 15] + E_tne[26, 14] + E_tnw[29, 15]
                values[counter_v + 53] = E_tne[0, 15] + E_tne[13, 15] + E_tne[26, 15]
                values[counter_v + 54] = - E_sw[9, 16] - E_sw[22, 16] - E_sw[35, 16]
                values[counter_v + 55] = E_se[9, 16] - E_sw[9, 17] - E_se[19, 16] - E_sw[22, 17] - E_se[32, 16] - E_sw[35, 17]
                values[counter_v + 56] = E_se[9, 17] - E_se[19, 17] - E_se[32, 17]
                values[counter_v + 57] = E_nw[22, 16] - E_sw[9, 18] - E_nw[6, 16] - E_nw[29, 16] - E_sw[22, 18] - E_sw[35, 18]
                values[counter_v + 58] = E_ne[6, 16] - E_nw[6, 17] + E_ne[19, 16] + E_se[9, 18] - E_sw[9, 19] + E_nw[22, 17] - E_ne[26, 16] - E_nw[29, 17] - E_se[19, 18] - E_sw[22, 19] - E_se[32, 18] - E_sw[35, 19]
                values[counter_v + 59] = E_ne[6, 17] + E_ne[19, 17] + E_se[9, 19] - E_ne[26, 17] - E_se[19, 19] - E_se[32, 19]
                values[counter_v + 60] = E_nw[22, 18] - E_nw[6, 18] - E_nw[29, 18]
                values[counter_v + 61] = E_ne[6, 18] - E_nw[6, 19] + E_ne[19, 18] + E_nw[22, 19] - E_ne[26, 18] - E_nw[29, 19]
                values[counter_v + 62] = E_ne[6, 19] + E_ne[19, 19] - E_ne[26, 19]
                values[counter_v + 63] = E_tsw[35, 16] - E_sw[9, 20] - E_tsw[16, 16] - E_sw[22, 20] - E_sw[35, 20] - E_tsw[3, 16]
                values[counter_v + 64] = E_tse[3, 16] - E_tsw[3, 17] + E_se[9, 20] - E_sw[9, 21] - E_tse[13, 16] - E_se[19, 20] - E_tsw[16, 17] - E_sw[22, 21] - E_se[32, 20] - E_sw[35, 21] + E_tse[32, 16] + E_tsw[35, 17]
                values[counter_v + 65] = E_tse[3, 17] + E_se[9, 21] - E_tse[13, 17] - E_se[19, 21] - E_se[32, 21] + E_tse[32, 17]
                values[counter_v + 66] = E_nw[22, 20] - E_tnw[0, 16] - E_tsw[3, 18] - E_sw[9, 22] - E_nw[6, 20] - E_nw[29, 20] + E_tnw[16, 16] - E_tsw[16, 18] - E_sw[22, 22] + E_tnw[29, 16] - E_sw[35, 22] + E_tsw[35, 18]
                values[counter_v + 67] = E_ne[6, 20] - E_nw[6, 21] + E_tne[0, 16] - E_tnw[0, 17] + E_ne[19, 20] + E_tse[3, 18] - E_tsw[3, 19] + E_se[9, 22] - E_sw[9, 23] + E_nw[22, 21] - E_ne[26, 20] + E_tne[13, 16] - E_tse[13, 18] - E_nw[29, 21] - E_se[19, 22] + E_tnw[16, 17] - E_tsw[16, 19] - E_sw[22, 23] + E_tne[26, 16] - E_se[32, 22] + E_tnw[29, 17] - E_sw[35, 23] + E_tse[32, 18] + E_tsw[35, 19]
                values[counter_v + 68] = E_ne[6, 21] + E_tne[0, 17] + E_ne[19, 21] + E_tse[3, 19] + E_se[9, 23] - E_ne[26, 21] + E_tne[13, 17] - E_tse[13, 19] - E_se[19, 23] + E_tne[26, 17] - E_se[32, 23] + E_tse[32, 19]
                values[counter_v + 69] = E_nw[22, 22] - E_tnw[0, 18] - E_nw[6, 22] - E_nw[29, 22] + E_tnw[16, 18] + E_tnw[29, 18]
                values[counter_v + 70] = E_ne[6, 22] - E_nw[6, 23] + E_tne[0, 18] - E_tnw[0, 19] + E_ne[19, 22] + E_nw[22, 23] - E_ne[26, 22] + E_tne[13, 18] - E_nw[29, 23] + E_tnw[16, 19] + E_tne[26, 18] + E_tnw[29, 19]
                values[counter_v + 71] = E_ne[6, 23] + E_tne[0, 19] + E_ne[19, 23] - E_ne[26, 23] + E_tne[13, 19] + E_tne[26, 19]
                values[counter_v + 72] = E_tsw[35, 20] - E_tsw[16, 20] - E_tsw[3, 20]
                values[counter_v + 73] = E_tse[3, 20] - E_tsw[3, 21] - E_tse[13, 20] - E_tsw[16, 21] + E_tse[32, 20] + E_tsw[35, 21]
                values[counter_v + 74] = E_tse[3, 21] - E_tse[13, 21] + E_tse[32, 21]
                values[counter_v + 75] = E_tnw[16, 20] - E_tsw[3, 22] - E_tnw[0, 20] - E_tsw[16, 22] + E_tnw[29, 20] + E_tsw[35, 22]
                values[counter_v + 76] = E_tne[0, 20] - E_tnw[0, 21] + E_tse[3, 22] - E_tsw[3, 23] + E_tne[13, 20] - E_tse[13, 22] + E_tnw[16, 21] - E_tsw[16, 23] + E_tne[26, 20] + E_tnw[29, 21] + E_tse[32, 22] + E_tsw[35, 23]
                values[counter_v + 77] = E_tne[0, 21] + E_tse[3, 23] + E_tne[13, 21] - E_tse[13, 23] + E_tne[26, 21] + E_tse[32, 23]
                values[counter_v + 78] = E_tnw[16, 22] - E_tnw[0, 22] + E_tnw[29, 22]
                values[counter_v + 79] = E_tne[0, 22] - E_tnw[0, 23] + E_tne[13, 22] + E_tnw[16, 23] + E_tne[26, 22] + E_tnw[29, 23]
                values[counter_v + 80] = E_tne[0, 23] + E_tne[13, 23] + E_tne[26, 23]

                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = len_xyz + len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = 2 * len_xyz + len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1

                counter_v += 81

    # divP_y
    for j in range(1, len_y - 1):
        for k in range(1, len_z - 1):

            # When dirichlet voxel skip node
            if not dir_cv[i, j, k, 1]:

                # Computing x and y divergence equations for P control volume
                E_sw = Emat[0, j - 1, k - 1]
                E_se = Emat[1, j - 1, k - 1]
                E_nw = Emat[0, j, k - 1]
                E_ne = Emat[1, j, k - 1]
                E_tsw = Emat[0, j - 1, k]
                E_tse = Emat[1, j - 1, k]
                E_tnw = Emat[0, j, k]
                E_tne = Emat[1, j, k]

                values[counter_v +  0] = - E_sw[11, 0] - E_sw[21, 0] - E_sw[34, 0]
                values[counter_v +  1] = E_se[11, 0] - E_sw[11, 1] - E_se[18, 0] - E_sw[21, 1] - E_se[31, 0] - E_sw[34, 1]
                values[counter_v +  2] = E_se[11, 1] - E_se[18, 1] - E_se[31, 1]
                values[counter_v +  3] = E_nw[21, 0] - E_nw[8, 0] - E_nw[28, 0] - E_sw[11, 2] - E_sw[21, 2] - E_sw[34, 2]
                values[counter_v +  4] = E_ne[8, 0] - E_nw[8, 1] + E_ne[18, 0] + E_nw[21, 1] - E_ne[25, 0] - E_nw[28, 1] + E_se[11, 2] - E_sw[11, 3] - E_se[18, 2] - E_sw[21, 3] - E_se[31, 2] - E_sw[34, 3]
                values[counter_v +  5] = E_ne[8, 1] + E_ne[18, 1] - E_ne[25, 1] + E_se[11, 3] - E_se[18, 3] - E_se[31, 3]
                values[counter_v +  6] = E_nw[21, 2] - E_nw[8, 2] - E_nw[28, 2]
                values[counter_v +  7] = E_ne[8, 2] - E_nw[8, 3] + E_ne[18, 2] + E_nw[21, 3] - E_ne[25, 2] - E_nw[28, 3]
                values[counter_v +  8] = E_ne[8, 3] + E_ne[18, 3] - E_ne[25, 3]
                values[counter_v +  9] = E_tsw[34, 0] - E_sw[21, 4] - E_sw[34, 4] - E_tsw[5, 0] - E_tsw[15, 0] - E_sw[11, 4]
                values[counter_v + 10] = E_se[11, 4] - E_sw[11, 5] - E_se[18, 4] - E_sw[21, 5] - E_se[31, 4] - E_sw[34, 5] + E_tse[5, 0] - E_tsw[5, 1] - E_tse[12, 0] - E_tsw[15, 1] + E_tse[31, 0] + E_tsw[34, 1]
                values[counter_v + 11] = E_se[11, 5] - E_se[18, 5] - E_se[31, 5] + E_tse[5, 1] - E_tse[12, 1] + E_tse[31, 1]
                values[counter_v + 12] = E_nw[21, 4] - E_nw[8, 4] - E_nw[28, 4] - E_sw[11, 6] - E_sw[21, 6] - E_sw[34, 6] - E_tnw[2, 0] - E_tsw[5, 2] + E_tnw[15, 0] - E_tsw[15, 2] + E_tnw[28, 0] + E_tsw[34, 2]
                values[counter_v + 13] = E_ne[8, 4] - E_nw[8, 5] + E_ne[18, 4] + E_nw[21, 5] - E_ne[25, 4] - E_nw[28, 5] + E_se[11, 6] - E_sw[11, 7] - E_se[18, 6] - E_sw[21, 7] - E_se[31, 6] + E_tne[2, 0] - E_sw[34, 7] - E_tnw[2, 1] + E_tse[5, 2] - E_tsw[5, 3] + E_tne[12, 0] - E_tse[12, 2] + E_tnw[15, 1] - E_tsw[15, 3] + E_tne[25, 0] + E_tnw[28, 1] + E_tse[31, 2] + E_tsw[34, 3]
                values[counter_v + 14] = E_ne[8, 5] + E_ne[18, 5] - E_ne[25, 5] + E_se[11, 7] - E_se[18, 7] - E_se[31, 7] + E_tne[2, 1] + E_tse[5, 3] + E_tne[12, 1] - E_tse[12, 3] + E_tne[25, 1] + E_tse[31, 3]
                values[counter_v + 15] = E_nw[21, 6] - E_nw[8, 6] - E_nw[28, 6] - E_tnw[2, 2] + E_tnw[15, 2] + E_tnw[28, 2]
                values[counter_v + 16] = E_ne[8, 6] - E_nw[8, 7] + E_ne[18, 6] + E_nw[21, 7] - E_ne[25, 6] - E_nw[28, 7] + E_tne[2, 2] - E_tnw[2, 3] + E_tne[12, 2] + E_tnw[15, 3] + E_tne[25, 2] + E_tnw[28, 3]
                values[counter_v + 17] = E_ne[8, 7] + E_ne[18, 7] - E_ne[25, 7] + E_tne[2, 3] + E_tne[12, 3] + E_tne[25, 3]
                values[counter_v + 18] = E_tsw[34, 4] - E_tsw[15, 4] - E_tsw[5, 4]
                values[counter_v + 19] = E_tse[5, 4] - E_tsw[5, 5] - E_tse[12, 4] - E_tsw[15, 5] + E_tse[31, 4] + E_tsw[34, 5]
                values[counter_v + 20] = E_tse[5, 5] - E_tse[12, 5] + E_tse[31, 5]
                values[counter_v + 21] = E_tnw[15, 4] - E_tsw[5, 6] - E_tnw[2, 4] - E_tsw[15, 6] + E_tnw[28, 4] + E_tsw[34, 6]
                values[counter_v + 22] = E_tne[2, 4] - E_tnw[2, 5] + E_tse[5, 6] - E_tsw[5, 7] + E_tne[12, 4] - E_tse[12, 6] + E_tnw[15, 5] - E_tsw[15, 7] + E_tne[25, 4] + E_tnw[28, 5] + E_tse[31, 6] + E_tsw[34, 7]
                values[counter_v + 23] = E_tne[2, 5] + E_tse[5, 7] + E_tne[12, 5] - E_tse[12, 7] + E_tne[25, 5] + E_tse[31, 7]
                values[counter_v + 24] = E_tnw[15, 6] - E_tnw[2, 6] + E_tnw[28, 6]
                values[counter_v + 25] = E_tne[2, 6] - E_tnw[2, 7] + E_tne[12, 6] + E_tnw[15, 7] + E_tne[25, 6] + E_tnw[28, 7]
                values[counter_v + 26] = E_tne[2, 7] + E_tne[12, 7] + E_tne[25, 7]
                values[counter_v + 27] = - E_sw[11, 8] - E_sw[21, 8] - E_sw[34, 8]
                values[counter_v + 28] = E_se[11, 8] - E_se[18, 8] - E_se[31, 8] - E_sw[11, 9] - E_sw[21, 9] - E_sw[34, 9]
                values[counter_v + 29] = E_se[11, 9] - E_se[18, 9] - E_se[31, 9]
                values[counter_v + 30] = E_nw[21, 8] - E_nw[8, 8] - E_nw[28, 8] - E_sw[11, 10] - E_sw[21, 10] - E_sw[34, 10]
                values[counter_v + 31] = E_ne[8, 8] + E_ne[18, 8] - E_ne[25, 8] - E_nw[8, 9] + E_nw[21, 9] + E_se[11, 10] - E_sw[11, 11] - E_nw[28, 9] - E_se[18, 10] - E_sw[21, 11] - E_se[31, 10] - E_sw[34, 11]
                values[counter_v + 32] = E_ne[8, 9] + E_ne[18, 9] + E_se[11, 11] - E_ne[25, 9] - E_se[18, 11] - E_se[31, 11]
                values[counter_v + 33] = E_nw[21, 10] - E_nw[8, 10] - E_nw[28, 10]
                values[counter_v + 34] = E_ne[8, 10] - E_nw[8, 11] + E_ne[18, 10] + E_nw[21, 11] - E_ne[25, 10] - E_nw[28, 11]
                values[counter_v + 35] = E_ne[8, 11] + E_ne[18, 11] - E_ne[25, 11]
                values[counter_v + 36] = E_tsw[34, 8] - E_tsw[15, 8] - E_tsw[5, 8] - E_sw[11, 12] - E_sw[21, 12] - E_sw[34, 12]
                values[counter_v + 37] = E_tse[5, 8] - E_tse[12, 8] + E_tse[31, 8] - E_tsw[5, 9] + E_se[11, 12] - E_sw[11, 13] - E_se[18, 12] - E_tsw[15, 9] - E_sw[21, 13] - E_se[31, 12] - E_sw[34, 13] + E_tsw[34, 9]
                values[counter_v + 38] = E_tse[5, 9] + E_se[11, 13] - E_tse[12, 9] - E_se[18, 13] - E_se[31, 13] + E_tse[31, 9]
                values[counter_v + 39] = E_tnw[15, 8] - E_tnw[2, 8] - E_nw[8, 12] + E_tnw[28, 8] + E_nw[21, 12] - E_tsw[5, 10] - E_sw[11, 14] - E_nw[28, 12] - E_tsw[15, 10] - E_sw[21, 14] - E_sw[34, 14] + E_tsw[34, 10]
                values[counter_v + 40] = E_ne[8, 12] - E_nw[8, 13] + E_tne[2, 8] + E_tne[12, 8] + E_tne[25, 8] + E_ne[18, 12] - E_tnw[2, 9] + E_tse[5, 10] + E_nw[21, 13] - E_tsw[5, 11] + E_se[11, 14] - E_sw[11, 15] - E_ne[25, 12] - E_tse[12, 10] - E_nw[28, 13] - E_se[18, 14] + E_tnw[15, 9] - E_tsw[15, 11] - E_sw[21, 15] - E_se[31, 14] + E_tnw[28, 9] - E_sw[34, 15] + E_tse[31, 10] + E_tsw[34, 11]
                values[counter_v + 41] = E_ne[8, 13] + E_ne[18, 13] + E_tne[2, 9] + E_tse[5, 11] + E_se[11, 15] - E_ne[25, 13] + E_tne[12, 9] - E_tse[12, 11] - E_se[18, 15] + E_tne[25, 9] - E_se[31, 15] + E_tse[31, 11]
                values[counter_v + 42] = E_nw[21, 14] - E_tnw[2, 10] - E_nw[8, 14] - E_nw[28, 14] + E_tnw[15, 10] + E_tnw[28, 10]
                values[counter_v + 43] = E_ne[8, 14] - E_nw[8, 15] + E_ne[18, 14] + E_tne[2, 10] - E_tnw[2, 11] + E_nw[21, 15] - E_ne[25, 14] + E_tne[12, 10] - E_nw[28, 15] + E_tnw[15, 11] + E_tne[25, 10] + E_tnw[28, 11]
                values[counter_v + 44] = E_ne[8, 15] + E_ne[18, 15] + E_tne[2, 11] - E_ne[25, 15] + E_tne[12, 11] + E_tne[25, 11]
                values[counter_v + 45] = E_tsw[34, 12] - E_tsw[15, 12] - E_tsw[5, 12]
                values[counter_v + 46] = E_tse[5, 12] - E_tsw[5, 13] - E_tse[12, 12] - E_tsw[15, 13] + E_tse[31, 12] + E_tsw[34, 13]
                values[counter_v + 47] = E_tse[5, 13] - E_tse[12, 13] + E_tse[31, 13]
                values[counter_v + 48] = E_tnw[15, 12] - E_tsw[5, 14] - E_tnw[2, 12] - E_tsw[15, 14] + E_tnw[28, 12] + E_tsw[34, 14]
                values[counter_v + 49] = E_tne[2, 12] - E_tnw[2, 13] + E_tse[5, 14] - E_tsw[5, 15] + E_tne[12, 12] - E_tse[12, 14] + E_tnw[15, 13] - E_tsw[15, 15] + E_tne[25, 12] + E_tnw[28, 13] + E_tse[31, 14] + E_tsw[34, 15]
                values[counter_v + 50] = E_tne[2, 13] + E_tse[5, 15] + E_tne[12, 13] - E_tse[12, 15] + E_tne[25, 13] + E_tse[31, 15]
                values[counter_v + 51] = E_tnw[15, 14] - E_tnw[2, 14] + E_tnw[28, 14]
                values[counter_v + 52] = E_tne[2, 14] - E_tnw[2, 15] + E_tne[12, 14] + E_tnw[15, 15] + E_tne[25, 14] + E_tnw[28, 15]
                values[counter_v + 53] = E_tne[2, 15] + E_tne[12, 15] + E_tne[25, 15]
                values[counter_v + 54] = - E_sw[11, 16] - E_sw[21, 16] - E_sw[34, 16]
                values[counter_v + 55] = E_se[11, 16] - E_sw[11, 17] - E_se[18, 16] - E_sw[21, 17] - E_se[31, 16] - E_sw[34, 17]
                values[counter_v + 56] = E_se[11, 17] - E_se[18, 17] - E_se[31, 17]
                values[counter_v + 57] = E_nw[21, 16] - E_nw[8, 16] - E_sw[11, 18] - E_nw[28, 16] - E_sw[21, 18] - E_sw[34, 18]
                values[counter_v + 58] = E_ne[8, 16] - E_nw[8, 17] + E_ne[18, 16] + E_nw[21, 17] + E_se[11, 18] - E_sw[11, 19] - E_ne[25, 16] - E_nw[28, 17] - E_se[18, 18] - E_sw[21, 19] - E_se[31, 18] - E_sw[34, 19]
                values[counter_v + 59] = E_ne[8, 17] + E_ne[18, 17] + E_se[11, 19] - E_ne[25, 17] - E_se[18, 19] - E_se[31, 19]
                values[counter_v + 60] = E_nw[21, 18] - E_nw[8, 18] - E_nw[28, 18]
                values[counter_v + 61] = E_ne[8, 18] - E_nw[8, 19] + E_ne[18, 18] + E_nw[21, 19] - E_ne[25, 18] - E_nw[28, 19]
                values[counter_v + 62] = E_ne[8, 19] + E_ne[18, 19] - E_ne[25, 19]
                values[counter_v + 63] = E_tsw[34, 16] - E_sw[11, 20] - E_tsw[15, 16] - E_sw[21, 20] - E_sw[34, 20] - E_tsw[5, 16]
                values[counter_v + 64] = E_tse[5, 16] - E_tsw[5, 17] + E_se[11, 20] - E_sw[11, 21] - E_tse[12, 16] - E_se[18, 20] - E_tsw[15, 17] - E_sw[21, 21] - E_se[31, 20] - E_sw[34, 21] + E_tse[31, 16] + E_tsw[34, 17]
                values[counter_v + 65] = E_tse[5, 17] + E_se[11, 21] - E_tse[12, 17] - E_se[18, 21] - E_se[31, 21] + E_tse[31, 17]
                values[counter_v + 66] = E_nw[21, 20] - E_tnw[2, 16] - E_nw[8, 20] - E_tsw[5, 18] - E_sw[11, 22] - E_nw[28, 20] + E_tnw[15, 16] - E_tsw[15, 18] - E_sw[21, 22] + E_tnw[28, 16] - E_sw[34, 22] + E_tsw[34, 18]
                values[counter_v + 67] = E_ne[8, 20] - E_nw[8, 21] + E_ne[18, 20] + E_tne[2, 16] - E_tnw[2, 17] + E_tse[5, 18] + E_nw[21, 21] - E_tsw[5, 19] + E_se[11, 22] - E_sw[11, 23] - E_ne[25, 20] + E_tne[12, 16] - E_tse[12, 18] - E_nw[28, 21] - E_se[18, 22] + E_tnw[15, 17] - E_tsw[15, 19] - E_sw[21, 23] + E_tne[25, 16] - E_se[31, 22] + E_tnw[28, 17] - E_sw[34, 23] + E_tse[31, 18] + E_tsw[34, 19]
                values[counter_v + 68] = E_ne[8, 21] + E_ne[18, 21] + E_tne[2, 17] + E_tse[5, 19] + E_se[11, 23] - E_ne[25, 21] + E_tne[12, 17] - E_tse[12, 19] - E_se[18, 23] + E_tne[25, 17] - E_se[31, 23] + E_tse[31, 19]
                values[counter_v + 69] = E_nw[21, 22] - E_tnw[2, 18] - E_nw[8, 22] - E_nw[28, 22] + E_tnw[15, 18] + E_tnw[28, 18]
                values[counter_v + 70] = E_ne[8, 22] - E_nw[8, 23] + E_ne[18, 22] + E_tne[2, 18] - E_tnw[2, 19] + E_nw[21, 23] - E_ne[25, 22] + E_tne[12, 18] - E_nw[28, 23] + E_tnw[15, 19] + E_tne[25, 18] + E_tnw[28, 19]
                values[counter_v + 71] = E_ne[8, 23] + E_ne[18, 23] + E_tne[2, 19] - E_ne[25, 23] + E_tne[12, 19] + E_tne[25, 19]
                values[counter_v + 72] = E_tsw[34, 20] - E_tsw[15, 20] - E_tsw[5, 20]
                values[counter_v + 73] = E_tse[5, 20] - E_tsw[5, 21] - E_tse[12, 20] - E_tsw[15, 21] + E_tse[31, 20] + E_tsw[34, 21]
                values[counter_v + 74] = E_tse[5, 21] - E_tse[12, 21] + E_tse[31, 21]
                values[counter_v + 75] = E_tnw[15, 20] - E_tsw[5, 22] - E_tnw[2, 20] - E_tsw[15, 22] + E_tnw[28, 20] + E_tsw[34, 22]
                values[counter_v + 76] = E_tne[2, 20] - E_tnw[2, 21] + E_tse[5, 22] - E_tsw[5, 23] + E_tne[12, 20] - E_tse[12, 22] + E_tnw[15, 21] - E_tsw[15, 23] + E_tne[25, 20] + E_tnw[28, 21] + E_tse[31, 22] + E_tsw[34, 23]
                values[counter_v + 77] = E_tne[2, 21] + E_tse[5, 23] + E_tne[12, 21] - E_tse[12, 23] + E_tne[25, 21] + E_tse[31, 23]
                values[counter_v + 78] = E_tnw[15, 22] - E_tnw[2, 22] + E_tnw[28, 22]
                values[counter_v + 79] = E_tne[2, 22] - E_tnw[2, 23] + E_tne[12, 22] + E_tnw[15, 23] + E_tne[25, 22] + E_tnw[28, 23]
                values[counter_v + 80] = E_tne[2, 23] + E_tne[12, 23] + E_tne[25, 23]

                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = len_xyz + len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = 2 * len_xyz + len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1

                counter_v += 81

    # divP_z
    for j in range(1, len_y - 1):
        for k in range(1, len_z - 1):

            # When dirichlet voxel skip node
            if not dir_cv[i, j, k, 2]:

                # Computing x and y divergence equations for P control volume
                E_sw = Emat[0, j - 1, k - 1]
                E_se = Emat[1, j - 1, k - 1]
                E_nw = Emat[0, j, k - 1]
                E_ne = Emat[1, j, k - 1]
                E_tsw = Emat[0, j - 1, k]
                E_tse = Emat[1, j - 1, k]
                E_tnw = Emat[0, j, k]
                E_tne = Emat[1, j, k]

                values[counter_v +  0] = - E_sw[10, 0] - E_sw[23, 0] - E_sw[33, 0]
                values[counter_v +  1] = E_se[10, 0] - E_sw[10, 1] - E_se[20, 0] - E_sw[23, 1] - E_se[30, 0] - E_sw[33, 1]
                values[counter_v +  2] = E_se[10, 1] - E_se[20, 1] - E_se[30, 1]
                values[counter_v +  3] = E_nw[23, 0] - E_nw[7, 0] - E_nw[27, 0] - E_sw[10, 2] - E_sw[23, 2] - E_sw[33, 2]
                values[counter_v +  4] = E_ne[7, 0] - E_nw[7, 1] + E_ne[20, 0] - E_ne[24, 0] + E_nw[23, 1] - E_nw[27, 1] + E_se[10, 2] - E_sw[10, 3] - E_se[20, 2] - E_sw[23, 3] - E_se[30, 2] - E_sw[33, 3]
                values[counter_v +  5] = E_ne[7, 1] + E_ne[20, 1] - E_ne[24, 1] + E_se[10, 3] - E_se[20, 3] - E_se[30, 3]
                values[counter_v +  6] = E_nw[23, 2] - E_nw[7, 2] - E_nw[27, 2]
                values[counter_v +  7] = E_ne[7, 2] - E_nw[7, 3] + E_ne[20, 2] - E_ne[24, 2] + E_nw[23, 3] - E_nw[27, 3]
                values[counter_v +  8] = E_ne[7, 3] + E_ne[20, 3] - E_ne[24, 3]
                values[counter_v +  9] = E_tsw[33, 0] - E_sw[23, 4] - E_sw[33, 4] - E_tsw[4, 0] - E_tsw[17, 0] - E_sw[10, 4]
                values[counter_v + 10] = E_se[10, 4] - E_sw[10, 5] - E_se[20, 4] - E_sw[23, 5] - E_se[30, 4] - E_sw[33, 5] + E_tse[4, 0] - E_tsw[4, 1] - E_tse[14, 0] - E_tsw[17, 1] + E_tse[30, 0] + E_tsw[33, 1]
                values[counter_v + 11] = E_se[10, 5] - E_se[20, 5] - E_se[30, 5] + E_tse[4, 1] - E_tse[14, 1] + E_tse[30, 1]
                values[counter_v + 12] = E_nw[23, 4] - E_nw[7, 4] - E_nw[27, 4] - E_sw[10, 6] - E_sw[23, 6] - E_sw[33, 6] - E_tnw[1, 0] - E_tsw[4, 2] + E_tnw[17, 0] - E_tsw[17, 2] + E_tnw[27, 0] + E_tsw[33, 2]
                values[counter_v + 13] = E_ne[7, 4] - E_nw[7, 5] + E_ne[20, 4] - E_ne[24, 4] + E_nw[23, 5] - E_nw[27, 5] + E_se[10, 6] - E_sw[10, 7] - E_se[20, 6] - E_sw[23, 7] - E_se[30, 6] + E_tne[1, 0] - E_sw[33, 7] - E_tnw[1, 1] + E_tse[4, 2] - E_tsw[4, 3] + E_tne[14, 0] - E_tse[14, 2] + E_tnw[17, 1] - E_tsw[17, 3] + E_tne[24, 0] + E_tnw[27, 1] + E_tse[30, 2] + E_tsw[33, 3]
                values[counter_v + 14] = E_ne[7, 5] + E_ne[20, 5] - E_ne[24, 5] + E_se[10, 7] - E_se[20, 7] - E_se[30, 7] + E_tne[1, 1] + E_tse[4, 3] + E_tne[14, 1] - E_tse[14, 3] + E_tne[24, 1] + E_tse[30, 3]
                values[counter_v + 15] = E_nw[23, 6] - E_nw[7, 6] - E_nw[27, 6] - E_tnw[1, 2] + E_tnw[17, 2] + E_tnw[27, 2]
                values[counter_v + 16] = E_ne[7, 6] - E_nw[7, 7] + E_ne[20, 6] - E_ne[24, 6] + E_nw[23, 7] - E_nw[27, 7] + E_tne[1, 2] - E_tnw[1, 3] + E_tne[14, 2] + E_tnw[17, 3] + E_tne[24, 2] + E_tnw[27, 3]
                values[counter_v + 17] = E_ne[7, 7] + E_ne[20, 7] - E_ne[24, 7] + E_tne[1, 3] + E_tne[14, 3] + E_tne[24, 3]
                values[counter_v + 18] = E_tsw[33, 4] - E_tsw[17, 4] - E_tsw[4, 4]
                values[counter_v + 19] = E_tse[4, 4] - E_tsw[4, 5] - E_tse[14, 4] - E_tsw[17, 5] + E_tse[30, 4] + E_tsw[33, 5]
                values[counter_v + 20] = E_tse[4, 5] - E_tse[14, 5] + E_tse[30, 5]
                values[counter_v + 21] = E_tnw[17, 4] - E_tsw[4, 6] - E_tnw[1, 4] - E_tsw[17, 6] + E_tnw[27, 4] + E_tsw[33, 6]
                values[counter_v + 22] = E_tne[1, 4] - E_tnw[1, 5] + E_tse[4, 6] - E_tsw[4, 7] + E_tne[14, 4] - E_tse[14, 6] + E_tnw[17, 5] - E_tsw[17, 7] + E_tne[24, 4] + E_tnw[27, 5] + E_tse[30, 6] + E_tsw[33, 7]
                values[counter_v + 23] = E_tne[1, 5] + E_tse[4, 7] + E_tne[14, 5] - E_tse[14, 7] + E_tne[24, 5] + E_tse[30, 7]
                values[counter_v + 24] = E_tnw[17, 6] - E_tnw[1, 6] + E_tnw[27, 6]
                values[counter_v + 25] = E_tne[1, 6] - E_tnw[1, 7] + E_tne[14, 6] + E_tnw[17, 7] + E_tne[24, 6] + E_tnw[27, 7]
                values[counter_v + 26] = E_tne[1, 7] + E_tne[14, 7] + E_tne[24, 7]
                values[counter_v + 27] = - E_sw[10, 8] - E_sw[23, 8] - E_sw[33, 8]
                values[counter_v + 28] = E_se[10, 8] - E_se[20, 8] - E_se[30, 8] - E_sw[10, 9] - E_sw[23, 9] - E_sw[33, 9]
                values[counter_v + 29] = E_se[10, 9] - E_se[20, 9] - E_se[30, 9]
                values[counter_v + 30] = E_nw[23, 8] - E_nw[7, 8] - E_nw[27, 8] - E_sw[10, 10] - E_sw[23, 10] - E_sw[33, 10]
                values[counter_v + 31] = E_ne[7, 8] + E_ne[20, 8] - E_ne[24, 8] - E_nw[7, 9] + E_se[10, 10] - E_sw[10, 11] + E_nw[23, 9] - E_nw[27, 9] - E_se[20, 10] - E_sw[23, 11] - E_se[30, 10] - E_sw[33, 11]
                values[counter_v + 32] = E_ne[7, 9] + E_ne[20, 9] + E_se[10, 11] - E_ne[24, 9] - E_se[20, 11] - E_se[30, 11]
                values[counter_v + 33] = E_nw[23, 10] - E_nw[7, 10] - E_nw[27, 10]
                values[counter_v + 34] = E_ne[7, 10] - E_nw[7, 11] + E_ne[20, 10] + E_nw[23, 11] - E_ne[24, 10] - E_nw[27, 11]
                values[counter_v + 35] = E_ne[7, 11] + E_ne[20, 11] - E_ne[24, 11]
                values[counter_v + 36] = E_tsw[33, 8] - E_tsw[17, 8] - E_tsw[4, 8] - E_sw[10, 12] - E_sw[23, 12] - E_sw[33, 12]
                values[counter_v + 37] = E_tse[4, 8] - E_tse[14, 8] + E_tse[30, 8] - E_tsw[4, 9] + E_se[10, 12] - E_sw[10, 13] - E_se[20, 12] - E_tsw[17, 9] - E_sw[23, 13] - E_se[30, 12] - E_sw[33, 13] + E_tsw[33, 9]
                values[counter_v + 38] = E_tse[4, 9] + E_se[10, 13] - E_tse[14, 9] - E_se[20, 13] - E_se[30, 13] + E_tse[30, 9]
                values[counter_v + 39] = E_tnw[17, 8] - E_tnw[1, 8] - E_nw[7, 12] + E_tnw[27, 8] - E_tsw[4, 10] - E_sw[10, 14] + E_nw[23, 12] - E_nw[27, 12] - E_tsw[17, 10] - E_sw[23, 14] - E_sw[33, 14] + E_tsw[33, 10]
                values[counter_v + 40] = E_ne[7, 12] - E_nw[7, 13] + E_tne[1, 8] + E_tne[14, 8] - E_tnw[1, 9] + E_tne[24, 8] + E_ne[20, 12] + E_tse[4, 10] - E_tsw[4, 11] + E_se[10, 14] - E_sw[10, 15] + E_nw[23, 13] - E_ne[24, 12] - E_nw[27, 13] - E_tse[14, 10] - E_se[20, 14] + E_tnw[17, 9] - E_tsw[17, 11] - E_sw[23, 15] - E_se[30, 14] + E_tnw[27, 9] - E_sw[33, 15] + E_tse[30, 10] + E_tsw[33, 11]
                values[counter_v + 41] = E_ne[7, 13] + E_tne[1, 9] + E_ne[20, 13] + E_tse[4, 11] + E_se[10, 15] - E_ne[24, 13] + E_tne[14, 9] - E_tse[14, 11] - E_se[20, 15] + E_tne[24, 9] - E_se[30, 15] + E_tse[30, 11]
                values[counter_v + 42] = E_nw[23, 14] - E_tnw[1, 10] - E_nw[7, 14] - E_nw[27, 14] + E_tnw[17, 10] + E_tnw[27, 10]
                values[counter_v + 43] = E_ne[7, 14] - E_nw[7, 15] + E_tne[1, 10] - E_tnw[1, 11] + E_ne[20, 14] + E_nw[23, 15] - E_ne[24, 14] - E_nw[27, 15] + E_tne[14, 10] + E_tnw[17, 11] + E_tne[24, 10] + E_tnw[27, 11]
                values[counter_v + 44] = E_ne[7, 15] + E_tne[1, 11] + E_ne[20, 15] - E_ne[24, 15] + E_tne[14, 11] + E_tne[24, 11]
                values[counter_v + 45] = E_tsw[33, 12] - E_tsw[17, 12] - E_tsw[4, 12]
                values[counter_v + 46] = E_tse[4, 12] - E_tsw[4, 13] - E_tse[14, 12] - E_tsw[17, 13] + E_tse[30, 12] + E_tsw[33, 13]
                values[counter_v + 47] = E_tse[4, 13] - E_tse[14, 13] + E_tse[30, 13]
                values[counter_v + 48] = E_tnw[17, 12] - E_tsw[4, 14] - E_tnw[1, 12] - E_tsw[17, 14] + E_tnw[27, 12] + E_tsw[33, 14]
                values[counter_v + 49] = E_tne[1, 12] - E_tnw[1, 13] + E_tse[4, 14] - E_tsw[4, 15] + E_tne[14, 12] - E_tse[14, 14] + E_tnw[17, 13] - E_tsw[17, 15] + E_tne[24, 12] + E_tnw[27, 13] + E_tse[30, 14] + E_tsw[33, 15]
                values[counter_v + 50] = E_tne[1, 13] + E_tse[4, 15] + E_tne[14, 13] - E_tse[14, 15] + E_tne[24, 13] + E_tse[30, 15]
                values[counter_v + 51] = E_tnw[17, 14] - E_tnw[1, 14] + E_tnw[27, 14]
                values[counter_v + 52] = E_tne[1, 14] - E_tnw[1, 15] + E_tne[14, 14] + E_tnw[17, 15] + E_tne[24, 14] + E_tnw[27, 15]
                values[counter_v + 53] = E_tne[1, 15] + E_tne[14, 15] + E_tne[24, 15]
                values[counter_v + 54] = - E_sw[10, 16] - E_sw[23, 16] - E_sw[33, 16]
                values[counter_v + 55] = E_se[10, 16] - E_sw[10, 17] - E_se[20, 16] - E_sw[23, 17] - E_se[30, 16] - E_sw[33, 17]
                values[counter_v + 56] = E_se[10, 17] - E_se[20, 17] - E_se[30, 17]
                values[counter_v + 57] = E_nw[23, 16] - E_sw[10, 18] - E_nw[7, 16] - E_nw[27, 16] - E_sw[23, 18] - E_sw[33, 18]
                values[counter_v + 58] = E_ne[7, 16] - E_nw[7, 17] + E_ne[20, 16] + E_se[10, 18] - E_sw[10, 19] + E_nw[23, 17] - E_ne[24, 16] - E_nw[27, 17] - E_se[20, 18] - E_sw[23, 19] - E_se[30, 18] - E_sw[33, 19]
                values[counter_v + 59] = E_ne[7, 17] + E_ne[20, 17] + E_se[10, 19] - E_ne[24, 17] - E_se[20, 19] - E_se[30, 19]
                values[counter_v + 60] = E_nw[23, 18] - E_nw[7, 18] - E_nw[27, 18]
                values[counter_v + 61] = E_ne[7, 18] - E_nw[7, 19] + E_ne[20, 18] + E_nw[23, 19] - E_ne[24, 18] - E_nw[27, 19]
                values[counter_v + 62] = E_ne[7, 19] + E_ne[20, 19] - E_ne[24, 19]
                values[counter_v + 63] = E_tsw[33, 16] - E_sw[10, 20] - E_tsw[17, 16] - E_sw[23, 20] - E_sw[33, 20] - E_tsw[4, 16]
                values[counter_v + 64] = E_tse[4, 16] - E_tsw[4, 17] + E_se[10, 20] - E_sw[10, 21] - E_tse[14, 16] - E_se[20, 20] - E_tsw[17, 17] - E_sw[23, 21] - E_se[30, 20] - E_sw[33, 21] + E_tse[30, 16] + E_tsw[33, 17]
                values[counter_v + 65] = E_tse[4, 17] + E_se[10, 21] - E_tse[14, 17] - E_se[20, 21] - E_se[30, 21] + E_tse[30, 17]
                values[counter_v + 66] = E_nw[23, 20] - E_tnw[1, 16] - E_tsw[4, 18] - E_sw[10, 22] - E_nw[7, 20] - E_nw[27, 20] + E_tnw[17, 16] - E_tsw[17, 18] - E_sw[23, 22] + E_tnw[27, 16] - E_sw[33, 22] + E_tsw[33, 18]
                values[counter_v + 67] = E_ne[7, 20] - E_nw[7, 21] + E_tne[1, 16] - E_tnw[1, 17] + E_ne[20, 20] + E_tse[4, 18] - E_tsw[4, 19] + E_se[10, 22] - E_sw[10, 23] + E_nw[23, 21] - E_ne[24, 20] - E_nw[27, 21] + E_tne[14, 16] - E_tse[14, 18] - E_se[20, 22] + E_tnw[17, 17] - E_tsw[17, 19] - E_sw[23, 23] + E_tne[24, 16] - E_se[30, 22] + E_tnw[27, 17] - E_sw[33, 23] + E_tse[30, 18] + E_tsw[33, 19]
                values[counter_v + 68] = E_ne[7, 21] + E_tne[1, 17] + E_ne[20, 21] + E_tse[4, 19] + E_se[10, 23] - E_ne[24, 21] + E_tne[14, 17] - E_tse[14, 19] - E_se[20, 23] + E_tne[24, 17] - E_se[30, 23] + E_tse[30, 19]
                values[counter_v + 69] = E_nw[23, 22] - E_tnw[1, 18] - E_nw[7, 22] - E_nw[27, 22] + E_tnw[17, 18] + E_tnw[27, 18]
                values[counter_v + 70] = E_ne[7, 22] - E_nw[7, 23] + E_tne[1, 18] - E_tnw[1, 19] + E_ne[20, 22] + E_nw[23, 23] - E_ne[24, 22] - E_nw[27, 23] + E_tne[14, 18] + E_tnw[17, 19] + E_tne[24, 18] + E_tnw[27, 19]
                values[counter_v + 71] = E_ne[7, 23] + E_tne[1, 19] + E_ne[20, 23] - E_ne[24, 23] + E_tne[14, 19] + E_tne[24, 19]
                values[counter_v + 72] = E_tsw[33, 20] - E_tsw[17, 20] - E_tsw[4, 20]
                values[counter_v + 73] = E_tse[4, 20] - E_tsw[4, 21] - E_tse[14, 20] - E_tsw[17, 21] + E_tse[30, 20] + E_tsw[33, 21]
                values[counter_v + 74] = E_tse[4, 21] - E_tse[14, 21] + E_tse[30, 21]
                values[counter_v + 75] = E_tnw[17, 20] - E_tsw[4, 22] - E_tnw[1, 20] - E_tsw[17, 22] + E_tnw[27, 20] + E_tsw[33, 22]
                values[counter_v + 76] = E_tne[1, 20] - E_tnw[1, 21] + E_tse[4, 22] - E_tsw[4, 23] + E_tne[14, 20] - E_tse[14, 22] + E_tnw[17, 21] - E_tsw[17, 23] + E_tne[24, 20] + E_tnw[27, 21] + E_tse[30, 22] + E_tsw[33, 23]
                values[counter_v + 77] = E_tne[1, 21] + E_tse[4, 23] + E_tne[14, 21] - E_tse[14, 23] + E_tne[24, 21] + E_tse[30, 23]
                values[counter_v + 78] = E_tnw[17, 22] - E_tnw[1, 22] + E_tnw[27, 22]
                values[counter_v + 79] = E_tne[1, 22] - E_tnw[1, 23] + E_tne[14, 22] + E_tnw[17, 23] + E_tne[24, 22] + E_tnw[27, 23]
                values[counter_v + 80] = E_tne[1, 23] + E_tne[14, 23] + E_tne[24, 23]

                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = len_xyz + len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(-1, 2):
                            j_indices[counter_j] = 2 * len_xyz + len_x * (len_y * (k + k2) + (j + j2)) + (i + i2)
                            counter_j += 1

                counter_v += 81


def fill_stress_matrices(int i, int len_x, int len_y, int len_z, double [:,:,:,:] u, double [:,:,:,:,:] Emat,
                        double [:,:,:,:] E_sw, double [:,:,:,:] E_se, double [:,:,:,:] E_nw, double [:,:,:,:] E_ne,
                        double [:,:,:,:] E_tsw, double [:,:,:,:] E_tse, double [:,:,:,:] E_tnw, double [:,:,:,:] E_tne,
                        double [:,:,:] u_sw, double [:,:,:] u_se, double [:,:,:] u_nw, double [:,:,:] u_ne,
                        double [:,:,:] u_tsw, double [:,:,:] u_tse, double [:,:,:] u_tnw, double [:,:,:] u_tne):

    cdef int j, k, i2, j2, k2, d, counter
    cdef double[81] u_local

    for j in range(1, len_y - 1):
        for k in range(1, len_z - 1):

            # local u keeping order ux_SW, ux_S, ux_SE, ux_W, ux_P, ux_E, ux_NW, ux_N, ux_NE, uy_SW...
            counter = 0
            for d in range(3):
                for k2 in range(-1, 2):
                    for j2 in range(-1, 2):
                        for i2 in range(3):
                            u_local[counter] = u[i2, j+j2, k+k2, d]
                            counter += 1

            ux_bsw, ux_bs, ux_bse, ux_bw, ux_b, ux_be, ux_bnw, ux_bn, ux_bne, \
            ux_sw, ux_s, ux_se, ux_w, ux_p, ux_e, ux_nw, ux_n, ux_ne, \
            ux_tsw, ux_ts, ux_tse, ux_tw, ux_t, ux_te, ux_tnw, ux_tn, ux_tne, \
            uy_bsw, uy_bs, uy_bse, uy_bw, uy_b, uy_be, uy_bnw, uy_bn, uy_bne, \
            uy_sw, uy_s, uy_se, uy_w, uy_p, uy_e, uy_nw, uy_n, uy_ne, \
            uy_tsw, uy_ts, uy_tse, uy_tw, uy_t, uy_te, uy_tnw, uy_tn, uy_tne, \
            uz_bsw, uz_bs, uz_bse, uz_bw, uz_b, uz_be, uz_bnw, uz_bn, uz_bne, \
            uz_sw, uz_s, uz_se, uz_w, uz_p, uz_e, uz_nw, uz_n, uz_ne, \
            uz_tsw, uz_ts, uz_tse, uz_tw, uz_t, uz_te, uz_tnw, uz_tn, uz_tne = u_local

            # Eight IVs
            u_sw[j - 1, k - 1, 0] = ux_bsw
            u_sw[j - 1, k - 1, 1] = ux_bs
            u_sw[j - 1, k - 1, 2] = ux_bw
            u_sw[j - 1, k - 1, 3] = ux_b
            u_sw[j - 1, k - 1, 4] = ux_sw
            u_sw[j - 1, k - 1, 5] = ux_s
            u_sw[j - 1, k - 1, 6] = ux_w
            u_sw[j - 1, k - 1, 7] = ux_p
            u_sw[j - 1, k - 1, 8] = uy_bsw
            u_sw[j - 1, k - 1, 9] = uy_bs
            u_sw[j - 1, k - 1, 10] = uy_bw
            u_sw[j - 1, k - 1, 11] = uy_b
            u_sw[j - 1, k - 1, 12] = uy_sw
            u_sw[j - 1, k - 1, 13] = uy_s
            u_sw[j - 1, k - 1, 14] = uy_w
            u_sw[j - 1, k - 1, 15] = uy_p
            u_sw[j - 1, k - 1, 16] = uz_bsw
            u_sw[j - 1, k - 1, 17] = uz_bs
            u_sw[j - 1, k - 1, 18] = uz_bw
            u_sw[j - 1, k - 1, 19] = uz_b
            u_sw[j - 1, k - 1, 20] = uz_sw
            u_sw[j - 1, k - 1, 21] = uz_s
            u_sw[j - 1, k - 1, 22] = uz_w
            u_sw[j - 1, k - 1, 23] = uz_p

            u_se[j - 1, k - 1, 0] =  ux_bs
            u_se[j - 1, k - 1, 1] =  ux_bse
            u_se[j - 1, k - 1, 2] =  ux_b
            u_se[j - 1, k - 1, 3] =  ux_be
            u_se[j - 1, k - 1, 4] =  ux_s
            u_se[j - 1, k - 1, 5] =  ux_se
            u_se[j - 1, k - 1, 6] =  ux_p
            u_se[j - 1, k - 1, 7] =  ux_e
            u_se[j - 1, k - 1, 8] =  uy_bs
            u_se[j - 1, k - 1, 9] =  uy_bse
            u_se[j - 1, k - 1, 10] = uy_b
            u_se[j - 1, k - 1, 11] = uy_be
            u_se[j - 1, k - 1, 12] = uy_s
            u_se[j - 1, k - 1, 13] = uy_se
            u_se[j - 1, k - 1, 14] = uy_p
            u_se[j - 1, k - 1, 15] = uy_e
            u_se[j - 1, k - 1, 16] = uz_bs
            u_se[j - 1, k - 1, 17] = uz_bse
            u_se[j - 1, k - 1, 18] = uz_b
            u_se[j - 1, k - 1, 19] = uz_be
            u_se[j - 1, k - 1, 20] = uz_s
            u_se[j - 1, k - 1, 21] = uz_se
            u_se[j - 1, k - 1, 22] = uz_p
            u_se[j - 1, k - 1, 23] = uz_e

            u_nw[j - 1, k - 1, 0] =  ux_bw
            u_nw[j - 1, k - 1, 1] =  ux_b
            u_nw[j - 1, k - 1, 2] =  ux_bnw
            u_nw[j - 1, k - 1, 3] =  ux_bn
            u_nw[j - 1, k - 1, 4] =  ux_w
            u_nw[j - 1, k - 1, 5] =  ux_p
            u_nw[j - 1, k - 1, 6] =  ux_nw
            u_nw[j - 1, k - 1, 7] =  ux_n
            u_nw[j - 1, k - 1, 8] =  uy_bw
            u_nw[j - 1, k - 1, 9] =  uy_b
            u_nw[j - 1, k - 1, 10] = uy_bnw
            u_nw[j - 1, k - 1, 11] = uy_bn
            u_nw[j - 1, k - 1, 12] = uy_w
            u_nw[j - 1, k - 1, 13] = uy_p
            u_nw[j - 1, k - 1, 14] = uy_nw
            u_nw[j - 1, k - 1, 15] = uy_n
            u_nw[j - 1, k - 1, 16] = uz_bw
            u_nw[j - 1, k - 1, 17] = uz_b
            u_nw[j - 1, k - 1, 18] = uz_bnw
            u_nw[j - 1, k - 1, 19] = uz_bn
            u_nw[j - 1, k - 1, 20] = uz_w
            u_nw[j - 1, k - 1, 21] = uz_p
            u_nw[j - 1, k - 1, 22] = uz_nw
            u_nw[j - 1, k - 1, 23] = uz_n

            u_ne[j - 1, k - 1, 0] =  ux_b
            u_ne[j - 1, k - 1, 1] =  ux_be
            u_ne[j - 1, k - 1, 2] =  ux_bn
            u_ne[j - 1, k - 1, 3] =  ux_bne
            u_ne[j - 1, k - 1, 4] =  ux_p
            u_ne[j - 1, k - 1, 5] =  ux_e
            u_ne[j - 1, k - 1, 6] =  ux_n
            u_ne[j - 1, k - 1, 7] =  ux_ne
            u_ne[j - 1, k - 1, 8] =  uy_b
            u_ne[j - 1, k - 1, 9] =  uy_be
            u_ne[j - 1, k - 1, 10] = uy_bn
            u_ne[j - 1, k - 1, 11] = uy_bne
            u_ne[j - 1, k - 1, 12] = uy_p
            u_ne[j - 1, k - 1, 13] = uy_e
            u_ne[j - 1, k - 1, 14] = uy_n
            u_ne[j - 1, k - 1, 15] = uy_ne
            u_ne[j - 1, k - 1, 16] = uz_b
            u_ne[j - 1, k - 1, 17] = uz_be
            u_ne[j - 1, k - 1, 18] = uz_bn
            u_ne[j - 1, k - 1, 19] = uz_bne
            u_ne[j - 1, k - 1, 20] = uz_p
            u_ne[j - 1, k - 1, 21] = uz_e
            u_ne[j - 1, k - 1, 22] = uz_n
            u_ne[j - 1, k - 1, 23] = uz_ne

            u_tsw[j - 1, k - 1, 0] =  ux_sw
            u_tsw[j - 1, k - 1, 1] =  ux_s
            u_tsw[j - 1, k - 1, 2] =  ux_w
            u_tsw[j - 1, k - 1, 3] =  ux_p
            u_tsw[j - 1, k - 1, 4] =  ux_tsw
            u_tsw[j - 1, k - 1, 5] =  ux_ts
            u_tsw[j - 1, k - 1, 6] =  ux_tw
            u_tsw[j - 1, k - 1, 7] =  ux_t
            u_tsw[j - 1, k - 1, 8] =  uy_sw
            u_tsw[j - 1, k - 1, 9] =  uy_s
            u_tsw[j - 1, k - 1, 10] = uy_w
            u_tsw[j - 1, k - 1, 11] = uy_p
            u_tsw[j - 1, k - 1, 12] = uy_tsw
            u_tsw[j - 1, k - 1, 13] = uy_ts
            u_tsw[j - 1, k - 1, 14] = uy_tw
            u_tsw[j - 1, k - 1, 15] = uy_t
            u_tsw[j - 1, k - 1, 16] = uz_sw
            u_tsw[j - 1, k - 1, 17] = uz_s
            u_tsw[j - 1, k - 1, 18] = uz_w
            u_tsw[j - 1, k - 1, 19] = uz_p
            u_tsw[j - 1, k - 1, 20] = uz_tsw
            u_tsw[j - 1, k - 1, 21] = uz_ts
            u_tsw[j - 1, k - 1, 22] = uz_tw
            u_tsw[j - 1, k - 1, 23] = uz_t

            u_tse[j - 1, k - 1, 0] =  ux_s
            u_tse[j - 1, k - 1, 1] =  ux_se
            u_tse[j - 1, k - 1, 2] =  ux_p
            u_tse[j - 1, k - 1, 3] =  ux_e
            u_tse[j - 1, k - 1, 4] =  ux_ts
            u_tse[j - 1, k - 1, 5] =  ux_tse
            u_tse[j - 1, k - 1, 6] =  ux_t
            u_tse[j - 1, k - 1, 7] =  ux_te
            u_tse[j - 1, k - 1, 8] =  uy_s
            u_tse[j - 1, k - 1, 9] =  uy_se
            u_tse[j - 1, k - 1, 10] = uy_p
            u_tse[j - 1, k - 1, 11] = uy_e
            u_tse[j - 1, k - 1, 12] = uy_ts
            u_tse[j - 1, k - 1, 13] = uy_tse
            u_tse[j - 1, k - 1, 14] = uy_t
            u_tse[j - 1, k - 1, 15] = uy_te
            u_tse[j - 1, k - 1, 16] = uz_s
            u_tse[j - 1, k - 1, 17] = uz_se
            u_tse[j - 1, k - 1, 18] = uz_p
            u_tse[j - 1, k - 1, 19] = uz_e
            u_tse[j - 1, k - 1, 20] = uz_ts
            u_tse[j - 1, k - 1, 21] = uz_tse
            u_tse[j - 1, k - 1, 22] = uz_t
            u_tse[j - 1, k - 1, 23] = uz_te

            u_tnw[j - 1, k - 1, 0] =  ux_w
            u_tnw[j - 1, k - 1, 1] =  ux_p
            u_tnw[j - 1, k - 1, 2] =  ux_nw
            u_tnw[j - 1, k - 1, 3] =  ux_n
            u_tnw[j - 1, k - 1, 4] =  ux_tw
            u_tnw[j - 1, k - 1, 5] =  ux_t
            u_tnw[j - 1, k - 1, 6] =  ux_tnw
            u_tnw[j - 1, k - 1, 7] =  ux_tn
            u_tnw[j - 1, k - 1, 8] =  uy_w
            u_tnw[j - 1, k - 1, 9] =  uy_p
            u_tnw[j - 1, k - 1, 10] = uy_nw
            u_tnw[j - 1, k - 1, 11] = uy_n
            u_tnw[j - 1, k - 1, 12] = uy_tw
            u_tnw[j - 1, k - 1, 13] = uy_t
            u_tnw[j - 1, k - 1, 14] = uy_tnw
            u_tnw[j - 1, k - 1, 15] = uy_tn
            u_tnw[j - 1, k - 1, 16] = uz_w
            u_tnw[j - 1, k - 1, 17] = uz_p
            u_tnw[j - 1, k - 1, 18] = uz_nw
            u_tnw[j - 1, k - 1, 19] = uz_n
            u_tnw[j - 1, k - 1, 20] = uz_tw
            u_tnw[j - 1, k - 1, 21] = uz_t
            u_tnw[j - 1, k - 1, 22] = uz_tnw
            u_tnw[j - 1, k - 1, 23] = uz_tn

            u_tne[j - 1, k - 1, 0] =  ux_p
            u_tne[j - 1, k - 1, 1] =  ux_e
            u_tne[j - 1, k - 1, 2] =  ux_n
            u_tne[j - 1, k - 1, 3] =  ux_ne
            u_tne[j - 1, k - 1, 4] =  ux_t
            u_tne[j - 1, k - 1, 5] =  ux_te
            u_tne[j - 1, k - 1, 6] =  ux_tn
            u_tne[j - 1, k - 1, 7] =  ux_tne
            u_tne[j - 1, k - 1, 8] =  uy_p
            u_tne[j - 1, k - 1, 9] =  uy_e
            u_tne[j - 1, k - 1, 10] = uy_n
            u_tne[j - 1, k - 1, 11] = uy_ne
            u_tne[j - 1, k - 1, 12] = uy_t
            u_tne[j - 1, k - 1, 13] = uy_te
            u_tne[j - 1, k - 1, 14] = uy_tn
            u_tne[j - 1, k - 1, 15] = uy_tne
            u_tne[j - 1, k - 1, 16] = uz_p
            u_tne[j - 1, k - 1, 17] = uz_e
            u_tne[j - 1, k - 1, 18] = uz_n
            u_tne[j - 1, k - 1, 19] = uz_ne
            u_tne[j - 1, k - 1, 20] = uz_t
            u_tne[j - 1, k - 1, 21] = uz_te
            u_tne[j - 1, k - 1, 22] = uz_tn
            u_tne[j - 1, k - 1, 23] = uz_tne

            E_sw[j - 1, k - 1] = Emat[0, j - 1, k - 1]
            E_se[j - 1, k - 1] = Emat[1, j - 1, k - 1]
            E_nw[j - 1, k - 1] = Emat[0, j, k - 1]
            E_ne[j - 1, k - 1] = Emat[1, j, k - 1]
            E_tsw[j - 1, k - 1] = Emat[0, j - 1, k]
            E_tse[j - 1, k - 1] = Emat[1, j - 1, k]
            E_tnw[j - 1, k - 1] = Emat[0, j, k]
            E_tne[j - 1, k - 1] = Emat[1, j, k]
