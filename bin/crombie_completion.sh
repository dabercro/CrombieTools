## 
#  @file crombie_completion.sh
#  
#  Sets the auto-complete available for people using the
#  crombie command line tools
#
#  @author Daniel Abercrombie <dabercro@mit.edu>
#

dir=$CROMBIEPATH/scripts                     # First get the directory of all the executable scripts
files=($dir/*.[p,s]?)                        # Get the list of files in the first level of this directory

subcommands=()                               # Initialize list of crombie subcommands

for f in ${files[@]}                         # For each file, create a subcommand that is just the basename
do                                           #   of the script with no extension
    f=$(basename $f)
    if [ "${f%%.*}" != "README" -a "${f%%.*}" != "noauto" ]
    then
        subcommands+="${f%%.*} "
    fi
done

_crombie_subs () {                           # Function to fill COMPREPLY, depending on the command line content

    cur=${COMP_WORDS[COMP_CWORD]}            # Get the current word
    prev=${COMP_WORDS[$((COMP_CWORD-1))]}    # Get the previous word
    
    if [ "$prev" = "crombie" ]               # If completing after "crombie", return the subcommands
    then

        COMPREPLY=( $(compgen -W "${subcommands[@]}" $cur ) )

    elif [ "$prev" = "submitlxbatch" ]       # Return the subcommands of "crombie submitlxbatch"
    then

        COMPREPLY=( $(compgen -W "hadd test resub fresh" $cur ) )

    elif [ "$prev" = "terminalslim" ]        # Return the subcommands of "crombie terminalslim"
    then

        COMPREPLY=( $(compgen -W "hadd resub fresh" $cur ) )

    elif [ "$prev" = "presentation" ]        # "crombie presentation" usually defaults to $CROMBIEDATE,
    then                                     #   so just return that

        COMPREPLY=( $(compgen -W "$CROMBIEDATE" $cur ) )

    elif [ "$prev" = "workspace" ]           # "crombie workspace" can only be varied with a test
    then

        COMPREPLY=( $(compgen -W "test" $cur ) )

    elif [ "${COMP_WORDS[1]}" = "compile" ]  # If "crombie compile" are the first two words,
    then                                     #   user can give an array of Crombie objects

        fullobjects=($CROMBIEPATH/*/src/*.cc)                 # Get the list of loadable objects
        objects=()                                            # Initialize fill list

        for obj in ${fullobjects[@]}
        do
            withext="$( basename $obj )"                      # Get basename and strip extenstions
            objects+="${withext%%.*} "                        #   to return the COMPREPLY
        done

        COMPREPLY=( $(compgen -W "${objects[@]}" $cur ) )

    else                                    # If nothing else, just return the defaults from bash

        COMPREPLY=()

    fi

    return 0

}

complete -o bashdefault -o default -F _crombie_subs crombie   # Call the previous function for "crombie" commands