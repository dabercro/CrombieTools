# Command Line Tools

The following tools are all contained in `CrombieTools/bin`, which is added to your `$PATH` by the installation script.
Now you should be able to call them from anywhere within your system.

<table cellpadding=20>
  <tr>
    <td align="left" valign="top">
      <code>CleanEmacsBackups</code>
    </td>
    <td align="left">
       Searches the existing directory and all subdirectories for `*~` files and removes them.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CompileCrombieTools</code>
    </td>
    <td align="left">
      Compiles all of the objects in the package.
      Can also take an optional argument to only compile named class (and dependencies).
      Will only recompile the objects if cleaned or the source files have been changed.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieClean</code>
    </td>
    <td align="left">
      Removes all files created by `CompileCrombieTools`.
      This can also take an optional arguments to only clean certain classes.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieSubmitLxbatch</code>
    </td>
    <td align="left">
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieTerminalSlimming</code>
    </td>
    <td align="left">
      This tools allows for running the [slimmer](docs/SLIMMING.md) interactively in a shell.
      Like `CrombieSubmitLxbatch`, this sources a local [CrombieSlimmingConfig.sh](docs/ENVCONFIG.md) to set a number of environment variables.
      `CrombieTerminal` also comes with the same subcommands as `CrombieTerminalSlimming.
    </td>
  </tr>
  <tr>
    <td align="left" valign="top">
      <code>CrombieWorkspace</code>
    </td>
    <td align="left">
      Copies a number of template files into the directory you call it from.
      These files are used to configure the Skimming tools.
      For additional example files, give the optional argument <code>CrombieSetupWorkspace test</code>.
    </td>
  </tr>
</table>
