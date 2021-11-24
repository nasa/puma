import numpy as np
import sys

DTYPE = np.float

def indexAt_3D(i, j, k, len_x, len_y, len_z):
    if i == -1:
        i = 0
    if i == len_x:
        i = len_x - 1
    if j == -1:
        j = 0
    if j == len_y:
        j = len_y - 1
    if k == -1:
        k = 0
    if k == len_z:
        k = len_z - 1
    return len_x * len_y * k + len_x * j + i


def setup_matrices_cy(double [:] _kf, int l_x, int l_y, int l_z, short bc_check, double [:, :, :] prescribed_bc):

    cdef int i, j, k
    cdef int index, ixm, ixp, iym, iyp, izm, izp

    cdef int l_xy = l_x * l_y
    cdef int l_xyz = l_xy * l_z

    # number of Array entries = (lengthX-2)*lengthY*lengthZ * 7 + 2*lengthY*lengthZ
    # have to split computation for large domains
    cdef unsigned long long nEntries = (l_x - 2)  # = (l_x - 2) * l_y * l_z * 7 + 2 * l_y * l_z
    nEntries *= l_y
    nEntries *= l_z
    nEntries *= 7
    nEntries += 2 * l_y * l_z
    _row = np.zeros(nEntries, dtype=np.uint32)
    _col = np.zeros(nEntries, dtype=np.uint32)
    _data = np.zeros(nEntries)
    cdef unsigned int[:] row = _row
    cdef unsigned int[:] col = _col
    cdef double[:] data = _data

    cdef unsigned long long count = 0
    for i in [0, l_x - 1]:
        for j in range(l_y):
            for k in range(l_z):
                index = l_xy * k + l_x * j + i
                row[count] = index
                col[count] = index
                data[count] = 1
                count += 1

    for i in range(1, l_x - 1):
        for j in [0, l_y - 1]:
            for k in range(l_z):
                index = l_xy * k + l_x * j + i
                row[count] = index
                col[count] = index

                if bc_check == 1 and prescribed_bc[i, j, k] != np.Inf:
                        data[count] = 1
                        count += 1
                else:
                    ixm = indexAt_3D(i - 1, j, k, l_x, l_y, l_z)
                    ixp = indexAt_3D(i + 1, j, k, l_x, l_y, l_z)
                    iym = indexAt_3D(i, j - 1, k, l_x, l_y, l_z)
                    iyp = indexAt_3D(i, j + 1, k, l_x, l_y, l_z)
                    izm = indexAt_3D(i, j, k - 1, l_x, l_y, l_z)
                    izp = indexAt_3D(i, j, k + 1, l_x, l_y, l_z)

                    data[count] = - _kf[ixm] * _kf[index] / (_kf[ixm] + _kf[index]) - _kf[ixp] * _kf[index] / (_kf[ixp] + _kf[index]) \
                                  - _kf[iym] * _kf[index] / (_kf[iym] + _kf[index]) - _kf[iyp] * _kf[index] / (_kf[iyp] + _kf[index]) \
                                  - _kf[izm] * _kf[index] / (_kf[izm] + _kf[index]) - _kf[izp] * _kf[index] / (_kf[izp] + _kf[index])

                    row[count + 1] = index
                    col[count + 1] = ixm
                    data[count + 1] = _kf[ixm] * _kf[index] / (_kf[ixm] + _kf[index])

                    row[count + 2] = index
                    col[count + 2] = ixp
                    data[count + 2] = _kf[ixp] * _kf[index] / (_kf[ixp] + _kf[index])

                    row[count + 3] = index
                    col[count + 3] = iym
                    data[count + 3] = _kf[iym] * _kf[index] / (_kf[iym] + _kf[index])

                    row[count + 4] = index
                    col[count + 4] = iyp
                    data[count + 4] = _kf[iyp] * _kf[index] / (_kf[iyp] + _kf[index])

                    row[count + 5] = index
                    col[count + 5] = izm
                    data[count + 5] = _kf[izm] * _kf[index] / (_kf[izm] + _kf[index])

                    row[count + 6] = index
                    col[count + 6] = izp
                    data[count + 6] = _kf[izp] * _kf[index] / (_kf[izp] + _kf[index])

                    count += 7

    for i in range(1, l_x - 1):
        for j in range(1, l_y - 1):
            for k in [0, l_z - 1]:
                index = l_xy * k + l_x * j + i
                row[count] = index
                col[count] = index

                if bc_check == 1 and prescribed_bc[i, j, k] != np.Inf:
                        data[count] = 1
                        count += 1
                else:
                    ixm = indexAt_3D(i - 1, j, k, l_x, l_y, l_z)
                    ixp = indexAt_3D(i + 1, j, k, l_x, l_y, l_z)
                    iym = indexAt_3D(i, j - 1, k, l_x, l_y, l_z)
                    iyp = indexAt_3D(i, j + 1, k, l_x, l_y, l_z)
                    izm = indexAt_3D(i, j, k - 1, l_x, l_y, l_z)
                    izp = indexAt_3D(i, j, k + 1, l_x, l_y, l_z)

                    data[count] = - _kf[ixm] * _kf[index] / (_kf[ixm] + _kf[index]) - _kf[ixp] * _kf[index] / (_kf[ixp] + _kf[index]) \
                                  - _kf[iym] * _kf[index] / (_kf[iym] + _kf[index]) - _kf[iyp] * _kf[index] / (_kf[iyp] + _kf[index]) \
                                  - _kf[izm] * _kf[index] / (_kf[izm] + _kf[index]) - _kf[izp] * _kf[index] / (_kf[izp] + _kf[index])

                    row[count + 1] = index
                    col[count + 1] = ixm
                    data[count + 1] = _kf[ixm] * _kf[index] / (_kf[ixm] + _kf[index])

                    row[count + 2] = index
                    col[count + 2] = ixp
                    data[count + 2] = _kf[ixp] * _kf[index] / (_kf[ixp] + _kf[index])

                    row[count + 3] = index
                    col[count + 3] = iym
                    data[count + 3] = _kf[iym] * _kf[index] / (_kf[iym] + _kf[index])

                    row[count + 4] = index
                    col[count + 4] = iyp
                    data[count + 4] = _kf[iyp] * _kf[index] / (_kf[iyp] + _kf[index])

                    row[count + 5] = index
                    col[count + 5] = izm
                    data[count + 5] = _kf[izm] * _kf[index] / (_kf[izm] + _kf[index])

                    row[count + 6] = index
                    col[count + 6] = izp
                    data[count + 6] = _kf[izp] * _kf[index] / (_kf[izp] + _kf[index])

                    count += 7

    cdef unsigned long long startCount = count
    cdef int nX = l_x - 2
    cdef int nY = l_y - 2
    cdef int nZ = l_z - 2
    cdef unsigned long long myCount

