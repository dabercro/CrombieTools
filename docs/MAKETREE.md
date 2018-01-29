# Crombie Maketree {#maketree}

The command

    crombie maketree flattree.txt

will create a header file `flattree.h` that contains a TTree-based data format.
All of the following instances of `flattree` will be adjusted to match the name of your configuration file.

The `flattree.h` file will define a class `flattree` with the following constructor declaration.

    flattree(const char* outfile_name, const char* name = "events");

Public members of the `flattree` class include all of the
[branches](@ref branchdef) by name, [prefix enums](@ref prefixdef),
[set functions](@ref setdef), and [reset(), fill()](@ref funcsig), and `write()`.

I want to make a quick comment about `write()` since all other features of `flattree` are documented in more detail below.
The declaration of `write()` is the following.

    void write(TObject* obj);

It is used to write other objects like `TH1` objects to the underlying file.
There is no need to use it to write the TTree to the file (which is a private member anyway to avoid such attempts).
That important writing step is handled by the `flattree` destructor right before closing the file.

The following sections describe the format of your configuration file and how they will affect the `flattree` format.

# Comments {#comments}

Comments start with `!` and can be started anywhere in the line.
(My only programming course was in FORTRAN, and I wanted to feel young again.)
Lines starting with comments and blank lines are completely ignored.

# Includes {#includes}

A single line of a header file name enclosed in `<>` or `""` will be added into the header file as an include.
These lines can be placed anywhere in the config file,
but they will be placed at the top of the header file in the order they appear in the config.
`<string>`, `<vector>`, `"TObject.h"`, `"TFile.h"`, and `"TTree.h"` are always included
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
so there is no need (in fact, it is a bug) for you to specify that yourself.

To change the fill declaration, just put `fill` in a line like the previous instead of `reset`.

# Defining Branches {#branchdef}

Like the [old flat tree generator](@ref flattrees) in CrombieTools, branches can be defined with a type and a default value like

    branch_name/F = 1

The branch is set to the expression after the `=` during the `reset()` function call.
This can allow values to be read directly from another tree.
For example, if we use the `reset()` redefiniton example in the [previous section](@ref funcsig),
we can always directly set the run, lumi, and event numbers with the following config.

    ! Define the new reset signature
    {reset(panda::Event& event)}

    run_number/I = event.runNumber
    lumi_number/I = event.lumiNumber
    event_number/l = event.eventNumber

    ! We can even do MET
    met/F = event.pfMet.pt

With these lines, these branches are entirely taken care of inside of `reset()`.
This is just one of many mechanisms we will cover to help you define how a branch is filled
in the same line that you declare the existence of the branch.

Another one of these mechanisms is that a branch can be filled at the end of the event by the `fill()` call.
This is done by placing the desired expression after `<-`.

    ! Include deltaPhi function
    "PlotTools/interface/KinematicFunctions.h"
    dphi_mets/F <- deltaPhi(metphi, trkmetphi)

    ! Maybe we want to do something fancy at the end
    ! Make sure you redefine a signature at most once though
    {fill(panda::Jet& jet1, panda::Jet& jet2)}
    dijet_m/F <- (jet1.p4() + jet2.p4()).M()

Branches can also be set by other functions, by using a `->` operator.
That will be covered in detail in (@ref setdef).

I would like to make One final about the branches.
No matter what order the branches are declared or what [prefixes](@ref prefixdef) are defined,
the branches are all booked in alphabetical order.
This may be slightly jarring if you are expecting `run_number` at the top of your TTree when opening your `.root` file.
However, this is typically much better than leaving ordering to the frantic analyzer.

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

Default `reset()` values can only be integers.
Note that the expressions following `=` can be much more creative,
but I see little reason to support that for defaults.

If you are happy with using all of the defaults for a branch, you can simply declare the branch name,
and the class writer will take care of all the rest.
The following line is perfectly valid in the configuration file.

    branch_name

These lines expand like the following

    ! At the start of the config, defaults are /F and = 0
    branch_float  ! Equivalent to "branch_float/F = 0"

    {/I}
    branch_zero   ! Equivalent to "branch_zero/I = 0"
    {-5}
    branch_neg    ! Equivalent to "branch_neg/I = -5"

Note, these default settings are not affected by scopes at all, which are described in the [next section](@ref prefixdef).

# Branch Prefixes {#prefixdef}

I like my trees really flat (with no vectors or arrays).
This can lead to tedious and repetitive typing and bugs.
To avoid this, the configuration file allows for users to make a number of prefixes,
which will all have identical branches, and identical methods of [setting](@ref setdef) values.
Here, we will just talk about the identical creation of branches.
A simple example is given by the following.

    jet1, jet2 {
      pt/F = -5
    }

The prefixes are `jet1`, and `jet2`.
Each declaration of prefixes starts a prefix scope.
Branches declared inside of a prefix scope are expanded into a format `prefix_branchname`.
In other words, the previous example will give you the same branches as the following.

    jet1_pt/F = -5
    jet2_pt/F = -5

Scopes can be nested. Each prefix is prepended to each new prefix, just like the branch names.
However, the `_` character is not inserted into the final branch name.

    {/F}
    {-5}

    jet, bjet {
      1, 2 {
        pt
        m
      }
    }

This is equivalent to the following.

     jet1_pt/F = -5
     bjet1_pt/F = -5
     jet2_pt/F = -5
     bjet2_pt/F = -5
     jet1_m/F = -5
     bjet1_m/F = -5
     jet2_m/F = -5
     bjet2_m/F = -5

## Prefix Hierarchy

In the very last example, I started mixing jets and b-jets.
Of course, we normally have more variables that we are interested in for b-jets.
There is a mechanism to have one set of prefixes contain all of the branches of a different set, and not vice versa.
This is a weak sort of inheritance.
It can be invoked by placing a `+` in front of a new prefix declaration while inside of some other prefix scope.

    {/F}
    {-5}

    bjet1, bjet2 {
      csv
      +jet1, jet2 {
        pt
      }
    }

This is equivalent to the following.

    bjet1_csv/F = -5
    bjet2_csv/F = -5
    bjet1_pt/F = -5
    bjet2_pt/F = -5
    jet1_pt/F = -5
    jet2_pt/F = -5

Note that jet and b-jets have `pt`, but only b-jets have `csv` members.

# Set Functions {#setdef}



## Converting Prefix Classes



# Expansion {#expand}



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

# TMVA Weights {#tmvabranch}
