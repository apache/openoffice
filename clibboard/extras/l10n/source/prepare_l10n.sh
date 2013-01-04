#!/bin/tcsh

foreach file ( $* )
    cat $file | grep -v "	en-US	" > $file.tmp
    set lang = `head -1 $file.tmp | cut -f 10`
    if ( $lang == "" ) then
        echo "Lang = $lang in $file ... ???";
        exit(-1)
    else if ( $lang == "#" ) then
        echo "File $file contains already header .. pls remove or fix me!"
        exit(-1)
    else
        echo "Processing language $lang"
        cat header $file.tmp > $file.tmp.tmp
        mkdir $lang
        cp $file.tmp.tmp $lang/localize.sdf
    endif
    rm $file.tmp $file.tmp.tmp
end