# to maybe be parallelized in the future
    for i in range(1, l_x - 1):
        sys.stdout.write("\rAssembling A matrix ... {:.1f}% ".format(float(i) / (l_x-2) * 100))
        for j in range(1, l_y - 1):
            for k in range(1, l_z - 1):

                myCount = (nZ * nY) * (i-1) # startCount + ((nZ * nY) * (i-1) + nZ * (j-1) + (k-1)) * 7
                myCount += nZ * (j-1)
                myCount += k-1
                myCount *= 7
                myCount += startCount

                index = l_xy * k + l_x * j + i
                row[myCount] = index
                col[myCount] = index

                if bc_check == 1 and prescribed_bc[i, j, k] != np.Inf:
                        data[myCount] = 1
                else:
                    ixm = l_xy * k + l_x * j + (i - 1)
                    ixp = l_xy * k + l_x * j + (i + 1)
                    iym = l_xy * k + l_x * (j - 1) + i
                    iyp = l_xy * k + l_x * (j + 1) + i
                    izm = l_xy * (k - 1) + l_x * j + i
                    izp = l_xy * (k + 1) + l_x * j + i

                    data[myCount] = - _kf[ixm] * _kf[index] / (_kf[ixm] + _kf[index]) - _kf[ixp] * _kf[index] / (_kf[ixp] + _kf[index]) \
                                    - _kf[iym] * _kf[index] / (_kf[iym] + _kf[index]) - _kf[iyp] * _kf[index] / (_kf[iyp] + _kf[index]) \
                                    - _kf[izm] * _kf[index] / (_kf[izm] + _kf[index]) - _kf[izp] * _kf[index] / (_kf[izp] + _kf[index])

                    row[myCount + 1] = index
                    col[myCount + 1] = ixm
                    data[myCount + 1] = _kf[ixm] * _kf[index] / (_kf[ixm] + _kf[index])

                    row[myCount + 2] = index
                    col[myCount + 2] = ixp
                    data[myCount + 2] = _kf[ixp] * _kf[index] / (_kf[ixp] + _kf[index])

                    row[myCount + 3] = index
                    col[myCount + 3] = iym
                    data[myCount + 3] = _kf[iym] * _kf[index] / (_kf[iym] + _kf[index])

                    row[myCount + 4] = index
                    col[myCount + 4] = iyp
                    data[myCount + 4] = _kf[iyp] * _kf[index] / (_kf[iyp] + _kf[index])

                    row[myCount + 5] = index
                    col[myCount + 5] = izm
                    data[myCount + 5] = _kf[izm] * _kf[index] / (_kf[izm] + _kf[index])

                    row[myCount + 6] = index
                    col[myCount + 6] = izp
                    data[myCount + 6] = _kf[izp] * _kf[index] / (_kf[izp] + _kf[index])

    return _row, _col, _data


