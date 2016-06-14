# Project Todo List

@todo Decouple all of the stuff in PlotBase that makes adjusting stack plots a pain

@todo Document code

@todo Make tests

@todo Make Histogram systematics adding function (AddSys(fileName,histName,startBin=1,endBin=0)) <br>
      This may be more of a PlotUtils kind of thing <br>
      Error can apply to TGraphs too then...

@todo __update Reweighter tool__ (decouple from PlotStack) <br>
      Clean up LimitTreeMaker

@todo Clean up/comment Submission scripts <br>
      Add Error checking for the config (Things should be filled) <br>
      Check for running jobs and offer to kill them <br>
      Make fresh flag in TempDir for Full, but don't remove full right away <br>
      Don't make FullDir right away

@todo Mark eligible functions as const or inline

@todo For plots in cutflow: <br>
      Add multiple lines for other files, can watch MC that way <br>
      Adjust to use triggers

@todo Check tdrStyle stuff out and consider making CrombieStyle

@todo Add dotted lines and arrows to PlotBase to indicate cuts

@todo Add label maker in PlotBase

@todo Add Simulation (instead of Preliminary) option, make CMS writing default
