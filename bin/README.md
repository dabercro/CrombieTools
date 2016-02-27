# Command Line Tools

<table cellpadding=50>
  <tr>
    <th align="left" valign="top">
      `CleanEmacsBackups`
    </th>
    <th align="left">
       Searches the existing directory and all subdirectories for `*~` files and removes them.
    </th>
  </tr>
  <tr>
    <th align="left" valign="top">
      `CompileCrombieTools`
    </th>
    <th align="left">
      Compiles all of the objects in the package.
      Can also take an optional argument to only compile named class (and dependencies).
      Will only recompile the objects if cleaned or the source files have been changed.
    </th>
  </tr>
  <tr>
    <th align="left" valign="top">
      `CrombieClean`
    </th>
    <th align="left">
      Removes all files created by `CompileCrombieTools`.
      This can also take an optional arguments to only clean certain classes.
    </th>
  </tr>
  <tr>
    <th align="left" valign="top">
      `CrombieSetupWorkspace`
    </th>
    <th align="left">
      Copies a number of template files into the directory you call it from.
      These files are used to configure the Skimming tools, described below.
    </th>
  </tr>
</table>
