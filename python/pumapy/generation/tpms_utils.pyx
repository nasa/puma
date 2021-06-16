from libc.math cimport sin, cos
import sys

DTYPE = float

def generate(double[:,:,:] matrix, l_x, l_y, l_z, wmin, wmax, qmin, qmax, equation):
    cdef int len_x = l_x
    cdef int len_y = l_y
    cdef int len_z = l_z
    cdef double q_min = qmin
    cdef double q_max = qmax
    cdef double w_min = wmin
    cdef double w_max = wmax
    cdef int i, j, k
    cdef double q, w

    if equation == 0:
        for i in range(len_x):
            sys.stdout.write("\rGenerating TPMS ... {:.1f}% ".format(float(i) / (len_x-1) * 100))
            for j in range(len_y):
                for k in range(len_z):
                    q = q = q_min + (float(k)/(float(len_z)-1))*(q_max-q_min)
                    w = w = w_min + (float(k)/(float(len_z)-1))*(w_max-w_min)
                    matrix[i,j,k] = sin(w * i) * sin(w * j) * sin(w * k) + sin(w * i) * cos(w * j) * cos(w * k) + \
                                    cos(w * i) * sin(w * j) * cos(w * k) + cos(w * i) * cos(w * j) * sin(w * k) + q
    elif equation == 1:
        for i in range(len_x):
            sys.stdout.write("\rGenerating TPMS ... {:.1f}% ".format(float(i) / (len_x-1) * 100))
            for j in range(len_y):
                for k in range(len_z):
                    q = q = q_min + (float(k)/(float(len_z)-1))*(q_max-q_min)
                    w = w = w_min + (float(k)/(float(len_z)-1))*(w_max-w_min)
                    matrix[i,j,k] = cos(w * i) * sin(w * j) + cos(w * j) * sin(w * k) + cos(w * k) * cos(w * i) + q
    elif equation == 2:
        for i in range(len_x):
            sys.stdout.write("\rGenerating TPMS ... {:.1f}% ".format(float(i) / (len_x-1) * 100))
            for j in range(len_y):
                for k in range(len_z):
                    q = q = q_min + (float(k)/(float(len_z)-1))*(q_max-q_min)
                    w = w = w_min + (float(k)/(float(len_z)-1))*(w_max-w_min)
                    matrix[i,j,k] =  cos(w * i) + cos(w * j) + cos(w * k) + q

    for i in range(len_x):
            for j in range(len_y):
                for k in range(len_z):
                    if matrix[i,j,k] < 0.8:
                        matrix[i,j,k] = 0.8
                    if matrix[i,j,k] > 1.2:
                        matrix[i,j,k] = 1.2

    return matrix