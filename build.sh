#/bin/sh


rm makefile

cbp2make -in xmlpretty.cbp -out makefile


if [ $1 = "clean" ]; then
    echo "make clean"
    make clean
fi

make

retval=$?

if [ $retval != 0 ]; then
    echo make returned $retval
    exit 1
fi







