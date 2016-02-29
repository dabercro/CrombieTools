# Project Todo List

@todo Make cuts template good at returning N-1 plots and a wrapper for this in CrombieTools.PlotTools.PlotStack

@todo Document code

@todo Make tests

@todo Write a tool that compares files after slimming/skimming.

@todo Make Histogram systematics adding function (AddSys(fileName,histName,startBin=1,endBin=0)) <br>
      This may be more of a PlotUtils kind of thing <br>
      Error can apply to TGraphs too then...

@todo MC reader and __update Reweighter tool__ (decouple from PlotStack) <br>
      Clean up LimitTreeMaker, it should probably inherit from TreeContainer, but TreeContainer needs a review

@todo Clean up Submission scripts <br>
      Add Error checking for the config (Things should be filled) <br>
      Check for running jobs and offer to kill them <br>
      Make fresh flag in TempDir for Full, but don't remove full right away <br>
      Somehow had empty trees from signal in Full... investigate <br>
      This came from empty Nero directories. Fix this.

@todo Mark eligible functions as const or inline

@todo For plots in cutflow: <br>
      Add multiple lines for other files, can watch MC that way <br>
      Adjust to use triggers

@todo Check tdrStyle stuff out and investigate CrombieStyle

@todo Revisit legend options in PlotBase
