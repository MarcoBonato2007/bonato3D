import numpy as np
import pygame
from matrices import coordinate, rotate_x, rotate_y
from triangles import process_triangles

#region initialization

pygame.init()
pygame.mouse.set_visible(False)

clock = pygame.time.Clock()

width, height = pygame.display.Info().current_w, pygame.display.Info().current_h
window = pygame.display.set_mode(
    size = (width, height),
    flags = pygame.SCALED|pygame.FULLSCREEN)

near = 0.01
far = 1000

tan_half_x_fov = np.tan(np.pi/6)
tan_half_y_fov = tan_half_x_fov/(width/height)

near_screen_x_ratio = 2*near*tan_half_x_fov/width
near_screen_y_ratio = 2*near*tan_half_y_fov/height

triangles = [
    # front and back faces
    [coordinate(-0.5, 1, 5), coordinate(0.5, 1, 5), coordinate(0.5, 0, 5), coordinate(0, 0, -1), "green"],
    [coordinate(-0.5, 1, 5), coordinate(0.5, 0, 5), coordinate(-0.5, 0, 5), coordinate(0, 0, -1), "blue"],
    
    [coordinate(-0.5, 1, 6), coordinate(0.5, 1, 6), coordinate(0.5, 0, 6), coordinate(0, 0, 1), "red"],
    [coordinate(-0.5, 1, 6), coordinate(0.5, 0, 6), coordinate(-0.5, 0, 6), coordinate(0, 0, 1), "violet"],
    
    # top and bottom faces
    [coordinate(-0.5, 1, 5), coordinate(0.5, 1, 5), coordinate(-0.5, 1, 6), coordinate(0, 1, 0), "turquoise"],
    [coordinate(0.5, 1, 5), coordinate(-0.5, 1, 6), coordinate(0.5, 1, 6), coordinate(0, 1, 0), "yellow"],
    
    [coordinate(-0.5, 0, 5), coordinate(0.5, 0, 5), coordinate(-0.5, 0, 6), coordinate(0, -1, 0), "orange"],
    [coordinate(0.5, 0, 5), coordinate(-0.5, 0, 6), coordinate(0.5, 0, 6), coordinate(0, -1, 0), "brown"],
    
    # right and left faces
    [coordinate(0.5, 1, 5), coordinate(0.5, 0, 5), coordinate(0.5, 1, 6), coordinate(1, 0, 0), "red"],
    [coordinate(0.5, 0, 5), coordinate(0.5, 1, 6), coordinate(0.5, 0, 6), coordinate(1, 0, 0), "gray"],
    
    [coordinate(-0.5, 1, 5), coordinate(-0.5, 0, 5), coordinate(-0.5, 1, 6), coordinate(-1, 0, 0), "cyan"],
    [coordinate(-0.5, 0, 5), coordinate(-0.5, 1, 6), coordinate(-0.5, 0, 6), coordinate(-1, 0, 0), "crimson"],
    
    # Origin
    # [coordinate(-0.25, 0.25, 0.25), coordinate(0.25, 0.25, 0.25), coordinate(0.25, -0.25, 0.25), coordinate(0, 0, -1), "white"],
    
    # # Floor
    # [coordinate(10, 0, 10), coordinate(-10, 0, 10), coordinate(-10, 0, -10), coordinate(0, 1, 0), "white"],
    # [coordinate(10, 0, 10), coordinate(10, 0, -10), coordinate(-10, 0, -10), coordinate(0, 1, 0), "white"]

]

camera_pos = np.array([0.0, 0.0, 0.0])
camera_roll = 0
camera_pitch = 0
camera_velocity = np.array([0.0, 0.0, 0.0])
camera_acceleration = np.array([0.0, -9.80665, 0])

time_step = 0
max_fps = 360

color_buffer = np.zeros((width, height))
z_buffer = np.ones((width, height)) * np.inf

#endregion

while True:
    window.fill("black")
    pygame.event.set_grab(True)
    
    #region events
    
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            pygame.quit()
            exit()
        elif event.type == pygame.MOUSEMOTION:
            dx, dy = event.rel
                                                
            v = np.array([dx*near_screen_x_ratio, -dy*near_screen_y_ratio, near])
            v /= np.linalg.norm(v)

            pitch_change = np.arcsin(-v[0])
            roll_change = np.arcsin(v[1]/np.cos(pitch_change))
                        
            camera_roll += roll_change
            camera_pitch += pitch_change
                                        
    keys = pygame.key.get_pressed()
    cam_pos_change = np.matrix([
        [0.01*(keys[pygame.K_d]-keys[pygame.K_a])],
        [0], # 0.01*(keys[pygame.K_r]-keys[pygame.K_f])
        [0.01*(keys[pygame.K_w]-keys[pygame.K_s])],
        [1]
    ])
    cam_pos_change = np.asarray((rotate_x(-camera_roll)*rotate_y(-camera_pitch)*cam_pos_change).A1)[:-1]
    camera_pos += cam_pos_change
        
    #endregion
                                                            
    output_triangles = process_triangles(
        triangles, 
        tan_half_x_fov,
        tan_half_y_fov,
        width,
        height,
        near, 
        far, 
        camera_pos,
        camera_roll,
        camera_pitch,
    )
                
    for coords, depths, color in output_triangles:
        pass
        # Rasterize the triangle
        # Implement an antialiasing option
        # For each pixel:
            # Find its depth
            # If depth < depth_buffer at that pixel:
                # Set depth buffer at that pixel to that pixel's depth
                # Set color buffer at that pixel to that triangle's color
                
    for coords, depths, color in output_triangles:
        pygame.draw.polygon(window, color, coords)
                                                
    pygame.display.update()
    
    time_step = clock.tick(max_fps)/1000
    
    # Velocity verlet integration
    camera_pos += time_step * (camera_velocity + time_step*camera_acceleration/2)
    camera_velocity += time_step * camera_acceleration
    
    # Clipping against the floor
    camera_pos[1] = max(camera_pos[1], 1)
        

# Make a depth buffer, make sure objects are drawn in order. Get multiple objects in the scene.

# Implement occlusion culling
    # For each object, manually create a bounding box, then just need to create occlusion culling tech for cubes/rects
    # Render only occluders onto depth buffer, then test against that?
    # Research hierarchical z-buffers
    
# Other features:
    # Obj importer
    # Lighting
    
# Optimizations:
    # Lower levels of detail the further you are
    # Use triangle strips
    # Use polar angles (only 2 values!) for normals
    # Try seeing how you can compact how each triangle is stored
    
    # Yeet GPU commands all at once instead of one at a time
    # In C++ use bitmasking to put all triangle data in a single int
    
# Game idea: Like an FPS but you can choose how space warps around you (e.g., choose ur function)
