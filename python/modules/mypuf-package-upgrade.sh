if test "$1" = "-help"
then
printf " mypuf-package-upgrade.sh 
	
 Este programa actualiza automáticamente el paquete de python 'mypuf'
 
 "
	exit
fi

cd $REPO_puf/python/modules/mypuf_module
rm -r dist
python3 setup.py sdist
cd dist/
pip install *
