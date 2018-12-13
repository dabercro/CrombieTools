# A bool to tell the submission whether to package CrombieTools or not
export UseCrombieTools=0

# Values used by submission to get CMSSW from CVMFS
export CrombieScram=slc6_amd64_gcc530
export CrombieRelease=CMSSW_8_0_12
export CrombieRedirector=cmsxrootd.fnal.gov
export CrombieProjectName=WTagging
export CrombieCondorOutput=/mnt/hadoop/cms/store/user/dabercro/test

# Only need this if you changed the default CMSSW somehow
#export CrombieCmsswBase=/afs/cern.ch/user/d/dabercro/public/Fall16/CMSSW_8_0_12

# If this is blank, the system will instead search in some local directory
export CrombieDatasets=datasets.txt

# Note any other files you need can be added into the condorinput directory here

source CrombieSlimmingConfig.sh
