import os
from subprocess import run

os.chdir("myaux_module") # Cambiamos a la carpeta del módulo

try: # Recuperamos el contenido de 'dist', si existe, y lo borramos
    old_content = os.listdir('dist') 
    os.remove(f"dist/{old_content[0]}")
except:
    pass

run(["python","setup.py", "sdist"]) #Ejecutamos 'setup.py'
content = os.listdir('dist') # Listamos el contenido de 'dist'
run(["python","-m","pip","install",f"dist/{content[0]}"]) # Instalamos el módulo.

