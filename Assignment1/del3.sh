echo "Temp files from last 3 days??"

find . -type f -mtime -3 \( -name "*.temp" -o -name "*.tmp" \) -print

echo "Do you want to delete the files??"
read ans

if [ "$ans" = "y" ]; then
        find . -type f -mtime -3 \( -name "*.temp" -o -name "*.tmp" \) -exec rm {} \;
        echo "Deleted"
else 
        echo "Canceled"
fi