def compute_flux(double[:,:,:] T, double[:,:,:] cond, l_x, l_y, l_z):

    cdef int len_x = l_x
    cdef int len_y = l_y
    cdef int len_z = l_z
    cdef int i,j,k

    cdef double flux_x = 0
    cdef double flux_y = 0
    cdef double flux_z = 0

    _q = np.zeros([len_x, len_y, len_z, 3])
    cdef double[:,:,:,:] q = _q

    for i in range(1, len_x - 1):
        for j in range(len_y):
            for k in range(len_z):
                if cond[i - 1, j, k] + cond[i, j, k] != 0:
                    q[i, j, k, 0] = cond[i - 1, j, k] * cond[i, j, k] / (cond[i - 1, j, k] + cond[i, j, k]) * (T[i, j, k] - T[i - 1, j, k])
                if cond[i + 1, j, k] + cond[i, j, k] != 0:
                    q[i, j, k, 0] += cond[i + 1, j, k] * cond[i, j, k] / (cond[i + 1, j, k] + cond[i, j, k]) * (T[i + 1, j, k] - T[i, j, k])
                flux_x += q[i, j, k, 0]
    for i in range(len_x):
        for j in range(1, len_y - 1):
            for k in range(len_z):
                if cond[i, j - 1, k] + cond[i, j, k] != 0:
                    q[i, j, k, 1] = cond[i, j - 1, k] * cond[i, j, k] / (cond[i, j - 1, k] + cond[i, j, k]) * (T[i, j, k] - T[i, j - 1, k])
                if cond[i, j + 1, k] + cond[i, j, k] != 0:
                    q[i, j, k, 1] += cond[i, j + 1, k] * cond[i, j, k] / (cond[i, j + 1, k] + cond[i, j, k]) * (T[i, j + 1, k] - T[i, j, k])
                flux_y += q[i, j, k, 1]
    for i in range(len_x):
        for j in range(len_y):
            for k in range(1, len_z - 1):
                if cond[i, j, k - 1] + cond[i, j, k] != 0:
                    q[i, j, k, 2] = cond[i, j, k - 1] * cond[i, j, k] / (cond[i, j, k - 1] + cond[i, j, k]) * (T[i, j, k] - T[i, j, k - 1])
                if cond[i, j, k + 1] + cond[i, j, k] != 0:
                    q[i, j, k, 2] += cond[i, j, k + 1] * cond[i, j, k] / (cond[i, j, k + 1] + cond[i, j, k]) * (T[i, j, k + 1] - T[i, j, k])
                flux_z += q[i, j, k, 2]
    flux_x /= ((len_x - 2) * len_y * len_z)
    flux_y /= (len_x * (len_y - 2) * len_z)
    flux_z /= (len_x * len_y * (len_z - 2))
    return flux_x, flux_y, flux_z, _q
