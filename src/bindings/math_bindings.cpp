#include "bindings.h"
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>
#include <nanobind/operators.h>

// Helper to print
#include <sstream>

namespace nb = nanobind;
using namespace JPH;

void register_math(nb::module_& m) {
    nb::class_<Vec3>(m, "Vec3")
        .def(nb::init<float, float, float>(), nb::arg("x")=0, nb::arg("y")=0, nb::arg("z")=0)
        .def("GetX", &Vec3::GetX)
        .def("GetY", &Vec3::GetY)
        .def("GetZ", &Vec3::GetZ)
        .def("SetX", &Vec3::SetX)
        .def("SetY", &Vec3::SetY)
        .def("SetZ", &Vec3::SetZ)
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(nb::self * float())
        .def("__repr__", [](const Vec3& v) {
            std::ostringstream oss;
            oss << "Vec3(" << v.GetX() << ", " << v.GetY() << ", " << v.GetZ() << ")";
            return oss.str();
        });

    nb::class_<Quat>(m, "Quat")
        .def("__init__", [](Quat* self) { new (self) Quat(Quat::sIdentity()); })
        .def(nb::init<float, float, float, float>())
        .def("__repr__", [](const Quat& q) {
            std::ostringstream oss;
            oss << "Quat(" << q.GetX() << ", " << q.GetY() << ", " << q.GetZ() << ", " << q.GetW() << ")";
            return oss.str();
        });
}
