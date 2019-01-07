##
#  @file crombie_completion.sh
#
#  Sets the auto-complete available for people using the
#  crombie command line tools
#
#  @author Daniel Abercrombie <dabercro@mit.edu>
#

dir=$CROMBIEPATH/scripts                     # First get the directory of all the executable scripts
files=($dir/*.[p,s]? $dir/*.R)               # Get the list of files in the first level of this directory

subcommands=()                               # Initialize list of crombie subcommands

for f in ${files[@]}                         # For each file, create a subcommand that is just the basename
do                                           #   of the script with no extension
    f=$(basename $f)
    if [ "${f%%.*}" != "README" -a "${f%%.*}" != "noauto" ]
    then
        subcommands+="${f%%.*} "
    fi
done

_crombie_subs () {                                  # Function to fill COMPREPLY, depending on the command line content

    cur=${COMP_WORDS[COMP_CWORD]}                   # Get the current word
    prev=${COMP_WORDS[$((COMP_CWORD-1))]}           # Get the previous word

    if [ "$prev" = "crombie" ]                      # If completing after "crombie", return the subcommands
    then

        COMPREPLY=( $(compgen -W "${subcommands[@]}" $cur ) )

    elif [ "$prev" = "submitlxbatch" ]              # Return the subcommands of "crombie submitlxbatch"
    then

        COMPREPLY=( $(compgen -W "hadd test resub fresh" $cur ) )

    elif [ "$prev" = "submitcondor" ]               # Return the subcommands of "crombie submitcondor"
    then

        COMPREPLY=( $(compgen -W "test fresh" $cur ) )

    elif [ "$prev" = "terminalslim" ]               # Return the subcommands of "crombie terminalslim"
    then

        COMPREPLY=( $(compgen -W "hadd resub fresh" $cur ) )

    elif [ "$prev" = "presentation" ]               # "crombie presentation" usually defaults to current date
    then                                            #   so just return that

        COMPREPLY=( $(compgen -W `date +%y%m%d` $cur ) )

    elif [ "$prev" = "backupslides" ]               # "crombie backupslides" has an "all" option to have all figures
    then                                            #   so just return that

        COMPREPLY=( $(compgen -W "all" $cur ) )

    elif [ "$prev" = "workspace" ]                  # "crombie workspace" can only be varied with a test
    then

        COMPREPLY=( $(compgen -W "test" $cur ) )

    elif [ "$prev" = "test" -a $COMP_CWORD -eq 2 ]  # "crombie test" has a fast version
    then

        COMPREPLY=( $(compgen -W "fast" $cur ) )

    elif [ "$prev" = "generatedocs" ]               # "crombie generatedocs" can be a test or copy
    then

        COMPREPLY=( $(compgen -W "copy test" $cur ) )

    elif [ "$prev" = "addxs" ]                      # "crombie addxs" can skip files commented in config
    then

        COMPREPLY=( $(compgen -W "skip" $cur ) )

    elif [ "${COMP_WORDS[1]}" = "generatedocs" ]    # "crombie generatedocs copy" can go to different places
    then

        if [ "$prev" = "copy" ]
        then
            COMPREPLY=( $(compgen -W "athena lxplus" $cur ) )
        else
            COMPREPLY=()
        fi

    elif [ "${COMP_WORDS[1]}" = "compile" ]         # If "crombie compile" are the first two words,
    then                                            #   user can give an array of Crombie objects

        fullobjects=($CROMBIEPATH/old/*/src/*.cc)   # Get the list of loadable objects
        objects=()                                  # Initialize fill list

        for obj in ${fullobjects[@]}
        do
            withext="$( basename $obj )"            # Get basename and strip extenstions
            objects+="${withext%%.*} "              #   to return the COMPREPLY
        done

        COMPREPLY=( $(compgen -W "${objects[@]}" $cur ) )

    else                                            # If nothing else, just return the defaults from bash

        COMPREPLY=()

    fi

    return 0

}

complete -o bashdefault -o default -F _crombie_subs crombie   # Call the previous function for "crombie" commands
