import numpy as np

class ViewError(Exception): pass

def is_point_in_view(
    point: np.matrix, 
    tan_half_x_fov: float, 
    tan_half_y_fov: float, 
    near: float, 
    far: float
) -> bool:
    
    x, y, z, _ = point.A1
    
    if near <= z <= far and abs(x) <= z*tan_half_x_fov and abs(y) <= z*tan_half_y_fov:
        return True
    else:
        return False

def is_line_in_view(
    start: np.matrix, 
    end: np.matrix, 
    tan_half_x_fov: float, 
    tan_half_y_fov: float, 
    near: float, 
    far: float
) -> bool:
    """Start and end are the start and end coordinates/points of the line."""
    
    ax, ay, az, _ = start.A1
    seg = end-start # a vector representing the line segment between the two points
    segx, segy, segz, _ = seg.A1
    
    # This function works by defining a point on the line as start + k*(end-start)
    # We then find a lower and upper bound for the value of k.
    # If this overlaps with the interval 0-1 then the line is in view, otherwise not.
    
    # Each tuple represents a test for x, y and z coordinates
    # Each represents two fractions for an upper and lower bound of k
    checks = (
        (-az*tan_half_x_fov-ax, segx+segz*tan_half_x_fov, az*tan_half_x_fov-ax, segx-segz*tan_half_x_fov), 
        (-az*tan_half_y_fov-ay, segy+segz*tan_half_y_fov, az*tan_half_y_fov-ay, segy-segz*tan_half_y_fov), 
        (near-az, segz, far-az, segz)
    )
        
    lower_bound = -np.inf
    upper_bound = np.inf
    
    for top1, bottom1, top2, bottom2 in checks:
        # We check if the bottom / denominator of each fraction is less than zero
        # If so we flip if we are finding a new upper bound or lower bound
        
        if bottom1 > 0:
            lower_bound = max(lower_bound, top1/bottom1)
        elif bottom1 < 0:
            upper_bound = min(upper_bound, top1/bottom1)
        elif top1 > 0:
            return False
        
        if bottom2 > 0:
            upper_bound = min(upper_bound, top2/bottom2)
        elif bottom2 < 0:
            lower_bound = max(lower_bound, top2/bottom2)
        elif top2 < 0:
            return False
            
    if upper_bound < 0 or lower_bound > 1:
        return False
    else:
        return True          
                
def is_triangle_in_view(vert1, vert2, vert3, tan_half_x_fov, tan_half_y_fov, near, far):
    lines = ((vert1, vert2), (vert1, vert3), (vert2, vert3))
    lines_in_view = [is_line_in_view(start, end, tan_half_x_fov, tan_half_y_fov, near, far) for start, end in lines]
            
    if any(lines_in_view):
        return True
    else:
        return False
    
def clip_triangle(vert1, vert2, vert3, near):
    # This is only ever called if a triangle is in view
    
    points_in_front_of_near = []
    points_behind_near = []
    for vert in (vert1, vert2, vert3):
        _, _, z, _ = vert.A1
        if z >= near:
            points_in_front_of_near.append(vert)
        else:
            points_behind_near.append(vert)
            
    if len(points_in_front_of_near) == 3:
        return [(vert1, vert2, vert3)] # There is no need to clip this triangle
    
    # v1 is the vertex on the opposite side of the near plane from the other two vertices
    elif len(points_in_front_of_near) == 2:
        v1 = points_behind_near[0]
        v2, v3 = points_in_front_of_near
    else:
        v1 = points_in_front_of_near[0]
        v2, v3 = points_behind_near
    
    v1z, v2z, v3z = v1.A1[2], v2.A1[2], v3.A1[2]
    intersect_1 = v1 + (near-v1z)/(v2z-v1z) * (v2-v1) # intersect point from v1 to v2
    intersect_2 = v1 + (near-v1z)/(v3z-v1z) * (v3-v1) # intersect point from v1 to v3
    
    if len(points_in_front_of_near) == 2:
        return [(intersect_1, intersect_2, v2), (v2, v3, intersect_2)]
    else:
        return [(intersect_1, intersect_2, v1)]
    