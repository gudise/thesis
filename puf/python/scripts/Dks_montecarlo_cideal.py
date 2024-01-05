"""
Este script realiza una simulación de montecarlo del estadístico Dks para un modelo de PUF cuasi-ideal, y calcula el 'valor p' para una significancia del 5%.
Se utiliza configurando el número de simulaciones y los parámetros de cada simulación en el apartado 'Configuración de la simulación'. Por defecto, este script ejecuta 10000 simulaciones en 5 procesos paralelos. Para cambiar esto debe modificarse la función 'master', cambiando el número de llamadas a la función 'func', y modificando si se desea el número de simulaciones realizadas por cada una de estas llamadas.
"""

from myutils import run_in_parallel
from myutils.stats import *
from mypuf.simul import *
from numpy import array,savetxt
from scipy.stats import johnsonsb
from datetime import datetime
    

def func(N,N_inst,N_rep,N_bits,p_intra,p_inter):
    intra,inter = Dks_montecarlo_cideal(N=N, N_inst=N_inst, N_rep=N_rep, N_bits=N_bits, p_intra=p_intra, p_inter=p_inter)   
    return [intra,inter]
    
                                        
def master(N_inst,N_rep,N_bits,p_intra,p_inter):
    result = array(run_in_parallel(func=func,
                             args=[
                                   [10000//5,N_inst,N_rep,N_bits,p_intra,p_inter],
                                   [10000//5,N_inst,N_rep,N_bits,p_intra,p_inter],
                                   [10000//5,N_inst,N_rep,N_bits,p_intra,p_inter],
                                   [10000//5,N_inst,N_rep,N_bits,p_intra,p_inter],
                                   [10000//5,N_inst,N_rep,N_bits,p_intra,p_inter]
                                  ]))
    intra=result[:,0,:].flatten()
    inter=result[:,1,:].flatten()

    savetxt(f'Dks_inst{N_inst}_rep{N_rep}_bits{N_bits}_p{p_intra:.3f}.txt', intra)
    savetxt(f'Dks_inst{N_inst}_rep{N_rep}_bits{N_bits}_p{p_inter:.3f}.txt', inter)
    
    _,dks_intra,_,_=fit_rv_cont(intra, johnsonsb, alpha=0.05)
    _,dks_inter,_,_=fit_rv_cont(inter, johnsonsb, alpha=0.05)
    
    return dks_intra,dks_inter


if __name__ == "__main__":
    ##Configuración de la simulación
    simul = [
            dict(
                N_inst=10,
                N_rep=10,
                N_bits=100,
                p_intra=0.01,
                p_inter=0.45
                ),
            dict(
                N_inst=10,
                N_rep=10,
                N_bits=100,
                p_intra=0.02,
                p_inter=0.49
                )
            ]
    
    ##Ejecución de la simulación (no tocar)
    for sim in simul:
        dks_intra,dks_inter = master(N_inst=sim['N_inst'], N_rep=sim['N_rep'], N_bits=sim['N_bits'], p_intra=sim['p_intra'], p_inter=sim['p_inter'])
        sim['dks_intra'] = dks_intra
        sim['dks_inter'] = dks_inter
        print()
        
    ##Log (no tocar)
    with open('log.txt','a') as f:
        f.write(f"Simulación {datetime.today().strftime('%d-%m-%Y %H:%M:%S')}:\n\n")
        for sim in simul:
            f.write(f"\tN_inst:{sim['N_inst']}, N_rep:{sim['N_rep']}, N_bits:{sim['N_bits']}, p_intra:{sim['p_intra']:.3f}, p_inter:{sim['p_inter']:.3f}\n")
            f.write(f"\tdks_intra = {sim['dks_intra']:.6f}\n")
            f.write(f"\tdks_inter = {sim['dks_inter']:.6f}\n\n")
        f.write("-----------------------------------\n\n")
        