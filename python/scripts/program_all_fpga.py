"""
Este script es un procesado del proyecto 'FPGA_device_tools' creado por Abel.
En su estado actual SOLO FUNCIONA EN WINDOWS, y SOLO CON PLACAS PYNQ-Z2 (de esto último no estoy tan seguro).

El script debe ejecutarse en la carpeta de proyecto creada por las funciones 'implement()' de 'ring_osc',
y se encarga de activar / desactivar secuencialmente todas las FPGA conectadas al ordenador, ejecutando
entre medias el script 'program_fpga.py'. Requiere permisos de administrador (aunque puede ejecutarse 
desde una terminal iniciada con permisos de administrador para no tener que concederlos manualmente
cada vez que se ejecute el script).
"""

import os
import subprocess
from types import SimpleNamespace
import ctypes
import enum
import sys


class SW(enum.IntEnum):
    HIDE = 0
    MAXIMIZE = 3
    MINIMIZE = 6
    RESTORE = 9
    SHOW = 5
    SHOWDEFAULT = 10
    SHOWMAXIMIZED = 3
    SHOWMINIMIZED = 2
    SHOWMINNOACTIVE = 7
    SHOWNA = 8
    SHOWNOACTIVATE = 4
    SHOWNORMAL = 1

class ERROR(enum.IntEnum):
    ZERO = 0
    FILE_NOT_FOUND = 2
    PATH_NOT_FOUND = 3
    BAD_FORMAT = 11
    ACCESS_DENIED = 5
    ASSOC_INCOMPLETE = 27
    DDE_BUSY = 30
    DDE_FAIL = 29
    DDE_TIMEOUT = 28
    DLL_NOT_FOUND = 32
    NO_ASSOC = 31
    OOM = 8
    SHARE = 26

def run_as_admin(main):
    def wrapper():
        if ctypes.windll.shell32.IsUserAnAdmin():
            main()
        else:
            print("Asking for administrator permissions...")
            params = sys.argv if sys.executable != sys.argv[0] else sys.argv[1:]
            hinstance = ctypes.windll.shell32.ShellExecuteW(
                None, 'runas', sys.executable, " ".join([f'"{param}"' for param in params]), None, SW.SHOWNORMAL
            )
            if hinstance <= 32:
                raise RuntimeError(ERROR(hinstance))

    return wrapper


FPGA_COMMAND_LIST = "pnputil /enum-devices /class USB /connected"
FPGA_COMMAND_ENABLE = "pnputil /enable-device %s"
FPGA_COMMAND_DISABLE = "pnputil /disable-device %s"
FPGA_COMMAND_DISABLE_RETRY = 10
FPGA_COMMAND_ENABLE_RETRY = 10

FPGA_DESCRIPTION = "USB Serial Converter A"

FPGA_STATUS_DISABLED = ["Disabled", "Deshabilitado"]
FPGA_STATUS_ENABLED = ["Started", "Iniciado"]

class FPGAs:
    def __init__(self):
        self.fpgas = []
        self.update()
        self.state = []

    def update(self):
        """
        Updates the state of the fpgas
        """

        # get all
        output = subprocess.check_output(FPGA_COMMAND_LIST, universal_newlines=True)

        # process
        devices = []
        for device in output.split("\n\n")[1:-1]:
            lines = [line.split(':', 2)[1].strip() for line in device.split("\n")]
            devices.append(SimpleNamespace(
                id=lines[0],
                device_description=lines[1],
                status=lines[5],
            ))

        # filter
        fpgas = [device for device in devices if device.device_description == FPGA_DESCRIPTION]

        # modify
        prefix = len(os.path.commonprefix([fpga.id for fpga in fpgas]))
        suffix = len(os.path.commonprefix([fpga.id[::-1] for fpga in fpgas]))
        for fpga in fpgas:
            fpga.enabled = (
                True if fpga.status in FPGA_STATUS_ENABLED
                else False if fpga.status in FPGA_STATUS_DISABLED
                else None
            )
            fpga.name = fpga.id[prefix:-suffix] if len(fpgas) > 1 else fpga.id

        # log
        print("FPGAs found:")
        for fpga in fpgas:
            print(">", fpga)
        self.fpgas = fpgas

    def enabled(self, i):
        """
        returns true if board 'i' is enabled, false if isn't, None if undefined/error
        """
        return self.fpgas[i].enabled

    def allEnabled(self):
        """
        returns true iff all boards are enabled (and can be disabled)
        """
        return all(self.enabled(i) is not False for i in self)

    def allDisabled(self):
        """
        returns true iff all boards are disabled (and can be enabled)
        """
        return all(self.enabled(i) is not True for i in self)

    def name(self, i):
        """
        returns the name of board 'i'
        """
        return self.fpgas[i].name

    def id(self, i):
        """
        Returns the id of board 'i'
        """
        return self.fpgas[i].id

    def toggle(self, i, state=None):
        """
        Sets the state of board 'i'
        Call without parameters to toggle
        """
        if self.enabled(i) is None: return
        if state is None: state = not self.enabled(i)
        self.enable(i) if state else self.disable(i)

    def enable(self, i):
        """
        Enables board 'i'
        """
        if self.enabled(i) is not False: return

        print("Enabling", i)
        for _ in range(FPGA_COMMAND_ENABLE_RETRY):
            try:
                print(subprocess.check_call(FPGA_COMMAND_ENABLE % self.fpgas[i].id))
                break
            except Exception:
                pass
        else:
            print("Unable to enable the device")
        self.fpgas[i].enabled = True

    def disable(self, i):
        """
        Disable board 'i'
        """
        if self.enabled(i) is not True: return

        print("Disabling", i)
        for _ in range(FPGA_COMMAND_DISABLE_RETRY):
            try:
                print(subprocess.check_call(FPGA_COMMAND_DISABLE % self.fpgas[i].id))
                break
            except Exception:
                pass
        else:
            print("Unable to disable the device")
        self.fpgas[i].enabled = False

    def get_state(self):
        """
        returns all the current states of all boards
        """
        return [(i, self.enabled(i)) for i in self]

    def __len__(self):
        """
        the length of this object is the number of available fpgas
        for convenience
        """
        return len(self.fpgas)

    def __iter__(self):
        """
        Iterating this object is the same as iterating range(len(self))
        for convenience
        """
        return range(len(self)).__iter__()


@run_as_admin
def program_all_fpga():
    """
    Esta función toma el PATH absoluto a un proyecto 
    'projdir/projname' tal y como es generado por las
    funciones 'implement()' del módulo 'ring_osc', y
    lee automáticamente el script 'program_fpga.py'
    para ejecutarlo iterativamente sobre todas las 
    placas PYNQ-Z2 que se encuentran conectadas.
    En su estado actual SOLO FUNCIONA EN WINDOWS.
    """
    connected_fpgas = FPGAs()
    states = connected_fpgas.get_state()
    for i in connected_fpgas:
        connected_fpgas.enable(i)
        for j in connected_fpgas:
            if j != i:
                connected_fpgas.disable(j)

        subprocess.run(["python","./program_fpga.py"])
    for i, state in states:
        connected_fpgas.toggle(i, state)
        
program_all_fpga()        