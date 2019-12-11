# delete existing a.out file
file="./a.out"
if [ -f "$file" ]
then 
    rm "$file"
fi

# compile main.c 
gcc main.c jval.o dllist.o jrb.o

# run main.c if the compile success
file="./a.out"
if [ -f "$file" ]
then 
    echo "------------------------------------------"
    echo "Complie success."
    echo "Run result."
    ./a.out
fi
