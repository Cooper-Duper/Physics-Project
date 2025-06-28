import numpy as np
import matplotlib.pyplot as plt
# plot exact solution first

omega = 1

dt = 0.5
time = np.arange(0, 2*np.pi, dt)

z1 = np.zeros_like(time)
z2 = np.zeros_like(time)

springconst = 98.69604401




# initial conditions
z1[0] = 0
z2[0] = 0

# Forward Euler iterations
for idx, t in enumerate(time[:-1]):
    z1[idx + 1] =z1[idx] + np.cos(t) * dt

plt.plot(time, z1, 'o--', label='Forward Euler solution')
time = np.linspace(0, 2*np.pi)
y_exact = (np.sin(time)
    )
plt.plot(time, y_exact, label='Exact')
plt.xlabel('time')
plt.ylabel('displacement')
plt.legend()
plt.grid(True)
plt.show()
