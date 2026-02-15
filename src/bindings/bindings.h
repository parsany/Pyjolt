#pragma once

#include <nanobind/nanobind.h>

namespace nb = nanobind;

void register_math(nb::module_& m);
void register_physics(nb::module_& m);
void register_shapes(nb::module_& m);
