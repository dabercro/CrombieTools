# Analysis Workspace Explanation

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

## Slimming

The first thing that will often be needed in an analysis is slimming
files into flat trees.
There are two essential steps that this layout assumes you need.
Tools to make these steps easier are available and are described [here](docs/SLIMMING.md).

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

@todo Fill this table...

<table cellpadding=20>
  <tr>
    <td align="left" valign="top">
      <code>CrombieFilesPerJob</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieNBatchProcs</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieQueue</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieNLocalProcs</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieFileBase</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieEosDir</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieRegDir</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieTempDir</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieFullDir</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieSkimDir</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieDirList</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieSlimmerScript</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieJobScriptList</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieCheckerScript</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieGoodRuns</code>
    </td>
    <td align="left">
    </td>
  </tr>
</table>

The environment variables used for the first step of changing ntuple formats
must be present with those names, otherwise you might run into trouble when
submitting jobs.

### Generating flat trees for output

@todo Update this section, and move it around, probably

The variables that you want to include in a flat tree can be specified in `OutTree.txt` or whatever you rename it to.
The format of the configuration file is `<branchName>/<type>=<default>`.
`<branchName>` should be easy to understand.
Valid entries for `<type>` include `F`, `I`, or `O` for float, integer, and boolean, respectively.
You can also use `VF`, `VI`, or `VO` for (pointers of) vectors of these types.
After writing a tree configuration file with name `OutTree.txt`, just run

    crombie maketree

This makes a class that contains your tree.
This is done automatically for you in the default `runSlimmer.py` template.
You can call each branch of the tree via a public member with the same name of the `<branchName>` and fill the whole tree with function `Fill()` at the end of each event.
If you do not set a value for a particular event, the branch will be filled with `<default>`.
You can then write the tree to a file via `WriteToFile(*TFile,"<WhatYouWantToCallTree>")`.
There's also other overloaded write and creation function using a file name you specifiy with the initializer.
See `slimmer.cc` for an example of how to write a flat tree writer using this class.

### Skimming

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

## Plotting

### Formatting MC Configuration Files

Each analysis will probably make use of multiple MC Samples.
You can keep track of those all with one simple MC Config.

#### Base Configuration

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
      the [environment configuration](docs/ENVCONFIG.md).
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

#### Adjustment Configuration

To avoid having duplicate entries in multple configurations, there is an easy way to switch out MC samples for different ones, while keeping the rest of the samples the same.
If a line starts with the keyword `skip` instead of a tree name and then lists a file, the MCReader will erase the MCFileInfo for that file.
A line like this simply contains:

    skip    <FileName>

A configuration file with lines like this can also contain lines like those in the base configuration.
This makes it easy to swap out files.
After reading one config, just read the adjusting configuration after before making limit trees or plotting.

## Documentation



## Miscellaneous

Other directories can of course be added by hand.
There are certain ways to still source the old configuration files if you need it,
and all of the command line and python tools are still available.
Just be careful that if you change the configuration in a separate directory,
the changes will be reflected in your miscellaneous directory.
An analysis should be as tightly coupled as possible.
