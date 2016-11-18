source ../slimmer/CrombieSlimmingConfig.sh

export CrombieMCConfig=MCConfig.txt
export CrombieSignalConfig=SignalConfig.txt
export CrombieExcept_example=MCAdjust.txt
export CrombieLuminosity=13000.0
export CrombieInFilesDir=$CrombieSkimDir
export CrombieOutPlotDir=/afs/cern.ch/user/d/dabercro/www/plots/$(CROMBIEDATE)
export CrombieOutLimitTreeDir=limits/$(CROMBIEDATE)

export CrombieCutsFile=cuts.py
