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
    void Postprocessing();

protected:
    LabSetting setting;
};

class FewShotsExperiment : public Experiment {
public:
    FewShotsExperiment(LabSetting setting) :
    Experiment(setting), pktDataset(std::string(magic_enum::enum_name(setting.datasetName))){}

    void Run() override;

    virtual void Preprocessing() = 0;
    virtual void RunFewShotOnce(LabSetting setting) = 0;

protected:
    groundnut::PacketDataset pktDataset;
};

class FSIoTExperiment : public FewShotsExperiment {
public:
    FSIoTExperiment(LabSetting setting) : FewShotsExperiment(setting) {}

    void Preprocessing() override;
    void RunFewShotOnce(LabSetting setting) override;
};

class ByteIoTExperiment : public FewShotsExperiment {
public:
    ByteIoTExperiment(LabSetting setting) : FewShotsExperiment(setting) {}

    void Preprocessing() override;
    void RunFewShotOnce(LabSetting setting) override;
};

class ShahidExperiment  : public FewShotsExperiment {
public:
    ShahidExperiment(LabSetting setting) : FewShotsExperiment(setting) {}

    void Preprocessing() override;
    void RunFewShotOnce(LabSetting setting) override;
};

class AhmedExperiment : public FewShotsExperiment {
public:
    AhmedExperiment(LabSetting setting) : FewShotsExperiment(setting) {
    }


    size_t CountCsvFiles(const std::filesystem::path& directory);
    void PcapToCsv();
    void CsvToFeatureData();

    void Preprocessing() override;
    void RunFewShotOnce(LabSetting setting) override;

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
                return std::make_unique<FSIoTExperiment>(setting);
            case MethodEnum::BYTEIOT:
                return std::make_unique<ByteIoTExperiment>(setting);
            case MethodEnum::SHAHID:
                return std::make_unique<ShahidExperiment>(setting);
            case MethodEnum::AHMED:
                return std::make_unique<AhmedExperiment>(setting);
            default:
                throw std::runtime_error("Invalid method name");
        }
    }

    static std::unique_ptr<Experiment> CreateAccAlignExperiment(const LabSetting& setting) {
        switch (setting.methodName) {
            case MethodEnum::FSIOT:
                return std::make_unique<FSIoTExperiment>(setting);
            case MethodEnum::BYTEIOT:
                return std::make_unique<ByteIoTExperiment>(setting);
            case MethodEnum::SHAHID:
                return std::make_unique<ShahidExperiment>(setting);
            default:
                throw std::runtime_error("Invalid method name");
        }
    }
};

#endif