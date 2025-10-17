from csv import reader
from numpy import *
from matplotlib.pyplot import *
from matplotlib.ticker import FuncFormatter
from matplotlib.colors import Normalize

def readcsv(filename):
	data = []
	header = []
	with open(filename) as file:
		line = 0
		for row in reader(file):
			if line == 0:
				data = [[]] * len(row)
				for i, col in enumerate(row):
					data[i] = []
			else:
				for i, col in enumerate(row):
					data[i].append(float(col))
			line += 1
	return array(data)
	
	
def timestr(x):
	if x >= 1e9:
		return '%.0f s' % (x / 1e9)
	elif x >= 1e6:
		return '%.0f ms' % (x / 1e6)
	elif x >= 1e3:
		return '%.0f μs' % (x / 1e3)
	else:
		return '%.0f ns' % x


def plotfpsdata():
	dat = array([
		readcsv('fps-system-sleep.csv'),
		readcsv('fps-spin-lock.csv'),
		readcsv('fps-precise-sleep.csv'),
		readcsv('fps-timer-sleep.csv')])
		
	mean = 1000 * (dat.mean(axis=2).flatten() - 1000 / 60)
	min  = 1000 * (amin(dat, axis=2).flatten() - 1000 / 60)
	max  = 1000 * (amax(dat, axis=2).flatten() - 1000 / 60)
	
	fig, ax = subplots()
	fig.subplots_adjust(left=0.2)
	
	ax.set_title('60 FPS Game loop error comparison')
	ax.set_xlabel('log error in milliseconds, lower is better')
	
	y = [4, 1, 2, 3]
	x = mean
	lab = ['system sleep', 'spin lock', 'precise sleep', 'timer sleep']
	ax.barh(y, x, log=True, tick_label=lab, xerr=[min,max], height=[0.5]*4)
	ax.set_xticklabels(['', '', '0.01', '0.1', '1', '10'])
	
	tight_layout()
	show()
	
	
def plotaccuracydata():
	dat = array([
		readcsv('accuracy-system-sleep.csv'),
		readcsv('accuracy-spin-lock.csv'),
		readcsv('accuracy-precise-sleep.csv'),
		readcsv('accuracy-timer-sleep.csv')])
		
	x = dat[0][0][0:-1]
	y = array([d[0][0:-1] + d[1][0:-1] for d in dat])
	
	fig, ax = subplots()
	
	ax.set_title('Sleep accuracy comparison')
	ax.set_xlabel('requested sleep time')
	ax.set_ylabel('actual sleep time')
	ax.set_ylim(ymin=min(x), ymax=max(x))
	ax.set_xlim(xmin=min(x), xmax=max(x))
	ax.set_aspect('equal', 'box')
	ax.grid(True)
	
	labels = [
		'system sleep',
		'spin lock',
		'precise sleep',
		'timer sleep']
	
	for i in range(len(y)):
		ax.loglog(x, y[i], 'o-', label=labels[i])
		
	formatter = FuncFormatter(lambda y, _: timestr(y))
	ax.xaxis.set_major_formatter(formatter)
	ax.yaxis.set_major_formatter(formatter)
	ax.xaxis.set_ticks(x)
	ax.yaxis.set_ticks(x)
	ax.legend(loc='lower right')
	setp(ax.get_xticklabels(), rotation=45, ha='right', rotation_mode='anchor')
	
	tight_layout()
	show()


def plotrobustness():
	dat = readcsv('robustness-precise-sleep.csv')
	
	times = dat[1][0:10]
	err = array([dat[2][i+0:i+10] / times for i in range(0, len(dat[0]), 10)])
	err = err.T
	
	fig, ax = subplots()
	ax.set_title('Relative sleep duration error')
	ax.set_ylabel('requested sleep time (ms)')
	ax.set_xlabel('scheduler period (ms)')
	
	im = ax.imshow(err, origin='lower', norm=Normalize(0, 0.3))
	
	ax.yaxis.set_ticks(list(range(len(times))))
	ax.xaxis.set_ticks(list(range(len(err[0]))))
	ax.yaxis.set_ticklabels(['%.0f' % (x / 1e6) for x in times])
	ax.xaxis.set_ticklabels([str(x) for x in range(1,11)])
	
	for i in range(len(err)):
		for j in range(len(err[i])):
			text = ax.text(j, i, '%.2f' % err[i][j], ha='center', va='center', color='w')
	
	tight_layout()
	show()
	
	
def plotcpudata():
	dat1 = readcsv('cpu-precise-sleep.csv')
	dat2 = readcsv('cpu-timer-sleep.csv')
	cpu1 = dat1[1]
	cpu2 = dat2[1]
	time = dat1[0]
		
	fig, ax = subplots()
	
	ax.set_title('CPU usage % at different sleep times')
	ax.set_xlabel('sleep time (ms)')
	ax.set_ylabel('CPU usage')
	ax.set_ylim(ymin=0, ymax=105)
	ax.set_xlim(xmin=min(time), xmax=max(time))
	ax.grid(True)
	
	ax.plot(time, cpu1, 'o-', label='precise sleep')
	ax.plot(time, cpu2, 'o-', label='timer sleep')
		
	yformat = FuncFormatter(lambda x, _: '%.0f%%'  % x)
	ax.yaxis.set_major_formatter(yformat)
	ax.legend(loc='best')	
	tight_layout()
	show()
	
	
# ----------------
plotfpsdata()
#plotaccuracydata()
#plotcpudata()
#plotrobustness()