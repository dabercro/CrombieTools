# Command Line Tools

The following tools are all contained in `CrombieTools/bin`, which is added to your `$PATH` by the installation script.
Now you should be able to call them from anywhere within your system.

<table cellpadding=20>
  <tr>
    <th align="left" valign="top">
      <code>CleanEmacsBackups</code>
    </th>
    <th align="left">
       Searches the existing directory and all subdirectories for `*~` files and removes them.
    </th>
  </tr>
  <tr>
    <th align="left" valign="top">
      <code>CompileCrombieTools</code>
    </th>
    <th align="left">
      Compiles all of the objects in the package.
      Can also take an optional argument to only compile named class (and dependencies).
      Will only recompile the objects if cleaned or the source files have been changed.
    </th>
  </tr>
  <tr>
    <th align="left" valign="top">
      <code>CrombieClean</code>
    </th>
    <th align="left">
      Removes all files created by `CompileCrombieTools`.
      This can also take an optional arguments to only clean certain classes.
    </th>
  </tr>
  <tr>
    <th align="left" valign="top">
      <code>CrombieSetupWorkspace</code>
    </th>
    <th align="left">
      Copies a number of template files into the directory you call it from.
      These files are used to configure the Skimming tools, described below.
    </th>
  </tr>
</table>
