source ../slimmer/CrombieSlimmingConfig.sh

export CrombieMCConfig=MCConfig.txt
export CrombieSignalConfig=SignalConfig.txt
export CrombieExcept_example=MCAdjust.txt
export CrombieLuminosity=13000.0
export CrombieInFilesDir=$CrombieSkimDir
export CrombieOutPlotDir=/afs/cern.ch/user/d/dabercro/www/plots/$(date +%y%m%d)
export CrombieOutLimitTreeDir=limits/$(date +%y%m%d)

export CrombieCutsFile=cuts.py
