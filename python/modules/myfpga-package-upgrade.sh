if test "$1" = "-help"
then
printf " fpga-package-upgrade.sh 
	
 Este programa actualiza automáticamente el paquete de python 'fpga'
 
 "
	exit
fi

cd $REPO_fpga/python/modules/myfpga_module
python3 setup.py sdist
cd dist/
pip install *

rm -r $REPO_fpga/python/modules/myfpga_module/dist
rm -r $REPO_fpga/python/modules/myfpga_module/myfpga.egg-info