if test "$1" = "-help"
then
printf " fpga-package-upgrade.sh 
	
 Este programa actualiza automáticamente el paquete de python 'fpga'
 
 "
	exit
fi

cd $REPO_fpga/python/modules/fpga_module
rm -r dist
python3 setup.py sdist
cd dist/
pip install *
