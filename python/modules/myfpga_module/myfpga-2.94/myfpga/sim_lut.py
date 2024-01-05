from numpy.random import normal, random
from numpy import interp

class WAVEFORM:
	def __init__(self, x=False, names=[], waves=[]):
		self.names = names
		self.waves = waves
		if not x:
			if len(waves)>0:
				self.x = [i for i in range(len(waves[0]))]
			else:
				self.x = []
		else:
			self.x = x
		
	def __call__(self, name):
		return self.x,self.waves[self.names.index(name)]
		
		
class LUT3:
	def __init__(self, config_ram, func_pot, O, I0='x', I1='x', I2='x', umbral=0.5, mu=0, sigma=0):
		self.ram = []
		for i in range(8):
			self.ram.append(config_ram%2)
			config_ram//=2
		self.func_pot = func_pot
		self.O = O
		self.I0 = I0
		self.I1 = I1
		self.I2 = I2
		self.umbral = umbral
		self.mu = mu
		self.sigma = sigma
		
	def __call__(self, O, I0, I1, I2):
		aux = 0
		if I0!='x':
			if I0 > self.umbral:
				aux = 1
		if I1!='x':
			if I1 > self.umbral:
				aux+=2
		if I2!='x':
			if I2 > self.umbral:
				aux+=4
		return delayLineModel(self.ram[aux], O, self.func_pot, self.mu, self.sigma)
		
		
class MODULO:
	def __init__(self, luts):
		self.luts = luts
		self.N = len(luts)
		
		## Guardamos los nombres de los cables que forman el sistema
		self.wnames=[]
		for i in luts:
			if i.I0 not in self.wnames:
				self.wnames.append(i.I0)
		for i in luts:
			if i.I1 not in self.wnames:
				self.wnames.append(i.I1)
		for i in luts:
			if i.I2 not in self.wnames:
				self.wnames.append(i.I2)
		for i in luts:
			if i.O not in self.wnames:
				self.wnames.append(i.O)
		
		## Asignamos valores iniciales a cada cable.
		self.w_old=[]
		for wname in self.wnames:
			if wname == 'x':
				self.w_old.append('x')
			else:
				self.w_old.append(random()) # Inicializar valores en los cables.
		self.w = self.w_old[:]
		
		## Corregimos los valores de los cables de salida incompatibles con las LUT del sistema
		for lut in luts:
			if self.probe(lut.I0)!='x':
				aux=round(self.probe(lut.I0))
			if self.probe(lut.I1)!='x':
				aux+=round(self.probe(lut.I1))
			if self.probe(lut.I2)!='x':
				aux+=round(self.probe(lut.I2))
			self.setw(lut.O, lut.ram[aux])
			
	def step(self):
		for lut in self.luts:
			O_index = self.wnames.index(lut.O)
			I0_index = self.wnames.index(lut.I0)
			I1_index = self.wnames.index(lut.I1)
			I2_index = self.wnames.index(lut.I2)
			
			self.w[O_index] = lut(O=self.w_old[O_index], I0=self.w_old[I0_index], I1=self.w_old[I1_index], I2=self.w_old[I2_index])
		
		for i in range(len(self.w)):
			self.w_old[i] = self.w[i]
			
	def setw(self, name, value):
		self.w_old[self.wnames.index(name)]=value
		self.w[self.wnames.index(name)]=value
		
	def probe(self, name):
		return self.w_old[self.wnames.index(name)]
		
	def run(self, t0=0, t1=100, dt=1, waveform_in=False, out=False):
		tspan = [t0+i*dt for i in range(int((t1-t0)/dt)+1)]
		
		waveform_proc_names = []
		waveform_proc_waves = []
		if waveform_in:
			for name in waveform_in.names:
				waveform_proc_names.append(name)
			for wave in waveform_in.waves:
				waveform_proc_waves.append(interp(tspan, [t0+j*((t1-t0)/(len(wave)-1)) for j in range(len(wave))], wave))
		waveform_proc = WAVEFORM(names=waveform_proc_names, waves=waveform_proc_waves)
		
		waveform_result_names = []
		waveform_result_waves = []
		if out:
			for out_name in out:
				waveform_result_names.append(out_name)
				waveform_result_waves.append([])
		else:
			for i in range(self.N):
				waveform_result_names.append(self.luts[i].O)
				waveform_result_waves.append([])
				
		for t in range(len(tspan)):
			for i in range(len(waveform_proc.names)):
				self.setw(waveform_proc.names[i], waveform_proc.waves[i][t])
				
			self.step()
			
			for i in range(len(waveform_result_names)):
				waveform_result_waves[i].append(self.probe(waveform_result_names[i]))
		waveform_result = WAVEFORM(x=tspan, names=waveform_result_names, waves=waveform_result_waves)
		
		return waveform_result
		
		
def numDerivative(func, x, eps=1e-10):
	return (func(x+eps)-func(x-eps))/(2*eps)


def delayLineModel(expected_value, old_value, func_pot, mu=0, sigma=0):
	result = old_value + numDerivative(func_pot, expected_value-old_value)+normal(mu, sigma)
	if result > 1:
		result=1
	elif result < 0:
		result=0
	return result