import numpy as np


def get_sphere(diameter):
    radius = diameter / 2.
    shape = (int(diameter+4), int(diameter+4), int(diameter+4))
    center_point = (diameter+4.) / 2. - 0.5
    point = np.array([center_point, center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)-point[:, None, None, None], axis=0)
    sphere = (255.-127.5*(1+np.tanh((dist-radius))))
    sphere[sphere < 0] = 0
    sphere = sphere.astype(np.uint16)
    return sphere


def get_circle(diameter):
    radius = diameter / 2.
    shape = (int(diameter+4), int(diameter+4))
    center_point = (int(diameter+4)) / 2.
    point = np.array([center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)+0.5-point[:, None, None], axis=0)
    circle = (255.-128*(1+np.tanh(2*(dist-radius))))
    circle[circle < 0] = 0
    circle = circle.astype(np.uint16)
    return circle


def get_circle_odd(diameter):
    radius = diameter / 2.
    if int(diameter) % 2 == 0:
        shape = (int(diameter+5), int(diameter+5))
        center_point = (int(diameter + 5)) / 2.
    else:
        shape = (int(diameter+4), int(diameter+4))
        center_point = (int(diameter + 4)) / 2.
    point = np.array([center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)+0.5-point[:, None, None], axis=0)
    circle = (255.-128*(1+np.tanh(2*(dist-radius))))
    circle[circle < 0] = 0
    circle = circle.astype(np.uint16)
    return circle, shape[0]


def get_circle_even(diameter):
    radius = diameter / 2.
    if int(diameter) % 2 == 0:
        shape = (int(diameter+4), int(diameter+4))
        center_point = (int(diameter + 4)) / 2.
    else:
        shape = (int(diameter+5), int(diameter+5))
        center_point = (int(diameter + 5)) / 2.
    point = np.array([center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)+0.5-point[:, None, None], axis=0)
    circle = (255.-128*(1+np.tanh(2*(dist-radius))))
    circle[circle < 0] = 0
    circle = circle.astype(np.uint16)
    return circle, shape[0]
