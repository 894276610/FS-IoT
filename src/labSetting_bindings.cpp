#include <pybind11/pybind11.h>
#include "labSetting.h"

namespace py = pybind11;

PYBIND11_MODULE(labSetting_module, m) {
    // 可选：添加模块文档字符串
    m.doc() = "LabSetting Python bindings";
    
    py::enum_<MethodEnum>(m, "MethodEnum")
    .value("FSIOT", MethodEnum::FSIOT)
    .value("BYTEIOT", MethodEnum::BYTEIOT)
    .value("AHMED", MethodEnum::AHMED)
    .value("SHAHID", MethodEnum::SHAHID)
    .export_values();
    
    py::enum_<groundnut::DatasetEnum>(m, "DatasetEnum")
        .value("UNSW201620", groundnut::DatasetEnum::UNSW201620)
        .value("BehavIoT2021", groundnut::DatasetEnum::BehavIoT2021)
        .value("UNSW_TEST", groundnut::DatasetEnum::UNSW_TEST)
        .value("Aalto_TEST", groundnut::DatasetEnum::Aalto_TEST)
        .value("UNSWMVP", groundnut::DatasetEnum::UNSWMVP)
        .export_values();

    py::enum_<ExperimentEnum>(m, "ExperimentEnum")
        .value("FEW_SHOTS", ExperimentEnum::FEW_SHOTS)
        .value("DIVISION", ExperimentEnum::DIVISION)
        .export_values();
        
    py::enum_<IndependentArgEnum>(m, "IndependentArgEnum")
        .value("TRAINING_SIZE", IndependentArgEnum::TRAINING_SIZE)
        .export_values();

    // 绑定 LabSetting
    // def GetSummaryMetricCsvPath(self):
    // return self.GetResultFolder() + "summaryMetric.csv";
    py::class_<LabSetting>(m, "LabSetting")
        .def(py::init<>())
        .def("ToString", &LabSetting::ToString)
        .def("GetPredictionCsvPath", &LabSetting::GetPredictionCsvPath)
        .def("GetSummaryMetricCsvPath", &LabSetting::GetSummaryMetricCsvPath)       
        .def("GetComparisonPath", &LabSetting::GetComparisonPath)
        .def_readwrite("start", &LabSetting::start)
        .def_readwrite("end", &LabSetting::end)
        .def_readwrite("step", &LabSetting::step)
        .def_readwrite("methodName", &LabSetting::methodName)
        .def_readwrite("datasetName", &LabSetting::datasetName)
        .def_readwrite("scenario", &LabSetting::scenario)
        .def_readwrite("baseFolder", &LabSetting::baseFolder)
        .def_readwrite("slotDuration", &LabSetting::slotDuration)
        .def_readwrite("trainRate", &LabSetting::trainRate)
        .def_readwrite("trainBudget", &LabSetting::trainBudget);
}