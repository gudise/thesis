from setuptools import setup

setup(name='thesis',
      version='1.0',
      description='Código escrito para mi tesis doctoral',
      author='gds',
      packages=['myutils','myfpga','mypuf'],
      zip_safe=False,
      package_data={'myfpga':['c-xilinx/sdk/*','tcl/*','verilog/*','scripts/*']} # Añadir más archivos si son necesarios para el módulo.
      )
