#include <pybind11/pybind11.h>
#include "labSetting.h"

namespace py = pybind11;

PYBIND11_MODULE(labSetting_module, m) {
    // 可选：添加模块文档字符串
    m.doc() = "LabSetting Python bindings";
    
    // 绑定 LabSetting
    py::class_<LabSetting>(m, "LabSetting")
        .def(py::init<>())
        .def("ToString", &LabSetting::ToString);
}
