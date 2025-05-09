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
        this->pktDataset.SetDatasetName(std::string(magic_enum::enum_name(setting.datasetName)));
    }
    
protected:
    virtual void DoFewShot();
    virtual void DoDivision();
    virtual void Preprocessing();
    virtual void RunFewShotOnce(LabSetting setting) = 0;
    virtual void Postprocessing();

protected:
    LabSetting setting;
    groundnut::PacketDataset pktDataset;
};

class FewShotsInterface {
public:
    virtual ~FewShotsInterface() = default; 
    virtual void RunFewShot() = 0;
};

class DivisionInterface {
public:
    virtual ~DivisionInterface() = default;
    virtual void RunDivision() = 0;
};

class FSIoTExperiment :  public Experiment, public FewShotsInterface,  public DivisionInterface {
public:
    FSIoTExperiment(LabSetting setting): Experiment(setting) {}
    void RunFewShot() override { DoFewShot(); }
    void RunDivision() override { DoDivision(); }
private:
    void RunFewShotOnce(LabSetting setting);
    void RunDivisionOnce(LabSetting setting);
};

class ByteIoTExperiment : public Experiment, public FewShotsInterface {
public:
    ByteIoTExperiment(LabSetting setting): Experiment(setting) {}
    void RunFewShot() override { DoFewShot(); }
    void RunDivision(){} // Not implemented
private:
    void RunFewShotOnce(LabSetting setting);
};

class ShahidExperiment  : public Experiment, public FewShotsInterface {
public:
    ShahidExperiment(LabSetting setting) : Experiment(setting) {}
    void RunFewShot() override { DoFewShot(); }
    void RunDivision(){} // Not implemented

private:
    void RunFewShotOnce(LabSetting setting);
};

class AhmedExperiment : public Experiment, public FewShotsInterface {
public:
    AhmedExperiment(LabSetting setting) : Experiment(setting) { }
    void RunFewShot() override { DoFewShot(); }
    void RunDivision(){}// Not implemented

private:
    void PcapToCsv();
    void CsvToFeatureData();
    void Preprocessing() override;
    void RunFewShotOnce(LabSetting setting);
};

class ExperimentFactory {

public:

    static std::unique_ptr<DivisionInterface> CreateDivisionExperiment(const LabSetting& setting) {
        switch (setting.methodName) {
            case MethodEnum::FSIOT:
                return std::make_unique<FSIoTExperiment>(setting);
            default:
                throw std::runtime_error("Invalid method name");
        }
    }

    static std::unique_ptr<FewShotsInterface> CreateFewShotsExperiment(const LabSetting& setting) {
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
};

#endif