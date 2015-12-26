# Need to have numpy, pandas and matplotlib installed for this script to work.
# aka: `pip install pandas numpy matplotlib`

# Modified from HPCE-CW3

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data_ranges = [(0, 15), (15,30), (30,45), (45, 60)]

legend = []

data = pd.read_csv("aws_initial_tests.csv", sep=',',header=None)
times = data.values[:,5][1:].astype(np.float_)
sizes = data.values[:,1][1:].astype(np.float_)

for r in data_ranges:
    # print len(times[r[0]:r[1]])
    time_y = times[r[0]:r[1]]
    size_x = sizes[r[0]:r[1]]
    cmd = data.values[r[1],0]

    plt.plot(size_x, time_y)

    legend.append(cmd)

plt.yscale('log')
plt.xscale('log')

plt.title('Comparison of original process.cpp and new submission')
plt.xlabel('Pixel width and height')
plt.ylabel('Execution time (seconds)')

plt.legend(legend, loc='upper left')
plt.show()

