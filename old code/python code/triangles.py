import numpy as np
from matrices import project, translate, rotate_x, rotate_y
from clipping import is_triangle_in_view, clip_triangle, ViewError

def project_point(point, tan_half_x_fov, tan_half_y_fov, width, height, near, far):
    M = project(tan_half_x_fov, tan_half_y_fov, width, height, near, far)
    x, y, z, w = point.A1
    new_point = M*point
    x, y, z, w = new_point.A1
    screen_coordinates, depth = (x/w, y/w), z/w
    return screen_coordinates, depth

def process_triangle(vert1, vert2, vert3, normal, tan_half_x_fov, tan_half_y_fov, near, far, camera_coords, camera_roll, camera_pitch):
    T = rotate_x(camera_roll)*rotate_y(camera_pitch)*translate(*-camera_coords) # transformation matrix
    vert1, vert2, vert3 = T*vert1, T*vert2, T*vert3
    normal = np.linalg.inv(T).T * normal
    normal_arr = normal.A1
    normal_arr[3] = 0
                                
    if not is_triangle_in_view(vert1, vert2, vert3, tan_half_x_fov, tan_half_y_fov, near, far):
        # Frustum culling
        raise ViewError("Triangle is outside of view")
    elif np.dot(vert1.A1, normal_arr) >= 0:
        # Backface culling
        raise ViewError("Triangle is facing away from view")
    else:
        return clip_triangle(vert1, vert2, vert3, near)

def project_triangle(vert1, vert2, vert3, tan_half_x_fov, tan_half_y_fov, width, height, near, far):
    vertices = [project_point(point, tan_half_x_fov, tan_half_y_fov, width, height, near, far) for point in (vert1, vert2, vert3)]
    screen_coordinates = [i[0] for i in vertices]
    depths = [i[1] for i in vertices]
    return screen_coordinates, depths

def process_triangles(triangles, tan_half_x_fov, tan_half_y_fov, width, height, near, far, camera_coords, camera_roll, camera_pitch):
    output_triangles = []
    for vert1, vert2, vert3, normal, color in triangles:
        try:
            for i in process_triangle(vert1, vert2, vert3, normal, tan_half_x_fov, tan_half_y_fov, near, far, camera_coords, camera_roll, camera_pitch):
                screen_coordinates, depths = project_triangle(*i, tan_half_x_fov, tan_half_y_fov, width, height, near, far)
                output_triangles.append((screen_coordinates, depths, color))
        except ViewError: # this triangle it outside of view, we can simply ignore it.
            pass
    return output_triangles
