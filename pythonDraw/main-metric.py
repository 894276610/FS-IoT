from classification import *
from pythonDraw.commonUtils import *
from settingTemplates import *
import os

setting = GetFewShotSettingTemplate();
# GetAhmedMetric(settings)

# create the path if not exist 
path = setting.GetCmPath()
if not os.path.exists(path):
    os.makedirs(path)
PlotConfusionMatrix(setting.GetResultCsvPath(), setting.GetCmPath(), width=setting.GetCmWidth(), len=setting.GetCmLen())
PlotFewShot(setting);