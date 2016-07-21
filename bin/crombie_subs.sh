
dir=$CROMBIEPATH/scripts
files=($dir/*.[p,s]?)

subcommands=()

for f in ${files[@]}
do
    f=$(basename $f)
    if [ "${f%%.*}" != "README" -a "${f%%.*}" != "noauto" ]
    then
        subcommands+="${f%%.*} "
    fi
done

_crombie_subs () {

    cur=${COMP_WORDS[COMP_CWORD]}
    prev=${COMP_WORDS[$((COMP_CWORD-1))]}
    
    if [ "$prev" = "crombie" ]
    then
        COMPREPLY=( $(compgen -W "${subcommands[@]}" $cur ) )
    else
        fileshere=$(ls)
        COMPREPLY=( $(compgen -W "${fileshere[@]}" $cur) )
    fi

    return 0

}

complete -F _crombie_subs crombie
