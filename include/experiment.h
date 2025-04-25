#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H

#include "labSetting.h"



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
    FewShotsExperiment(LabSetting setting) : Experiment(setting) {}

    void Run() override;
};

// class BurstExtractionExperiment : public Experiment {
// public:
//     BurstExtractionExperiment(LabSetting setting) : Experiment(setting) {}

//     void Run() override;
// };

// class SameAccuracyExperiment : public Experiment {
// public:
//     SameAccuracyExperiment(LabSetting setting) : Experiment(setting) {}

//     void Run() override;
// };

std::unique_ptr<Experiment> CreateExperiment(LabSetting setting);

#endif