export CrombieFilesPerJob=10
export CrombieNumberProcs=1
export CrombieQueue=8nh

export CrombieNLocalProcs=`getconf _NPROCESSORS_ONLN`

export CrombieEosDir=/store/user/dabercro
export CrombieRegDir=/afs/cern.ch/work/d/dabercro/eos/cms$CrombieEosDir
export CrombieTempDir=/afs/cern.ch/work/d/dabercro/public/TempOut
export CrombieFullDir=/afs/cern.ch/work/d/dabercro/public/FullOut
export CrombieSkimDir=/afs/cern.ch/work/d/dabercro/public/SkimOut
export CrombieDirList

export CrombieSlimmerScript=runSlimmer.py
export CrombieJobScriptList=CrombieJobScriptList.txt
