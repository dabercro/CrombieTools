# Crombie Maketree {#maketree}

The command

    crombie maketree flattree.txt

will create a header file `flattree.h` that contains a TTree-based data format.
All of the following instances of `flattree` will be adjusted to match the name of your configuration file.

The `flattree.h` file will define a class `flattree` with the following constructor declaration.

    flattree(const char* outfile_name, const char* name = "events");

Public members of the `flattree` class include all of the
[branches](@ref branchdef) by name, [prefix enums](@ref prefixdef),
[set functions](@ref setdef), and [`reset()`, `fill()`](@ref funcsig), and `write()`.

I want to make a quick comment about `write()` since all other features of `flattree` are documented in more detail below.
The declaration of `write()` is the following.

    void write(TObject* obj);

It is used to write other objects like `TH1` objects to the underlying file.
There is no need to use it to write the TTree to the file (which is a private member anyway to avoid such attempts).
That important writing step is handled by the `flattree` destructor right before closing the file.

The following sections describe the format of your configuration file and how they will affect the `flattree` format.

# Comments

Lines beginning with the character `!` are comments and are ignored by the parser.
(My only programming course was in FORTRAN, and I wanted to feel young again.)
Blank lines are also ignored.

# Includes

A single line of a header file name enclosed in `<>` or `""` will be added into the header file as an include.
These lines can be placed anywhere in the config file,
but they will be placed at the top of the header file in the order they appear in the config.
`<string>`, `<vector>`, `<unordered_map>`, `"TObject.h"`, `"TFile.h"`, and `"TTree.h"` are always included
since they are used in the class implementation, so there is no need to include any of these again.

# Changing reset() and fill() Declarations {#funcsig}

In each event loop, you should call `flattree`'s `reset()` towards the beginning and `fill()` at the end.
`reset()` puts all of the branch members back to default values, and `fill()` writes to the output tree.
This way, continuing the loop before `fill()` will act as a filter.
By default, `reset()` and `fill()` do not take any arguments.
However, we will see that the way branches are set inside these functions can be flexible, so we might want to pass arguments.
To change the signature of `reset()`, we would place a line like the following in the configuration file.

    {reset(panda::Event& event)}

The class implementation prepends all of the argument types with `const` automatically,
so there is not need (in fact, is a bug) for you to specify that yourself.

To change the fill declaration, just put `fill` in a line like the previous instead of `reset`.

# Defining Branches {#branchdef}



## Setting Default Values

If you want to avoid typing `/F` or `= 0` every line,
you can instead leave that part of the branch definition out.
Default values will instead be used.
The defaults for type and `reset()` values are `F` and `0`, respectively,
but you can change these values yourself.
This is useful if you're about to declare a bunch of `I` or something.
To change default types you can place a line like the following in your config file.

    {/I}

Subsequent unspecified branches will be of the `Int_t` type.
Default `reset()` values are set the following way.

    {-5}

# Branch Prefixes {#prefixdef}



# Set Functions {#setdef}



# Expansion



## Inline Expansions



## Expanding to Multiple Lines

The other way to expand lines in the configuration file has the example format.

    particle_$ -> input.$$() | pt, eta, phi, m

This line is expanded into four separate lines, one for each of the comma separated expressions following the `|`.
In each line, `$$` is replaced by the capitalized expression, and `$` is replaced by the expression as typed.
These two capitalizations are to handle differences between panda objects and ROOT kinematic functions.
So this example would be internally expanded by the parser into

    particle_pt -> input.Pt()
    particle_eta -> input.Eta()
    particle_phi -> input.Phi()
    particle_m -> input.M()

Since this is inside a function with `input` as an argument (you can tell by the `->` present),
prefixes and stuff will also be added into the final tree.

Again, this should be used to reduce the amount of repetitive typing and related bugs creeping in.

# TMVA Weights
