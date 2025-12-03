import numpy as np

def coordinate(x, y, z): 
    return np.matrix(
        [[x], 
         [y], 
         [z], 
         [1]])

def translate(x, y, z):
    return np.matrix(
        [[1, 0, 0, x], 
         [0, 1, 0, y], 
         [0, 0, 1, z], 
         [0, 0, 0, 1]])

def rotate_x(theta):
    return np.matrix(
        [[1, 0, 0, 0], 
         [0, np.cos(theta), -np.sin(theta), 0], 
         [0, np.sin(theta), np.cos(theta), 0], 
         [0, 0, 0, 1]])

def rotate_y(theta):
    return np.matrix(
        [[np.cos(theta), 0, np.sin(theta), 0], 
         [0, 1, 0, 0], 
         [-np.sin(theta), 0, np.cos(theta), 0], 
         [0, 0, 0, 1]])

def project(tan_half_x_fov, tan_half_y_fov, width, height, near, far):
    return np.matrix([
        [width/(2*tan_half_x_fov), 0, width/2, 0], 
        [0, -height/(2*tan_half_y_fov), height/2, 0], 
        [0, 0, far+near, -near*far],
        [0, 0, 1, 0]
    ])
