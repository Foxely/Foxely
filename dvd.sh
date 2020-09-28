for f in *.c; do 
    mv -- "$f" "${f%.c}.cpp"
done