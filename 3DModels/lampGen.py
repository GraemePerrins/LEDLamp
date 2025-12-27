import numpy as np
import trimesh
from shapely.geometry import Point, Polygon
from shapely.ops import unary_union
import argparse
import math
import sys
import time

def get_args():
    # Use ArgumentDefaultsHelpFormatter to print default values in --help
    parser = argparse.ArgumentParser(
        description="Generate a 3D printable hollow cylinder with smooth-edged flat spiral bands.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    
    # Cylinder Dimensions
    parser.add_argument('--outer-diameter', type=float, default=100.0, help='Outer diameter of cylinder (mm)')
    parser.add_argument('--height', type=float, default=230.0, help='Total height (mm)')
    parser.add_argument('--wall-thickness', type=float, default=2.0, help='Wall thickness (mm)')
    
    # Cap Dimensions
    parser.add_argument('--cap-thickness', type=float, default=3.0, help='Thickness of top cap (mm)')
    parser.add_argument('--num-slots', type=int, default=5, help='Number of slots in cap')
    parser.add_argument('--slot-inset', type=float, default=4.0, help='Inset from outer edge to slot outer edge (mm)')
    parser.add_argument('--slot-arc-length', type=float, default=15.0, help='Length of slot arc along outer edge (mm)')
    parser.add_argument('--slot-radial-width', type=float, default=3.0, help='Width of slot from outer to inner radius (mm)')
    
    # Pattern Dimensions
    parser.add_argument('--plain-bottom', type=float, default=20.0, help='Height of plain bottom section (mm)')
    parser.add_argument('--plain-top', type=float, default=5.0, help='Height of plain top section below cap (mm)')
    parser.add_argument('--num-turns', type=float, default=8.0, help='Number of spiral turns')
    parser.add_argument('--ridge-width', type=float, default=4.0, help='Width of the spiral tape (mm)')
    parser.add_argument('--ridge-height', type=float, default=2.0, help='Protrusion height of spiral (mm)')
    
    # Edge Smoothing
    parser.add_argument('--edge-smooth-ratio', type=float, default=0.2, 
                        help='Fraction of width on each side to curve. (e.g. 0.2 = 20% curve left, 60% flat, 20% curve right)')
    
    # Mesh Quality
    parser.add_argument('--segments', type=int, default=120, help='Radial segments for cylinder (resolution)')
    parser.add_argument('--output', type=str, default='hollow_cylinder_smooth_solid.stl', help='Output filename')
    
    return parser.parse_args()

def create_slotted_cap(args):
    """
    Creates the top cap using Shapely 2D booleans and extrudes it.
    """
    print("-> Generating Top Cap...")
    radius = args.outer_diameter / 2.0
    
    cap_res = args.segments * 2
    base_circle = Point(0, 0).buffer(radius, resolution=cap_res)
    
    slot_outer_r = radius - args.slot_inset
    slot_inner_r = slot_outer_r - args.slot_radial_width
    
    if slot_outer_r <= 0:
        print("Warning: Slot definition results in negative radius. Creating solid cap.")
        return trimesh.creation.extrude_polygon(base_circle, height=args.cap_thickness)

    slot_angle_rad = args.slot_arc_length / slot_outer_r
    
    slots = []
    angle_step = (2 * math.pi) / args.num_slots
    
    for i in range(args.num_slots):
        center_angle = i * angle_step
        start_angle = center_angle - (slot_angle_rad / 2)
        end_angle = center_angle + (slot_angle_rad / 2)
        
        points_outer = []
        points_inner = []
        arc_steps = 10 
        theta_vals = np.linspace(start_angle, end_angle, arc_steps)
        
        for t in theta_vals:
            points_outer.append((slot_outer_r * math.cos(t), slot_outer_r * math.sin(t)))
            points_inner.append((slot_inner_r * math.cos(t), slot_inner_r * math.sin(t)))
            
        poly_points = points_outer + points_inner[::-1]
        slot_poly = Polygon(poly_points)
        slots.append(slot_poly)
        
    if slots:
        all_slots = unary_union(slots)
        final_shape = base_circle.difference(all_slots)
    else:
        final_shape = base_circle
        
    mesh = trimesh.creation.extrude_polygon(final_shape, height=args.cap_thickness)
    z_pos = args.height - args.cap_thickness
    mesh.apply_translation([0, 0, z_pos])
    
    return mesh

def create_hollow_cylinder(args):
    """
    Creates the main body hollow cylinder using a 2D annulus extrusion.
    """
    print("-> Generating Hollow Cylinder Body...")
    outer_r = args.outer_diameter / 2.0
    inner_r = outer_r - args.wall_thickness
    
    outer_circle = Point(0, 0).buffer(outer_r, resolution=args.segments)
    inner_circle = Point(0, 0).buffer(inner_r, resolution=args.segments)
    annulus = outer_circle.difference(inner_circle)
    
    cyl_height = args.height - args.cap_thickness
    mesh = trimesh.creation.extrude_polygon(annulus, height=cyl_height)
    return mesh

def calculate_plateau_height(u, max_height, edge_fraction):
    """
    Calculates a 'Plateau' profile height.
    u: 0.0 to 1.0 across the width
    max_height: full protrusion height
    edge_fraction: percentage of width devoted to the curved edge
    """
    if edge_fraction > 0.5: edge_fraction = 0.5
    if edge_fraction < 0.0: edge_fraction = 0.0
    
    if edge_fraction == 0:
        return max_height

    if u < edge_fraction:
        # Left Edge Curve (0 to max)
        x = u / edge_fraction
        return max_height * math.sin(x * math.pi / 2)
    elif u > (1.0 - edge_fraction):
        # Right Edge Curve (max to 0)
        x = (1.0 - u) / edge_fraction
        return max_height * math.sin(x * math.pi / 2)
    else:
        # Flat Center
        return max_height

def create_solid_spiral_band(args, clockwise=True):
    """
    Generates a SOLID WATERTIGHT spiral band with smoothed edges.
    Creates both a Top Surface (curved) and a Bottom Surface (inset) and stitches them.
    """
    direction_text = "Clockwise" if clockwise else "Counter-Clockwise"
    print(f"-> Generating {direction_text} Spiral Band (Solid Volume)...")
    
    outer_r = args.outer_diameter / 2.0
    # Overlap to ensure intersection with cylinder wall
    overlap = 0.05 
    base_radius = outer_r - overlap
    
    z_start = args.plain_bottom
    z_end = args.height - args.cap_thickness - args.plain_top
    h_span = z_end - z_start
    
    if h_span <= 0:
        return trimesh.Trimesh()

    # Resolution
    num_samples = 1200   # Steps along length
    width_segments = 16  # Steps across width (resolution of the edge curve)
    
    t_vals = np.linspace(0, 1, num_samples)
    
    # Store vertices for Top and Bottom surfaces
    top_verts_grid = []
    bot_verts_grid = []
    
    total_angle = 2 * math.pi * args.num_turns
    angle_mult = -1 if clockwise else 1
    
    zs = z_start + t_vals * h_span
    thetas = t_vals * total_angle * angle_mult
    pitch_comp = h_span / total_angle 
    
    # --- 1. Generate Vertex Grids ---
    for i in range(num_samples):
        theta = thetas[i]
        z = zs[i]
        
        # Frame calculation
        nx, ny = math.cos(theta), math.sin(theta)
        norm_vec = np.array([nx, ny, 0])
        
        tx, ty = -math.sin(theta) * angle_mult, math.cos(theta) * angle_mult
        tz = pitch_comp / base_radius
        tan_vec = np.array([tx, ty, tz])
        tan_vec = tan_vec / np.linalg.norm(tan_vec)
        
        binorm_vec = np.cross(norm_vec, tan_vec)
        binorm_vec = binorm_vec / np.linalg.norm(binorm_vec)
        
        center_pt = np.array([base_radius * nx, base_radius * ny, z])
        
        row_top = []
        row_bot = []
        
        for j in range(width_segments):
            u = j / (width_segments - 1)
            
            # Width Offset (Linear across band)
            w_offset = (u - 0.5) * args.ridge_width
            
            # Height Offset (Plateau Profile)
            h_offset = calculate_plateau_height(u, args.ridge_height, args.edge_smooth_ratio)
            
            # Top Vertex: On the curve
            v_top = center_pt + (binorm_vec * w_offset) + (norm_vec * h_offset)
            
            # Bottom Vertex: Buried in the wall
            # Note: The bottom is just the base radius (no height offset)
            v_bot = center_pt + (binorm_vec * w_offset) 
            
            row_top.append(v_top)
            row_bot.append(v_bot)
            
        top_verts_grid.extend(row_top)
        bot_verts_grid.extend(row_bot)

    # Convert to flattened list for trimesh
    all_vertices = top_verts_grid + bot_verts_grid
    
    faces = []
    
    # Helper to access grid indices
    # top grid is indices 0 to N-1
    # bot grid is indices N to 2N-1
    offset_bot = len(top_verts_grid)
    
    def get_top(row, col):
        return row * width_segments + col
        
    def get_bot(row, col):
        return offset_bot + (row * width_segments + col)

    # --- 2. Generate Faces ---
    
    for i in range(num_samples - 1):
        for j in range(width_segments - 1):
            
            # --- Top Surface (Visible) ---
            # Connects top grid points
            t0 = get_top(i, j)
            t1 = get_top(i, j+1)
            t2 = get_top(i+1, j)
            t3 = get_top(i+1, j+1)
            
            # Quad t0, t1, t3, t2
            faces.append([t0, t1, t3])
            faces.append([t0, t3, t2])
            
            # --- Bottom Surface (Hidden/Internal) ---
            # Connects bottom grid points
            # Winding must be reversed (Clockwise) so normal points Inward/Down
            b0 = get_bot(i, j)
            b1 = get_bot(i, j+1)
            b2 = get_bot(i+1, j)
            b3 = get_bot(i+1, j+1)
            
            # Quad b0, b1, b3, b2 -> Reversed -> b0, b2, b3; b0, b3, b1
            faces.append([b0, b2, b3])
            faces.append([b0, b3, b1])
            
            # --- Side Wall: Left Edge (j=0) ---
            if j == 0:
                tl0 = get_top(i, 0)
                tl1 = get_top(i+1, 0)
                bl0 = get_bot(i, 0)
                bl1 = get_bot(i+1, 0)
                
                # Connect Top Left to Bottom Left
                # Quad tl0, tl1, bl1, bl0
                faces.append([tl0, tl1, bl1])
                faces.append([tl0, bl1, bl0])
                
            # --- Side Wall: Right Edge (j=max) ---
            if j == width_segments - 2:
                # The index of the last column is width_segments-1
                last = width_segments - 1
                tr0 = get_top(i, last)
                tr1 = get_top(i+1, last)
                br0 = get_bot(i, last)
                br1 = get_bot(i+1, last)
                
                # Connect Top Right to Bottom Right
                # Quad tr0, br0, br1, tr1
                faces.append([tr0, br0, br1])
                faces.append([tr0, br1, tr1])

    # --- 3. End Caps ---
    # Stitch the Start Profile (i=0) and End Profile (i=max)
    
    # Start Cap (i=0)
    # Connect Top row 0 to Bottom row 0
    for j in range(width_segments - 1):
        ts0 = get_top(0, j)
        ts1 = get_top(0, j+1)
        bs0 = get_bot(0, j)
        bs1 = get_bot(0, j+1)
        
        # Quad ts0, bs0, bs1, ts1
        faces.append([ts0, bs0, bs1])
        faces.append([ts0, bs1, ts1])
        
    # End Cap (i=max)
    last_row = num_samples - 1
    for j in range(width_segments - 1):
        te0 = get_top(last_row, j)
        te1 = get_top(last_row, j+1)
        be0 = get_bot(last_row, j)
        be1 = get_bot(last_row, j+1)
        
        # Quad te0, te1, be1, be0
        faces.append([te0, te1, be1])
        faces.append([te0, be1, be0])

    mesh = trimesh.Trimesh(vertices=all_vertices, faces=faces)
    mesh.fix_normals()
    return mesh

def main():
    args = get_args()
    start_time = time.time()
    
    # Calculate percentage for display
    flat_percent = (1.0 - (2 * args.edge_smooth_ratio)) * 100
    edge_percent = args.edge_smooth_ratio * 100
    
    print("="*60)
    print(f"Generating Binary STL: {args.output}")
    print(f"Dimensions: {args.outer_diameter}mm OD x {args.height}mm H")
    print(f"Pattern: Double Helix, {args.num_turns} turns")
    print(f"Profile: {flat_percent:.0f}% Flat Center, {edge_percent:.0f}% Curved Edge on each side")
    print("="*60)
    
    cap_mesh = create_slotted_cap(args)
    cylinder_mesh = create_hollow_cylinder(args)
    spiral_cw = create_solid_spiral_band(args, clockwise=True)
    spiral_ccw = create_solid_spiral_band(args, clockwise=False)
    
    print("-> Combining meshes...")
    combined = trimesh.util.concatenate([cap_mesh, cylinder_mesh, spiral_cw, spiral_ccw])
    
    print("-> Processing mesh (merging vertices, fixing normals)...")
    combined.merge_vertices()
    combined.remove_duplicate_faces()
    combined.remove_unreferenced_vertices()
    combined.fix_normals()
    
    print("-" * 30)
    print(f"Vertices: {len(combined.vertices)}")
    print(f"Faces:    {len(combined.faces)}")
    print(f"Is Watertight: {combined.is_watertight}")
    if combined.is_watertight:
        print("Success: Mesh is fully watertight.")
    else:
        print("Note: Mesh contains overlapping volumes (standard for slicers).")
        
    print(f"-> Exporting to {args.output}...")
    combined.export(args.output)
    
    elapsed = time.time() - start_time
    print(f"Done in {elapsed:.2f} seconds.")

if __name__ == "__main__":
    main()

