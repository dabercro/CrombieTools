# Analysis Workspace {#workspace}

A workspace is where all analysis-specific configurations are placed.
You can create a workspace with the name `Workspace` like the following.

    mkdir Workspace
    cd Workspace
    crombie workspace

A new workspace will be created from the `CrombieTools/templates` directory
and have the following layout.

    Workspace/
    |-- docs
    |   |-- Workspace.tex
    |   |-- download.sh
    |   |-- figs
    |   `-- presentation.tex
    |-- plotter
    |   |-- CrombiePlotterConfig.sh
    |   |-- MCConfig.txt
    |   `-- cuts.py
    `-- slimmer
        |-- CrombieSlimmingConfig.sh
        |-- FlatSkimmer.sh
        |-- JobScriptList.txt
        `-- runSlimmer.py

<!---
  Note to self: this was generated using
      tree Workspace/ --charset ASCII
  after making a new workspace
-->

Each subdirectory has distinct function, which are described below.

# Slimming {#slimming}

The first thing that will often be needed in an analysis is slimming
files into flat trees.
There are two essential steps that this layout assumes you need.

<ol>

  <li> <strong>Changing the file format</strong>
       The user can go from some large, inclusive ntuples to something small and flat.
       Crombie Tools currently supports two methods to do this in batch jobs.
       One is through submissions through LXBATCH to run on files on EOS.
       The other method is to run on files interactively in a terminal.
       To allow the user to run this step less often, they are encouraged to
       not perform too much skimming of events in this step,
       as that can performed in the next step.

  <li> <strong>Skimming events from trees</strong>
       This can be done with a simple cut string, as well as using a Good Run list.

</ol>

To understand how to do these steps, take a look inside of the file `slimmer/CrombieSlimmingConfig.sh`.
The variables all start with `Crombie` to ensure that the names do not overlap with any
environment variables that may be set by other tools used by the user.
The meanings of each variable is listed below.

# Environment Variables {#envconfig}

These are the environment variables used in the slimming and skimming of an analysis.

<table cellpadding=20>
  <tr>
    <td align="left" valign="top">
      <code>CrombieFilesPerJob</code>
    </td>
    <td align="left">
      This specifies the number of files on EOS each LXBATCH job will read.
      Keep this constant between resubmissions, because it directly determines which files
      are run together.
      The other variables setting up the queue and number of cores won't change anything.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieNBatchProcs</code>
    </td>
    <td align="left">
      The number of cores used in each LXBATCH job.
      If you really need a fast turn around for a resubmission, change this instead of the number 
      of jobs per file.
      It's recommended that you use 4 cores for the 2nw4cores queue and 1 core for all others.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieQueue</code>
    </td>
    <td align="left">
      Specifies which LXBATCH queue will be submitted to.
      Examples are `8nm`, `1nh`, `8nh`, `1nd`, `2nd`, `1nw`, `2nw`, and `2nw4cores`.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieNLocalProcs</code>
    </td>
    <td align="left">
      The number of processors used for local slimming, skimming, plotting, etc.
      The default number uses all available processors, which is perhaps not always desired.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieFileBase</code>
    </td>
    <td align="left">
      Each file name of the flat files you'll work with start with `<base>_` and that base is
      set here so that your LXBATCH jobs make the correct name.
      These namespaces are useful for knowing what each ntuple was used for previously.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieEosDir</code>
    </td>
    <td align="left">
      This can be one of two things.

        - A directory on EOS where you will look for all datasets.
        - A local .txt file which has a list of directories to look for datasets.

      The submission tool will figure out which one of the two you set this variable
      as by checking if a local file with that name exists.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieRegDir</code>
    </td>
    <td align="left">
      `crombie terminalslim` does not run on EOS necessarily, like `crombie submitlxbatch` does.
      This is the variable that sets what folder to look in for datasets.
      Note that this only takes a relative directory, not a list of directories.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieTempDir</code>
    </td>
    <td align="left">
      A location to store the direct LXBATCH output.
      This is the directory that will be checked when the tool is trying to determine
      what files have and have not been successfully created.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieFullDir</code>
    </td>
    <td align="left">
      This is simply location of the hadded LXBATCH output.
      That is, all the files of the same dataset will be combined.
      This directory will also hold a list of the original locations of the datasets
      on EOS so that differences in dataset location can be detected for separate runs.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieSkimDir</code>
    </td>
    <td align="left">
      This is the location of the flat trees run through a good runs skim as well
      as any other cuts added.
      This is not a necessary variable, but is used in the template of `FlatSkimmer.sh`.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieDirList</code>
    </td>
    <td align="left">
      If left blank, all of the datasets in the `CrombieEosDir` or `CrombieRegDir` will be run on.
      Otherwise, this variable should name a local .txt file that has a list of datasets that
      you want to run on.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieSlimmerScript</code>
    </td>
    <td align="left">
      This names the script that the LXBATCH job will run.
      Make sure that the script is executable (`chmod +x`),
      and that it takes two arguments:

        - The input file name
        - The output file name

      If no arguments are passed, make sure the script compiles everything
      that it needs (with `LoadMacro('..+')` for example).
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieJobScriptList</code>
    </td>
    <td align="left">
      This variable names a local .txt file that names the relative paths of all files that should be copied
      to the LXBATCH node for the job to be completed.
      This will often include macros and headers needed.
      All of these files must be in the `slimmer` subdirectory.
      Full path names or using `..` will not work.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieCheckerScript</code>
    </td>
    <td align="left">
      Names a script that checks the output of each file run on in the LXBATCH job.
      The script should return a non-zero exit code if there is a problem with the output file.
      The script should return exit code 5 for fatal errors that will abort the job.
      The job will also abort for any non-zero exit code for the hadded output of the LXBATCH job.
      These errors will be reported in a local file, `LxbatchFileChecks.log`.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieGoodRuns</code>
    </td>
    <td align="left">
      This is a variable also only used by the template of `FlatSkimmer.sh`, so it's optional.
      It names the location of the good runs JSON for the the skimmer to use.
    </td>
  </tr>
