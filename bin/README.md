# Command Line Tools Reference {#commandref}

@brief Reference describing the tools available through the `crombie` command.

A number of scripts are available to the user through the `crombie` command line tool.
They can be accessed by entering 

    crombie <subcommand>

The list of available subcommands are given below.

<table cellpadding=20>
  <tr>
    <td align="left" valign="top">
      <code>backupslides</code>
    </td>
    <td align="left">
      From a [workspace](@ref workspace) `docs` directory or presentation subdirectory,
      `crombie backupslides` searches the `figs` sub-directory for `.pdf` files.
      If the `.pdf` file is not included in any `.tex` files in the local directory
      (excluding `backup_slides.tex`), the `.pdf` image will be placed on a frame in
      `backup_slides.tex` to be included in a presentation.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>clean</code>
    </td>
    <td align="left">
      Removes all files created by `crombie compile`.
      This can also take an optional arguments to only clean certain classes.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>compile</code>
    </td>
    <td align="left">
      Compiles all of the objects in the package.
      Can also take optional arguments to only compile named class (and dependencies).
      Will only recompile the objects if cleaned or the source files have been changed.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>diff</code>
    </td>
    <td align="left">
      Compares the flat n-tuples withing two different directories.
      Basic functionality is to compare the number of events in histograms and trees
      inside each identically basenamed file.
      Individual branches can also be compared.
      The output of `crombie diff -h` is shown for more information.

      <pre>
        usage: crombie diff [-h] [--numproc NUM] [--treename TREENAME]
                            [--skip-branches [BRANCHES [BRANCHES ...]]] [--verbose]
                            [--check-branch]
                            DIRECTORY DIRECTORY
        
        Checks two directories to make sure they have the same number of files and
        number of events in the tree and hist for each file
        
        positional arguments:
          DIRECTORY             The names of the two directories to compare.
        
        optional arguments:
          -h, --help            show this help message and exit
          --numproc NUM, -n NUM
                                Number of processes that FlatSkimmer will spawn.
          --treename TREENAME, -t TREENAME
                                The name of the trees to friend.
          --skip-branches [BRANCHES [BRANCHES ...]], -s [BRANCHES [BRANCHES ...]]
                                Set branches to skip comparison.
          --verbose, -v         Give a verbose checker to watch progress.
          --check-branch, -b    Checks if two branches have the same entries.
      </pre>
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>findtree</code>
    </td>
    <td align="left">
      This checks if a `.root` file has a tree inside it, and if the tree has entries.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>generatedocs</code>
    </td>
    <td align="left">
      If you have doxygen installed, this command will automatically generate the
      documentation for Crombie Tools. There are two subcommands.
      <table cellpadding=10>
        <tr>
          <td align="left" valign="top">
            <code>copy</code>
          </td>
          <td align="left">
            Will copy the documentation to a server through ssh.
            See the file itself for how to change server locations.
          </td>
        </tr>
        <tr>
          <td align="left" valign="top">
            <code>test</code>
          </td>
          <td align="left">
            Will generate the html only (default also makes a LaTeX manual).
          </td>
        </tr>
      </table>
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>maketree</code>
    </td>
    <td align="left">
      This automatically generates a tree from a configuration file.
      There is more information on these configuration files [here](@ref flattrees).
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>presentation</code>
    </td>
    <td align="left">
      From inside a [workspace's](@ref workspace) `docs` directory, this command
      creates a dated directory and copies a local `presentation.tex` into that directory.
      The copied file is named according to username, the workspace name, and the date.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>removefailed</code>
    </td>
    <td align="left">
      In CRAB jobs, there are often partial N-tuples output in a `failed` directory in the output location.
      `crombie removefailed` checks the `failed` directory and deletes each file inside if there is a larger
      file located in the successfully completed files.
      If the `failed` directory is cleared in this way, the directory is removed.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>submitlxbatch</code>
    </td>
    <td align="left">
      `crombie submitlxbatch` sources a local [CrombieSlimmingConfig.sh](@ref envconfig) to set a number of variables
      that are used to submit [slimming](@ref slimming) jobs to LXBATCH.

      There are additional subcommands available to `crombie submitlxbatch`.
      <table cellpadding=10>
        <tr>
          <td align="left" valign="top">
            <code>fresh</code>
          </td>
          <td align="left">
            Deletes the files in [$CrombieTempDir](@ref envconfig) and resubmits all possible jobs.
          </td>
        </tr>
        <tr>
          <td align="left" valign="top">
            <code>hadd</code>
          </td>
          <td align="left">
            Does not submit jobs, but goes directly to hadding what finished [slimmed](@ref slimming) files are available.
          </td>
        </tr>
        <tr>
          <td align="left" valign="top">
            <code>resub</code>
          </td>
          <td align="left">
            Resubmits using the existing `.txt` files inside [$CrombieTempDir](@ref envconfig).
            The usual behavior is to recreate these files from the input directories every time.
          </td>
        </tr>
        <tr>
          <td align="left" valign="top">
            <code>test</code>
          </td>
          <td align="left">
            Instead of submitting jobs, the submit commands are echoed to stdout.
            If all output files are available, the hadding step will still be attempted.
          </td>
        </tr>
      </table>
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>terminalslim</code>
    </td>
    <td align="left">
      This tools allows for running the [slimmer](@ref slimming) interactively in a shell.
      Like `submitlxbatch`, this sources a local [CrombieSlimmingConfig.sh](@ref envconfig) to set a number of environment variables.
      `crombie terminalslim` also comes with the same subcommands as `crombie submitlxbatch`, except for `test`.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>update</code>
    </td>
    <td align="left">
      This updates the Crombie Tools package to match the upstream of your current branch.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>workspace</code>
    </td>
    <td align="left">
      Copies a number of template files into the directory you call it from.
      These files are used to configure the Skimming tools.
      This is a [workspace](@ref workspace).
      For additional example files, give the optional argument <code>crombie workspace test</code>.
    </td>
  </tr>
</table>
