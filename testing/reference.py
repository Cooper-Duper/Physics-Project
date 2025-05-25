import numpy as np
import matplotlib.pyplot as plt
# plot exact solution first

omega = 1

dt = 0.0001
time = np.arange(0, 4.501, dt)

z1 = np.zeros_like(time)
z2 = np.zeros_like(time)

springconst = 98.69604401


def force(pos):
    totalForce = springconst * (10 + pos)
    return totalForce * (pos - 0) / np.abs(0 - pos)


# initial conditions
z1[0] = -11
z2[0] = 0

# Forward Euler iterations
for idx, t in enumerate(time[:-1]):
    z1[idx+1] = z1[idx] + z2[idx] * dt
    z2[idx+1] = z2[idx] + dt * force(z1[idx]) / 10

plt.plot(time, z1, 'o--', label='Forward Euler solution')
time = np.linspace(0, 3)
y_exact = (-10 - np.cos(np.sqrt(98.69604401 / 10) * time)
    )
plt.plot(time, y_exact, label='Exact')
plt.xlabel('time')
plt.ylabel('displacement')
plt.legend()
plt.grid(True)
plt.show()
