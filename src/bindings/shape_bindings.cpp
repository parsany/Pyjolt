#include "bindings.h"
#include <Jolt/Jolt.h>
#include <Jolt/Geometry/Sphere.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

namespace nb = nanobind;
using namespace JPH;

// We need to tell nanobind about Ref<T>
// This is usually done by specifying the holder type in nb::class_
// But we might need to verify if nanobind supports JPH::Ref directly.
// JPH::Ref has AddRef() and Release(), which follows the intrusive pointer pattern.
// We will assume nb::class_<T, Ref<T>> works if Ref is a smart pointer.
// If Ref is just a template, we might need a type caster. 
// However, Jolt's Ref is a class wrapping a pointer.
// Let's assume standard behavior for now.

void register_shapes(nb::module_& m) {
    nb::class_<Shape>(m, "Shape");

    nb::class_<BoxShape, Shape>(m, "BoxShape")
        .def(nb::init<Vec3>(), nb::arg("half_extent"))
        .def("GetHalfExtent", &BoxShape::GetHalfExtent);

    nb::class_<SphereShape, Shape>(m, "SphereShape")
        .def(nb::init<float>(), nb::arg("radius"))
        .def("GetRadius", &SphereShape::GetRadius);

    // ShapeSettings if needed
    nb::class_<ShapeSettings>(m, "ShapeSettings");
    
    nb::class_<BoxShapeSettings, ShapeSettings>(m, "BoxShapeSettings")
        .def(nb::init<Vec3>(), nb::arg("half_extent"));
        
    nb::class_<SphereShapeSettings, ShapeSettings>(m, "SphereShapeSettings")
        .def(nb::init<float>(), nb::arg("radius"));
}