</table>

# Generating flat trees for output {#flattrees}

There is a tool for generating flat tree classes for the user.
The variables that you want to include in a flat tree can be specified in `OutTree.txt` or whatever you rename it to.
The format of the configuration file is `<branchName>/<type>=<default>`.
`<branchName>` should be easy to understand.
Valid entries for `<type>` are the following:

<table>
  <tr><td>`F`</td><td>float</td></tr>
  <tr><td>`I`</td><td>int</td></tr>
  <tr><td>`i`</td><td>unsigned int</td></tr>
  <tr><td>`L`</td><td>long</td></tr>
  <tr><td>`l`</td><td>unsigned long</td></tr>
  <tr><td>`O`</td><td>bool</td></tr>
</table>      

Any other types will be assumed to be TObjects.
The header file for the listed TObject will be included automatically.
This feature is not tested extensively since I don't use it.

You can also preface a type with `V` for (pointers of) vectors of these types.
After writing a tree configuration file with name `OutTree.txt`, just run

    crombie maketree OutTree

This makes a class that contains your tree.
This is done automatically for you in the default `runSlimmer.py` template.
You can call each branch of the tree via a public member with the same name of the `<branchName>`
and fill the whole tree with function `Fill()` at the end of each event.
If you do not set a value for a particular event, the branch will be filled with `<default>`.
You can then write the tree to a file via `WriteToFile(*TFile,"<WhatYouWantToCallTree>")`.
There's also other overloaded write and creation function using a file name you specifiy with the initializer.
See `slimmer.cc` for an example of how to write a flat tree using this class.

# Skimming

