#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H

#include "labSetting.h"
#include "magic_enum.hpp"
#include <pybind11/embed.h>

namespace py = pybind11;

class Experiment {
public:
    Experiment(LabSetting setting)
    {
        this->setting = setting;
    }

    virtual void Run() = 0;

protected:
    LabSetting setting;
};

class FewShotsExperiment : public Experiment {
public:
    FewShotsExperiment(LabSetting setting) :
    Experiment(setting), pktDataset(std::string(magic_enum::enum_name(setting.datasetName))){}

    void Run() override;

    virtual void Preprocessing() = 0;
    virtual void RunOnce(LabSetting setting) = 0;

protected:
    groundnut::PacketDataset pktDataset;
};

class FSIoTFewShotsExperiment : public FewShotsExperiment {
public:
    FSIoTFewShotsExperiment(LabSetting setting) : FewShotsExperiment(setting) {}

    void Preprocessing() override;
    void RunOnce(LabSetting setting) override;
};

class ByteIoTFewShotsExperiment : public FewShotsExperiment {
public:
    ByteIoTFewShotsExperiment(LabSetting setting) : FewShotsExperiment(setting) {}

    void Preprocessing() override;
    void RunOnce(LabSetting setting) override;
};

class ShahidFewShotsExperiment  : public FewShotsExperiment {
public:
    ShahidFewShotsExperiment(LabSetting setting) : FewShotsExperiment(setting) {}

    void Preprocessing() override;
    void RunOnce(LabSetting setting) override;
};

class AhmedFewShotsExperiment : public FewShotsExperiment {
public:
    AhmedFewShotsExperiment(LabSetting setting) : FewShotsExperiment(setting) {
        py::initialize_interpreter();
    }

    ~AhmedFewShotsExperiment() {
        py::finalize_interpreter();
    }

    size_t CountCsvFiles(const std::filesystem::path& directory);
    void PcapToCsv();
    void CsvToFeatureData();

    void Preprocessing() override;
    void RunOnce(LabSetting setting) override;

};

class ExperimentFactory {
public:
static std::unique_ptr<Experiment> CreateExperiment(const LabSetting& setting) {
    switch (setting.scenario) {
        case ExperimentEnum::FEW_SHOTS:
            return CreateFewShotsExperiment(setting);
        case ExperimentEnum::SAME_ACCURACY:
            return CreateAccAlignExperiment(setting);
        default:
            throw std::runtime_error("Invalid experiment scenario");
    }
}

private:
    static std::unique_ptr<Experiment> CreateFewShotsExperiment(const LabSetting& setting) {
        switch (setting.methodName) {
            case MethodEnum::FSIOT:
                return std::make_unique<FSIoTFewShotsExperiment>(setting);
            case MethodEnum::BYTEIOT:
                return std::make_unique<ByteIoTFewShotsExperiment>(setting);
            case MethodEnum::SHAHID:
                return std::make_unique<ShahidFewShotsExperiment>(setting);
            case MethodEnum::AHMED:
                return std::make_unique<AhmedFewShotsExperiment>(setting);
            default:
                throw std::runtime_error("Invalid method name");
        }
    }

    static std::unique_ptr<Experiment> CreateAccAlignExperiment(const LabSetting& setting) {
        switch (setting.methodName) {
            case MethodEnum::FSIOT:
                return std::make_unique<FSIoTFewShotsExperiment>(setting);
            case MethodEnum::BYTEIOT:
                return std::make_unique<ByteIoTFewShotsExperiment>(setting);
            case MethodEnum::SHAHID:
                return std::make_unique<ShahidFewShotsExperiment>(setting);
            default:
                throw std::runtime_error("Invalid method name");
        }
    }
};

#endif