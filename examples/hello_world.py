import pyjolt
import time
import math

# Constants
NUM_BODIES = 100

def run():
    print("Initializing Jolt...")
    pyjolt.init()

    # Create key objects
    temp_allocator = pyjolt.TempAllocatorImpl(10 * 1024 * 1024)
    job_system = pyjolt.JobSystemThreadPool(2048, 8, -1)
    
    # Layer filters
    bp_layer_interface = pyjolt.BPLayerInterfaceImpl()
    obj_vs_bp_filter = pyjolt.ObjectVsBroadPhaseLayerFilterImpl()
    obj_layer_filter = pyjolt.ObjectLayerPairFilterImpl()

    # Create Physics System
    physics_system = pyjolt.PhysicsSystem()
    physics_system.Init(1024, 0, 1024, 1024, 
                        bp_layer_interface, obj_vs_bp_filter, obj_layer_filter)

    body_interface = physics_system.GetBodyInterface()

    # Create Floor
    floor_shape_settings = pyjolt.BoxShapeSettings(pyjolt.Vec3(100.0, 1.0, 100.0))
    floor_settings = pyjolt.BodyCreationSettings(floor_shape_settings, 
                                                 pyjolt.Vec3(0.0, -1.0, 0.0), 
                                                 pyjolt.Quat(), 
                                                 pyjolt.EMotionType.Static, 
                                                 0) # Layer 0 = NON_MOVING
    floor = body_interface.CreateBody(floor_settings)
    body_interface.AddBody(floor.GetID(), pyjolt.EActivation.DontActivate)

    # Create Falling Sphere
    sphere_shape_settings = pyjolt.SphereShapeSettings(0.5)
    sphere_settings = pyjolt.BodyCreationSettings(sphere_shape_settings, 
                                                  pyjolt.Vec3(0.0, 10.0, 0.0), 
                                                  pyjolt.Quat(), 
                                                  pyjolt.EMotionType.Dynamic, 
                                                  1) # Layer 1 = MOVING
    sphere_id = body_interface.CreateBody(sphere_settings).GetID()
    body_interface.AddBody(sphere_id, pyjolt.EActivation.Activate)

    # Optimize Broadphase
    physics_system.OptimizeBroadPhase()

    # Simulate
    delta_time = 1.0 / 60.0
    print("Simulating...")
    for i in range(60): # 1 second
        physics_system.Update(delta_time, 1, temp_allocator, job_system)
        
        # Output position
        pos = body_interface.GetPosition(sphere_id)
        if i % 10 == 0:
            print(f"Step {i}: Sphere Height = {pos.GetY():.4f}")

    print("Done!")

if __name__ == "__main__":
    run()
