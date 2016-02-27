# Command Line Tools

<table cellpadding=50>
  <tr>
    <th valign="top">
      `CleanEmacsBackups`
    </th>
    <th>
       Searches the existing directory and all subdirectories for `*~` files and removes them.
    </th>
  </tr>
  <tr>
    <th valign="top">
      `CompileCrombieTools`
    </th>
    <th>
      Compiles all of the objects in the package.
      Can also take an optional argument to only compile named class (and dependencies).
      Will only recompile the objects if cleaned or the source files have been changed.
    </th>
  </tr>
  <tr>
    <th valign="top">
      `CrombieClean`
    </th>
    <th>
      Removes all files created by `CompileCrombieTools`.
      This can also take an optional arguments to only clean certain classes.
    </th>
  </tr>
  <tr>
    <th valign="top">
      `CrombieSetupWorkspace`
    </th>
    <th>
      Copies a number of template files into the directory you call it from.
      These files are used to configure the Skimming tools, described below.
    </th>
  </tr>
</table>
