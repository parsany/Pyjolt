#include <nanobind/nanobind.h>
#include "bindings/bindings.h"

// Jolt includes
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace nb = nanobind;

// Debug callbacks
static void TraceImpl(const char *inFMT, ...) {
    va_list list;
    va_start(list, inFMT);
    vprintf(inFMT, list);
    va_end(list);
    printf("\n");
}

#ifdef JPH_ENABLE_ASSERTS
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine) {
    printf("%s:%d: (%s) %s\n", inFile, inLine, inExpression, inMessage ? inMessage : "");
    return true; // Breakpoint
}
#endif

void init_jolt() {
    JPH::RegisterDefaultAllocator();
    JPH::Trace = TraceImpl;
#ifdef JPH_ENABLE_ASSERTS
    JPH::AssertFailed = AssertFailedImpl;
#endif

    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}

NB_MODULE(pyjolt, m) {
    m.doc() = "JoltPhysics Python bindings using nanobind";

    // Expose init function? 
    // Or just init on module load?
    // Let's expose an init function for control
    m.def("init", &init_jolt, "Initialize the Jolt library");

    register_math(m);
    register_physics(m);
    register_shapes(m);
}
