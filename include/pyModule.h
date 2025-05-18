#include <iostream>
#include <labSetting.h>
#include <pybind11/embed.h>


struct PyModule
{
    std::string absPath;
    std::string pyFileName;
    std::string pyFunctionName;
    LabSetting setting; 
    

    static PyModule CreatePlotFewShots(const LabSetting& labSetting) {
        return {
            .absPath = labSetting.GetPythonDrawFolder(),
            .pyFileName = "plotFewShots",
            .pyFunctionName = "PlotFewShot",
            .setting = labSetting
        };
    }

    inline void Run()
    {
        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")(absPath);
        std::cout << "sys.path.append(" << absPath << ")" << std::endl;
    
        py::module_ plotScript = py::module_::import(pyFileName.c_str()); 
        plotScript.attr(pyFunctionName.c_str())(setting);
    }
};