The environment variables used for skimming the flat trees afterwards are
actually optional, if you edit the file `slimmer/FlatSkimmer.sh` to accommodate that.
The user is encouraged to edit `FlatSkimmer.sh`,
which makes use of a flexible tool, `crombie skim`.
Note, `crombie skim` is not included in the [command line references](bin/README.md)
because it is not recommended that this command is used interactively.
It is much more efficient and stable to create a script like `FlatSkimmer.sh`.
`crombie skim` takes files from an input directory, skims them, and places them in
a separate directory.
Here is the help message to help you understand how to customize this.

    usage: crombie skim [-h] [--numproc NUM] [--indir DIR] [--outdir DIR]
                        [--json FILE] [--cut CUT] [--tree NAME]
                        [--copy [NAMES [NAMES ...]]] [--run EXPR] [--lumi EXPR]
                        [--freq NUM] [--filters [FILE [FILE ...]]] [--duplicate]
    
    Slims the contents of one directory into another one
    
    optional arguments:
      -h, --help            show this help message and exit
      --numproc NUM, -n NUM
                            Number of processes that FlatSkimmer will spawn.
      --indir DIR, -i DIR   Directory that contains input files to be slimmed.
      --outdir DIR, -o DIR  Directory where slimmed stuff will be placed.
      --json FILE, -j FILE  Good runs json file location to be used.
      --cut CUT, -c CUT     Cut used in slimming.
      --tree NAME, -t NAME  Name of tree that will be slimmed.
      --copy [NAMES [NAMES ...]]
                            List other object names to copy into the slimmed file.
      --run EXPR, -r EXPR   Set the expression for Run Number.
      --lumi EXPR, -l EXPR  Set the expression for Lumi Number.
      --freq NUM, -f NUM    Set the reporting frequency.
      --filters [FILE [FILE ...]], -e [FILE [FILE ...]]
                            Set the filter files.
      --duplicate, -d       Turn on duplicate checking.

After running `FlatSkimmer.sh`, you should have your small ntuples ready to work with.

# Plotting {#plotting}

The next subdirectory of a workspace is the plotting directory.
This comes with it's own list of environment variables.
@todo Make the plotter environment variables

# Formatting MC Configuration Files {#formatmc}

Each analysis will probably make use of multiple MC Samples.
You can keep track of those all with one simple MC Config.

## Base Configuration

You will generally have one main configuration file with most of your background samples listed.
Signal samples should be kept in a separate configuration file, since these will be marked as signal or background when read.
Each sample should be contained in a single `.root` file.
The MC Config will keep track of these files, one row at a time.
The order of the elements should be this:

    <LimitTreeName>    <FileName>    <CrossSection>    <LegendEntry>    <FillColorOrLineStyle>

The elements are space delimited.

<table cellpadding=5>
  <tr>
    <td align="left" valign="top" width="15%">
      Limit Tree Name    
    </td>
    <td align="left" valign="top">
      This is the base of the tree that will be made by LimitTreeMaker for this file.
      The name should be unique for each file.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      File Name
    </td>
    <td align="left" valign="top">
      This is the name of the file for the given sample.
      The file name does not need to be absolute, as the input directory is set in 
      FileConfigReader::SetInDirectory(), usually by reading
      the [environment configuration](@ref envconfig).
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      Cross Section
    </td>
    <td align="left" valign="top">
      This should be the cross section of the sample in pb.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      Legend Entry
    </td>
    <td align="left" valign="top">
      This is the legend entry that will be made in all of the stack plots using this config for the given file.
      If you want to have spaces in your legend entry, place `_` instead (since the elements of the config are space delimited).
      These are all replaced with spaces by FileConfigReader::ReadMCConfig().
      Legend entries being repeated next to each other will cause multiple files to merge into the same stack element.
      A shortcut to using the legend entry in the previous line is to just put `.` as the Legend Entry.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      Fill Color or Line Style
    </td>
    <td align="left" valign="top">
      For background MC, this specifies the fill color, using the Color_t enums from ROOT.
      If you wish to give a custom RGB color, just make this entry `rgb` and follow that with the red, blue, and green components space delimited out of 255.
      If the legend entry of this line matches the entry in the previous line, the color is ignored (but must still be in the config).
      Again, placing a `.` in this case is a useful shortcut.

      For signal samples, this entry should give the linestyle you wish to use for the sample.
    </td>
  </tr>
</table>

## Adjustment Configuration

To avoid having duplicate entries in multple configurations, there is an easy way to switch out MC samples for different ones, while keeping the rest of the samples the same.
If a line starts with the keyword `skip` instead of a tree name and then lists a file, the MCReader will erase the MCFileInfo for that file.
A line like this simply contains:

    skip    <FileName>

A configuration file with lines like this can also contain lines like those in the base configuration.
This makes it easy to swap out files.
After reading one config, just read the adjusting configuration after before making limit trees or plotting.

# Documentation

@todo Document the documentation subdirectory

# Miscellaneous

Other directories can of course be added by hand.
There are certain ways to still source the old configuration files if you need it,
and all of the command line and python tools are still available.
Just be careful that if you change the configuration in a separate directory,
the changes will be reflected in your miscellaneous directory.
An analysis should be as tightly coupled as possible.
