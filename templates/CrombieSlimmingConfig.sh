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

export CrombieGoodRuns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_v2.txt