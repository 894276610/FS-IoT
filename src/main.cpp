
#include "labSetting.h"
#include "experiment.h"

int main() {
   
    LabSetting settings = GetFewShotSettingTemplate();
    ExperimentFactory::CreateExperiment(settings)->Run();
    
    return 0;
}