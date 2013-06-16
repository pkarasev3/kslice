#echo "generating moc file:  moc-qt4 ../include/KViewer.h  -o  ../src/moc_KViewer.cpp "

# TODO: make this automatic from QT variable
/usr/local/Trolltech/Qt-4.8.4/bin/moc  ../include/KViewer.h -o ../src/moc_KViewer.cpp
