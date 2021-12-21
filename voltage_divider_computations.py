# Voltage divider computations
import matplotlib.pyplot as plt

r1 = 100000
r2 = 22000
vin = 17

def voltDiv(vin,r1,r2):
	names = ['vout', 'rtot', 'i', 'vd1', 'vd2', 'p1', 'p2', 'ptot']
	vout = vin*r2/(r2+r1)
	rtot = r1+r2
	i = vin/rtot
	vd1 = r1*i
	vd2 = r2*i
	p1 = vd1*i
	p2 = vd2*i
	ptot = p1+p2
	return names, np.array([vout,rtot,i,vd1,vd2,p1,p2,ptot])


names, vals = voltDiv(vin,r1,r2)
print([(names[i], '%.8f' %vals[i]) for i in range(len(names))])


r2s = np.arange(10000, 100000, 100)
plt.plot(r2s, voltDiv(vin,r1,r2s)[1][5], label=names[5])
plt.plot(r2s, voltDiv(vin,r1,r2s)[1][0], label=names[0])
