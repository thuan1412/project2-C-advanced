file="./a.out"
if [ -f "$file" ]
then 
    rm "$file"
fi
gcc main.c jval.o dllist.o jrb.o

file="./a.out"
if [ -f "$file" ]
then 
    echo "------------------------------------------"
    echo "Complie success."
    echo "Run result."
    ./a.out
fi
