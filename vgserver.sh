valgrind --tool=drd \
         --verbose \
         --log-file=vgserver.txt \
         clientserver/build/dbserver 3
