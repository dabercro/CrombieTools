export CrombieFilesPerJob=10
export CrombieNBatchProcs=1
export CrombieQueue=8nh

export CrombieNLocalProcs=2

export CrombieFileBase=test
export CrombieEosDir=/store/user/dabercro/Nero/v1.2
export CrombieRegDir=/afs/cern.ch/work/d/dabercro/eos/cms$CrombieEosDir
export CrombieTempDir=$CROMBIEPATH/test/TempOut
export CrombieFullDir=$CROMBIEPATH/test/FullOut
export CrombieSkimDir=$CROMBIEPATH/test/SkimOut
export CrombieDirList=

export CrombieSlimmerScript=runSlimmer.py
export CrombieJobScriptList=CrombieJobScriptList.txt
export CrombieCheckerScript="crombie findtree"

export CrombieGoodRuns=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_v2.txt
