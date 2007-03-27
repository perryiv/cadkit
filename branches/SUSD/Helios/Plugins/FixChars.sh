#! /bin/sh

cd "$1"

echo `pwd`

HEADERS=`ls *.h`
SOURCES=`ls *.cpp`

for FILE in ${HEADERS}
do
echo "Fixing ${FILE}"
tr -d '\357\273\277' < "$FILE" > "${FILE}.new"
mv "${FILE}.new" "${FILE}"
done

for FILE in ${SOURCES}
do
echo "Fixing ${FILE}"
tr -d '\357\273\277' < "$FILE" > "${FILE}.new"
mv "${FILE}.new" "${FILE}"
done
