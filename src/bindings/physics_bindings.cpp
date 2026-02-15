#include "bindings.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <nanobind/operators.h>

namespace nb = nanobind;
using namespace JPH;

// Default implementations for system callbacks
namespace Layers {
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING = 1;
    static constexpr int NUM_LAYERS = 2;
}


namespace BroadPhaseLayers {
    static constexpr BroadPhaseLayer NON_MOVING(0);
    static constexpr BroadPhaseLayer MOVING(1);
    static constexpr uint NUM_LAYERS = 2;
};

class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface {
public:
    uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }
    
    BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        if (inLayer == Layers::NON_MOVING) return BroadPhaseLayers::NON_MOVING;
        return BroadPhaseLayers::MOVING;
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char *GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
        switch ((BroadPhaseLayer::Type)inLayer) {
            case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
            case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
            default: JPH_ASSERT(false); return "INVALID";
        }
    }
#endif
};

class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override {
        switch (inLayer1) {
            case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING: return true;
            default: JPH_ASSERT(false); return false;
        }
    }
};

class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter {
public:
    bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override {
        switch (inObject1) {
            case Layers::NON_MOVING: return inObject2 == Layers::MOVING;
            case Layers::MOVING: return true;
            default: JPH_ASSERT(false); return false;
        }
    }
};

void register_physics(nb::module_& m) {
    // BodyID
    nb::class_<BodyID>(m, "BodyID")
        .def(nb::init<>())
        .def("GetIndex", &BodyID::GetIndex)
        .def("GetSequenceNumber", &BodyID::GetSequenceNumber)
        .def(nb::self == nb::self)
        .def(nb::self != nb::self)
        .def("__hash__", [](const BodyID &b) { return std::hash<BodyID>{}(b); });

    // Helper Classes
    nb::class_<BroadPhaseLayerInterface>(m, "BroadPhaseLayerInterface");
    nb::class_<BPLayerInterfaceImpl, BroadPhaseLayerInterface>(m, "BPLayerInterfaceImpl")
        .def(nb::init<>());
        
    nb::class_<ObjectVsBroadPhaseLayerFilter>(m, "ObjectVsBroadPhaseLayerFilter");
    nb::class_<ObjectVsBroadPhaseLayerFilterImpl, ObjectVsBroadPhaseLayerFilter>(m, "ObjectVsBroadPhaseLayerFilterImpl")
        .def(nb::init<>());

    nb::class_<ObjectLayerPairFilter>(m, "ObjectLayerPairFilter");
    nb::class_<ObjectLayerPairFilterImpl, ObjectLayerPairFilter>(m, "ObjectLayerPairFilterImpl")
        .def(nb::init<>());

    // TempAllocator
    nb::class_<TempAllocator>(m, "TempAllocator");
    nb::class_<TempAllocatorImpl, TempAllocator>(m, "TempAllocatorImpl")
        .def(nb::init<uint>(), nb::arg("size"));

    // JobSystem
    nb::class_<JobSystem>(m, "JobSystem");
    nb::class_<JobSystemThreadPool, JobSystem>(m, "JobSystemThreadPool")
        .def(nb::init<uint, uint, int>(), 
             nb::arg("max_jobs"), nb::arg("max_barriers"), nb::arg("num_threads") = -1);

    // BodyCreationSettings
    nb::class_<BodyCreationSettings>(m, "BodyCreationSettings")
        .def(nb::init<>())
        .def(nb::init<const ShapeSettings*, RVec3, Quat, EMotionType, ObjectLayer>(),
             nb::arg("shape_settings"), nb::arg("position"), nb::arg("rotation"), 
             nb::arg("motion_type"), nb::arg("object_layer"))
        .def(nb::init<const Shape*, RVec3, Quat, EMotionType, ObjectLayer>(),
             nb::arg("shape"), nb::arg("position"), nb::arg("rotation"), 
             nb::arg("motion_type"), nb::arg("object_layer"))
        .def_rw("mPosition", &BodyCreationSettings::mPosition)
        .def_rw("mRotation", &BodyCreationSettings::mRotation)
        .def_rw("mMotionType", &BodyCreationSettings::mMotionType)
        .def_rw("mObjectLayer", &BodyCreationSettings::mObjectLayer);

    // BodyInterface
    nb::class_<BodyInterface>(m, "BodyInterface")
        .def("CreateBody", &BodyInterface::CreateBody, nb::rv_policy::reference)
        .def("AddBody", &BodyInterface::AddBody)
        .def("RemoveBody", &BodyInterface::RemoveBody)
        .def("IsActive", &BodyInterface::IsActive)
        .def("GetPosition", &BodyInterface::GetPosition)
        .def("SetPosition", &BodyInterface::SetPosition)
        .def("GetLinearVelocity", &BodyInterface::GetLinearVelocity)
        .def("SetLinearVelocity", &BodyInterface::SetLinearVelocity);
    
    // Body (Opaque handle mostly)
    nb::class_<Body>(m, "Body")
        .def("GetID", &Body::GetID)
        .def("GetPosition", &Body::GetPosition);

    // PhysicsSystem
    nb::class_<PhysicsSystem>(m, "PhysicsSystem")
        .def(nb::init<>())
        .def("Init", &PhysicsSystem::Init,
             nb::arg("max_bodies"), nb::arg("num_body_mutexes"), nb::arg("max_body_pairs"), nb::arg("max_contact_constraints"),
             nb::arg("broad_phase_layer_interface"), nb::arg("object_vs_broad_phase_layer_filter"), nb::arg("object_layer_pair_filter"))
        .def("OptimizeBroadPhase", &PhysicsSystem::OptimizeBroadPhase)
        .def("Update", &PhysicsSystem::Update,
             nb::arg("delta_time"), nb::arg("collision_steps"), nb::arg("temp_allocator"), nb::arg("job_system"))
        .def("GetBodyInterface", [](PhysicsSystem& self) -> BodyInterface& {
            return self.GetBodyInterface();
        }, nb::rv_policy::reference)
        .def("GetBodyInterfaceNoLock", [](PhysicsSystem& self) -> BodyInterface& {
            return self.GetBodyInterfaceNoLock();
        }, nb::rv_policy::reference);

    // Enums
    nb::enum_<EMotionType>(m, "EMotionType")
        .value("Static", EMotionType::Static)
        .value("Kinematic", EMotionType::Kinematic)
        .value("Dynamic", EMotionType::Dynamic);

    nb::enum_<EActivation>(m, "EActivation")
        .value("Activate", EActivation::Activate)
        .value("DontActivate", EActivation::DontActivate);

    nb::enum_<EPhysicsUpdateError>(m, "EPhysicsUpdateError")
        .value("None", EPhysicsUpdateError::None)
        .value("ManifoldCacheFull", EPhysicsUpdateError::ManifoldCacheFull)
        .value("BodyPairCacheFull", EPhysicsUpdateError::BodyPairCacheFull)
        .value("ContactConstraintsFull", EPhysicsUpdateError::ContactConstraintsFull);
}
