#include <pybind11/embed.h>
#include <iostream>
namespace py = pybind11;

void Wrapper()
{
    //py::initialize_interpreter();

    // 获取 sys.path 并添加你的模块路径
    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("append")("/home/kunling/iot-device-fingerprinting-main/src/scripts");

    // 现在可以导入你的模块（假设模块文件名为 `custom_module.py`）
    py::module_ trainScript = py::module_::import("ModelTrainingScript");

    // 调用模块中的函数（假设有函数 `process_data()`）
    std::string datasetName = "UNSWMVP";
    std::string iotBase = "/media/kunling/BigE/IoT/";
    std::string start = "30";
    std::string end = "60";
    std::string step = "30";
    trainScript.attr("runFewShot")(iotBase, datasetName, start, end, step);

    //py::finalize_interpreter();
}
int main() {   
    py::initialize_interpreter();

    Wrapper();
    std::cout << "Wrapper Done 1" << std::endl;
    Wrapper();
    std::cout << "Wrapper Done 2"<< std::endl;
    return 0;
}