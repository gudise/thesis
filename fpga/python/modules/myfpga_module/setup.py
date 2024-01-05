from setuptools import setup

setup(name='myfpga',
      version='2.94',
      description='Módulo de Python para el proyecto fpga.',
      author='gds',
      packages=['myfpga'],
      zip_safe=False,
      package_data={'myfpga':['c-xilinx/sdk/*','tcl/*','verilog/*','scripts/*']} # Añadir más archivos si son necesarios para el módulo.
      )
