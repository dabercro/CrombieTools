# Slimming Instructions

## Generating flat trees for output

The variables that you want to include in a flat tree can be specified in `OutTree.txt` or whatever you rename it to.
The format of the configuration file is `<branchName>/<type>=<default>`.
`<branchName>` should be easy to understand.
Valid entries for `<type>` include `F`, `I`, or `O` for float, integer, and boolean, respectively.
You can also use `VF`, `VI`, or `VO` for (pointers of) vectors of these types.
After writing a tree configuration file with name `OutTree.txt`, just run

    $CROMBIEPATH/scripts/MakeTree.sh

This makes a class that contains your tree.
This is done automatically for you in the default `runSlimmer.py` template.
You can call each branch of the tree via a public member with the same name of the `<branchName>` and fill the whole tree with function `Fill()` at the end of each event.
If you do not set a value for a particular event, the branch will be filled with `<default>`.
You can then write the tree to a file via `WriteToFile(*TFile,"<WhatYouWantToCallTree>")`.
There's also other overloaded write and creation function using a file name you specifiy with the initializer.
See `slimmer.cc` for an example of how to write a flat tree writer using this class.

@todo Write instructions on running slimmer and skimming
