# Project Todo List

@todo Document code <br>
      Group things that should be grouped

@todo Finish combining InDirectoryHolder with classes that would benefit. Check if inFile exists in AddInDir()
@todo Create reporter class to give updates on progress based on number of events.
@todo Rewrite classes that I want to run in parallel so that the shallow copy from python works.

@todo Make tests

@todo Write a tool that compares files after slimming/skimming.

@todo Make Histogram systematics adding function (AddSys(fileName,histName,startBin=1,endBin=0)) <br>
      This may be more of a PlotUtils kind of thing <br>
      Error can apply to TGraphs too then...

@todo Make generic MC reader and __update reweighter tool__ (decouple from stackplotter) <br>
      Clean up Limit Tree Maker

@todo Clean up Submission scripts <br>
      Add Error checking for the config (Things should be filled) <br>
      Check for running jobs and offer to kill them <br>
      Make fresh flag in TempDir for Full, but don't remove full right away <br>
      Somehow had empty trees from signal in Full... investigate <br>
      This came from empty Nero directories. Fix this.

@todo Mark eligible functions as const or inline
@todo Add check for existing output file into FlatSkimmer itself
@todo Test Parallelization.py and remove CrombieFlatSkimmer

@todo For plots in cutflow: <br>
      Add multiple lines for other files, can watch MC that way <br>
      Adjust to use triggers

@todo Check tdrStyle stuff out and investigate CrombieStyle
@todo Revisit legend options in PlotBase
